// Copyright 2016 The Fuchsia Authors
// Copyright (c) 2014-2015 Travis Geiselbrecht
//
// Use of this source code is governed by a MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT

#include <arch/arm64/periphmap.h>
#include <dev/interrupt.h>
#include <dev/uart.h>
#include <kernel/thread.h>
#include <lib/cbuf.h>
#include <lib/debuglog.h>
#include <pdev/driver.h>
#include <pdev/uart.h>
#include <platform/debug.h>
#include <reg.h>
#include <stdio.h>
#include <trace.h>
#include <zircon/boot/driver-config.h>

// values read from zbi
static vaddr_t uart_base;
#define AUX_MU_IO_REG   (uart_base+0x40)
#define AUX_MU_LSR_REG  (uart_base+0x54)

static void mini_uart_putc(char c){
    while(1) {
		if(*((volatile uint32_t*)AUX_MU_LSR_REG)&0x20) 
			break;
	}
	*((volatile uint32_t*)AUX_MU_IO_REG) = (uint32_t)c;
}

static int mini_uart_getc(bool wait){
    char c;
    while(1) {
		if(*((volatile uint32_t*)AUX_MU_LSR_REG)&0x01) 
			break;
	}
    c = *((volatile uint32_t*)AUX_MU_IO_REG)&0xFF;
	return c;
}

/* panic-time getc/putc */
static int mini_uart_pputc(char c){
    mini_uart_putc(c);
    return 1;
}

static int mini_uart_pgetc(){
    return mini_uart_getc(false);
}

static void mini_uart_dputs(const char* str, size_t len,
                        bool block, bool map_NL){
    while (len > 0){
        if(*str == '\n' && map_NL){
            mini_uart_putc('\r');
            mini_uart_putc('\n');
        }else{
            mini_uart_putc(*str);
        }
        str++;
        len--;
    }
}

const char* panic_start = "PANIC! PANPAN!\n";

static void mini_start_panic() {
    mini_uart_dputs(panic_start, 16, false, false);
}

static const struct pdev_uart_ops uart_ops = {
    .getc = mini_uart_getc,
    .pputc = mini_uart_pputc,
    .pgetc = mini_uart_pgetc,
    .start_panic = mini_start_panic,
    .dputs = mini_uart_dputs,
};

const char* ss = "hello world i am mini uart";

static void mini_uart_init_early(const void* driver_data, uint32_t length) {
    ASSERT(length >= sizeof(dcfg_simple_t));
    auto driver = static_cast<const dcfg_simple_t*>(driver_data);
    ASSERT(driver->mmio_phys && driver->irq);

    uart_base = periph_paddr_to_vaddr(driver->mmio_phys);
    ASSERT(uart_base);

    pdev_register_uart(&uart_ops);

    mini_uart_dputs(ss, 27, false, false);
    mini_uart_putc('\n');
    for(int i = 1; i <= 16; i++){
        char temp = (uart_base >> (64 - i * 4)) & 0xF;
        if(temp < 10){
            mini_uart_putc(temp + '0');
        }else{
            mini_uart_putc(temp - 10 + 'A');
        }
    }
    mini_uart_putc('\n');
}


LK_PDEV_INIT(mini_uart_init_early, KDRV_MINI_UART, mini_uart_init_early, LK_INIT_LEVEL_PLATFORM_EARLY)
