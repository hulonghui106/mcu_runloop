#include "main.h"
#include "runloop.h"

void Key_init(void) {

  GPIO_Config_T gpioConfig;

  gpioConfig.mode = GPIO_MODE_IN_PU;
  gpioConfig.speed = GPIO_SPEED_10MHz;
  gpioConfig.intEn = GPIO_EINT_ENABLE;
  // gpioConfig.intEn = GPIO_EINT_DISABLE;

  // PD1
  gpioConfig.pin = GPIO_PIN_1; // PIN
  EINT_Config(EINT_PORT_D, EINT_TRIGGER_FALLING);
  // EINT_Config(EINT_PORT_D, EINT_TRIGGER_RISING);
  NVIC_EnableIRQRequest(EINTD_IRQn, 0X0f);
  GPIO_Config(GPIOD, &gpioConfig);

}

void send_user_key(uint8_t key) {
  runloop_port_event_t event;
  event.dst_port_id = user_key_task_id;
  event.event_id = key;
  event.param_len = 0;
  runloop_msg_post(&event);
}

// 50ms callback
void key_scan_polling(void) {
  static uint8_t last_pd1 = 1;
 
  uint8_t pd1 = GPIO_ReadInputBit(GPIOD, GPIO_PIN_1);

  // PD1下降沿
  if ((pd1 == 0) && (last_pd1 == 1)) {
    send_user_key(KEY_DOWN);
  }
  last_pd1 = pd1;
}

// 外部IO中断
void EINTD_IRQHandler(void) { EINT_ClearIntFlag(EINT_PORT_D); }

void EINTC_IRQHandler(void) { EINT_ClearIntFlag(EINT_PORT_C); }

void EINTA_IRQHandler(void) { EINT_ClearIntFlag(EINT_PORT_A); }
