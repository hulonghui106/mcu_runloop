#include "Board.h"
/*
pd3: bt_led
pd2: mic_led
pc3: key_led

*/

void Led_gpio_init(void) {
  GPIO_Config_T gpioConfig;
  /* bt_led configuration */
  gpioConfig.mode = GPIO_MODE_OUT_OD;
  gpioConfig.pin = GPIO_PIN_3;
  gpioConfig.speed = GPIO_SPEED_10MHz;
  GPIO_Config(GPIOD, &gpioConfig);

  /* mic_led configuration */
  gpioConfig.pin = GPIO_PIN_2;
  GPIO_Config(GPIOD, &gpioConfig);

  /* key_led configuration */
  gpioConfig.pin = GPIO_PIN_3;
  GPIO_Config(GPIOC, &gpioConfig);
  
}

void bt_led_set(uint8_t n) {
  if (n)
    GPIO_SetBit(GPIOD, GPIO_PIN_3);
  else
    GPIO_ClearBit(GPIOD, GPIO_PIN_3);
}

void mic_led_set(uint8_t n) {
  if (n)
    GPIO_SetBit(GPIOD, GPIO_PIN_2);
  else
    GPIO_ClearBit(GPIOD, GPIO_PIN_2);
}

void key_led_set(uint8_t n) {
  if (n)
    GPIO_SetBit(GPIOC, GPIO_PIN_3);
  else
    GPIO_ClearBit(GPIOC, GPIO_PIN_3);
}




/*
//PWM?
PB4 B
PB5 G
PA3 R
*/
