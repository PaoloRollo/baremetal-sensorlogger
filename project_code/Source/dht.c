#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include "io.h"
#include "dht.h"

int confirm(int pin, int us, int level) {
    // wait one more count to ensure
    int cnt = us / 10 + 1;

    int ok=0 , i=0;
    for (; i < cnt; ++i) {
        if (DigIO_getValue(pin) != level) {ok=1; break;}
        _delay_us(10);
    }

    if (!ok) return -1;
    return 0;
}

uint8_t bits2byte(int* data) {
    uint8_t b = 0;
    for (int i = 0; i < 8; i++) b += data[i] << (7 - i);
    return b;
}

int DHT_readSensor(int* data, uint8_t pin) {
	//request
  DigIO_setDirection(pin, 1);
  DigIO_setValue(pin, 0);
	_delay_ms(20);
	DigIO_setValue(pin, 1);
	_delay_us(30);
	DigIO_setDirection(pin, 0);

	//acknowledge or timeout
  if (confirm(pin, 80, 0)) return -1;
  if (confirm(pin, 80, 1)) return -1;

	//read output or timeout
  // DHT11 data transmite:
  //  1. PULL LOW ends before 50us, then -> start, else -> error
  //  2. PULL HIGH ends before 26-28us -> bit = 0
  //  3. PULL HIGH ends before 70us -> bit = 1, else -> error
  int ok, j;
	for (int i=0; i<40; ++i) {  //data = 5 bytes = 40 bytes
    //1.
    if (confirm(pin, 50, 0)) return -1;

		//2. 3.
    ok = 0;
    j=0;
    for (; j<8; ++j) {
        if (DigIO_getValue(pin) != 1) {ok=1; break;}
        _delay_us(10);
    }
    if (!ok) return -1;
    data[i] = (j > 3? 1:0);
	}

  uint8_t humidity = bits2byte(data);
  uint8_t humidity2 = bits2byte(data + 8);
  uint8_t temperature = bits2byte(data + 16);
  uint8_t temperature2 = bits2byte(data + 24);
  uint8_t check = bits2byte(data + 32);
  uint8_t expect = humidity + humidity2 + temperature + temperature2;
  if (check != expect) return -1;

	return 0;
}
