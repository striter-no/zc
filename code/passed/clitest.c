#include "mds/std/net/tcp_client.h"
#include <mds/modules.h>

option fmain(variable *args, size_t argc){
    var io = mInclude(std_io);
    var net = mInclude(net_tcp_client);

    TCPClientSock sock = net.create("127.0.0.1", 9000, true);
    try(net.initSocket(&sock));
    try(net.connect(&sock));
    stream* str = try(net.streamCreate(&sock, 1024, 1024)).data;

    io.println("Connected to the server: %s:%d", sock.ip, sock.port);
    while (true){
        var inp = io.input("> ");
        if (strcmp(inp.data, "e\n") == 0){
            io.println("exiting...");
            break;
        }

        io.println("writing %zu...", inp.size);
        try(io.sio.swrite(*str, inp.data, inp.size));
        delvar(&inp);

        io.println("reading...");
        var got = try(io.sio.sread(*str));
        io.println("%s\n", got.data);
        delvar(&got);
    }

    free(str);
    net.end(&sock);
    return noerropt;
}