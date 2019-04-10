#include "utils.h"
#include <avr/io.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "io.h"
#include "uart.h"
#include "timer.h"
#include "dht.h"

struct UART* uart;

void answer_send(char* msg){
  int len = strlen(msg);
	for(int i=0; i<len; ++i, ++msg) UART_putChar((uint8_t)*msg, uart);
}

int N_LOG;
uint8_t DHT_PIN=8;
uint16_t timer_duration;

void timerFn(void* args){
  //answer_send("MSG: TIMER PROC!\n");
  uint16_t* argint=(uint16_t*)args;
  *argint=(*argint)+1;
  uint16_t tick=*argint;

  if (tick >= timer_duration) {
    answer_send("MSG: TIMER PROC!\n");

    int buf[40];
    //memset(buf, 0, 40);
    DHT_readSensor(buf, DHT_PIN);

    uint8_t h = bits2byte(buf);
    uint8_t t = bits2byte(buf + 16);


    //send log
    char mesg[BUFFER_SIZE];
    //memset(mesg, 0, BUFFER_SIZE);
    sprintf(mesg, "LOG: %d Temperature: %d Humidity: %d\n", N_LOG, t, h);
    answer_send(mesg);
    ++N_LOG;

    *argint=0;
  }
}

int main(void){
  //initialization of the UART
  uart=UART_init(115200);
  answer_send("MSG: UART initialized!\n");

  //initialization of the timer;
  volatile uint16_t tick=0;
  timer_duration=30;
  N_LOG=0;
  struct Timer* timer=Timer_create(timerFn, (void*) &tick);
  Timer_start(timer);
  answer_send("MSG: Timer started with an initial period of 30 second..\n");

  //
  uint8_t c;
  char r_message[BUFFER_SIZE];
  int i;
  answer_send("MSG: Ready to Listen!\n");
  while(1) {
    //memset(r_message, 0, BUFFER_SIZE);

    i=0;
    while(1){
      c=UART_getChar(uart);
      answer_send("Please wait..\n");
      if (i==0 && c!='a' && c!='b' && c!='q') continue;
      r_message[i]=c;
      ++i;
      r_message[i]='\0';
      if (c=='\n' || c=='\0') break;
    }

    if (r_message[0]=='a') {
      //Timer_stop(timer);
      answer_send("Log Request recieved..\n");

      int buf[40];
      //memset(buf, 0, 40);
      DHT_readSensor(buf, DHT_PIN);

      uint8_t h = bits2byte(buf);
      uint8_t t = bits2byte(buf + 16);

      //send log
      char mesg[BUFFER_SIZE];
      //memset(mesg, 0, BUFFER_SIZE);
      sprintf(mesg, "Instant LOG: %d Temperature: %d Humidity: %d\n", N_LOG, t, h);
      answer_send(mesg);
      ++N_LOG;

      //tick=0;
      //Timer_start(timer);
    }
    else if (r_message[0]=='b') {
      Timer_stop(timer);
      answer_send("Timer Set Request recieved..\n");
      //memset(r_message, 0, BUFFER_SIZE);

      i=0;
      while(1){
        c=UART_getChar(uart);
        answer_send("Please wait..\n");
        if (i==0 && c!='a' && c!='b' && c!='c') continue;
        r_message[i]=c;
        ++i;
        r_message[i]='\0';
        if (c=='\n' || c=='\0') break;
      }

      if (r_message[0]=='a') timer_duration=30;
      else if (r_message[0]=='b') timer_duration=60;
      else if (r_message[0]=='c') timer_duration=120;

      tick=0;
      //timer=Timer_create(timerFn, (void*) &tick);
      Timer_start(timer);
    }

    answer_send("MSG: Going to wait 10 seconds..\n");
    _delay_ms(10000); //wait 10 sec
  }
}
