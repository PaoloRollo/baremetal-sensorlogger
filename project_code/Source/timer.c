#include "timer.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>


// here we hook to timer 5 which is not used in any of the primary arduino pins
// its 16 bit and allows us a decent resolution

typedef struct Timer{
  //uint16_t duration_ms;
  TimerFn fn;
  void* args;
} Timer;

static struct Timer t;
static uint16_t duration_ms=1000;

// creates a timer that has a duration of ms milliseconds
// bound to the device device
// each duration_ms the function timer_fn will be called
// with arguments timer args
Timer* Timer_create(TimerFn timer_fn, void* timer_args){
  memset(&t, 0, sizeof(Timer));
  Timer* timer=&t;
  //timer->duration_ms=duration_ms;
  timer->fn=timer_fn;
  timer->args=timer_args;
  return timer;
}

// starts a timer
void Timer_start(struct Timer* timer){
  cli();
  uint16_t ocrval=(uint16_t)(15.625 * duration_ms);
  TCCR1A = 0;
  TCCR1B = 0;
  OCR1A = ocrval;
  TCCR1B |= (1 << WGM12) | (1 << CS10) | (1 << CS12);
  TIMSK1 |= (1 << OCIE1A);
  sei();
}

// stops a timer
void Timer_stop(struct Timer* timer){
  cli();
  TIMSK1 &= ~(1 << OCIE1A);
  sei();
}

ISR(TIMER1_COMPA_vect) {
  //TCNT1 = 0;
  if(t.fn) (*t.fn)(t.args);
}
