#ifndef CAN_PDU_H
#define CAN_PDU_H

#include <stdint.h>
#include <stdbool.h>

#define CAN_PDU_MAX_DATA_LEN	64U

/* 统一报文结构体 */
typedef struct
{
	uint8_t channel;						//来自哪个通道
	bool is_std;							//是否为标准帧
	uint32_t id;							//报文ID
	bool is_remote;							//是否为远程帧
	uint8_t data[CAN_PDU_MAX_DATA_LEN];		//报文数据
	uint8_t length;							//数据长度
	bool is_fd;								//是否为CANFD报文
}Can_Pdu_t;



#endif
