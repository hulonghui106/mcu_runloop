#include "main.h"
#include "runloop.h"

/*
pd1: down
pc7 up
pc6 led
pc5 vol-
pc4 vol+
*/

void Key_init(void) {

  GPIO_Config_T gpioConfig;

  gpioConfig.mode = GPIO_MODE_IN_PU;
  gpioConfig.speed = GPIO_SPEED_10MHz;
  gpioConfig.intEn = GPIO_EINT_DISABLE;
  //   gpioConfig.intEn = GPIO_EINT_ENABLE;
  //   EINT_Config(EINT_PORT_D, EINT_TRIGGER_FALLING);
  //   NVIC_EnableIRQRequest(EINTD_IRQn, 0X0f);

  // PD1
  gpioConfig.pin = GPIO_PIN_1; // PIN
  GPIO_Config(GPIOD, &gpioConfig);

  // PC7
  gpioConfig.pin = GPIO_PIN_7;
  GPIO_Config(GPIOC, &gpioConfig);

  // PC6
  gpioConfig.pin = GPIO_PIN_6;
  GPIO_Config(GPIOC, &gpioConfig);

  // PC5
  gpioConfig.pin = GPIO_PIN_5;
  GPIO_Config(GPIOC, &gpioConfig);

  // PC4
  gpioConfig.pin = GPIO_PIN_4;
  GPIO_Config(GPIOC, &gpioConfig);
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
  static uint8_t last_pc7 = 1;
  static uint8_t last_pc6 = 1;
  static uint8_t last_pc5 = 1;
  static uint8_t last_pc4 = 1;

  uint8_t pd1 = GPIO_ReadInputBit(GPIOD, GPIO_PIN_1);

  uint8_t portC = GPIO_ReadInputPort(GPIOC);
  uint8_t pc7 = portC & GPIO_PIN_7;
  uint8_t pc6 = portC & GPIO_PIN_6;
  uint8_t pc5 = portC & GPIO_PIN_5;
  uint8_t pc4 = portC & GPIO_PIN_4;

  // PD1下降沿
  if ((pd1 == 0) && (last_pd1 == 1)) {
    send_user_key(KEY_DOWN);
  }
  last_pd1 = pd1;

  // PC7下降沿
  if ((pc7 == 0) && (last_pc7 == 1)) {
    send_user_key(KEY_UP);
  }
  last_pc7 = pc7;

  // PC6下降沿
  if ((pc6 == 0) && (last_pc6 == 1)) {
    send_user_key(KEY_LED);
  }
  last_pc6 = pc6;

  // PC5下降沿
  if ((pc5 == 0) && (last_pc5 == 1)) {
    send_user_key(KEY_VOL_DEC);
  }
  last_pc5 = pc5;

  // PC4下降沿
  if ((pc4 == 0) && (last_pc4 == 1)) {
    send_user_key(KEY_VOL_ADD);
  }
  last_pc4 = pc4;
}


