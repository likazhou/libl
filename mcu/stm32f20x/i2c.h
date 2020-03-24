#ifndef __ARCH_I2C_H__
#define __ARCH_I2C_H__


#ifdef __cplusplus
extern "C" {
#endif













//External Functions
sys_res arch_I2cInit(i2c_t *p);
void arch_I2cIoHandler(i2c_t *p);
void arch_I2cErrorHandler(i2c_t *p);


#ifdef __cplusplus
}
#endif

#endif

