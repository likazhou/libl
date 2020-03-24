#ifndef __SFS_H__
#define __SFS_H__


#ifdef __cplusplus
extern "C" {
#endif



//Public Defines
#if SFS_RECORD_LEN == 4
#define sfs_id_t		u32
#elif SFS_RECORD_LEN == 8
#define sfs_id_t		u64
#else
#error "SFS_RECORD_LEN must be 4 or 8!!!"
#endif



//Public Typedefs
typedef flash_dev_t		sfs_t;




//Externel Functions
void sfs_SystemInit(void);
sys_res sfs_Init(sfs_t *p);
sys_res sfs_Write(sfs_t *p, sfs_id_t id, const void *data, size_t len);
int sfs_Read(sfs_t *p, sfs_id_t id, void *data, size_t len);
int sfs_Find(sfs_t *p, sfs_id_t from, sfs_id_t to, void *data, size_t len);
sys_res sfs_Delete(sfs_t *p, sfs_id_t id);


#ifdef __cplusplus
}
#endif

#endif


