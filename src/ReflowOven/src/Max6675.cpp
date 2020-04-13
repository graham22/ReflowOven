
// Slightly modified Adafruit's MAX6675 library
// https://github.com/adafruit/MAX6675-library

#ifndef MAX6675_cpp
#define MAX6675_cpp

#include <math.h>
#include "Max6675.h"
#include <SPI.h>

/* Constructor- configure pins */
MAX6675::MAX6675(int8_t SCLK, int8_t CS, int8_t MISO) {
  _sclk = SCLK;
  _cs = CS;
  _miso = MISO;
  //pinMode(cs, OUTPUT);
  //pinMode(sclk, OUTPUT); 
  //pinMode(miso, INPUT);
  //digitalWrite(cs, HIGH);
}

// If the SPI library has transaction support, these functions
// establish settings and protect from interference from other
// libraries.  Otherwise, they simply do nothing.
#ifdef SPI_HAS_TRANSACTION
static inline void spi_begin(void) __attribute__((always_inline));
static inline void spi_begin(void) {
	SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
}
static inline void spi_end(void) __attribute__((always_inline));
static inline void spi_end(void) {
	SPI.endTransaction();
}
#else
#define spi_begin()
#define spi_end()
#endif

void MAX6675::begin(void) {

	pinMode(_cs, OUTPUT);
	csport = portOutputRegister(digitalPinToPort(_cs));
	cspinmask = digitalPinToBitMask(_cs);

#if defined (__AVR__)
	SPI.begin();
	SPI.setClockDivider(SPI_CLOCK_DIV2); // 8 MHz (full! speed!)
	SPI.setBitOrder(MSBFIRST);
	SPI.setDataMode(SPI_MODE0);
	mySPCR = SPCR;
#elif defined(TEENSYDUINO)
	SPI.begin();
	SPI.setClockDivider(SPI_CLOCK_DIV2); // 8 MHz (full! speed!)
	SPI.setBitOrder(MSBFIRST);
	SPI.setDataMode(SPI_MODE0);
#elif defined (__arm__)
	SPI.begin();
	SPI.setClockDivider(11); // 8-ish MHz (full! speed!)
	SPI.setBitOrder(MSBFIRST);
	SPI.setDataMode(SPI_MODE0);
#endif

}

uint8_t MAX6675::spiread(void) {
	uint8_t r = 0;

#if defined (__AVR__)
	uint8_t backupSPCR = SPCR;
	SPCR = mySPCR;
	SPDR = 0x00;
	while (!(SPSR & _BV(SPIF)));
	r = SPDR;
	SPCR = backupSPCR;
#elif defined(TEENSYDUINO)
	r = SPI.transfer(0x00);
#elif defined (__arm__)
	SPI.setClockDivider(11); // 8-ish MHz (full! speed!)
	SPI.setBitOrder(MSBFIRST);
	SPI.setDataMode(SPI_MODE0);
	r = SPI.transfer(0x00);
#endif

	return r;
}

uint8_t MAX6675::readdata(void) {
	digitalWrite(_cs, LOW);
	uint8_t r = spiread();
	digitalWrite(_cs, HIGH);
	return r;
}



//float MAX6675::readCelsius(void) {
//	uint16_t v;
//	v = analogRead(A7);
//	return v*0.4;  // convert from raw to celsius
//}

float MAX6675::readCelsius(void) {

	uint16_t v;

	digitalWrite(_cs, LOW);
	_delay_ms(1);

	v = spiread();
	v <<= 8;
	v |= spiread();

	digitalWrite(_cs, HIGH);

	if (v & 0x4) {
		// uh oh, no thermocouple attached!
		/*return NAN;*/
		return -100;
	}

	v >>= 3;

	return v*0.25;
}

float MAX6675::readFahrenheit(void) {
	return readCelsius() * 9.0 / 5.0 + 32;
}

//byte MAX6675::spiread(void) {
//	int i;
//	byte d = 0;
//
//	for (i = 7; i >= 0; i--)
//	{
//		digitalWrite(_sclk, LOW);
//		_delay_ms(1);
//		if (digitalRead(_miso)) {
//			//set the bit to 0 no matter what
//			d |= (1 << i);
//		}
//
//		digitalWrite(_sclk, HIGH);
//		_delay_ms(1);
//	}
//
//	return d;
//}

#endif // MAX6675_cpp

