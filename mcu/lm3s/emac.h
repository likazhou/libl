#ifndef __LM3SETHIF_H__
#define __LM3SETHIF_H__



#ifdef __cplusplus
extern "C" {
#endif



#define NIOCTL_GADDR		0x01
#define ETHERNET_MTU		1500


//External Functions
rt_err_t eth_device_ready(struct eth_device* dev);
int eth_device_init(struct eth_device* dev, char* name);
void eth_system_device_init(void);


#ifdef __cplusplus
}
#endif


#endif

