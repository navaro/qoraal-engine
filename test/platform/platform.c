#include <stdio.h>
#include <stdlib.h>
#include "platform.h"
#include "qoraal/common/rtclib.h"
#include "qoraal/common/dictionary.h"
#include "qoraal-flash/nvram/nvol3.h"
#include "qoraal-flash/registry.h"

#define PLATFORM_FLASH_SIZE     (1024*1024*10)

void *      platform_malloc (QORAAL_HEAP heap, size_t size) ;
void        platform_free (QORAAL_HEAP heap, void *mem) ;
void        platform_print (const char *format) ;
void        platform_assert (const char *format) ;
uint32_t    platform_wdt_kick (void) ;
uint32_t    platform_current_time (void) ;

void        platform_logger_cb (void* channel, LOGGERT_TYPE_T type, uint8_t facility, const char* msg) ;

int32_t     platform_flash_read (uint32_t addr, uint32_t len, uint8_t * data) ;
int32_t     platform_flash_write (uint32_t addr, uint32_t len, const uint8_t * data) ;
int32_t     platform_flash_erase (uint32_t addr_start, uint32_t addr_end) ;

static uint8_t        _platform_flash[PLATFORM_FLASH_SIZE]  ;


REGISTRY_INSTANCE_DECL(registry_cfg, \
        0, 
        (64*1024), 
        24, 
        128, 
        101)

static const QORAAL_CFG_T       qoraal_cfg = { .malloc = platform_malloc, .free = platform_free, .debug_print = platform_print, .debug_assert = platform_assert, .current_time = platform_current_time, .wdt_kick = platform_wdt_kick};
static const QORAAL_FLASH_CFG_T qoraal_flash_cfg = { .flash_read = platform_flash_read, .flash_write = platform_flash_write, .flash_erase = platform_flash_erase};
static LOGGER_CHANNEL_T         log_channel = { .fp = platform_logger_cb, .user = (void*)0, .filter = { { .mask = SVC_LOGGER_MASK, .type = SVC_LOGGER_SEVERITY_LOG | SVC_LOGGER_FLAGS_PROGRESS }, {0,0} } };


int32_t         
platform_init ()
{
    return 0 ;
}

int32_t         
platform_start ()
{
    os_thread_sleep (100) ;

    qoraal_instance_init (&qoraal_cfg) ;
    qoraal_flash_instance_init (&qoraal_flash_cfg) ;
    qoraal_svc_init (0) ;
    os_sys_start () ;
    qoraal_svc_start () ;

    svc_logger_channel_add (&log_channel) ;
    platform_flash_erase (0, PLATFORM_FLASH_SIZE-1) ;
    registry_init () ;
    registry_start (&registry_cfg) ;

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
}

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
platform_wdt_kick (void)
{
    return 20 ;
}

void
platform_logger_cb (void* channel, LOGGERT_TYPE_T type, uint8_t facility, const char* msg)
{
    printf("--- %s\n", msg) ;
}

int32_t
platform_flash_erase (uint32_t addr_start, uint32_t addr_end)
{
    if (addr_end < addr_start) return E_PARM ;
    if (addr_start >= PLATFORM_FLASH_SIZE) return E_PARM ;
    if (addr_end >= PLATFORM_FLASH_SIZE) {
        addr_end = PLATFORM_FLASH_SIZE - 1 ;
    }
    memset ((void*)(_platform_flash + addr_start), 0xFF, addr_end - addr_start) ;

    return EOK ;
}

int32_t
platform_flash_write (uint32_t addr, uint32_t len, const uint8_t * data)
{
    uint32_t i ;
    if (addr >= PLATFORM_FLASH_SIZE) return E_PARM ;
    if (addr + len >= PLATFORM_FLASH_SIZE) return E_PARM ;

    for (i=0; i<len; i++) {
        _platform_flash[i+addr] &= data[i] ;
    }


    // memcpy ((void*)(_nvram_test + addr), data, len) ;

    return EOK ;
}

int32_t
platform_flash_read (uint32_t addr, uint32_t len, uint8_t * data)
{
    if (addr >= PLATFORM_FLASH_SIZE) return E_PARM ;
    if (addr + len >= PLATFORM_FLASH_SIZE) return E_PARM ;

    memcpy (data, (void*)(_platform_flash + addr), len) ;

    return EOK ;
}



