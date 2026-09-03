#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <stdint.h>
#include <stdbool.h>
#include "can_pdu.h"

//环形缓冲区容量
#define RING_BUFFER_SIZE 64U

void RingBuf_Init(void);
bool RingBuf_Write(const Can_Pdu_t *pdu);
bool RingBuf_Read(Can_Pdu_t *pdu);
bool RingBuf_IsEmpty(void);
uint32_t RingBuf_GetCount(void);


#endif
