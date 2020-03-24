#ifndef __FS_POSIX_H__
#define __FS_POSIX_H__

#ifdef __cplusplus
extern "C" {
#endif

//Include Headers
#include <fs/dfs_posix.h>



//External Functions
int fs_open(const char *file, int flags, int mode);
int fs_close(int fd);
int fs_read(int fd, void *buf, int len);
int fs_write(int fd, const void *buf, int len);
int fs_lseek(int fd, int offset, int dir);

int fs_unlink(const char *pathname);

int fs_stat(const char *file, struct stat *buf);

int fs_mkdir(const char *path, rt_uint16_t mode);
int fs_rmdir(const char *pathname);
DIR_POSIX* fs_opendir(const char* name);
struct dirent* fs_readdir(DIR_POSIX *d);
rt_off_t fs_telldir(DIR_POSIX *d);
void fs_seekdir(DIR_POSIX *d, rt_off_t offset);
void fs_rewinddir(DIR_POSIX *d);
int fs_closedir(DIR_POSIX* d);










#ifdef __cplusplus
}
#endif


#endif

