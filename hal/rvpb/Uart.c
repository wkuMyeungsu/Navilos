#include "stdint.h"
#include "Uart.h"
#include "HalUart.h"
#include "HalInterrupt.h" // 인터럽트 설정 관련 함수를 사용한다.

/* hal/rvpb/Regs.c : Uart 변수를 extern으로 불러옴 */
extern volatile PL011_t* Uart;

static void interrupt_handler(void);

/* HalUart.h : void Hal_uart_init(void) 구현 */
void Hal_uart_init(void) {
	
	// Enable UART
	Uart->uartcr.bits.UARTEN = 0;	// 레지스터를 변경하기 위해 하드웨어를 잠시 꺼둠.
	Uart->uartcr.bits.TXE = 1;	// UART 출력을 킴.
	Uart->uartcr.bits.RXE = 1;	// UART 입력을 킴.
	Uart->uartcr.bits.UARTEN = 1;	// 꺼뒀던 하드웨어 전체를 다시 킴.
	
	// Enable input interrupt
	Uart->uartimsc.bits.RXIM = 1;

	// Register UART interrupt handler
	Hal_interrupt_enable(UART_INTERRUPT0);
	Hal_interrupt_register_handler(interrupt_handler, UART_INTERRUPT0);
}

void Hal_uart_put_char(uint8_t ch) {
	
	while(Uart->uartfr.bits.TXFF);
	Uart->uartdr.all = (ch & 0xFF);
}

uint8_t Hal_uart_get_char(void)
{
	uint8_t data;

	while(Uart->uartfr.bits.RXFE);

	data = Uart->uartdr.all;

	// Check for an error flag
	if (data & 0xFFFFFF00)
	{
		// Clear the error
		Uart->uartrsr.all = 0xFF;
		return 0;
	}

	return (uint8_t)(data & 0xFF);
}

static void interrupt_handler(void)
{
	uint8_t ch = Hal_uart_get_char();
	Hal_uart_put_char(ch);
}
