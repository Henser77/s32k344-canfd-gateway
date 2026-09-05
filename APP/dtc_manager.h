#ifndef DTC_MANAGER_H
#define DTC_MANAGER_H


#include <stdint.h>
#include <stddef.h>

#define DTC_MAX_COUNT	16U

typedef struct
{
	uint32_t dtc_code;	/* DTC 编号，如 0x015001 */
	uint8_t status;		/* 状态位，0x01表示 active 活跃故障 */
}DtcRecord_t;

void DtcManager_Init(void);
uint32_t DtcManager_GetCount(void);
const DtcRecord_t *DtcManager_GetRecord(uint32_t index);
void DtcManager_ClearAll(void);



#endif
