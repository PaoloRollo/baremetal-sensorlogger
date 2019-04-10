#pragma once
#include "utils.h"
#include <stdint.h>

struct Timer;

typedef void (*TimerFn)(void*);

// creates a timer that has a duration of ms milliseconds
// bound to the device device
// each duration_ms the function timer_fn will be called
// with arguments timer args
struct Timer* Timer_create(TimerFn timer_fn, void* timer_args);

// starts a timer
void Timer_start(struct Timer* timer);

// stops a timer
void Timer_stop(struct Timer* timer);
