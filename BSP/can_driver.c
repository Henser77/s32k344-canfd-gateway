#include "can_driver.h"
#include "FlexCAN_Ip.h"

typedef struct
{
    uint8_t instance;      /* FlexCAN 硬件实例号，如 INST_FLEXCAN_0 */
    uint32_t mbIdx;        /* 邮箱号，如 0, 1, 2... */
    bool is_tx;            /* true 表示这是发送通道，false 表示接收通道 */
} CanDrv_ChannelMapEntry;

//逻辑通道映射表
static const CanDrv_ChannelMapEntry channel_map[CAN_CH_MAX] =
{
    [CAN_CH0_TX]  = { .instance = INST_FLEXCAN_0, .mbIdx = 0, .is_tx = true  },
    [CAN_CH0_RX0] = { .instance = INST_FLEXCAN_0, .mbIdx = 1, .is_tx = false },
    [CAN_CH0_RX1] = { .instance = INST_FLEXCAN_0, .mbIdx = 2, .is_tx = false },
    [CAN_CH0_RX2] = { .instance = INST_FLEXCAN_0, .mbIdx = 3, .is_tx = false },

    [CAN_CH1_TX]  = { .instance = INST_FLEXCAN_1, .mbIdx = 0, .is_tx = true  },
    [CAN_CH1_RX0] = { .instance = INST_FLEXCAN_1, .mbIdx = 1, .is_tx = false },
    [CAN_CH1_RX1] = { .instance = INST_FLEXCAN_1, .mbIdx = 2, .is_tx = false },
    [CAN_CH1_RX2] = { .instance = INST_FLEXCAN_1, .mbIdx = 3, .is_tx = false },

    [CAN_CH2_TX]  = { .instance = INST_FLEXCAN_2, .mbIdx = 0, .is_tx = true  },
    [CAN_CH2_RX0] = { .instance = INST_FLEXCAN_2, .mbIdx = 1, .is_tx = false },
    [CAN_CH2_RX1] = { .instance = INST_FLEXCAN_2, .mbIdx = 2, .is_tx = false },
    [CAN_CH2_RX2] = { .instance = INST_FLEXCAN_2, .mbIdx = 3, .is_tx = false },
    [CAN_CH2_RX3] = { .instance = INST_FLEXCAN_2, .mbIdx = 4, .is_tx = false },
};

static const CanDrv_ChannelCfgType channel_cfg_table[CAN_CH_MAX] =
{
    [CAN_CH0_TX]  = { .is_fd = true,  .max_data_len = 64u },
    [CAN_CH0_RX0] = { .is_fd = true,  .max_data_len = 64u },
    [CAN_CH0_RX1] = { .is_fd = true,  .max_data_len = 64u },
    [CAN_CH0_RX2] = { .is_fd = true,  .max_data_len = 64u },

    [CAN_CH1_TX]  = { .is_fd = true,  .max_data_len = 64u },
    [CAN_CH1_RX0] = { .is_fd = true,  .max_data_len = 64u },
    [CAN_CH1_RX1] = { .is_fd = true,  .max_data_len = 64u },
    [CAN_CH1_RX2] = { .is_fd = true,  .max_data_len = 64u },

    [CAN_CH2_TX]  = { .is_fd = false, .max_data_len = 8u  },
    [CAN_CH2_RX0] = { .is_fd = false, .max_data_len = 8u  },
    [CAN_CH2_RX1] = { .is_fd = false, .max_data_len = 8u  },
    [CAN_CH2_RX2] = { .is_fd = false, .max_data_len = 8u  },
    [CAN_CH2_RX3] = { .is_fd = false, .max_data_len = 8u  },
};

static Flexcan_Ip_MsgBuffType rx_buffers[CAN_CH_MAX];  /* 每个通道一个，发送通道不用 */

static CanDrv_Callback user_callback = NULL;
static CanDrv_ErrorCallback user_error_callback = NULL;

static bool is_init = false;

