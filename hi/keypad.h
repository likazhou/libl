#ifndef __KEYPAD_H__
#define __KEYPAD_H__



#ifdef __cplusplus
extern "C" {
#endif



//Public Defines
#define KEY_T_DIR			0	//直接接入
#define KEY_T_ARR			1	//阵列扫描







//External Functions
void key_Init(void);
int key_Read(void);


#ifdef __cplusplus
}
#endif

#endif

