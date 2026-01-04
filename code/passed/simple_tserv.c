#include <mds/modules.h>

struct __std {
    std_io io;
    std_kvtable (^kvtable)(void);
    
} std;
void __std_init(){
    std.io = *(std_io*)__mf_mInclude("std.io");
}

typedef struct TCPServSocket tcpsock;
typedef struct TCPServClient tcpcli;
option fmain(variable *args, size_t argc){
    var io = std.io;
    var net = *(tcp_serv*)mInclude(std.net.tcp.server);

    tcpsock *sock = td(net.create("127.0.0.1", 9000, false));

    var defer = lambda(){
        net.close(sock);
        free(sock);
    };

    tryd(net.bind(sock), defer);
    tryd(net.listen(sock, 1), defer);
    tcpcli *cli = tryd(net.accept(sock), defer).data;
    
    stream clstr = io.sio.openStream(cli->fd, 1024, 1024); 
    {  
        var inp = tryd(io.sio.sread(clstr), defer);
        if (inp.size > 0)
            io.print("Got from client %zu bytes: %s", inp.size, inp.data);

        char echo[] = "Hello from server";
        tryd(io.sio.swrite(
            clstr, (u8*)echo, strlen(echo) + 1
        ), defer);
        
    } io.sio.closeStream(&clstr);

    discard(net.cli_close(cli));
    free(cli);

    discard(net.close(sock));
    free(sock);
    
    return noerropt;
}