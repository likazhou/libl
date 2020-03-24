#ifndef __LIBL_H__
#define __LIBL_H__


#ifdef __cplusplus
extern "C" {
#endif



#if defined(__CC_ARM)   /* ARMCC compiler */
#define PACK_STRUCT_FIELD(x) x
#define PACK_STRUCT_STRUCT __attribute__ ((__packed__))
#define PACK_STRUCT_BEGIN
#define PACK_STRUCT_END
#define WEAK __weak
#define inline __inline
#elif defined(__IAR_SYSTEMS_ICC__)   /* IAR Compiler */
#define PACK_STRUCT_BEGIN
#define PACK_STRUCT_STRUCT
#define PACK_STRUCT_END
#define PACK_STRUCT_FIELD(x) x
#define PACK_STRUCT_USE_INCLUDES
#define WEAK __weak
#elif defined(__GNUC__)     /* GNU GCC Compiler */
#define PACK_STRUCT_FIELD(x) x
#define PACK_STRUCT_STRUCT __attribute__((packed))
#define PACK_STRUCT_BEGIN
#define PACK_STRUCT_END
#define WEAK __attribute__((weak))
#elif defined(_MSC_VER)
#define PACK_STRUCT_FIELD(x) x
#define PACK_STRUCT_STRUCT
#define PACK_STRUCT_BEGIN
#define PACK_STRUCT_END
#define WEAK
#endif

	
//Header Includes
#include <def.h>

#include "bsp_cfg.h"

#include <lib/lib.h>

#include <lib/memory.h>
#if LIB_ZIP_ENABLE
#include <lib/zip/cceman.h>
#endif

#include <lib/buffer.h>

#include <lib/dqueue.h>

#include <mtd/flash.h>

#include <fs/bkp/bkp.h>

#if OS_TYPE
#include <fs/fs.h>

#include <chl/chl.h>

#include <cp/cp.h>

#include <hi/gui.h>

#include <dbg/dbg.h>
#endif

#if USB_ENABLE
#include <usb/usb.h>
#endif

#if TCPPS_ENABLE
#include <net/net.h>
#endif


//Warning
#if WDG_ENABLE == 0
#warning "WatchDog is disabled........................."
#endif

//External Macros
#if WDG_ENABLE == 0
#define wdg_Reload(...)
#endif


#ifdef __cplusplus
}
#endif


#endif

