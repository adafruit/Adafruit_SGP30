#include <Wire.h>
#include "Adafruit_SGP30.h"

Adafruit_SGP30 sgp;

void setup() {
  Serial.begin(115200);
  Serial.println("SGP30 test");

  if (! sgp.begin()){
    Serial.println("Sensor not found :(");
    while (1);
  }
  Serial.print("Found SGP30 serial #");
  Serial.print(sgp.serialnumber[0], HEX);
  Serial.print(sgp.serialnumber[1], HEX);
  Serial.println(sgp.serialnumber[2], HEX);
}


void loop() {
  sgp.IAQmeasure();
  Serial.print("TVOC "); Serial.print(sgp.TVOC); Serial.print("\t");
  Serial.print("eCO2 "); Serial.println(sgp.eCO2);
  delay(1000);
}