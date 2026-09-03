#ifndef GATEWAY_CFG_H
#define GATEWAY_CFG_H

#include <stdint.h>
#include "can_pdu.h"
#include "can_driver.h"

/* 路由动作类型 */
typedef enum
{
    ROUTE_ACTION_FORWARD = 0,          /* 原样转发 */
    ROUTE_ACTION_CONVERT_TO_FD,        /* 经典CAN → CAN FD */
    ROUTE_ACTION_CONVERT_TO_CLASSIC    /* CAN FD → 经典CAN */
} Gateway_ActionType;

/* 单条路由规则 */
typedef struct
{
    CanDrv_ChannelType src_ch;   /* 来源通道，如 CAN_CH2_RX */
    uint32_t           src_id;   /* 来源报文 ID */
    Gateway_ActionType action;   /* 动作类型 */
    CanDrv_ChannelType dst_ch;   /* 目标通道，如 CAN_CH0_TX */
    uint32_t           dst_id;   /* 转发后的 ID */
    uint8_t            data_len; /* 转发数据长度 */
} Gateway_RouteEntry;

/* 路由表 */
static const Gateway_RouteEntry gateway_route_table[] =
{
		{
		    /* 诊断仪经典 0x100 → 转 FD 从 CAN0 发出 */
		    .src_ch = CAN_CH2_RX,
		    .src_id = 0x100u,
		    .action = ROUTE_ACTION_CONVERT_TO_FD,
		    .dst_ch = CAN_CH0_TX,
		    .dst_id = 0x100u,
		    .data_len = 64u
		},
		{
		    /* FD 网段上的 ECU（CAN1）收到 0x100 后，转 经典 从CAN2发出 */
		    .src_ch = CAN_CH1_RX,
		    .src_id = 0x100u,
		    .action = ROUTE_ACTION_CONVERT_TO_CLASSIC,
		    .dst_ch = CAN_CH2_TX,
		    .dst_id = 0x200u,
		    .data_len = 8u
		}
};



#define GATEWAY_ROUTE_COUNT \
    (sizeof(gateway_route_table) / sizeof(gateway_route_table[0]))


#endif
