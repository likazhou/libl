#ifndef __I2CBUS_H__
#define __I2CBUS_H__


#ifdef __cplusplus
extern "C" {
#endif








//External Functions
sys_res i2cbus_Init(i2c_t *p);
sys_res i2cbus_Write(i2c_t *p, int nDev, const void *pData, size_t nLen);
sys_res i2cbus_Read(i2c_t *p, int nDev, void *pData, size_t nLen);


#ifdef __cplusplus
}
#endif

#endif

