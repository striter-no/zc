#pragma once

#include <mds/std/_preincl/defs.h>
#include <mds/std/io/main.h>
#include <mds/std/net/main.h>
#include <mds/std/threading/main.h>
#include <mds/std/filesystem/main.h>
#include <mds/std/string/main.h>
#include <mds/std/containers/main.h>
#include <mds/std/memory/main.h>
#include <mds/std/time/main.h>
#include <mds/std/system/main.h>
#include <mds/std/math/main.h>
#include <mds/std/crypto/main.h>

#include <mds/std/_preincl/globals.h>
#include <mds/std/debug/testing/main.h>

void __inc_mods_setup(){
    __si_setup();
    __net_setup();
    __multiprocessing_setup();
    __fs_setup();
    __strings_setup();
    __containers_setup();
    __mem_setup();
    __time_setup();
    __signal_setup();
    __math_setup();
    __crypto_setup();
}