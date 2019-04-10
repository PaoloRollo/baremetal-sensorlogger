#include <avr/io.h>
#include <avr/pgmspace.h>
#include "io.h"
#include "pins.h"

void DigIO_setDirection(uint8_t pin, PinDirection dir) {
  const Pin* mapping=pins+pin;
  uint8_t mask=1<<mapping->bit;
  if (dir)
    *(mapping->dir_register)|=mask;
  else
    *(mapping->dir_register)&=~mask;
}

PinDirection DigIO_getDirection(uint8_t pin){
  const Pin* mapping=pins+pin;
  uint8_t value=*(mapping->dir_register);
  return (value >> pins[pin].bit)&0x1;
}

void DigIO_setValue(uint8_t pin, uint8_t value) {
  const Pin* mapping=pins+pin;
  uint8_t mask=1<<mapping->bit;
  if (value)
    *(mapping->out_register)|=mask;
  else
    *(mapping->out_register)&=~mask;
}

uint8_t DigIO_getValue(uint8_t pin){
  const Pin* mapping=pins+pin;
  uint8_t value=*(mapping->in_register);
  return (value >> pins[pin].bit)&0x1;
}
