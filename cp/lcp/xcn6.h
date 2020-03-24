#ifndef __LCP_XCN6_H__
#define __LCP_XCN6_H__

#ifdef __cplusplus
extern "C" {
#endif










//External Functions
sys_res xcn12_Meter(plc_t *p, buf b, int nCode, const void *pAdr, int nRelay, const void *pRtAdr, 
							const void *pData, size_t nLen);
sys_res xcn12_Broadcast(plc_t *p, const void *pAdr, const void *pData, size_t nLen);

sys_res xcn6_MeterRead(plc_t *p, buf b, const void *pAdr, int nRelay, const void *pRtAdr, 
								const void *pData, size_t nLen);
sys_res xcn6_MeterWrite(plc_t *p, buf b, const void *pAdr, int nRelay, const void *pRtAdr, 
								const void *pData, size_t nLen);
sys_res xcn6_Broadcast(plc_t *p, const void *pAdr, const void *pData, size_t nLen);


#ifdef __cplusplus
}
#endif

#endif


