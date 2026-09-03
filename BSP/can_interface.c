#include "can_interface.h"
#include "can_driver.h"
#include "can_pdu.h"
#include "ring_buffer.h"
#include "Siul2_Port_Ip.h"
#include "Siul2_Dio_Ip.h"


/* 注册给 BSP 层的回调函数 */
static void CanIf_RxTxCallback(CanDrv_ChannelType channel, CanDrv_EventType event)
{
	if (event == CAN_EVENT_TX_COMPLETE)
	{
		if(channel == CAN_CH0_TX)
		{
			Siul2_Dio_Ip_WritePin(LED_Red_PORT, LED_Red_PIN, 1);
		}


	}


    if (event == CAN_EVENT_RX_COMPLETE)
    {
    	if (channel == CAN_CH2_RX)
		{
			Siul2_Dio_Ip_WritePin(LED_Yellow_PORT, LED_Yellow_PIN, 1);
		}

    	if(channel == CAN_CH1_RX)
		{
			Siul2_Dio_Ip_WritePin(LED_Green_PORT, LED_Green_PIN, 1);
		}
        Can_Pdu_t pdu;

        /* 直接把 BSP 数据读进 PDU 结构体 */
        if (Can_ReadReceivedData(channel,
                                 &pdu.id,
                                 pdu.data,
                                 &pdu.length,
                                 &pdu.is_fd,
                                 &pdu.is_std,
                                 &pdu.is_remote) == CAN_DRV_OK)
        {
            pdu.channel = channel;

            /* 送入中间件缓冲 */
            RingBuf_Write(&pdu);
        }
    }

}

void CanIf_Init(void)
{
    /* 把回调函数注册给 BSP 层 */
    Can_RegisterCallback(CanIf_RxTxCallback);
}
