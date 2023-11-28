
#include "apm32f00x_int.h"

void NMI_Handler(void) {}

void HardFault_Handler(void) {}

void SVC_Handler(void) {}

void PendSV_Handler(void) {}

extern uint32_t sysTick;
void SysTick_Handler(void) { ++sysTick; }


