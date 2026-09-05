#include "dtc_manager.h"

static DtcRecord_t dtc_list[DTC_MAX_COUNT];		//DTC存储表
static uint32_t dtc_count = 0;

void DtcManager_Init(void)
{
    dtc_count = 0;

    /* 初始化时放一个模拟故障 */
    dtc_list[0].dtc_code = 0x015001u;
    dtc_list[0].status   = 0x01u;
    dtc_count = 1;
}

uint32_t DtcManager_GetCount(void)
{
    return dtc_count;
}

const DtcRecord_t *DtcManager_GetRecord(uint32_t index)
{
    if (index >= dtc_count)
    {
        return NULL;
    }

    return &dtc_list[index];
}

void DtcManager_ClearAll(void)
{
    dtc_count = 0;
}

