
#include "apm32f00x_int.h"

void NMI_Handler(void) {}

void HardFault_Handler(void) {}

void SVC_Handler(void) {}

void PendSV_Handler(void) {}

extern uint32_t sysTick;
void SysTick_Handler(void) { sysTick++; }


extern void USART1_RxIsr(void);
void USART1_RX_IRQHandler(void)
{
    USART1_RxIsr();
}


//外部IO中断,暂时不用
void EINTD_IRQHandler(void)
{
    EINT_ClearIntFlag(EINT_PORT_C);
    EINT_ClearIntFlag(EINT_PORT_D);
}
