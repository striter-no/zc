#pragma once

#include <stdio.h>
#include <string.h>
#include <netinet/in.h>

#include <arpa/inet.h>

#include <sys/socket.h>
#include <unistd.h>
#include <mds/std/_preincl/defs.h>
#include <mds/std/_preincl/globals.h>
#include <mds/std/memory/allocators/abstract.h>
#include <mds/std/_preincl/base.h>

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#ifndef IPV4_ADDRLEN
#define IPV4_ADDRLEN 15
#endif

struct TCPServSocket {
    bool is_blocking;
    struct sockaddr_in addr;

    int  fd;
    char ip[IPV4_ADDRLEN];
    u16  port;

    AbstractAllocator *absa;
};

struct TCPServClient {
    struct sockaddr_in addr;
    char ip[IPV4_ADDRLEN];
    u16  port;
    int  fd;
};

option __tcp_serv_create(const char ip[IPV4_ADDRLEN],const u16  port,bool       blocking);
option __tcp_serv_bind(struct TCPServSocket *sock);
option __tcp_serv_listen(struct TCPServSocket *sock, size_t max_clients);
option __tcp_serv_accept(struct TCPServSocket *sock);
option __tcp_serv_close(struct TCPServSocket *sock);
option __tcp_client_close(struct TCPServClient *sock);
#ifdef TCP_SERVER_IMPLEMENTATION

option __tcp_serv_listen(struct TCPServSocket *sock, size_t max_clients){
    if (!sock) throw(    "Cannot start listening on server, *sock ptr is NULL",    "ServSock.Listen.NULL_ptr",    -1
    );

    if (listen(sock->fd, max_clients) < 0) throw(    "Cannot start listening on server, listen function failed",    "ServSock.Listen.Failed",    1
    );

    return noerropt;
}

option __tcp_client_close(struct TCPServClient *sock){
    if (!sock) throw(    "Cannot close client, *sock ptr is NULL",    "ServClientSock.Close.NULL_ptr",    -1
    );

    close(sock->fd);
    return noerropt;
}

option __tcp_serv_accept(struct TCPServSocket *sock){
    if (!sock) throw(    "Cannot accept client on server, *sock ptr is NULL",    "ServSock.Accept.NULL_ptr",    -1
    );

    struct TCPServClient *cli = try(sock->absa->alloc(sock->absa->real, sizeof(struct TCPServClient))).data;
    if (!cli) throw(    "Cannot accept new client, malloc failed",    "ServSock.Accept.Malloc.Failed",    -2
    );

    socklen_t len = sizeof(cli->addr);

    cli->fd = accept4(    sock->fd, 
        (struct sockaddr*)&cli->addr, 
        &len, 
        sock->is_blocking ? SOCK_CLOEXEC: (SOCK_CLOEXEC | SOCK_NONBLOCK)
    );

    if (cli->fd == -1) throw(    "Cannot accept new client, accept function failed",    "ServSock.Accept.Failed",    1
    );

    cli->port = ntohs(cli->addr.sin_port);
    strcpy(cli->ip, inet_ntoa(cli->addr.sin_addr));

    return opt(cli, sizeof(*cli), true);
}

option __tcp_serv_close(struct TCPServSocket *sock){
    if (!sock) throw(    "Cannot close server, *sock ptr is NULL",    "ServSock.Close.NULL_ptr",    -1
    );

    close(sock->fd);
    return noerropt;
}

option __tcp_serv_create(const char ip[IPV4_ADDRLEN],const u16  port,bool       blocking){
    AbstractAllocator *absa = try(global.get(".absa")).data;
    
    struct TCPServSocket *sock = try(absa->alloc(absa->real, sizeof(struct TCPServSocket))).data;
    if (!sock) throw(    "Failed to allocate memory for socket",    "ServSock.Create.Malloc.Failed",    -1
    );
    
    *sock = (struct TCPServSocket){
        .addr = (struct sockaddr_in){
            .sin_addr = {inet_addr(ip)},        
            .sin_port = htons(port),        
            .sin_family = AF_INET
        },    
        .fd = socket(AF_INET, SOCK_STREAM, 0),    
        .port = port,    
        .ip = "",    
        .is_blocking = blocking,
        .absa = absa
    };
    strcpy(sock->ip, ip);
    
    if (sock->fd < 0) throw(    
        "Cannot create TCP server socket", 
        "ServSock.Create.Failed", 
        1
    );

    if (setsockopt(    sock->fd, 
        SOL_SOCKET, 
        SO_REUSEADDR, 
        &(int){1}, 
        sizeof(int)
    ) < 0) throw(    
        "Cannot set socket option SO_REUSEADDR",    
        "ServSock.SetSockOpt.Failed",    
        2
    );

    return opt(sock, sizeof(*sock), true);
}

option __tcp_serv_bind(struct TCPServSocket *sock){
    if (!sock) throw(    "Cannot bind server, *sock ptr is NULL",    "ServSock.Bind.NULL_ptr",    -1
    );
    
    if (bind(    sock->fd,    (const struct sockaddr*)&sock->addr,    sizeof(sock->addr)
    ) < 0) throw(    "Cannot bind server, bind function failed",    "ServSock.Bind.Failed",    1
    );

    return noerropt;
}


#endif