#include "ring_buffer.h"
#include "string.h"

static Can_Pdu_t buffer[RING_BUFFER_SIZE] ;

/* 写索引：下一个新数据要放的位置 */
static volatile uint32_t head = 0;

/* 读索引：下一个要取走的数据位置 */
static volatile uint32_t tail = 0;

/* 当前缓冲区中有效元素个数 */
static volatile uint32_t count = 0;

void RingBuf_Init(void)
{
	memset(buffer, 0, sizeof(buffer));
	head 	= 0;
	tail 	= 0;
	count 	= 0;
}

bool RingBuf_Write(const Can_Pdu_t *pdu)
{
	if (pdu == NULL)
	{
		return false;   /* 参数错误 */
	}

	 /* 如果缓冲区已满，丢弃新数据 */
	 if (count >= RING_BUFFER_SIZE)
	 {
		 return false;
	 }

	 /* 将数据拷贝到 head 指向的位置 */
	 buffer[head] = *pdu;

	 /* head 前进，如果到达末尾则绕回 0 */
	 head = (head + 1) % RING_BUFFER_SIZE;

	 /* 有效元素个数加一 */
	 count++;

	 return true;
}

bool RingBuf_Read(Can_Pdu_t *pdu)
{
	if (pdu == NULL)
	{
		return false;   /* 参数错误 */
	}

	/* 如果缓冲区为空，返回失败 */
	if(count == 0)
	{
		return false;
	}

	/* 从 tail 指向的位置取出数据 */
	*pdu = buffer[tail];

	/* tail 前进，如果到达末尾则绕回 0 */
	tail = (tail + 1) % RING_BUFFER_SIZE;

	/* 有效元素个数减一 */
	count--;

	return true;

}

bool RingBuf_IsEmpty(void)
{
    return (count == 0);
}

uint32_t RingBuf_GetCount(void)
{
    return count;
}
