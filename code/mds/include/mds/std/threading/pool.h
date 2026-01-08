#pragma once
#include "mdthreads.h"
#include "awaiter.h"
#include "conditions.h"
#include <mds/std/containers/queue.h>
#include <mds/std/containers/hashtable.h>

typedef struct {
    ;
} PoolTask;

typedef struct {
    ;
} PoolPromise;

typedef struct {
    queue input_tasks;
} ThreadPool;

