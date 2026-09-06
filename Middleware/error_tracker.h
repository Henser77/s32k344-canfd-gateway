#ifndef ERR_TRACKER_H
#define ERR_TRACKER_H

#include <stdint.h>

/* 错误类型定义 */
typedef enum
{
    ERR_CAN0_BUS_OFF = 0,
    ERR_CAN1_BUS_OFF,
    ERR_CAN2_BUS_OFF,
    ERR_CAN0_ERROR_PASSIVE,
    ERR_CAN1_ERROR_PASSIVE,
    ERR_CAN2_ERROR_PASSIVE,
    ERR_RINGBUF_OVERFLOW,
    ERR_UDS_UNKNOWN_SERVICE,
    ERR_ROUTE_NO_MATCH,
    ERR_MAX
} ErrTracker_ErrorId;

/**
 * @brief 上报一次错误，对应错误计数加一
 */
void ErrTracker_Report(ErrTracker_ErrorId err_id);

/**
 * @brief 读取某个错误的当前计数
 */
uint32_t ErrTracker_GetCount(ErrTracker_ErrorId err_id);

/**
 * @brief 清零所有错误计数
 */
void ErrTracker_ClearAll(void);

#endif