CanDrv_StatusType Can_Init(void)
{
	Flexcan_Ip_StatusType flexcan_status;

	if(is_init)return CAN_DRV_OK;

	flexcan_status = FlexCAN_Ip_Init(INST_FLEXCAN_0, &FlexCAN_State0, &FlexCAN_Config0);
	if(flexcan_status != FLEXCAN_STATUS_SUCCESS)return CAN_DRV_ERR_INIT_FAILED;
	flexcan_status = FlexCAN_Ip_Init(INST_FLEXCAN_1, &FlexCAN_State1, &FlexCAN_Config1);
	if(flexcan_status != FLEXCAN_STATUS_SUCCESS)return CAN_DRV_ERR_INIT_FAILED;
	flexcan_status = FlexCAN_Ip_Init(INST_FLEXCAN_2, &FlexCAN_State2, &FlexCAN_Config2);
	if(flexcan_status != FLEXCAN_STATUS_SUCCESS)return CAN_DRV_ERR_INIT_FAILED;

	flexcan_status = FlexCAN_Ip_SetStartMode(INST_FLEXCAN_0);
	if(flexcan_status != FLEXCAN_STATUS_SUCCESS)return CAN_DRV_ERR_START_FAILED;
	flexcan_status = FlexCAN_Ip_SetStartMode(INST_FLEXCAN_1);
	if(flexcan_status != FLEXCAN_STATUS_SUCCESS)return CAN_DRV_ERR_START_FAILED;
	flexcan_status = FlexCAN_Ip_SetStartMode(INST_FLEXCAN_2);
	if(flexcan_status != FLEXCAN_STATUS_SUCCESS)return CAN_DRV_ERR_START_FAILED;

	is_init = true;
	return CAN_DRV_OK;
}


CanDrv_StatusType Can_Deinit(void)
{
	Flexcan_Ip_StatusType flexcan_status;

	if(!is_init)return CAN_DRV_ERR_NOT_INIT;

	flexcan_status = FlexCAN_Ip_Deinit(INST_FLEXCAN_0);
	if(flexcan_status != FLEXCAN_STATUS_SUCCESS)return CAN_DRV_ERR_DEINIT_FAILED;
	flexcan_status = FlexCAN_Ip_Deinit(INST_FLEXCAN_1);
	if(flexcan_status != FLEXCAN_STATUS_SUCCESS)return CAN_DRV_ERR_DEINIT_FAILED;
	flexcan_status = FlexCAN_Ip_Deinit(INST_FLEXCAN_2);
	if(flexcan_status != FLEXCAN_STATUS_SUCCESS)return CAN_DRV_ERR_DEINIT_FAILED;

	is_init = false;
	return CAN_DRV_OK;
}

CanDrv_ChannelCfgType CanDrv_GetChannelCfg(CanDrv_ChannelType channel)
{
    CanDrv_ChannelCfgType cfg;

    if (channel < CAN_CH_MAX)
    {
        cfg = channel_cfg_table[channel];
    }
    else
    {
        cfg.is_fd = false;
        cfg.max_data_len = 8u;
    }

    return cfg;
}

CanDrv_StatusType Can_Send(CanDrv_ChannelType channel, uint32_t msgId,
		const uint8_t *data, uint8_t length, bool is_fd)
{
	Flexcan_Ip_StatusType flexcan_status;

	if (!is_init)return CAN_DRV_ERR_NOT_INIT;
	if (channel >= CAN_CH_MAX)return CAN_DRV_ERR_PARAM;
	if (data == NULL) return CAN_DRV_ERR_PARAM;
	if ((!is_fd && length > 8) || (is_fd && length > 64))return CAN_DRV_ERR_PARAM;

	const CanDrv_ChannelMapEntry *entry = &channel_map[channel];
	if (!entry->is_tx) return CAN_DRV_ERR_PARAM;   // 不是发送通道

	Flexcan_Ip_DataInfoType	tx_info = {
		.msg_id_type = FLEXCAN_MSG_ID_STD,  	// 标准帧
		.data_length = length,                  // 必须明确指定长度
		.is_polling  = FALSE,                 	// 中断模式
		.is_remote   = FALSE,					// 数据帧
		.fd_enable   = is_fd,                 	// 想用FD就显式改为TRUE
		.enable_brs  = is_fd,                 	/* 可选：使能位速率切换 */
		.fd_padding  = 0x00U,                	/* DLC大于数据长度时的填充值 */
	};

	flexcan_status = FlexCAN_Ip_Send(entry->instance, entry->mbIdx,
			&tx_info, msgId, data);
	if(flexcan_status == FLEXCAN_STATUS_SUCCESS)
	{
		return CAN_DRV_OK;
	}
	else
	{
		return CAN_DRV_ERR_SEND_FAILED;
	}
}


