

#include "main.h"
#include "runloop.h"

#define ENTER_HALT_N 30
//--------------------var------------------------

uint32_t sysTick = 0;
uint8_t user_key_task_id;
uint8_t user_uart_task_id;

static runloop_timer_t timer_50ms;
static runloop_timer_t timer_500ms;

//--------------------sleep------------------------
//#define EN_HALT_SLEEP_MODE

#ifdef EN_HALT_SLEEP_MODE 

uint8_t is_enter_halt = 0;
uint8_t enter_halt_cnt = ENTER_HALT_N;

// HALT模式
static void runloop_enter_sleep(void) {
  if (enter_halt_cnt == 0) {
    is_enter_halt = 1;
    en_pd5_int();
    PMU_EnterHaltModeWFI();
  }

  // init
  if (is_enter_halt) {
    is_enter_halt = 0;
    SysTick_Config(RCM_GetMasterClockFreq() / 100); // 10ms
    Led_gpio_init();
    Key_init();
    USART_Init();
    BuzzerInit();

    enter_halt_cnt = ENTER_HALT_N;
  }
}
#else

 // WAIT 模式
static void runloop_enter_sleep(void) { PMU_EnterWaitMode(); }

#endif

//--------------------timer------------------------

static void timer_50ms_func(void) { key_scan_polling(); }

static void timer_500ms_func(void) {
  GPIO_Toggle(GPIOD, GPIO_PIN_3);
  // uart_send_key(1);

#ifdef EN_HALT_SLEEP_MODE 
  if (enter_halt_cnt > 0) {
    --enter_halt_cnt;
  }
#endif

}

//--------------------task------------------------

static int user_key_task(runloop_port_event_t *param) {
  
  switch (param->event_id) {
  case KEY_DOWN:
    break;
  case KEY_UP:
    break;
  case KEY_LED:
    break;
  case KEY_VOL_DEC:
    break;
  case KEY_VOL_ADD:
    break;
  default:
    break;
  }

  uart_send_key(param->event_id);

#ifdef EN_HALT_SLEEP_MODE 
  enter_halt_cnt = ENTER_HALT_N;
#endif

  return 0;
}

static int user_uart_task(runloop_port_event_t *param) {
  
  USART1_Send(param->param, param->param_len);

#ifdef EN_HALT_SLEEP_MODE 
  enter_halt_cnt = ENTER_HALT_N;
#endif

  return 0;
}

//--------------------main------------------------

// 切换时钟源，让晶振pin作IO用
void RCM_ChangeCLKSource(void) {
  RCM_SwitchClockStruct_T switchClockStruct;
  /* switch HIRC as master clock  */
  switchClockStruct.clockSource = RCM_MCLKSRC_HIRC;
  /* Close the current clock if switching succeed.In this case,Close the HXT
   * clock */
  switchClockStruct.currentClkState = RCM_CLOCK_STATE_OFF;
  /* Disable interrupt when switching is completed */
  switchClockStruct.intEnable = DISABLE;
  /* Automatic switching */
  switchClockStruct.mode = RCM_SWITCH_MODE_AUTO;
  RCM_ConfigClockSwitch(&switchClockStruct);
}

int main(void) {
  RCM_ChangeCLKSource();

  // system tick setup
  SysTick_Config(RCM_GetMasterClockFreq() / 100); // 10ms
  Led_gpio_init();
  Key_init();
  USART_Init();
  BuzzerInit();
#ifdef EN_HALT_SLEEP_MODE 
  // WUPT_Init();
#endif

  // 50ms timer
  runloop_timer_init(&timer_50ms, timer_50ms_func);
  runloop_timer_start(&timer_50ms, 50, 1);

  // 500ms timer
  runloop_timer_init(&timer_500ms, timer_500ms_func);
  runloop_timer_start(&timer_500ms, 500, 1);

  // key proc task
  user_key_task_id = runloop_port_create(user_key_task);

  // uart proc task
  user_uart_task_id = runloop_port_create(user_uart_task);

  // 重启标志，调试用
  const char *hello = "Boot!";
  USART1_Send((uint8_t *)hello, 6);

  while (1) {

    if (sysTick > 0) {
      runloop_proc_timer(); // runloop timer polling
      sysTick = 0;
    }

    runloop_proc_list_msg(); // runloop event polling

    runloop_enter_sleep(); // runloop enter sleep
  }
}
