#ifndef UDS_DIAG_H
#define UDS_DIAG_H


#include "can_pdu.h"

/* 诊断请求和响应的 CAN ID */
#define UDS_TX_ID       0x710u

/* 诊断接收配置 */
#define UDS_RX_CHANNEL      CAN_CH2_RX1
#define UDS_RX_ID           0x700u
#define UDS_RX_DATA_LEN     8u
#define UDS_RX_IS_FD        false

void UDS_Process(const Can_Pdu_t *rxpdu);


#endif
