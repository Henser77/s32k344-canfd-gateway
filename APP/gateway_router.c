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
    static uint8_t heartbeat_counter = 0;

    heartbeat_counter++;

    uint8_t heartbeat_data[8];
    heartbeat_data[0] = heartbeat_counter;
    heartbeat_data[1] = 0x02u;
    heartbeat_data[2] = 0x03u;
    heartbeat_data[3] = 0x04u;
    heartbeat_data[4] = 0x05u;
    heartbeat_data[5] = 0x06u;
    heartbeat_data[6] = 0x07u;
    heartbeat_data[7] = 0x08u;

    Can_Send(CAN_CH2_TX, 0x777u, heartbeat_data, 8u, false);
}


