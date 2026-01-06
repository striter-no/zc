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

option fmain(variable *args, size_t argc){
    var io = std.io.term;
    var net = std.net.tcp.server;
    var epoll = std.io.epoll;
    // var thrd = *(threads*)mInclude(std.threading);

    io.println("Creating socket");
    tcpsock *sock = td(net.create("127.0.0.1", 9000, true)); // true for non-blocking mode

    var defer = lambda(){
        net.close(sock);
        free(sock);
    };

    io.println("Binding...");
    tryd(net.bind(sock), defer);
    io.println("Listening...");
    tryd(net.listen(sock, 20), defer);
    
    io.println("Epoll init...");
    epoller *eplr = td(epoll.init());
    epoll.add(*eplr, sock->fd, EPOLLIN, NULL);
    
    var defer2 = lambda(){
        net.close(sock); 
        epoll.close(*eplr);
        free(eplr); 
        free(sock);
    };

    io.println("Starting main cycle");
    while(true){
        var events = tryd(epoll.waitev(eplr, -1), defer2);

        var iter = lambda(void *_event, size_t inx){
            var event = (struct epoll_event*)_event;
            state *ptr = event->data.ptr;
            if (ptr == NULL){
                state *clistate = malloc(sizeof(state));
                *clistate = (state){
                    .cli = tryd(net.accept(sock), defer2).data,
                    .input_buffer = mvar(NULL, 0, true),
                    .output_buffer = mvar(NULL, 0, true)
                };
                clistate->stream = std.io.sio.openStream(clistate->cli->fd, 0, 0); // sizes in async mode doesn't matter

                epoll.add(*eplr, clistate->cli->fd, EPOLLIN, clistate);
            } else if (event->events & (EPOLLHUP | EPOLLERR)) {
                io.println("<- client disconnected");
                int fd = ptr->cli->fd;
                close(fd);
                free(ptr->cli);
                free(ptr);
                epoll.delete(*eplr, fd);
            } else if (event->events & (EPOLLIN)) {
                var jread = try(std.io.sio.aio_sread(ptr->stream));
                if (jread.size == 0) { // end of data
                    epoll.modify(*eplr, ptr->cli->fd, EPOLLOUT, ptr);
                    return noerropt;
                } else {
                    if (jread.size < 0){ // gracefully shutted down
                        jread.size *= -1;
                        try(addvar(&ptr->input_buffer, jread));
                        epoll.modify(*eplr, ptr->cli->fd, EPOLLOUT, ptr);
                        return noerropt;
                    }

                    try(addvar(&ptr->input_buffer, jread));
                    
                    io.println(
                        "-> %s:%d : (%zu) %.*s\n", 
                        ptr->cli->ip, ptr->cli->port, 
                        ptr->input_buffer.size, ptr->input_buffer.size, ptr->input_buffer.data
                    );

                    movevar(&ptr->input_buffer, &ptr->output_buffer);
                    epoll.modify(*eplr, ptr->cli->fd, EPOLLOUT, ptr);
                }
            } else if (event->events & (EPOLLOUT)) {
                // io.println("> EPOLLOUT");

                io.println("> writing");
                var jwrite_opt = std.io.sio.aio_swrite(ptr->stream, ptr->output_buffer.data, ptr->output_buffer.size);
                if (is_error(jwrite_opt) && gerror(jwrite_opt).code == -2) {
                    io.println("<- client disconnected");
                    int fd = ptr->cli->fd;
                    close(fd);
                    free(ptr->cli);
                    free(ptr);
                    epoll.delete(*eplr, fd);
                    return noerropt;
                }

                var jwrite = tv(jwrite_opt);
                if (jwrite.size == 0 || jwrite.size == ptr->output_buffer.size) { // end of data
                    io.println("> epollin");
                    epoll.modify(*eplr, ptr->cli->fd, EPOLLIN, ptr);
                    delvar(&ptr->output_buffer);
                    return noerropt;
                } else ptr->output_buffer.size -= jwrite.size;
            }
            return noerropt;
        };

        try(events.lforeach(&events, sizeof(struct epoll_event), iter));
    }

    discard(epoll.close(*eplr));
    free(eplr);

    discard(net.close(sock));
    free(sock);
    
    return noerropt;
}