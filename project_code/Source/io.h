#pragma once
#include <stdint.h>

typedef enum {
  Input=0x0,
  Output=0x1
} PinDirection;

void DigIO_setDirection(uint8_t pin, PinDirection dir);

PinDirection DigIO_getDirection(uint8_t pin);

void DigIO_setValue(uint8_t pin, uint8_t value);

uint8_t DigIO_getValue(uint8_t pin);
