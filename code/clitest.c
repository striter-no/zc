#include <mds/core_impl.h>
#include <mds/modules.h>

option fmain(variable *args, size_t argc){
    var io = std.io.term;
    var net = std.net.tcp.client;

    TCPClientSock sock = net.create("127.0.0.1", 9000, true);
    try(net.initSocket(&sock));
    try(net.connect(&sock));
    stream* str = try(net.streamCreate(&sock, 1024, 1024)).data;

    io.println("Connected to the server: %s:%d", sock.ip, sock.port);
    while (true){
        var inp = try(io.input("> "));
        if (strcmp(inp.data, "e\n") == 0){
            io.println("exiting...");
            break;
        }

        io.println("writing %zu...", inp.size);
        try(std.io.sio.swrite(*str, inp.data, inp.size));
        delvar(&inp);

        io.println("reading...");
        var got = try(std.io.sio.sread(*str));

        if (std.mem.sentinel.is_sentineled(got.data, got.size, 1, "\0", 1))
            io.println("%s\n", got.data);
        else
            io.println("%.*s\n", got.size, got.data);
        // delvar(&got);
    }

    free(str);
    net.end(&sock);
    return noerropt;
}