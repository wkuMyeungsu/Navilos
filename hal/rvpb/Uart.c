#include "stdint.h"
#include "Uart.h"
#include "HalUart.h"

/* hal/rvpb/Regs.c : Uart 변수를 extern으로 불러옴 */
extern volatile PL011_t* Uart;

/* HalUart.h : void Hal_uart_init(void) 구현 */
void Hal_uart_init(void) {
	
	// Enable UART
	Uart->uartcr.bits.UARTEN = 0;	// 레지스터를 변경하기 전에 하드웨어를 일단 꺼둠.
	Uart->uartcr.bits.TXE = 1;	// UART 출력을 킴.
	Uart->uartcr.bits.RXE = 1;	// UART 입력을 킴.
	Uart->uartcr.bits.UARTEN = 1;	// 꺼뒀던 하드웨어 전체를 다시 킴.
}

/* HalUart.h : void Hal_uart_put_char(uint8_t ch) 구현 */
// 알파벳 한 글자를 UART를 통해서 출력함.
void Hal_uart_put_char(uint8_t ch) {
	
	while(Uart->uartfr.bits.TXFF);	// 출력 버퍼가 0이 될 때까지 기다림.
	Uart->uartdr.all = (ch & 0xFF);	// 데이터 레지스터를 통해 알파벳 한 글자를 입력 버퍼로 보냄.
}

/* 비효율적인 Hal_uart_get_char()함수 */
// 최적화 1단계 : 개별적으로 접근하던 코드 수정
// 최적화 2단계 : 변수를 통해서 UARTDR에 두 번 접근하던 로직을 개선
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
