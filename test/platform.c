/*
 *  Copyright (C) 2015-2025, Navaro, All Rights Reserved
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  This file is part of CORAL Connect (https://navaro.nl)
 */

#if defined CFG_OS_POSIX
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined _WIN32 && defined CFG_OS_POSIX
#include <winsock2.h>
#endif
#include "qoraal/platform.h"
#include "qoraal/common/rtclib.h"
#include "qoraal/common/dictionary.h"

static uint32_t                     _platform_flash_size = 0 ;
static uint8_t    *                 _platform_flash = 0 ;

int32_t         
platform_init (uint32_t flash_size)
{
    _platform_flash_size = flash_size & ~(PLATFORM_FLASH_SECTOR_SIZE - 1) ;
    return 0 ;
}

int32_t         
platform_start ()
{
#if 0 // defined _WIN32 && defined CFG_OS_POSIX
    // Initialize Winsock if on Windows
    WSADATA wsaData;
    int ret = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (ret != 0) {
        return -1; // Winsock initialization failed
    }
#endif
    os_thread_sleep (100) ;
    if (_platform_flash_size) {
        _platform_flash = (uint8_t*)platform_malloc (QORAAL_HeapAuxiliary, _platform_flash_size) ;
        if (!_platform_flash) return E_NOMEM ;
        platform_flash_erase (0, _platform_flash_size-1) ;
    }
    return 0 ;
}

int32_t         
platform_stop ()
{
    platform_free (QORAAL_HeapAuxiliary, _platform_flash) ;
#if 0 // defined _WIN32 && defined CFG_OS_POSIX
     WSACleanup();
#endif
    return 0 ;
}

void *      
platform_malloc (QORAAL_HEAP heap, size_t size)
{
    return malloc (size) ;
}
void        

platform_free (QORAAL_HEAP heap, void *mem)
{
    free (mem) ;
}

void
platform_print (const char *format)
{
    printf ("%s", format) ;
    fflush(stdout);
}

#ifdef _WIN32

#include <conio.h>
#include <ctype.h>

int32_t
platform_getch(uint32_t timeout_ms)
{
    int c = _getch();

    /* Normalize Enter */
    if (c == '\r')
        c = '\n';

    /* Local echo */
    if (c == '\n') {
        putchar('\n');
    } else if (c == 8 || c == 127) {
        /* Backspace: delete previous char visually */
        fputs("\b \b", stdout);
    } else if (isprint(c)) {
        putchar(c);
    }
    fflush(stdout);

    return c;
}
#else
#include <termios.h>
#include <unistd.h>
#include <ctype.h>

int32_t
platform_getch(uint32_t timeout_ms)
{
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);

    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);  /* raw-ish, no echo */
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    int c = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

    /* Normalize Enter */
    if (c == '\r')
        c = '\n';

    /* Local echo */
    if (c == '\n') {
        putchar('\n');
    } else if (c == 8 || c == 127) {
        fputs("\b \b", stdout);
    } else if (isprint(c)) {
        putchar(c);
    }
    fflush(stdout);

    return c;
}

#endif


void
platform_assert (const char *format)
{
    printf ("%s", format) ;
    abort () ;
}

uint32_t    
platform_current_time (void)
{
    return os_sys_timestamp () / 1000 ;
}

uint32_t 
platform_rand (void)
{
    return rand () ;
}

uint32_t 
platform_wdt_kick (void)
{
    return 20 ;
}

int32_t
platform_flash_erase (uint32_t addr_start, uint32_t addr_end)
{
    if (!_platform_flash) return E_UNEXP ;
    addr_start &= ~(PLATFORM_FLASH_SECTOR_SIZE - 1);
    if (addr_start >= _platform_flash_size)  return E_PARM;
    if (addr_end < addr_start) return E_PARM;
    if (addr_end >= _platform_flash_size) addr_end = _platform_flash_size - 1;
    addr_end |= (PLATFORM_FLASH_SECTOR_SIZE - 1);
    memset((void *)(_platform_flash + addr_start), 0xFF, addr_end - addr_start + 1);
    return EOK;
}

int32_t
platform_flash_write (uint32_t addr, uint32_t len, const uint8_t * data)
{
    if (!_platform_flash) return E_UNEXP ;
    if (addr >= _platform_flash_size) return E_PARM ;
    if (addr + len >= _platform_flash_size) return E_PARM ;
    for (uint32_t i=0; i<len; i++) {
        _platform_flash[i+addr] &= data[i] ;
    }
    return EOK ;
}

int32_t
platform_flash_read (uint32_t addr, uint32_t len, uint8_t * data)
{
    if (!_platform_flash) return E_UNEXP ;
    if (addr >= _platform_flash_size) return E_PARM ;
    if (addr + len >= _platform_flash_size) return E_PARM ;
    memcpy (data, (void*)(_platform_flash + addr), len) ;
    return EOK ;
}

#elif defined CFG_OS_ZEPHYR

#include <zephyr/kernel.h>
//#include <zephyr/random/rand32.h>
#include <zephyr/sys/printk.h>

#include "platform.h"
#include <stdlib.h>

extern size_t console_write(const uint8_t *data, size_t len, uint32_t timeout_ms) ;
extern int32_t console_get_char(uint32_t timeout_ms) ;

static uint32_t _platform_flash_size;

int32_t
platform_init(uint32_t flash_size)
{
    _platform_flash_size = flash_size;
    return 0;
}

int32_t
platform_start(void)
{
    ARG_UNUSED(_platform_flash_size);
    return 0;
}

int32_t
platform_stop(void)
{
    return 0;
}

void *
platform_malloc(QORAAL_HEAP heap, size_t size)
{
    ARG_UNUSED(heap);
    return malloc(size);
}

void
platform_free(QORAAL_HEAP heap, void *mem)
{
    ARG_UNUSED(heap);
    free(mem);
}

void
platform_print(const char *format)
{
    console_write (format, strlen(format), 1000) ;
   // printk("%s", format);
}

int32_t 
platform_getch (uint32_t timeout_ms)
{
    return console_get_char(timeout_ms);
}

void
platform_assert(const char *format)
{
    printk("%s", format);
    k_oops();
}

uint32_t
platform_current_time(void)
{
    return k_uptime_get_32() / 1000U;
}

uint32_t
platform_rand(void)
{
    return rand();
}

uint32_t
platform_wdt_kick(void)
{
    return 20U;
}

int32_t
platform_flash_erase(uint32_t addr_start, uint32_t addr_end)
{
    ARG_UNUSED(addr_start);
    ARG_UNUSED(addr_end);
    return E_NOIMPL;
}

int32_t
platform_flash_write(uint32_t addr, uint32_t len, const uint8_t *data)
{
    ARG_UNUSED(addr);
    ARG_UNUSED(len);
    ARG_UNUSED(data);
    return E_NOIMPL;
}

int32_t
platform_flash_read(uint32_t addr, uint32_t len, uint8_t *data)
{
    ARG_UNUSED(addr);
    ARG_UNUSED(len);
    ARG_UNUSED(data);
    return E_NOIMPL;
}

#endif /* CFG_OS_POSIX || CFG_OS_ZEPHYR */
