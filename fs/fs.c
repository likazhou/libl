#if FS_ENABLE

#include <fs/dfs.c>
#include <fs/dfs_fs.c>
#include <fs/dfs_file.c>
#include <fs/dfs_posix.c>
#include <fs/fs_posix.c>

#if ROMFS_ENABLE
#include <fs/romfs/romfs.c>
#include <fs/romfs/dfs_romfs.c>
#endif

#if NFS_ENABLE
#include <fs/dfs_nfs.c>
#include <fs/nfs/mount_clnt.c>
#include <fs/nfs/mount_xdr.c>
#include <fs/nfs/nfs_clnt.c>
#include <fs/nfs/nfs_xdr.c>
#endif


#include <fs/elmfat/ff.c>
#include <fs/dfs_elm.c>

#if NANDFS_ENABLE
#include <fs/nftl/fil_bad.c>
#include <fs/nftl/fil_wear.c>
#include <fs/nftl/llayer.c>
#include <fs/nftl/mlayer.c>
#include <fs/dfs_nftl.c>
#endif

#if SPIFS_ENABLE
#include <fs/dfs_spif.c>
#endif

#if USBMSC_ENABLE
#include <fs/dfs_usbmsc.c>
#endif


//Private Defines
#define FS_LOCK_ENABLE		0


//Private Variables
#if FS_LOCK_ENABLE
static os_sem_t fs_sem;
#endif


//Private Macros
#if FS_LOCK_ENABLE
#define fs_Lock()					os_sem_wait(&fs_sem)
#define fs_Unlock()					os_sem_signal(&fs_sem)
#else
#define fs_Lock()
#define fs_Unlock()
#endif



//External Functions
#if USBMSC_ENABLE
sys_res fs_usb_Mount()
{

	/* mount usb mass storage fat partition 0 as um0 directory */
	if (dfs_mount("usbmsc0", FS_USBMSC_PATH, "elm", 0, 0))
	{
		rt_kprintf("USB Mass Storage mount failed!\n");
		return SYS_R_ERR;
	}
	return SYS_R_OK;
}

sys_res fs_usb_Unmount()
{

	/* unmount usb mass storage fat partition 0 as um0 directory */
	if (dfs_unmount(FS_USBMSC_PATH))
	{
		rt_kprintf("USB Mass Storage unmount failed!\n");
		return SYS_R_ERR;
	}
	return SYS_R_OK;
}

int fs_usb_IsReady()
{

	rt_device_t dev_id;

	dev_id = rt_device_find("usbmsc0");
	
	if (dev_id == NULL)
		return 0;
	if (usbmsc_isready(dev_id) != RT_EOK)
		return 0;
	
	return 1;
}
#endif


sys_res fs_init()
{
	
#if FS_LOCK_ENABLE
	os_sem_init(&fs_sem, 1);
#endif

	/* init the device filesystem */
	dfs_init();

#if ROMFS_ENABLE
	/* init the rom filesystam*/
	dfs_romfs_init();
	/* mount romfs as root directory */
	if (dfs_mount(RT_NULL, "/", "rom", 0, &romfs_root))
	{
		rt_kprintf("Rom FileSystem mount failed!\n");
		return SYS_R_ERR;
	}
#endif

#if NFS_ENABLE
	/* init the net file system */
	nfs_init();
#endif

	/* init the elm chan FatFs filesystam*/
	elm_init();

#if NANDFS_ENABLE
	/* init the nftl layer */
	nftl_dev_Init();
	/* mount nand fat partition 0 as nf0 directory */
	if (dfs_mount("nftl0", FS_NAND_PATH, "elm", 0, 0))
	{
		rt_kprintf("Nand FileSystem mount failed!\n");
		return SYS_R_ERR;
	}
#endif

#if SPIFS_ENABLE
	/* init the spiflash layer */
	spif_dev_Init();
	/* mount spiflash fat partition 0 as sf0 directory */
	if (dfs_mount("spif0", FS_SPIF_PATH, "elm", 0, 0))
	{
		rt_kprintf("SpiFlash FileSystem mount failed!\n");
		return SYS_R_ERR;
	}
#endif

#if USBMSC_ENABLE
	/* init the USB Mass Storage layer */
	usbmsc_dev_Init();
	fs_usb_Mount();
#endif

	return SYS_R_OK;
}




#endif

