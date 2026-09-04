#ifdef __cplusplus
extern "C"{
#endif

/* Including necessary configuration files. */
#include "Mcal.h"
#include "Clock_Ip.h"
#include "IntCtrl_Ip.h"
#include "Siul2_Port_Ip.h"


#include "can_driver.h"
#include "can_interface.h"

#include "ring_buffer.h"

#include "gateway_router.h"
#include "gateway_cfg.h"

#include "uds_diag.h"

volatile int exit_code = 0;
/* User includes */



/*!
  \brief The main function for the project.
  \details The startup initialization sequence is the following:
 * - startup asm routine
 * - main()
*/
int main(void)
{
    /* Write your code here */

	/* 硬件初始化 */
	Clock_Ip_Init(&Clock_Ip_aClockConfig[0]);
	IntCtrl_Ip_Init(&IntCtrlConfig_0);
	Siul2_Port_Ip_Init(NUM_OF_CONFIGURED_PINS_PortContainer_0_BOARD_InitPeripherals,
			g_pin_mux_InitConfigArr_PortContainer_0_BOARD_InitPeripherals);

	/* 中间件和 BSP 初始化 */
	RingBuf_Init();

	CanDrv_StatusType status;
	status = Can_Init();
	if(status != CAN_DRV_OK)while(1){};


	 /* 3. 初始化接口层（注册回调） */
	 CanIf_Init();

	/* 根据路由表自动配置 BSP 接收 */
	for (uint32_t i = 0; i < GATEWAY_ROUTE_COUNT; i++)
	{
		const Gateway_RouteEntry *entry = &gateway_route_table[i];

		uint8_t data_len = 64u;
		bool is_fd = true;

		if (entry->action == ROUTE_ACTION_CONVERT_TO_CLASSIC)
		{
			data_len = 8u;
			is_fd = false;
		}

		Can_StartReceive(entry->src_ch,
						 entry->src_id,
						 0xFFFFFFFF,
						 data_len,
						 is_fd);
	}

	/* 根据 UDS 配置自动配置诊断接收 */
	Can_StartReceive(UDS_RX_CHANNEL,
	                 UDS_RX_ID,
	                 0xFFFFFFFF,
	                 UDS_RX_DATA_LEN,
	                 UDS_RX_IS_FD);

	Gateway_Init();

    for(;;)
    {

    	Gateway_Process();
    	Gateway_ProcessPeriodicTasks();


        if(exit_code != 0)
        {
            break;
        }
    }
    return exit_code;
}

#ifdef __cplusplus
}
#endif

/** @} */
