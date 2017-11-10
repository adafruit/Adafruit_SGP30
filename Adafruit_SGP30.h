/**************************************************************************/
/*! 
    @file     Adafruit_SGP30.h
    @author   ladyada(Adafruit Industries)
	@license  BSD (see license.txt)
	
	This is a library for the Adafruit SGP30 Gas Sensor breakout board
	----> http://www.adafruit.com/products/xxxx
	
	Adafruit invests time and resources providing this open source code, 
	please support Adafruit and open-source hardware by purchasing 
	products from Adafruit!

	@section  HISTORY
*/
/**************************************************************************/

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#include <Wire.h>

// the i2c address
#define SGP30_I2CADDR_DEFAULT 0x58

// commands and constants
#define SGP30_FEATURESET       0x0020
#define SGP30_CRC8_POLYNOMIAL  0x31
#define SGP30_CRC8_INIT        0xFF
#define SGP30_WORD_LEN         2


class Adafruit_SGP30 {
 public:
  Adafruit_SGP30();
  boolean begin(void);
  boolean IAQinit(void);
  boolean IAQmeasure(void);

  uint16_t TVOC, eCO2;
  uint16_t serialnumber[3];
 private:
  uint8_t _i2caddr;

  void write(uint8_t address, uint8_t *data, uint8_t n);
  void read(uint8_t address, uint8_t *data, uint8_t n);
  boolean readWordFromCommand(uint8_t command[], uint8_t commandLength, uint16_t delay, uint16_t *readdata = NULL, uint8_t readlen = 0);
  uint8_t generateCRC(uint8_t data[], uint8_t datalen);
};
