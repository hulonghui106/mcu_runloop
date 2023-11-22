

#include "main.h"
#include "runloop.h"

uint32_t sysTick = 0;
uint8_t user_key_task_id;

static runloop_timer_t timer_50ms;
static runloop_timer_t timer_500ms;

static void timer_50ms_func(void) { key_scan_polling(); }

static void timer_500ms_func(void) { GPIO_Toggle(GPIOD, GPIO_PIN_3); }

static int user_key_task(runloop_port_event_t *param) {

  switch (param->event_id) {
  case KEY_DOWN:
    bt_led_set(1);
    break;
  case KEY_UP:
    key_led_set(1);
    break;
  case KEY_LED:
    mic_led_set(1);
    break;
  case KEY_VOL_DEC:
    break;
  case KEY_VOL_ADD:
    break;
  default:
    break;
  }

  return 0;
}

static void runloop_enter_sleep(void) {}

int main(void) {
  // system tick setup
  SysTick_Config(RCM_GetMasterClockFreq() / 1000); // 1ms

  Led_gpio_init();
  Key_init();
  USART_Init();

  // 50ms timer
  runloop_timer_init(&timer_50ms, timer_50ms_func);
  runloop_timer_start(&timer_50ms, 50, 1);

  // 500ms timer
  runloop_timer_init(&timer_500ms, timer_500ms_func);
  runloop_timer_start(&timer_500ms, 500, 1);

  // led task
  user_key_task_id = runloop_port_create(user_key_task);

  while (1) {

    if (sysTick > 0) {
      runloop_proc_timer(); // runloop timer polling
      sysTick = 0;
    }

    runloop_proc_list_msg(); // runloop event polling

    runloop_enter_sleep(); // runloop enter sleep
  }
}
