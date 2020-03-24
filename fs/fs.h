#ifndef __FS_H__
#define __FS_H__

#ifdef __cplusplus
extern "C" {
#endif

//Include Headers
#if SFS_ENABLE
#include <fs/sfs/sfs.h>
#endif
#if FS_ENABLE
#include <fs/fs_posix.h>
#endif

//Public Defines




//External Functions
sys_res fs_init(void);
sys_res fs_usb_Mount(void);
sys_res fs_usb_Unmount(void);
int fs_usb_IsReady(void);




#ifdef __cplusplus
}
#endif

#endif

