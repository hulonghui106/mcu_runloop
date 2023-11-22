#ifndef __OS_TASK_H___
#define __OS_TASK_H___

#ifndef NULL
#define NULL (void *)(0)
#endif

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
// typedef unsigned long long uint64_t;

typedef signed char int8_t;
typedef signed short int16_t;
typedef signed int int32_t;
// typedef long long int64_t;

//---------------------event---------------------
typedef struct _port_event_ {
  struct _port_event_ *next;
  uint8_t dst_port_id;
  uint8_t src_port_id;
  uint8_t event_id;
  void *param;
  uint8_t param_len;
} runloop_port_event_t;

void runloop_msg_post(runloop_port_event_t *evt);

//-------------------port----------------
typedef int (*runloop_port_func_t)(runloop_port_event_t *param);
uint8_t runloop_port_create(runloop_port_func_t port_func);
void runloop_port_delete(uint8_t port_id);

//------------------timer-----------------------------
typedef void (*runloop_timer_func_t)(void);

typedef struct runloop_timer_struct {
  struct runloop_timer_struct *next;
  uint32_t period;
  runloop_timer_func_t timer_func;
  uint8_t repeat_flag;
  uint32_t period_bak;
} runloop_timer_t;

void runloop_timer_init(runloop_timer_t *ptimer,
                        runloop_timer_func_t pfunction);
void runloop_timer_destroy(runloop_timer_t *ptimer);
void runloop_timer_start(runloop_timer_t *ptimer, uint32_t ms,
                         uint8_t repeat_flag);
void runloop_timer_stop(runloop_timer_t *ptimer);

//---------------loop callback---------------------------
void runloop_proc_timer(void);
void runloop_proc_list_msg(void);
#endif
