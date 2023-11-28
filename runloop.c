#include "runloop.h"

#include <stdlib.h>
#include <string.h>

#include "apm32f00x_wwdt.h"

#define PORT_FUNC_N (15)

// port array
static runloop_port_func_t port_func_array[PORT_FUNC_N];


//--------------------- runloop list----------------------

static runloop_port_event_t *event_list_head = NULL;
static runloop_port_event_t *event_list_tail = NULL;

void runloop_msg_post(runloop_port_event_t *evt) {
  if (evt->dst_port_id == 0)
    return;

  runloop_port_event_t *p = malloc(sizeof(runloop_port_event_t));
  if (p == NULL) {
    WWDT_SoftwareReset();
  }

  memcpy(p, evt, sizeof(runloop_port_event_t));
  if (evt->param_len > 0) {
    p->param = malloc(evt->param_len);

    if (p->param == NULL) {
      WWDT_SoftwareReset();
    }
    memcpy(p->param, evt->param, evt->param_len);
  }

  p->next = NULL;

  if (event_list_tail == NULL) {
    event_list_tail = p;
    event_list_head = p;
  } else {
    event_list_tail->next = p;
    event_list_tail = p;
  }
}

void runloop_proc_list_msg(void) {
  runloop_port_event_t *p;
  while (event_list_head != NULL) {
    // run
    runloop_port_func_t func =
        port_func_array[event_list_head->dst_port_id - 1];
    if (func) {
      func(event_list_head);
    }

    // clear
    if (event_list_head->param_len > 0) {
      free(event_list_head->param);
    }
    p = event_list_head;
    event_list_head = event_list_head->next;
    free(p);
  }

  event_list_tail = NULL;
}

uint8_t runloop_port_create(runloop_port_func_t port_func) {
  uint8_t i = 0;
  for (; i < PORT_FUNC_N; ++i) {
    if (port_func_array[i] == NULL) {
      port_func_array[i] = port_func;
      break;
    }
  }

  if (i == PORT_FUNC_N) {
    return 0; // fail
  }

  return i + 1;
}

void runloop_port_delete(uint8_t port_id) {
  port_func_array[port_id - 1] = NULL;
}



//-------------------- timer list------------------------

static runloop_timer_t *timer_list_head = NULL;
static runloop_timer_t *timer_list_tail = NULL;
void runloop_timer_init(runloop_timer_t *ptimer,
                        runloop_timer_func_t pfunction) {
  if (timer_list_tail == NULL) {
    timer_list_tail = ptimer;
    timer_list_head = ptimer;
  } else {
    timer_list_tail->next = ptimer;
    timer_list_tail = ptimer;
  }

  ptimer->period = 0;
  ptimer->timer_func = pfunction;
  ptimer->next = NULL;
}

void runloop_timer_destroy(runloop_timer_t *ptimer) {

  if (timer_list_head == ptimer) {
    timer_list_head = timer_list_head->next;
    return;
  }

  runloop_timer_t *p = timer_list_head;
  while (p != NULL) {
    if (p->next == ptimer) {
      p->next = ptimer->next; // 从list里清除
      break;
    }

    p = p->next;
  }
}

void runloop_timer_start(runloop_timer_t *ptimer, uint32_t ms,
                         uint8_t repeat_flag) {
  ptimer->period = ms / 10 + 1;
  ptimer->period_bak = ptimer->period;
  ptimer->repeat_flag = repeat_flag;
}

void runloop_timer_stop(runloop_timer_t *ptimer) {
  ptimer->period = 0;
  ptimer->repeat_flag = 0;
}

void runloop_proc_timer(void) {
  runloop_timer_t *ptimer = timer_list_head;
  while (ptimer != NULL) {
    if (ptimer->period > 1) {
      ptimer->period -= 1;
      if (ptimer->period == 1) {
        if (ptimer->timer_func) {
          (ptimer->timer_func)();
        }

        if (ptimer->repeat_flag) {
          ptimer->period = ptimer->period_bak;
        } else {
          runloop_timer_destroy(ptimer);
        }
      }
    }
    ptimer = ptimer->next;
  }
}


//--------------------runloop delay----------------------

void user_delay_10us(uint8_t n) {
  for (uint8_t i = 0; i < n; i++) {
    for (uint32_t j = 0; j < 48; j++) { //48Mhz
      asm volatile("nop");
    }
  }
}

void user_delay_100us(uint32_t n) {
  for (uint32_t i = 0; i < n; i++) {
    user_delay_10us(10);
  }
}