CanDrv_StatusType Can_StartReceive(CanDrv_ChannelType channel, uint32_t msgId,
		uint32_t msgIdMask, uint8_t dataLength, bool is_fd)
{
	Flexcan_Ip_StatusType flexcan_status;

	if (!is_init)return CAN_DRV_ERR_NOT_INIT;
	if (channel >= CAN_CH_MAX)return CAN_DRV_ERR_PARAM;
	if ((!is_fd && dataLength > 8) || (is_fd && dataLength > 64))return CAN_DRV_ERR_PARAM;

	const CanDrv_ChannelMapEntry *entry = &channel_map[channel];
	if (entry->is_tx) return CAN_DRV_ERR_PARAM;   /* 不是接收通道 */

	Flexcan_Ip_DataInfoType rx_config = {
			.data_length = dataLength,
			.msg_id_type = FLEXCAN_MSG_ID_STD,
			.is_remote   = false,
			.is_polling  = false,
			.fd_enable   = is_fd,
			.fd_padding  = 0U,
			.enable_brs  = is_fd
	};

	flexcan_status = FlexCAN_Ip_ConfigRxMb(entry->instance, entry->mbIdx,
			&rx_config, msgId);
	if(flexcan_status != FLEXCAN_STATUS_SUCCESS)return CAN_DRV_ERR_CONFIG_FAILED;

	flexcan_status = FlexCAN_Ip_Receive(entry->instance, entry->mbIdx,
		&rx_buffers[channel], false);
	if (flexcan_status != FLEXCAN_STATUS_SUCCESS)return CAN_DRV_ERR_START_RECEIVE_FAILED;

	return CAN_DRV_OK;
}

CanDrv_StatusType Can_ReadReceivedData(CanDrv_ChannelType channel,
                                       uint32_t *msgId, uint8_t *data,
                                       uint8_t *length, bool *is_fd,
									   bool *is_std, bool *is_remote)
{
    if (!is_init) return CAN_DRV_ERR_NOT_INIT;
    if (channel >= CAN_CH_MAX) return CAN_DRV_ERR_PARAM;
    if (msgId == NULL || data == NULL || length == NULL ||
        is_fd == NULL || is_std == NULL || is_remote == NULL)
    {
        return CAN_DRV_ERR_PARAM;
    }


    const CanDrv_ChannelMapEntry *entry = &channel_map[channel];
    if (entry->is_tx) return CAN_DRV_ERR_PARAM;

    Flexcan_Ip_MsgBuffType *rx_buffer = &rx_buffers[channel];
    uint32_t cs = rx_buffer->cs;

    *msgId  = rx_buffer->msgId;
    *length = rx_buffer->dataLen;
	for(uint8_t i = 0; i < *length; i++)
	{
		data[i] = rx_buffer->data[i];
	}

	// 提取三个标志位
	*is_fd     = (cs >> 31) & 0x1;   // EDL 位
	*is_remote = (cs >> 0)  & 0x1;   // RTR 位
	*is_std    = ((cs >> 1) & 0x1) ? false : true;  // IDE: 1=扩展帧, 0=标准帧

	/* 关键：读取完成后，立刻重新启动接收，准备收下一帧 */
	Flexcan_Ip_StatusType status =
		FlexCAN_Ip_Receive(entry->instance, entry->mbIdx,
						   rx_buffer, false);

	if (status != FLEXCAN_STATUS_SUCCESS)
	{
		return CAN_DRV_ERR_RESTART_RECEIVE_FAILED;
	}

	return CAN_DRV_OK;
}


