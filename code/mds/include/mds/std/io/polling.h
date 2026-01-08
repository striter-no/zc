#ifndef POLLING_HEADER
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <mds/std/_preincl/base.h>
#include <mds/std/_preincl/defs.h>
#include <mds/std/_preincl/globals.h>
#include <mds/std/memory/allocators/abstract.h>

#include <mds/std/containers/table.h>
#include <sys/epoll.h>
#include <fcntl.h>

#define MAX_EPOLL_EVENTS 64

typedef struct {
    void *data_ptr;
    u32   act_events;
} epoller_cli;

typedef struct {

    kvtable clients;
    struct epoll_event events[MAX_EPOLL_EVENTS];
    int epfd;
    void *issuer;
} epoller;

option __epoller_init(void *issuer);
option __epoller_close(epoller *eplr);
option __epoller_modify(epoller *eplr, int fd_to_mod, u32 new_events, void *dataptr);
option __epoller_add(epoller *eplr, int fd_to_add, u32 events, void *dataptr);
option __epoller_delete(epoller *eplr, int fd_to_del);
option __epoller_waitev(epoller *eplr, int timeout);
#ifdef POLLING_IMPLEMENTATION

option __epoller_add(epoller *eplr, int fd_to_add, u32 events, void *dataptr){
    if (eplr->epfd < 0) throw(
        "Poller (epoll) cannot add new object, epfd < 0",
        "Poller.Add.NegativeEpfd",
        -1
    );

    if (epoll_ctl(eplr->epfd, EPOLL_CTL_ADD, fd_to_add, &(struct epoll_event){
        .events = events,
        .data.ptr = dataptr
    }) < 0){
        throw(
            "Poller (epoll) cannot add new object, epoll_ctl(add) failed",
            "Poller.Add.Failed",
            1
        );
    }

    AbstractAllocator *absa = try(global.get(".absa")).data;
    epoller_cli *cli = try(absa->alloc(absa->real, sizeof(epoller_cli))).data;
    cli->data_ptr = dataptr;
    cli->act_events = events;

    fprintf(stderr, "new cli...\n");
    try(__kvtable_shallow_set(&eplr->clients, nv(fd_to_add), mvar(
        cli, sizeof(epoller_cli), true
    )));
    return noerropt;
}

option __epoller_init(void *issuer){
    AbstractAllocator *absa = try(global.get(".absa")).data;

    epoller *obj = try(absa->alloc(absa->real, sizeof(epoller))).data;
    if (!obj) throw(
        "Poller (epoll) cannot initialize properly, malloc failed", 
        "Poller.Init.Malloc.Failed", 
        -1
    );
    
    obj->epfd = epoll_create1(0);
    if (obj->epfd < 0) {
        try(absa->free(absa->real, obj));
        throw(
            "Poller (epoll) cannot create epfd object, epoll_create1() failed",
            "Poller.Init.Epoll.Failed",
            1
        );
    }
    obj->issuer = issuer;
    obj->clients = __kvtable_new();
    return opt(obj, sizeof(epoller), true);
}

option __epoller_modify(epoller *eplr, int fd_to_mod, u32 new_events, void *dataptr){
    if (eplr->epfd < 0) throw(
        "Poller (epoll) cannot add new object, epfd < 0",
        "Poller.Add.NegativeEpfd",
        -1
    );

    if (epoll_ctl(eplr->epfd, EPOLL_CTL_MOD, fd_to_mod, &(struct epoll_event){
        .events = new_events,
        .data.ptr = dataptr
    }) < 0){
        throw(
            "Poller (epoll) cannot add new object, epoll_ctl(mod) failed",
            "Poller.Mod.Failed",
            1
        );
    }
    
    epoller_cli *cli = ((variable*)(try(__kvtable_refat(&eplr->clients, nv(fd_to_mod))).data))->data;
    cli->data_ptr = dataptr;
    cli->act_events = new_events;

    return noerropt;
}

option __epoller_delete(epoller *eplr, int fd_to_del){
    if (eplr->epfd < 0) throw(
        "Poller (epoll) cannot delete object, epfd < 0",
        "Poller.Del.NegativeEpfd",
        -1
    );

    if (epoll_ctl(eplr->epfd, EPOLL_CTL_DEL, fd_to_del, NULL) < 0){
        throw(
            "Poller (epoll) cannot delete object, epoll_ctl(del) failed",
            "Poller.Del.Failed",
            1
        );
    }

    AbstractAllocator *absa = try(global.get(".absa")).data;
    epoller_cli *cli = ((variable*)(try(__kvtable_refat(&eplr->clients, nv(fd_to_del))).data))->data;
    try(absa->free(absa->real, cli));
    __kvtable_delat(&eplr->clients, nv(fd_to_del));

    return noerropt;
}

option __epoller_close(epoller *eplr){
    AbstractAllocator *absa = try(global.get(".absa")).data;
    __kvtable_dfclean(&eplr->clients, NULL, ^(variable *v){
        absa->free(absa->real, v->data);
    });
    __kvtable_free(&eplr->clients);
    close(eplr->epfd);
    return noerropt;
}

option __epoller_waitev(epoller *eplr, int timeout){
    if (eplr->epfd < 0) throw(
        "Poller (epoll) cannot wait events, epfd < 0",
        "Poller.EventsWait.NegativeEpfd",
        -1
    );
    
    int av_num = epoll_wait(
        eplr->epfd, 
        eplr->events, 
        MAX_EPOLL_EVENTS, 
        timeout
    );
    return opt(eplr->events, av_num, false);
}


#endif

#endif
#define POLLING_HEADER
