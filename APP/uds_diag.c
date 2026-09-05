#include "uds_diag.h"
#include "dtc_manager.h"
#include "can_driver.h"
#include <stddef.h>

/* 诊断服务 ID */
#define UDS_SID_SESSION_CONTROL       0x10u   /* 会话控制 */
#define UDS_SID_READ_DATA_BY_ID       0x22u   /* 读数据 */
#define UDS_SID_TESTER_PRESENT        0x3Eu   /* 握手 */
#define UDS_SID_READ_DTC_INFORMATION  0x19u	  /* 读DTC */
#define UDS_SID_CLEAR_DTC_INFORMATION 0x14u	  /* 清除DTC */

/* 否定响应 */
#define UDS_NEG_RESPONSE_SID          0x7Fu
#define UDS_NRC_SERVICE_NOT_SUPPORTED 0x11u

void UDS_Process(const Can_Pdu_t *rx_pdu)
{
    uint8_t sid;
    uint8_t resp_data[CAN_PDU_MAX_DATA_LEN];
    uint8_t resp_len = 0;

    if (rx_pdu == NULL)
    {
        return;
    }

    sid = rx_pdu->data[0];

    switch (sid)
    {
        case UDS_SID_SESSION_CONTROL:
        {
            resp_data[0] = 0x06u;  /* 响应长度 */
            resp_data[1] = 0x50u;  /* SID + 0x40 */
            resp_data[2] = 0x03u;  /* 扩展会话 */
            resp_data[3] = 0x00u;
            resp_data[4] = 0x32u;  /* P2 超时 */
            resp_data[5] = 0x00u;
            resp_data[6] = 0xC8u;  /* P2* 超时 */
            resp_data[7] = 0x00u;
            resp_len = 8u;
            break;
        }

        case UDS_SID_READ_DATA_BY_ID:
        {
            resp_data[0] = 0x04u;  /* 响应长度 */
            resp_data[1] = 0x62u;  /* SID + 0x40 */
            resp_data[2] = rx_pdu->data[1];  /* DID 高字节 */
            resp_data[3] = rx_pdu->data[2];  /* DID 低字节 */
            resp_data[4] = 0x01u;  /* 软件版本号 */
            resp_data[5] = 0x05u;
            resp_data[6] = 0x00u;
            resp_data[7] = 0x00u;
            resp_len = 8u;
            break;
        }

        case UDS_SID_TESTER_PRESENT:
        {
            resp_data[0] = 0x02u;
            resp_data[1] = 0x7Eu;
            resp_data[2] = 0x00u;
            resp_data[3] = 0x00u;
            resp_data[4] = 0x00u;
            resp_data[5] = 0x00u;
            resp_data[6] = 0x00u;
            resp_data[7] = 0x00u;
            resp_len = 8u;
            break;
        }

        case UDS_SID_READ_DTC_INFORMATION:
        {
            uint32_t dtc_count = DtcManager_GetCount();

            resp_data[0] = 0x06u;
            resp_data[1] = 0x59u;   /* 0x19 + 0x40 */
            resp_data[2] = rx_pdu->data[1];  /* 子功能回显 */
            resp_data[3] = rx_pdu->data[2];  /* 状态掩码 */
            resp_data[4] = 0x00u;
            resp_data[5] = 0x00u;
            resp_data[6] = 0x00u;
            resp_data[7] = 0x00u;

            if (dtc_count > 0u)
            {
                const DtcRecord_t *dtc = DtcManager_GetRecord(0);

                resp_data[4] = (uint8_t)(dtc->dtc_code >> 16);
                resp_data[5] = (uint8_t)(dtc->dtc_code >> 8);
                resp_data[6] = (uint8_t)(dtc->dtc_code);
                resp_data[7] = dtc->status;
            }

            resp_len = 8u;
            break;
        }

        case UDS_SID_CLEAR_DTC_INFORMATION:
        {
            DtcManager_ClearAll();

            resp_data[0] = 0x01u;
            resp_data[1] = 0x54u;   /* 0x14 + 0x40 */
            resp_data[2] = 0x00u;
            resp_data[3] = 0x00u;
            resp_data[4] = 0x00u;
            resp_data[5] = 0x00u;
            resp_data[6] = 0x00u;
            resp_data[7] = 0x00u;
            resp_len = 8u;
            break;
        }

        default:
        {
            resp_data[0] = 0x03u;
            resp_data[1] = UDS_NEG_RESPONSE_SID;
            resp_data[2] = sid;
            resp_data[3] = UDS_NRC_SERVICE_NOT_SUPPORTED;
            resp_data[4] = 0x00u;
            resp_data[5] = 0x00u;
            resp_data[6] = 0x00u;
            resp_data[7] = 0x00u;
            resp_len = 8u;
            break;
        }
    }

    Can_Send(CAN_CH2_TX, UDS_TX_ID, resp_data, resp_len, false);
}
