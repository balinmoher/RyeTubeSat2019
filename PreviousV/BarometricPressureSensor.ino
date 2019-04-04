/*
 SCP1000 Barometric Pressure Sensor Display

 Shows the output of a Barometric Pressure Sensor on a
 Uses the SPI library. For details on the sensor, see:
 http://www.sparkfun.com/commerce/product_info.php?products_id=8161
 http://www.vti.fi/en/support/obsolete_products/pressure_sensors/

 This sketch adapted from Nathan Seidle's SCP1000 example for PIC:
 http://www.sparkfun.com/datasheets/Sensors/SCP1000-Testing.zip

 Circuit:
 SCP1000 sensor attached to pins 6, 7, 10 - 13:
 DRDY: pin 6
 CSB: pin 7
 MOSI: pin 11
 MISO: pin 12
 SCK: pin 13

 created 31 July 2010
 modified 14 August 2010
 by Tom Igoe
 */

//Sensor's memory register addresses:
const int PRESSURE = 0x1F;      //3 most significant bits of pressure
const int PRESSURE_LSB = 0x20;  //16 least significant bits of pressure
const int TEMPERATURE = 0x21;   //16 bit temperature reading
const byte READ = 0b11111100;     // SCP1000's read command
const byte WRITE = 0b00000010;   // SCP1000's write command

// pins used for the connection with the sensor
// the other you need are controlled by the SPI library):
const int chipSelectPin = 9;

unsigned int readSolarCurrent() {
  
  pinMode(chipSelectPin, OUTPUT);
  
  // start the SPI library:
  SPI.begin();
  
  // take the chip select low to select the device:
  digitalWrite(chipSelectPin, LOW);
  
  // send a value of 0 to read the first byte returned:
  SPI.transfer(0b10000111);
  byte result1 = SPI.transfer(0x00);
  byte result2 = SPI.transfer(0x00);
  result1 = result1 << 2;
  result2 = result2 >> 6;
  byte result = result1 | result2;
  
  // take the chip select high to de-select:
  digitalWrite(chipSelectPin, HIGH);
delay(100);
Serial.println("RESULT 1");
  Serial.println(result1);
  delay(100);
  Serial.println("RESULT 2");
  Serial.println(result2);
  Serial.println(result);
  Serial.flush();
  
  // return the result:
  return (result);
}
