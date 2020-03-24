#ifndef __PCA955X_H__
#define __PCA955X_H__




#ifdef __cplusplus
extern "C" {
#endif










//External Functions
sys_res pca955x_Init(i2c_t *p);
int pca955x_GpioRead(i2c_t *p, int n);
void pca955x_GpioSet(i2c_t *p, int nPin, int nHL);



#ifdef __cplusplus
}
#endif

#endif

