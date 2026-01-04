#pragma once
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <mds/std/_preincl/base.h>
#include <mds/std/_preincl/defs.h>
#include <mds/std/_preincl/globals.h>
#include <mds/std/memory/allocators/abstract.h>

#include <sys/epoll.h>
#include <fcntl.h>

#define MAX_EPOLL_EVENTS 64

typedef struct {
    struct epoll_event events[MAX_EPOLL_EVENTS];
    int epfd;
} epoller;

option __epoller_init();
option __epoller_close(epoller eplr);
option __epoller_modify(epoller eplr, int fd_to_mod, u32 new_events, void *dataptr);
option __epoller_add(epoller eplr, int fd_to_add, u32 events, void *dataptr);
option __epoller_delete(epoller eplr, int fd_to_del);
option __epoller_waitev(epoller *eplr, int timeout);
#ifdef POLLING_IMPLEMENTATION

option __epoller_add(epoller eplr, int fd_to_add, u32 events, void *dataptr){
    if (eplr.epfd < 0) throw(
        "Poller (epoll) cannot add new object, epfd < 0",
        "Poller.Add.NegativeEpfd",
        -1
    );

    if (epoll_ctl(eplr.epfd, EPOLL_CTL_ADD, fd_to_add, &(struct epoll_event){
        .events = events,
        .data.ptr = dataptr
    }) < 0){
        throw(
            "Poller (epoll) cannot add new object, epoll_ctl(add) failed",
            "Poller.Add.Failed",
            1
        );
    }

    return noerropt;
}

option __epoller_init(){
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

    return opt(obj, sizeof(epoller), true);
}

option __epoller_modify(epoller eplr, int fd_to_mod, u32 new_events, void *dataptr){
    if (eplr.epfd < 0) throw(
        "Poller (epoll) cannot add new object, epfd < 0",
        "Poller.Add.NegativeEpfd",
        -1
    );

    if (epoll_ctl(eplr.epfd, EPOLL_CTL_MOD, fd_to_mod, &(struct epoll_event){
        .events = new_events,
        .data.ptr = dataptr
    }) < 0){
        throw(
            "Poller (epoll) cannot add new object, epoll_ctl(mod) failed",
            "Poller.Mod.Failed",
            1
        );
    }

    return noerropt;
}

option __epoller_delete(epoller eplr, int fd_to_del){
    if (eplr.epfd < 0) throw(
        "Poller (epoll) cannot delete object, epfd < 0",
        "Poller.Del.NegativeEpfd",
        -1
    );

    if (epoll_ctl(eplr.epfd, EPOLL_CTL_DEL, fd_to_del, NULL) < 0){
        throw(
            "Poller (epoll) cannot delete object, epoll_ctl(del) failed",
            "Poller.Del.Failed",
            1
        );
    }

    return noerropt;
}

option __epoller_close(epoller eplr){
    close(eplr.epfd);
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