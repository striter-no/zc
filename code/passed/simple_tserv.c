#include <mds/core_impl.h>
#include <mds/modules.h>

typedef struct TCPServSocket tcpsock;
typedef struct TCPServClient tcpcli;
option fmain(variable *args, size_t argc){
    var io = std.io.term;
    var net = std.net.tcp.server;

    tcpsock *sock = td(net.create("127.0.0.1", 9000, false));

    var defer = lambda(){
        net.close(sock);
        free(sock);
    };

    tryd(net.bind(sock), defer);
    tryd(net.listen(sock, 1), defer);
    tcpcli *cli = tryd(net.accept(sock), defer).data;
    
    stream clstr = std.io.sio.openStream(cli->fd, 1024, 1024); 
    {  
        var inp = tryd(std.io.sio.sread(clstr), defer);
        if (inp.size > 0)
            io.print("Got from client %zu bytes: %s", inp.size, inp.data);

        char echo[] = "Hello from server";
        tryd(std.io.sio.swrite(
            clstr, (u8*)echo, strlen(echo) + 1
        ), defer);
        
    } std.io.sio.closeStream(&clstr);

    discard(net.cli_close(cli));
    free(cli);

    discard(net.close(sock));
    free(sock);
    
    return noerropt;
}