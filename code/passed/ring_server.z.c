#include <mds/core_impl.h>
#include <mds/modules.h>

typedef struct TCPServSocket tcpsock;
typedef struct TCPServClient tcpcli;
typedef struct epoll_event epoll_event;

typedef struct {
    tcpcli *cli;
    epoller *eplr;
    stream stream;
    variable input_buffer;
    variable output_buffer;
} state;

option async_accept    (epoll_event *ev, epoller *eplr);
option async_disconnect(void *args);
option async_input     (void *args);
option async_output    (void *args);

option async_main(void *args){
    var net = std.net.tcp.server;
    var epoll = std.io.epoll;
    var async = std.async;

    tcpsock *sock = td(net.create("127.0.0.1", 9000, true)); // true for non-blocking mode
    epoller *eplr = td(epoll.init(sock));
    async_ring ring = async.ring_init(eplr);

    defer(^{
        discard(epoll.close(*eplr));
        discard(net.close(sock));
    });

    try(net.bind(sock));
    try(net.listen(sock, 20));
    epoll.add(*eplr, sock->fd, EPOLLIN, NULL);
    async.ring_firstcallback(&ring, async_accept);
    async.ring_subscribe(&ring, EPOLLIN, async_input);
    async.ring_subscribe(&ring, EPOLLOUT, async_output);
    async.ring_subscribe(&ring, EPOLLERR | EPOLLHUP, async_disconnect);

    while(true){
        var cr = try(async.async(async.ring_iter, &ring, NETWORKING_STACK)).data;
        try(async.await(cr));
    }
}

option fmain(variable *args, size_t argc){
    var async = std.async;

    var mcoro = try(async.async(async_main, NULL, LARGE_STACK * 2));
    try(async.run(mcoro.data));
    return noerropt;
}

option async_accept(epoll_event *ev, epoller *eplr){
    std.io.term.println("accepted new client");
    state *clistate = galloc(sizeof(state));
    *clistate = (state){
        .cli = try(std.net.tcp.server.accept(eplr->issuer)).data,
        .input_buffer = mvar(NULL, 0, true),
        .output_buffer = mvar(NULL, 0, true),
        .eplr = eplr
    };
    clistate->stream = std.io.sio.openStream(clistate->cli->fd, 0, 0);

    std.io.epoll.add(*eplr, clistate->cli->fd, EPOLLIN, clistate);
    return noerropt;
}

option async_disconnect(void *dataptr){
    std.io.term.println("client disconnected");
    state *st = dataptr;
    
    std.io.epoll.delete(*st->eplr, st->cli->fd);
    close(st->cli->fd);
    gfree(st->cli); 
    gfree(st);
    return noerropt;
}

option async_input(void *dataptr){
    std.io.term.println("client input...");
    state *st = dataptr;
    var jread = try(std.io.sio.nbio_sread(st->stream));
    suspend

    if (jread.size == 0) {
        std.io.epoll.modify(*st->eplr, st->cli->fd, EPOLLOUT, st);
        return noerropt;
    }
    if (jread.size < 0)
        return async_disconnect(dataptr);

    try(addvar(&st->input_buffer, jread));
    
    std.io.term.println(
        "-> %s:%d : (%zu) %.*s\n", 
        st->cli->ip, st->cli->port, 
        st->input_buffer.size, st->input_buffer.size, st->input_buffer.data
    );

    gmovevar(&st->input_buffer, &st->output_buffer);
    std.io.epoll.modify(*st->eplr, st->cli->fd, EPOLLOUT, st);
    
    return noerropt;
}

option async_output(void *dataptr){
    std.io.term.println("client output...");
    state *st = dataptr;
    var jwrite_opt = std.io.sio.nbio_swrite(st->stream, st->output_buffer.data, st->output_buffer.size);
    if (is_error(jwrite_opt) && gerror(jwrite_opt).code == -2)
        return async_disconnect(dataptr);
    suspend
    var jwrite = tv(jwrite_opt);
    if (jwrite.size == 0 || jwrite.size == st->output_buffer.size) { // end of data
        std.io.epoll.modify(*st->eplr, st->cli->fd, EPOLLIN, st);
        gdelvar(&st->output_buffer);
        return noerropt;
    } else st->output_buffer.size -= jwrite.size;

    return noerropt;
}