static CanDrv_ChannelType CanDrv_FindChannel(uint8_t instance, uint32_t mbIdx)
{
    for (int i = 0; i < CAN_CH_MAX; i++)
    {
        if (channel_map[i].instance == instance && channel_map[i].mbIdx == mbIdx)
        {
            return (CanDrv_ChannelType)i;
        }
    }
    return CAN_CH_MAX;   // 没找到，返回无效值
}

CanDrv_StatusType Can_RegisterCallback(CanDrv_Callback callback)
{
    user_callback = callback;
    return CAN_DRV_OK;
}

CanDrv_StatusType Can_RegisterErrorCallback(CanDrv_ErrorCallback callback)
{
    user_error_callback = callback;
    return CAN_DRV_OK;
}

static void CanDrv_RtdCallbackHandler(uint8_t instance, Flexcan_Ip_EventType eventType,
                                      uint32_t buffIdx)
{
    CanDrv_ChannelType channel = CanDrv_FindChannel(instance, buffIdx);
    if (channel == CAN_CH_MAX) return;   // 不是我们管理的通道，忽略

    CanDrv_EventType bspEvent;
    switch (eventType)
    {
        case FLEXCAN_EVENT_TX_COMPLETE:
            bspEvent = CAN_EVENT_TX_COMPLETE;
            break;
        case FLEXCAN_EVENT_RX_COMPLETE:
            bspEvent = CAN_EVENT_RX_COMPLETE;
            break;
        case FLEXCAN_EVENT_ERROR:
            bspEvent = CAN_EVENT_ERROR;
            break;
        default:
            return; // 忽略其他事件
    }

    if (user_callback != NULL)
    {
        user_callback(channel, bspEvent);
    }
}


void FlexCAN0_Callback(uint8_t instance, Flexcan_Ip_EventType eventType,
                       uint32_t buffIdx, const Flexcan_Ip_StateType *flexcanState)
{
    CanDrv_RtdCallbackHandler(instance, eventType, buffIdx);
}

void FlexCAN1_Callback(uint8_t instance, Flexcan_Ip_EventType eventType,
                       uint32_t buffIdx, const Flexcan_Ip_StateType *flexcanState)
{
    CanDrv_RtdCallbackHandler(instance, eventType, buffIdx);
}

void FlexCAN2_Callback(uint8_t instance, Flexcan_Ip_EventType eventType,
                       uint32_t buffIdx, const Flexcan_Ip_StateType *flexcanState)
{
    CanDrv_RtdCallbackHandler(instance, eventType, buffIdx);
}

void FlexCAN0_ErrCallback(uint8 instance, Flexcan_Ip_EventType eventType,
                          uint32 u32ErrStatus,
                          const Flexcan_Ip_StateType * flexcanState)
{
    if (user_error_callback != NULL)
    {
        if (eventType == FLEXCAN_EVENT_BUSOFF)
        {
            user_error_callback(CAN_CH0_TX, CAN_DRV_ERR_EVENT_BUSOFF);
        }
    }
}

void FlexCAN1_ErrCallback(uint8 instance, Flexcan_Ip_EventType eventType,
                                            uint32 u32ErrStatus,
                                            const Flexcan_Ip_StateType * flexcanState)
{
	if (user_error_callback != NULL)
	    {
	        if (eventType == FLEXCAN_EVENT_BUSOFF)
	        {
	            user_error_callback(CAN_CH1_TX, CAN_DRV_ERR_EVENT_BUSOFF);
	        }
	    }
}

void FlexCAN2_ErrCallback(uint8 instance, Flexcan_Ip_EventType eventType,
                                            uint32 u32ErrStatus,
                                            const Flexcan_Ip_StateType * flexcanState)
{
	if (user_error_callback != NULL)
	    {
	        if (eventType == FLEXCAN_EVENT_BUSOFF)
	        {
	            user_error_callback(CAN_CH2_TX, CAN_DRV_ERR_EVENT_BUSOFF);
	        }
	    }
}









