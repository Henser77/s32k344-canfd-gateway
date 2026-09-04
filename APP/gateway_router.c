#include "gateway_router.h"
#include "gateway_cfg.h"
#include "can_pdu.h"
#include "can_driver.h"
#include "ring_buffer.h"
#include "uds_diag.h"


void Gateway_Init(void)
{

}


void Gateway_Process(void)
{
	Can_Pdu_t pdu;
	uint8_t data[CAN_PDU_MAX_DATA_LEN];
	uint8_t len;
	bool is_fd;
	const Gateway_RouteEntry *entry;

	if(!RingBuf_Read(&pdu))
	{
		return;
	}

	/* 诊断请求拦截 */
	if(pdu.channel == CAN_CH2_RX1 && pdu.id == UDS_RX_ID)
	{
		UDS_Process(&pdu);
		return;
	}

	//匹配缓冲区里满足条件的报文
	for(uint32_t i = 0; i < GATEWAY_ROUTE_COUNT; i++)
	{
		entry = &gateway_route_table[i];

		if(pdu.channel != entry->src_ch) continue;
		if(pdu.id != entry->src_id)	continue;

		//根据路由规则转发报文
		len = entry->data_len;
		if(len > pdu.length)
		{
			len = pdu.length;
		}
		for (uint8_t j = 0; j < len; j++)
		{
			data[j] = pdu.data[j];
		}

		is_fd = true;
		if(entry->action == ROUTE_ACTION_CONVERT_TO_CLASSIC)
		{
			is_fd = false;
		}

		Can_Send(entry->dst_ch, entry->dst_id, data, len, is_fd);
	}
}


void Gateway_ProcessPeriodicTasks(void)
{
    static uint32_t tick = 0;

    tick++;
    if (tick >= 5000000)
    {
        uint8_t heartbeat_data[8] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};

        Can_Send(CAN_CH2_TX, 0x777u, heartbeat_data, 8u, false);
        tick = 0;
    }
}


