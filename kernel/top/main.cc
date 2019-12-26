// Copyright 2016 The Fuchsia Authors
// Copyright (c) 2013-2015 Travis Geiselbrecht
//
// Use of this source code is governed by a MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT

/*
 * Main entry point to the OS. Initializes modules in order and creates
 * the default thread.
 */
#include "lk/main.h"

#include <arch.h>
#include <debug.h>
#include <kernel/init.h>
#include <kernel/mutex.h>
#include <kernel/thread.h>
#include <kernel/percpu.h>
#include <lib/heap.h>
#include <lib/debuglog.h>
#include <lk/init.h>
#include <platform.h>
#include <string.h>
#include <target.h>
#include <vm/init.h>
#include <vm/vm.h>
#include <zircon/compiler.h>

extern void (*const __init_array_start[])();
extern void (*const __init_array_end[])();

static uint secondary_idle_thread_count;

static int bootstrap2(void* arg);

static void call_constructors() {
    for (void (*const* a)() = __init_array_start; a != __init_array_end; a++)
        (*a)();
}

// called from arch code
void lk_main() {
    // serial prints to console based on compile time switch
    // 根据编译时的ENABLE_KERNEL_LL_DEBUG选项，设置是否打印debug log
    dlog_bypass_init_early();

    // get us into some sort of thread context
    // 将当前运行状态记录到一个thread中，并设置该thread为当前正在运行的thread
    thread_init_early();

    // deal with any static constructors
    // 根据cpp的语言特性，手动调用某些对象的隐形初始化函数
    call_constructors();

    // early arch stuff
    // [0x1, 0xffff]
    lk_primary_cpu_init_level(LK_INIT_LEVEL_EARLIEST, LK_INIT_LEVEL_ARCH_EARLY - 1);
    arch_early_init();

    // do any super early platform initialization
    // [0x10000, 0x1ffff]
    /*
     * code_patching in $zx/kernel/lib/code_patching.cc
     */
    lk_primary_cpu_init_level(LK_INIT_LEVEL_ARCH_EARLY, LK_INIT_LEVEL_PLATFORM_EARLY - 1);
    platform_early_init();

    // do any super early target initialization
    // [0x20000, 0x2ffff]
    /*
     * 
     */
    lk_primary_cpu_init_level(LK_INIT_LEVEL_PLATFORM_EARLY, LK_INIT_LEVEL_TARGET_EARLY - 1);
    target_early_init();

    dprintf(INFO, "\nwelcome to Zircon, i am panpan\n\n");

    dprintf(INFO, "KASLR: .text section at %p\n", __code_start);

    // [0x30000, 0x3ffff]
    /*
     * global_prng_seed in $zx/kernel/lib/crypto/global_crypto.cc
     */
    lk_primary_cpu_init_level(LK_INIT_LEVEL_TARGET_EARLY, LK_INIT_LEVEL_VM_PREHEAP - 1);
    dprintf(INFO, "initializing vm pre-heap\n");
    vm_init_preheap();

    // bring up the kernel heap
    // [0x40000, 0x4ffff]
    /*
     * elf_build_id in $zx/kernel/lib/version/version.cc
     * version in $zx/kernel/lib/version/version.cc
     */
    lk_primary_cpu_init_level(LK_INIT_LEVEL_VM_PREHEAP, LK_INIT_LEVEL_HEAP - 1);
    dprintf(INFO, "initializing heap\n");
    heap_init();

    // [0x50000, 0x5ffff]
    /*
     * console in $zx/kernel/lib/console/console.cc
     * x86_resource_init in $zx/kernel/platform/pc/memory.cc
     */
    lk_primary_cpu_init_level(LK_INIT_LEVEL_HEAP, LK_INIT_LEVEL_VM - 1);
    dprintf(INFO, "initializing vm\n");
    vm_init();

    // initialize the kernel
    // [0x60000, 0x6ffff]
    /*
     * display_memtype in $zx/kernel/platform/pc/memory.cc
     * acpi_tables in $zx/kernel/lib/acpi_tables/acpi_tables.cc
     * hpet in $zx/kernel/platform/pc/hpet.cc
     * apic in $zx/kernel/platform/pc/interrupt.cc
     * system_topology_init in $zx/kernel/arch/x86/system_topology.cc
     * timer in $zx/kernel/platform/pc/timer.cc
     * percpu_heap_init in $zx/kernel/kernel/percpu.cc
     */
    lk_primary_cpu_init_level(LK_INIT_LEVEL_VM, LK_INIT_LEVEL_KERNEL - 1);
    dprintf(INFO, "initializing kernel\n");
    kernel_init();

    // [0x70000, 0x7ffff]
    /*
     * global_prng_thread_safe in $zx/kernel/lib/crypto/global_crypto.cc
     */
    lk_primary_cpu_init_level(LK_INIT_LEVEL_KERNEL, LK_INIT_LEVEL_THREADING - 1);

    // create a thread to complete system initialization
    dprintf(INFO, "creating bootstrap completion thread\n");
    thread_t* t = thread_create("bootstrap2", &bootstrap2, NULL, DEFAULT_PRIORITY);
    thread_detach(t);
    thread_resume(t);

    // become the idle thread and enable interrupts to start the scheduler
    thread_become_idle();
}

