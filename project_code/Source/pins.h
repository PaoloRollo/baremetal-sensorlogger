#include <stdint.h>

#define PINS_NUM 14

typedef struct{
  volatile uint8_t* in_register;
  volatile uint8_t* out_register;
  volatile uint8_t* dir_register;
  uint8_t bit;
}  Pin;

extern const Pin pins[];
