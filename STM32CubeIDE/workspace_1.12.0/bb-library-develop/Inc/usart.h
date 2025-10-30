#ifndef _USART_H_
#define _USART_H_

void MX_USART1_UART_Init(void);
void MX_USART2_UART_Init(void);
void MX_USART3_UART_Init(void);
void MX_USART4_UART_Init(void);
void MX_USART5_UART_Init(void);
void MX_USART6_UART_Init(void);

void USART1_IRQHandler(void);
void USART2_IRQHandler(void);
void USART3_8_IRQHandler(void);

#endif // _USART_H_
