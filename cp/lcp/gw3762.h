#ifndef __GW3762_H__
#define __GW3762_H__


#ifdef __cplusplus
extern "C" {
#endif

//Public Defines

//GW376.2 AFN Defines
#define GW3762_AFN_CONFIRM			0x00
#define GW3762_AFN_RESET			0x01
#define GW3762_AFN_TRANSMIT			0x02
#define GW3762_AFN_DATA_FETCH		0x03
#define GW3762_AFN_DATA_SET			0x05
#define GW3762_AFN_REPORT			0x06
#define GW3762_AFN_ROUTE_FETCH		0x10
#define GW3762_AFN_ROUTE_SET		0x11
#define GW3762_AFN_ROUTE_CTRL		0x12
#define GW3762_AFN_ROUTE_TRANSMIT	0x13
#define GW3762_AFN_ROUTE_REQUEST	0x14
#define GW3762_AFN_AUTOREPORT		0xF0





//Public Typedefs





//External Functions
sys_res gw3762_Analyze(plc_t *p);

sys_res gw3762_Broadcast(plc_t *p, const void *pAdr, const void *pData, size_t nLen);
sys_res gw3762_MeterRead(plc_t *p, const void *pAdr, int nRelay, const void *pRtAdr, const void *pData, size_t nLen);
sys_res gw3762_MeterRT(plc_t *p, const void *pAdr, const void *pData, size_t nLen);

sys_res gw3762_Confirm(plc_t *p, int nFlag, size_t nTmo);
sys_res gw3762_HwReset(plc_t *p);
sys_res gw3762_ParaReset(plc_t *p);
sys_res gw3762_InfoGet(plc_t *p, int nRetry);
sys_res gw3762_ModAdrSet(plc_t *p);
sys_res gw3762_SubAdrQty(plc_t *p, u16 *pQty);
sys_res gw3762_SubAdrRead(plc_t *p, int nSn, u16 *pQty, u8 *pAdr);
sys_res gw3762_StateGet(plc_t *p, int nRetry);
sys_res gw3762_SubAdrAdd(plc_t *p, int nSn, const void *pAdr, int nPrtl);
sys_res gw3762_SubAdrDelete(plc_t *p, const void *pAdr);
sys_res gw3762_ModeSet(plc_t *p, int nMode);
sys_res gw3762_MeterProbe(plc_t *p, int nTime);
sys_res gw3762_RtCtrl(plc_t *p, u16 nDT, int nRetry);
sys_res gw3762_RequestAnswer(plc_t *p, int nPhase, const void *pAdr, int nIsRead, const void *pData, size_t nLen);

sys_res gw3762_Transmit(plc_t *p, buf b, const void *pData, size_t nLen);

sys_res gw3762_Es_ModeGet(plc_t *p, u8 *pMode);
sys_res gw3762_Es_ModeSet(plc_t *p, int nMode);


#ifdef __cplusplus
}
#endif


#endif

