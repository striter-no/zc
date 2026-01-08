#include <mds/core_impl.h>
#include <mds/modules.h>

typedef struct TCPServSocket tcpsock;
typedef struct TCPServClient tcpcli;

typedef struct {
    tcpcli *cli;
    stream stream;
    variable input_buffer;
    variable output_buffer;
} state;


option client_accept(epoller *eplr, struct epoll_event *event, tcpsock *sock);
option client_disconnect(epoller *eplr, state *ptr);
option client_input(epoller *eplr, state *ptr);
option client_output(epoller *eplr, state *ptr);

option fmain(variable *args, size_t argc){
    var net = std.net.tcp.server;
    var epoll = std.io.epoll;
    
    tcpsock *sock = td(net.create("127.0.0.1", 9000, true)); // true for non-blocking mode
    epoller *eplr = td(epoll.init(NULL));

    defer(^{
        discard(epoll.close(*eplr));
        discard(net.close(sock));
    });

    try(net.bind(sock));
    try(net.listen(sock, 20));
    epoll.add(*eplr, sock->fd, EPOLLIN, NULL);
    
    var iter = lambda(void *_event, size_t inx){
        var event = (struct epoll_event*)_event;
        state *ptr = event->data.ptr;
        if (ptr == NULL){                           
            try(client_accept(eplr, event, sock)); 
            return noerropt;
        }
        
        if (event->events & (EPOLLHUP | EPOLLERR)) 
            try(client_disconnect(eplr, ptr));
        
        if (event->events & (EPOLLIN))             
            try(client_input(eplr, ptr));
        
        if (event->events & (EPOLLOUT))            
            try(client_output(eplr, ptr));
        return noerropt;
    };

    while(true){
        var events = try(epoll.waitev(eplr, -1));
        try(events.lforeach(&events, sizeof(struct epoll_event), iter));
    }
    
    return noerropt;
}

option client_accept(epoller *eplr, struct epoll_event *event, tcpsock *sock){
    state *clistate = malloc(sizeof(state));
    *clistate = (state){
        .cli = try(std.net.tcp.server.accept(sock)).data,
        .input_buffer = mvar(NULL, 0, true),
        .output_buffer = mvar(NULL, 0, true)
    };
    clistate->stream = std.io.sio.openStream(clistate->cli->fd, 0, 0); // sizes in async mode doesn't matter

    std.io.epoll.add(*eplr, clistate->cli->fd, EPOLLIN, clistate);
    return noerropt;
}

option client_disconnect(epoller *eplr, state *ptr){
    int fd = ptr->cli->fd;
    close(fd);
    free(ptr->cli);
    free(ptr);
    std.io.epoll.delete(*eplr, fd);

    return noerropt;
}

option client_input(epoller *eplr, state *ptr){
    var jread = try(std.io.sio.nbio_sread(ptr->stream));
    if (jread.size == 0) { // end of data
        std.io.epoll.modify(*eplr, ptr->cli->fd, EPOLLOUT, ptr);
        return noerropt;
    } else {
        if (jread.size < 0){ // gracefully shutted down
            jread.size *= -1;
            try(addvar(&ptr->input_buffer, jread));
            std.io.epoll.modify(*eplr, ptr->cli->fd, EPOLLOUT, ptr);
            return noerropt;
        }

        try(addvar(&ptr->input_buffer, jread));
        
        std.io.term.println(
            "-> %s:%d : (%zu) %.*s\n", 
            ptr->cli->ip, ptr->cli->port, 
            ptr->input_buffer.size, ptr->input_buffer.size, ptr->input_buffer.data
        );

        movevar(&ptr->input_buffer, &ptr->output_buffer);
        std.io.epoll.modify(*eplr, ptr->cli->fd, EPOLLOUT, ptr);
    }

    return noerropt;
}

option client_output(epoller *eplr, state *ptr){
    var jwrite_opt = std.io.sio.nbio_swrite(ptr->stream, ptr->output_buffer.data, ptr->output_buffer.size);
    if (is_error(jwrite_opt) && gerror(jwrite_opt).code == -2) {
        int fd = ptr->cli->fd;
        close(fd);
        free(ptr->cli);
        free(ptr);
        std.io.epoll.delete(*eplr, fd);
        return noerropt;
    }

    var jwrite = tv(jwrite_opt);
    if (jwrite.size == 0 || jwrite.size == ptr->output_buffer.size) { // end of data
        std.io.epoll.modify(*eplr, ptr->cli->fd, EPOLLIN, ptr);
        delvar(&ptr->output_buffer);
        return noerropt;
    } else ptr->output_buffer.size -= jwrite.size;

    return noerropt;
}
