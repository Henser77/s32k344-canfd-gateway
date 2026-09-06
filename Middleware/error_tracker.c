#include "error_tracker.h"

static uint32_t error_count[ERR_MAX];

void ErrTracker_Report(ErrTracker_ErrorId err_id)
{
    if (err_id >= ERR_MAX)
    {
        return;
    }

    error_count[err_id]++;
}

uint32_t ErrTracker_GetCount(ErrTracker_ErrorId err_id)
{
    if (err_id >= ERR_MAX)
    {
        return 0u;
    }

    return error_count[err_id];
}

void ErrTracker_ClearAll(void)
{
    for (uint32_t i = 0; i < (uint32_t)ERR_MAX; i++)
    {
        error_count[i] = 0u;
    }
}
