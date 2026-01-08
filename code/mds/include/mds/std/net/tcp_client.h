#ifndef TCP_CLIENT_HEADER
#include "mds/std/_preincl/base.h"
#include "mds/std/io/stream.h"
#include <mds/std/_preincl/defs.h>
#include <mds/std/_preincl/globals.h>
#include <mds/std/memory/allocators/abstract.h>
#include <string.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <poll.h>

#ifndef IPV4_ADDRLEN
#define IPV4_ADDRLEN 15
#endif

typedef struct {
    struct sockaddr_in servaddr;
    int fd;

    char ip[IPV4_ADDRLEN];
    u16  port;
    bool is_blocking;
} TCPClientSock;

TCPClientSock __tcp_client_create(const char serv_ip[IPV4_ADDRLEN],u16        serv_port,bool       is_blocking);
option __tcp_client_initSocket(TCPClientSock *sock);
option __tcp_client_connect(TCPClientSock *sock);
option __tcp_client_streamCreate(TCPClientSock *sock, size_t read_bfsize, size_t write_bfsize);
void __tcp_client_end(TCPClientSock *sock);

#ifdef TCP_CLIENT_IMPLEMENTATION

option __tcp_client_initSocket(TCPClientSock *sock){
    if (!sock) throw(    "TCPClientSocket: failed to init socket, sock ptr is NULL", 
        "TCPClientSocket.Init.Failed.Ptr.IsNULL", 
        -1
    );
    
    sock->fd = socket(AF_INET, SOCK_STREAM | (sock->is_blocking? 0: SOCK_NONBLOCK), 0);
    if (sock->fd == -1) throw(    "TCPClientSocket: failed to init socket, socket() return negative fd", 
        "TCPClientSocket.Init.Failed.NegativeFD", 
        -2
    );

    return noerropt;
}

TCPClientSock __tcp_client_create(const char serv_ip[IPV4_ADDRLEN],u16        serv_port,bool       is_blocking){
    var out = (TCPClientSock){
        .servaddr = {
            .sin_addr = {inet_addr(serv_ip)},        .sin_family = AF_INET,        .sin_port = htons(serv_port)
        },    .ip = "",    .port = serv_port,    .is_blocking = is_blocking
    };
    strcpy(out.ip, serv_ip);
    return out;
}

option __tcp_client_connect(TCPClientSock *sock){
    if (!sock) throw(    "TCPClientSocket: failed to connect to the server, sock ptr is NULL", 
        "TCPClientSocket.Connection.Failed.Ptr.IsNULL", 
        -1
    );

    if (sock->fd < 0) throw(    "TCPClientSocket: failed to connect to the server, have negative fd", 
        "TCPClientSocket.Connection.Failed.NegativeFD", 
        -2
    );

    socklen_t len = sizeof(sock->servaddr);
    int connst = connect(sock->fd, (const struct sockaddr *)&sock->servaddr, len);
    if (sock->is_blocking && connst < 0){
        throw(        "TCPClientSocket: failed to connect to the server, connect() returned fail status",        "TCPClientSocket.Connection.Failed",        -3
        );
    } else if (!sock->is_blocking && connst < 0){
        struct pollfd pfd = {.fd = sock->fd, .events = POLLOUT};
        if (poll(&pfd, 1, 5000) <= 0)
            throw(            "TCPClientSocket: failed to connect to the server, connect() returned timeout error",            "TCPClientSocket.Connection.Timeout",            -4
            );
    }

    return noerropt;
}

option __tcp_client_streamCreate(TCPClientSock *sock, size_t read_bfsize, size_t write_bfsize){
    if (!sock) throw(    "TCPClientSocket: failed to create stream, sock ptr is NULL", 
        "TCPClientSocket.StreamCreation.Failed.Ptr.IsNULL", 
        -1
    );

    if (sock->fd < 0) throw(    "TCPClientSocket: failed to create stream, have negative fd", 
        "TCPClientSocket.StreamCreation.Failed.NegativeFD", 
        -2
    );

    AbstractAllocator *absa = try(global.get(".absa")).data;
    stream *out = try(absa->alloc(absa->real, sizeof(stream))).data;
    if (!out) throw(    "TCPClientSocket: failed to create stream, malloc() failed", 
        "TCPClientSocket.StreamCreation.Malloc.Failed", 
        -3
    );

    *out = __openStream(sock->fd, read_bfsize, write_bfsize);
    return opt(out, sizeof(stream), true);
}

void __tcp_client_end(TCPClientSock *sock){
    if (!sock) return;
    if (sock->fd < 0) return;
    close(sock->fd);   
}


#endif
#endif
#define TCP_CLIENT_HEADER
