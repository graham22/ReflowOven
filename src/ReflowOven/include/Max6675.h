
// Slightly modified Adafruit's MAX6675 library
// https://github.com/adafruit/MAX6675-library

#ifndef MAX6675_h
#define MAX6675_h

#include "Arduino.h"

namespace ReflowOven
{
class MAX6675
{
public:
	MAX6675(int8_t SCLK, int8_t CS, int8_t MISO);
	void begin(void);
	float readCelsius(void);
	float readFahrenheit(void);
	// For compatibility with older versions:

private:
	int8_t _sclk, _miso, _cs;
	volatile uint8_t *mosiport, *clkport, *csport;
	uint8_t mosipinmask, clkpinmask, cspinmask;
	uint8_t mySPCR;
	uint8_t spiread(void);
	uint8_t readdata(void);
};
} // namespace ReflowOven

#endif // MAX6675_h
