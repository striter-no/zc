#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif // for accept4
#pragma once

#include <mds/genmod.h>
#include <stdio.h>

#include <stdarg.h>

#include "tcp_server.h"
#include "tcp_client.h"

typedef struct {
    Module _minfo;

    option (*create)(
        const char ip[IPV4_ADDRLEN],
        const u16  port,
        bool       is_blocking
    );

    option (*bind)(struct TCPServSocket *sock);
    option (*listen)(struct TCPServSocket *sock, size_t max_clients);
    option (*accept)(struct TCPServSocket *sock);
    option (*close)(struct TCPServSocket *sock);
    option (*cli_close)(struct TCPServClient *sock);

} tcp_serv;
tcp_serv __tcp_serv;

typedef struct {
    Module _minfo;

    TCPClientSock (*create)(
        const char serv_ip[IPV4_ADDRLEN],
        u16        serv_port,
        bool       is_blocking
    );
    option (*initSocket)(TCPClientSock *sock);
    option (*connect)(TCPClientSock *sock);
    option (*streamCreate)(TCPClientSock *sock, size_t read_bfsize, size_t write_bfsize);
    void (*end)(TCPClientSock *sock);
} tcp_cli;
tcp_cli __tcp_cli;

void __net_setup(){
    __tcp_serv._minfo = mModuleNew("std.net.tcp.server");
    __tcp_serv.create = __tcp_serv_create;
    __tcp_serv.bind = __tcp_serv_bind;
    __tcp_serv.listen = __tcp_serv_listen;
    __tcp_serv.accept = __tcp_serv_accept;
    __tcp_serv.close = __tcp_serv_close;
    __tcp_serv.cli_close = __tcp_client_close;

    __tcp_cli._minfo = mModuleNew("std.net.tcp.client");
    __tcp_cli.create = __tcp_client_create;
    __tcp_cli.initSocket = __tcp_client_initSocket;
    __tcp_cli.connect = __tcp_client_connect;
    __tcp_cli.streamCreate = __tcp_client_streamCreate;
    __tcp_cli.end = __tcp_client_end;
}

#define TCP_SERV_INJECT_MOD {__tcp_serv._minfo, &__tcp_serv}
#define TCP_CLI_INJECT_MOD  {__tcp_cli._minfo, &__tcp_cli}