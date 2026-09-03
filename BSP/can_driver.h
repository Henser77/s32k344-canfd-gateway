#ifndef CAN_DRIVER_H
#define CAN_DRIVER_H


#include <stdint.h>
#include <stdbool.h>


/* 逻辑通道定义（隐藏硬件实例和邮箱） */
typedef enum
{
    CAN_CH0_TX = 0,
    CAN_CH0_RX,
    CAN_CH1_TX,
    CAN_CH1_RX,
    CAN_CH2_TX,
    CAN_CH2_RX,
    CAN_CH_MAX
} CanDrv_ChannelType;

/* BSP 事件类型 */
typedef enum
{
    CAN_EVENT_TX_COMPLETE = 0,
    CAN_EVENT_RX_COMPLETE,
    CAN_EVENT_ERROR
} CanDrv_EventType;

/* 状态码 */
typedef enum
{
	CAN_DRV_OK = 0,
	CAN_DRV_ERR_INIT_FAILED,
	CAN_DRV_ERR_NOT_INIT,
	CAN_DRV_ERR_START_FAILED,
	CAN_DRV_ERR_DEINIT_FAILED,
	CAN_DRV_ERR_PARAM,
	CAN_DRV_ERR_SEND_FAILED,
	CAN_DRV_ERR_CONFIG_FAILED,
	CAN_DRV_ERR_START_RECEIVE_FAILED,
	CAN_DRV_ERR_RESTART_RECEIVE_FAILED
}CanDrv_StatusType;



//定义了一个叫CanDrv_Callback的函数指针类型
typedef void (*CanDrv_Callback)(CanDrv_ChannelType channel, CanDrv_EventType event);


CanDrv_StatusType Can_Init(void);
CanDrv_StatusType Can_Deinit(void);
CanDrv_StatusType Can_Send(CanDrv_ChannelType channel, uint32_t msgId,
                           const uint8_t *data, uint8_t length, bool is_fd);
CanDrv_StatusType Can_RegisterCallback(CanDrv_Callback callback);
CanDrv_StatusType Can_StartReceive(CanDrv_ChannelType channel, uint32_t msgId,
		uint32_t msgIdMask, uint8_t dataLength, bool is_fd);
CanDrv_StatusType Can_ReadReceivedData(CanDrv_ChannelType channel,
                                       uint32_t *msgId, uint8_t *data,
                                       uint8_t *length, bool *is_fd,
									   bool *is_std, bool *is_remote);

#endif
