#include "stdint.h"
#include "stdbool.h"

#include "ARMv7AR.h"
#include "task.h"

/* 변수명 참고
   
   1. s -> static  : 파일 상단에 선언된 정적 변수
   2. p -> pointer : 포인터 변수

*/

static KernelTcb_t  sTask_list[MAX_TASK_NUM];
static uint32_t     sAllocated_tcb_index;

static void Scheduler_round_robin_algorithm(void);
static uint32_t	    sCurrent_tcb_index;

static KernelTcb_t* sCurrent_tcb;

void Kernel_task_init(void)
{
	sAllocated_tcb_index = 0;

	for(uint32_t i = 0; i < MAX_TASK_NUM; i++) {
		sTask_list[i].stack_base = (uint8_t*)(TASK_STACK_START + 
					   (i * USR_TASK_STACK_SIZE));
		sTask_list[i].sp = (uint32_t)sTask_list[i].stack_base +
				   USR_TASK_STACK_SIZE - 4;
		sTask_list[i].sp -= sizeof(KernelTaskContext_t);
		KernelTaskContext_t* ctx = (KernelTaskContext_t*)sTask_list[i].sp;
		ctx->pc = 0;
		ctx->spsr = ARM_MODE_BIT_SYS;
	}
}

uint32_t Kernel_task_create(KernelTaskFunc_t startFunc, uint32_t priority)
{
	KernelTcb_t* new_tcb = &sTask_list[sAllocated_tcb_index++];

	if (sAllocated_tcb_index > MAX_TASK_NUM) {
		return NOT_ENOUGH_TASK_NUM;
	}

	new_tcb->priority = priority;	// 우선순위를 TCB에 등록

	KernelTaskContext_t* ctx = (KernelTaskContext_t*)new_tcb->sp;
	ctx->pc = (uint32_t)startFunc;

	return (sAllocated_tcb_index - 1);
}

static void Scheduler_round_robin_algorithm(void)
{
	sCurrent_tcb_index++;
	sCurrent_tcb_index %= sAllocated_tcb_index;
	sCurrent_tcb = &sTask_list[sCurrent_tcb_index];
}

static void Scheduler_priority_algorithm(void)
{
	for(uint32_t i = 0; i < sAllocated_tcb_index; i++)
	{
		KernelTcb_t* pNextTcb = &sTask_list[i];
		if (pNextTcb != sCurrent_tcb)
		{
			if (pNextTcb->priority <= sCurrent_tcb->priority) {
				sCurrent_tcb = pNextTcb;
			}
		}
	}
}
