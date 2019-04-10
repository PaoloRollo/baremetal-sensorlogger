#pragma once
#include "utils.h"
#include <stdint.h>

int confirm(int pin, int us, int level);

uint8_t bits2byte(int* data);

//reads humidity and temperature values read from the sensor into a log
int DHT_readSensor(int* data, uint8_t pin);
