#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include <string.h>
#include "uart.h"

void setBaud57600(void) {
#define BAUD 57600
#include <util/setbaud.h>
  UBRR0H = UBRRH_VALUE;
  UBRR0L = UBRRL_VALUE;

#ifdef USE_2X
  UCSR0A |= (1<<U2X0);
#endif
#undef BAUD
}

void setBaud115200(void) {
#define BAUD 115200
#include <util/setbaud.h>
  UBRR0H = UBRRH_VALUE;
  UBRR0L = UBRRL_VALUE;

#ifdef USE_2X
  UCSR0A |= (1<<U2X0);
#endif
#undef BAUD
}

typedef struct UART{
  int tx_buffer[BUFFER_SIZE];
  volatile uint8_t tx_start;
  volatile uint8_t tx_end;
  volatile uint8_t tx_size;

  int rx_buffer[BUFFER_SIZE];
  volatile uint8_t rx_start;
  volatile uint8_t rx_end;
  volatile uint8_t rx_size;

  uint32_t baud;
} UART;

static UART uart;

struct UART* UART_init(uint32_t baud) {
  UART* u=&uart;

  switch(baud){
  case 57600:
    {
      setBaud57600();
      u->baud=57600;
      break;
    }
  case 115200:
    {
      setBaud115200();
      u->baud=115200;
      break;
    }
  }

  u->tx_start=0;
  u->tx_end=0;
  u->tx_size=0;
  u->rx_start=0;
  u->rx_end=0;
  u->rx_size=0;

  UCSR0C = _BV(UCSZ01) | _BV(UCSZ00); /* 8-bit data */
  UCSR0B = _BV(RXEN0) | _BV(TXEN0) | _BV(RXCIE0);   /* Enable RX and TX */
  sei();

  return u;
}

void UART_putChar(uint8_t c, struct UART* u) {
  while (u->tx_size>=BUFFER_SIZE);  //until there is some space in the buffer
  ATOMIC_BLOCK(ATOMIC_FORCEON){
    u->tx_buffer[u->tx_end]=c;
    BUFFER_PUT(u->tx, BUFFER_SIZE);
  }
  UCSR0B |= _BV(UDRIE0); // enable transmit interrupt
}

ISR (USART_UDRE_vect) {
  if (! uart.tx_size){
    UCSR0B &= ~_BV(UDRIE0);
  } else {
    UDR0 = uart.tx_buffer[uart.tx_start];
    BUFFER_GET(uart.tx, BUFFER_SIZE);
  }
}

uint8_t UART_getChar(struct UART* u){
  while(u->rx_size==0);  //untill there is nothing to read in the buffer
  uint8_t c;
  ATOMIC_BLOCK(ATOMIC_FORCEON){
    c=u->rx_buffer[u->rx_start];
    BUFFER_GET(u->rx, BUFFER_SIZE);
  }
  return c;
}


ISR (USART_RX_vect) {
  uint8_t c=UDR0;
  if (uart.rx_size<BUFFER_SIZE){
    uart.rx_buffer[uart.rx_end] = c;
    BUFFER_PUT(uart.rx, BUFFER_SIZE);
  }
}
