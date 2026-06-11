#include "stdint.h"
#include "stdbool.h"
#include "HalTimer.h"

void delay(uint32_t ms)
{

#if 0
	// 기존 교재의 코드
	uint32_t goal = Hal_timer_get_1ms_counter() + ms;

	while(goal != Hal_timer_get_1ms_counter());
#endif

#if 1
	// 새롭게 적용하는 안전한 Wrap-around 방식의 코드
	uint32_t start = Hal_timer_get_1ms_counter();	
	while((Hal_timer_get_1ms_counter() - start) < ms);
#endif




}