static int bootstrap2(void*) {
    dprintf(SPEW, "top of bootstrap2()\n");

    // [0x80000, 0x8ffff]
    /*
     * libobject in $zx/kernel/object/glue.cc
     * dpc in
     */
    lk_primary_cpu_init_level(LK_INIT_LEVEL_THREADING, LK_INIT_LEVEL_ARCH - 1);
    arch_init();

    // initialize the rest of the platform
    dprintf(SPEW, "initializing platform\n");
    // [0x90000, 0x9ffff]
    /*
     * x86_perfmon in $zx/kernel/arch/x86/perf_mon.cc
     */
    lk_primary_cpu_init_level(LK_INIT_LEVEL_ARCH, LK_INIT_LEVEL_PLATFORM - 1);
    platform_init();

    // initialize the target
    dprintf(SPEW, "initializing target\n");
    // [0xa0000, 0xaffff]
    /*
     * x86_pcie_init in $zx/kernel/platform/pc/platform_pcie.cc
     */
    lk_primary_cpu_init_level(LK_INIT_LEVEL_PLATFORM, LK_INIT_LEVEL_TARGET - 1);
    target_init();

    // print all lk_hook we can call
    print_all_lk_init();
    dprintf(SPEW, "moving to last init level\n");
    // [0xb0000, 0xffffffff]
    /*
     * debuglog in $zx/kernel/lib/debuglog/debuglog.cc
     * kcounters in $zx/kernel/lib/counters/counters.cc
     * ktrace in $zx/kernel/lib/ktrace/ktrace.cc
     * kernel_shell in $zx/kernel/lib/console/console.cc
     * userboot in $zx/kernel/lib/userabi/userboot.cc
     */
    lk_primary_cpu_init_level(LK_INIT_LEVEL_TARGET, LK_INIT_LEVEL_LAST);

    return 0;
}

void lk_secondary_cpu_entry() {
    uint cpu = arch_curr_cpu_num();

    if (cpu > secondary_idle_thread_count) {
        dprintf(CRITICAL, "Invalid secondary cpu num %u, SMP_MAX_CPUS %d, secondary_idle_thread_count %u\n",
                cpu, SMP_MAX_CPUS, secondary_idle_thread_count);
        return;
    }

    // secondary cpu initialize from threading level up. 0 to threading was handled in arch
    lk_init_level(LK_INIT_FLAG_SECONDARY_CPUS, LK_INIT_LEVEL_THREADING, LK_INIT_LEVEL_LAST);

    dprintf(SPEW, "entering scheduler on cpu %u\n", cpu);
    thread_secondary_cpu_entry();
}

void lk_init_secondary_cpus(uint secondary_cpu_count) {
    if (secondary_cpu_count >= SMP_MAX_CPUS) {
        dprintf(CRITICAL, "Invalid secondary_cpu_count %u, SMP_MAX_CPUS %d\n",
                secondary_cpu_count, SMP_MAX_CPUS);
        secondary_cpu_count = SMP_MAX_CPUS - 1;
    }
    for (uint i = 0; i < secondary_cpu_count; i++) {
        thread_t* t = thread_create_idle_thread(i + 1);
        if (!t) {
            dprintf(CRITICAL, "could not allocate idle thread %u\n", i + 1);
            secondary_idle_thread_count = i;
            break;
        }
    }
    secondary_idle_thread_count = secondary_cpu_count;
}
