#pragma once
#define 	F_CPU   16000000UL
#include <util/delay.h>

typedef struct LOG{
  uint8_t n;
  uint8_t temperature;
  uint8_t humidity;
} LOG;

#define LOG_DIM sizeof(LOG)

#define BUFFER_SIZE 300
#define UART_BUFFER_SIZE 256

#define BUFFER_PUT(_IDX, _BSIZE_MAX)	\
  ++_IDX ## _end;					\
  if (_IDX ## _end>=_BSIZE_MAX)				\
    _IDX ## _end=0;					\
  ++_IDX ## _size;

#define BUFFER_GET(_IDX, _BSIZE_MAX)	\
  ++_IDX ## _start;				\
  if (_IDX ## _start>=_BSIZE_MAX)		\
    _IDX ## _start =0;				\
  --_IDX ## _size;
