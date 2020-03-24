#include <fs/romfs/dfs_romfs.h>

const struct romfs_dirent _root_dirent[] = {
	{ROMFS_DIRENT_DIR, "sf0", NULL, 0},
	{ROMFS_DIRENT_DIR, "um0", NULL, 0},
	{ROMFS_DIRENT_DIR, "nf0", NULL, 0},
};

const struct romfs_dirent romfs_root = {ROMFS_DIRENT_DIR, "/", (const rt_uint8_t *)_root_dirent, sizeof(_root_dirent)/sizeof(_root_dirent[0])};

