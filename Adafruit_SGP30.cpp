/**************************************************************************/
/*! 
    @file     Adafruit_SGP30.cpp
    @author   ladyada (Adafruit Industries)
	@license  BSD (see license.txt)
	
	This is a library for the Adafruit SGP30 Gas Sensor breakout board
	----> http://www.adafruit.com/products/3709
	
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

#include "Adafruit_SGP30.h"

#define I2C_DEBUG

/**************************************************************************/
/*! 
    @brief  Instantiates a new SGP30 class
*/
/**************************************************************************/
Adafruit_SGP30::Adafruit_SGP30() {
}

/**************************************************************************/
/*! 
    @brief  Setups the hardware and detects a valid SGP30. Initializes Wire
    then reads the serialnumber and checks that we are talking to an SGP30
    @returns True if SGP30 found on I2C, False if something went wrong!
*/
/**************************************************************************/
boolean Adafruit_SGP30::begin(void) {
  _i2caddr = SGP30_I2CADDR_DEFAULT;
  Wire.begin();

  
  uint8_t command[2];
  command[0] = 0x36;
  command[1] = 0x82;
  if (! readWordFromCommand(command, 2, 10, serialnumber, 3)) 
    return false;

  uint16_t featureset;
  command[0] = 0x20;
  command[1] = 0x2F;
  if (! readWordFromCommand(command, 2, 10, &featureset, 1)) 
    return false;
  //Serial.print("Featureset 0x"); Serial.println(featureset, HEX);
  if (featureset != SGP30_FEATURESET) 
    return false;
  if (! IAQinit()) 
    return false;

  return true;
}

/**************************************************************************/
/*! 
    @brief  Commands the sensor to begin the IAQ algorithm. Must be called after startup.
    @returns True if command completed successfully, false if something went wrong!
*/
/**************************************************************************/
boolean Adafruit_SGP30::IAQinit(void) {
  uint8_t command[2];
  command[0] = 0x20;
  command[1] = 0x03;
  return readWordFromCommand(command, 2, 10);
}

/**************************************************************************/
/*! 
    @brief  Commands the sensor to take a single eCO2/VOC measurement. Places results in {@link TVOC} and {@link eCO2}
    @returns True if command completed successfully, false if something went wrong!
*/
/**************************************************************************/
boolean Adafruit_SGP30::IAQmeasure(void) {
  uint8_t command[2];
  command[0] = 0x20;
  command[1] = 0x08;
  uint16_t reply[2];
  if (! readWordFromCommand(command, 2, 10, reply, 2))
    return false;
  TVOC = reply[1];
  eCO2 = reply[0];
  return true;
}
 

/**************************************************************************/
/*! 
    @brief  I2C low level interfacing
*/
/**************************************************************************/


boolean Adafruit_SGP30::readWordFromCommand(uint8_t command[], uint8_t commandLength, uint16_t delayms, uint16_t *readdata, uint8_t readlen)
{
  uint8_t data;

  Wire.beginTransmission(_i2caddr);

#ifdef I2C_DEBUG
  Serial.print("-> ");
#endif

  for (uint8_t i=0; i<commandLength; i++) {
    Wire.write(command[i]);
#ifdef I2C_DEBUG
    Serial.print("0x"); Serial.print(command[i], HEX); Serial.print(", ");
#endif
  }
#ifdef I2C_DEBUG
  Serial.println();
#endif
  Wire.endTransmission();

  delay(delayms);

  if (readlen == 0) 
    return true;

  uint8_t replylen = readlen * (SGP30_WORD_LEN +1);
  if (Wire.requestFrom(_i2caddr, replylen) != replylen) 
    return false;
  uint8_t replybuffer[replylen];
#ifdef I2C_DEBUG
  Serial.print("<- ");
#endif  
  for (uint8_t i=0; i<replylen; i++) {
    replybuffer[i] = Wire.read();
#ifdef I2C_DEBUG
    Serial.print("0x"); Serial.print(replybuffer[i], HEX); Serial.print(", ");
#endif
  }

#ifdef I2C_DEBUG
  Serial.println();
#endif

  for (uint8_t i=0; i<readlen; i++) {
    uint8_t crc = generateCRC(replybuffer+i*3, 2);
#ifdef I2C_DEBUG
    Serial.print("CRC calced: 0x"); Serial.print(crc, HEX);
    Serial.print(" vs. 0x"); Serial.println(replybuffer[i * 3 + 2], HEX);
#endif
    if (crc != replybuffer[i * 3 + 2])
      return false;
    // success! store it
    readdata[i] = replybuffer[i*3];
    readdata[i] <<= 8;
    readdata[i] |= replybuffer[i*3 + 1];
#ifdef I2C_DEBUG
    Serial.print("Read: 0x"); Serial.println(readdata[i], HEX);
#endif
  }
  return true;
}

uint8_t Adafruit_SGP30::generateCRC(uint8_t *data, uint8_t datalen) {
  // calculates 8-Bit checksum with given polynomial
  uint8_t crc = SGP30_CRC8_INIT;

  for (uint8_t i=0; i<datalen; i++) {
    crc ^= data[i];
    for (uint8_t b=0; b<8; b++) {
      if (crc & 0x80)
	crc = (crc << 1) ^ SGP30_CRC8_POLYNOMIAL;
      else
	crc <<= 1;
    }
  }
  return crc;
}
