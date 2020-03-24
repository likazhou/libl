#ifndef __GDRCP_H__
#define __GDRCP_H__


//传送方向定义
#define GDRCP_CDIR_RECV					0x00	//主站发出
#define GDRCP_CDIR_SEND					0x01	//终端发出


//错误编码
#define GDRCP_ERR_NO_ERROR				0x00	//正确,无错误
#define GDRCP_ERR_RELAY_FALE			0x01	//中继命令没有返回
#define GDRCP_ERR_INVALID				0x02	//设置内容非法
#define GDRCP_ERR_NO_PERMIT				0x03	//密码权限不足
#define GDRCP_ERR_NO_DATA				0x04	//无此项数据
#define GDRCP_ERR_TIME_INVALID			0x05	//命令时间失效
#define GDRCP_ERR_NO_ADDR				0x11	//目标地址不存在
#define GDRCP_ERR_SEND_FAIL				0x12	//发送失败
#define GDRCP_ERR_SMS_LONG				0x13	//短消息帧太长

#define GDRCP_DATA_INVALID				0xFFFFFFFF

#endif

