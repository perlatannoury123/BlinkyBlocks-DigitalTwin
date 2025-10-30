#ifndef _TIMERS_H_
#define _TIMERS_H_

void MX_TIM1_Init(void);
void MX_TIM6_Init(void);
void MX_TIM7_Init(void);
void TIM1_BRK_UP_TRG_COM_IRQHandler(void);
void TIM6_DAC_IRQHandler(void);

#endif // _TIMERS_H_
