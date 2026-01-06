#include <mds/core_impl.h>
#include <mds/modules.h>

typedef struct TCPServSocket tcpsock;
typedef struct TCPServClient tcpcli;
option fmain(variable *args, size_t argc){
    var io = std.io.term;
    var net = std.net.tcp.server;

    tcpsock *sock = td(net.create("127.0.0.1", 9000, true));

    var _defer = lambda(){
        net.close(sock);
    };

    tryd(net.bind(sock), _defer);
    tryd(net.listen(sock, 1), _defer);
    tcpcli *cli = tryd(net.accept(sock), _defer).data;
    
    stream clstr = std.io.sio.openStream(cli->fd, 1024, 1024); 
    {  
        var inp = tryd(std.io.sio.sread(clstr), _defer);
        if (inp.size > 0)
            io.print("Got from client %zu bytes: %.*s", inp.size, inp.size, inp.data);

        char echo[] = "Hello from server";
        tryd(std.io.sio.swrite(
            clstr, (u8*)echo, strlen(echo) + 1
        ), _defer);
        
    } std.io.sio.closeStream(&clstr);

    discard(net.cli_close(cli));
    discard(net.close(sock));
    return noerropt;
}