/*********************************************************************************
   Temperature Sensor 1 Function

    purpose : implements the main procedure of taking 4 consecutive temperature readings
    and taking the average of these 4 temperatures to give a more accurate temperature
    reading.
    inputs: Call to action
    outputs: Temperature reading for the temperature sensor one
    Units: Celsius

 *********************************************************************************/

void T1Write() { // reads and stores the temperature from sensor 1
  int tempT1Pin = A0;
  float voltage1;
  int T1 = analogRead(tempT1Pin);
//  float T2 = analogRead(tempT1Pin);
//  float T3 = analogRead(tempT1Pin);
//  float T4 = analogRead(tempT1Pin);
//  //T1 = (T1 + T2 + T3 + T4) / 4; // final temperature stored is the average of 4 temperature readings
  
  voltage1 = T1 * 5 / 1.0240;
  data[1] = 25 - ((750 - voltage1) / 100);

  Serial.println(F("T1:"));
  Serial.println(data[1]);
}

/*********************************************************************************
   Temperature Sensor 2 Function

    purpose : implements the main procedure of taking 4 consecutive temperature readings
    and taking the average of these 4 temperatures to give a more accurate temperature
    reading.
    inputs: Call to action
    outputs: Temperature reading for the temperature sensor one
    Units: Celsius

 *********************************************************************************/

void T2Write() { // reads and stores the temperature from sensor 2
  float voltage2;
  int tempT2Pin = A1;
  int T2 = analogRead(tempT2Pin);
//  float T1 = analogRead(tempT2Pin);
//  float T3 = analogRead(tempT2Pin);
//  float T4 = analogRead(tempT2Pin);
//  T2 = (T1 + T2 + T3 + T4) / 4;
  
  voltage2 = T2 * 5 / 1024;
  data[2] = 25 - ((750 - voltage2) / 100);

  Serial.print(F("Voltage: "));
  Serial.println(voltage2); 

  Serial.println(F("T2:"));
  Serial.println(data[2]);
}


/*********************************************************************************
   Battery Voltage Function

    purpose : gets voltage of the battery
    inputs: Call to action
    outputs: battery voltage
    
 *********************************************************************************/

void getVBat() {
  Vbat = analogRead(batteryADC) *  3.7 / 1023;  //gets battery voltage
  data[10] = Vbat; //inserts battery voltage into data array
  data[0] = 0;
}

/*********************************************************************************
   Lux Sensor  Function

    purpose : implements the main procedure of taking 6 lux readings and
    averaging the readings out.  Calibration was done previously.
    inputs: Call to action
    outputs: Lux value of respective environment
    Units: Lux (k-ohm)
 *********************************************************************************/

void P1Write() {
  int phRPin = 2;

  float P1 = analogRead(phRPin) * 5 / 1.024;  //reads lux values
  float P2 = analogRead(phRPin) * 5 / 1.024;
  float P3 = analogRead(phRPin) * 5 / 1.024;
  float P4 = analogRead(phRPin) * 5 / 1.024;
  float P5 = analogRead(phRPin) * 5 / 1.024;
  float P6 = analogRead(phRPin) * 5 / 1.024;

  P1 = (P1 + P2 + P3 + P4 + P5 + P6) / 6;  //averages 6 readings
  float lux = 5.5651 * exp(P1 * .0010110);

  float lux_1 = lux;  //converts lux values and interts into data array
  float lux_2 = lux;
  data[3] = lux_1;

  Serial.println(F("LUX:"));
  Serial.println(data[3]);
}

/*********************************************************************************
   IMU Function

    purpose : Get gyroscope and magnetometer data
    inputs: Call to action
    outputs: Gyroscope and magnetometer sensor values

 *********************************************************************************/

int factor = 100;
void IMUv2Setup() {
  
  // Before initializing the IMU, there are a few settings
  // we may need to adjust. Use the settings struct to set
  // the device's communication mode and addresses:
  imu.settings.device.commInterface = IMU_MODE_I2C;
  imu.settings.device.mAddress = LSM9DS1_M;
  imu.settings.device.agAddress = LSM9DS1_AG;
  // The above lines will only take effect AFTER calling
  // imu.begin(), which verifies communication with the IMU
  // and turns it on.

  if (!imu.begin())
  {
    Serial.println(F("Failed to communicate with LSM9DS1."));
  }
}

void IMUv2Call() {
  
  imu.readGyro();
  float GX = (float)((imu.gx)) / factor;
  float GY = (float)((imu.gy)) / factor;
  float GZ = (float)((imu.gz)) / factor;

  data[4] = GX;
  data[5] = GY;
  data[6] = GZ;

  imu.readMag();
  float MX = (float)(imu.mx);
  float MY = (float)(imu.my);
  float MZ = (float)(imu.mz);

  // Calculate the attitude (rad)
  float Zanglerad = atan2(MX, MY);
  float Xanglerad = atan2(MY, MZ);
  float Yanglerad = atan2(MZ, MX);

  // Correction angles
  if (Zanglerad < 0) {
    Zanglerad += 2 * PI;
  }
  if (Zanglerad > 2 * PI) {
    Zanglerad -= 2 * PI;
  }
  //X angles correction
  if (Xanglerad < 0) {
    Xanglerad += 2 * PI;
  }
  if (Xanglerad > 2 * PI) {
    Xanglerad -= 2 * PI;
  }
  //Y angle corrections
  if (Yanglerad < 0) {
    Yanglerad += 2 * PI;
  }
  if (Yanglerad > 2 * PI) {
    Yanglerad -= 2 * PI;
  }

  float ZangleDegrees = Zanglerad * 180 / M_PI;
  float YangleDegrees = Yanglerad * 180 / M_PI;
  float XangleDegrees = Xanglerad * 180 / M_PI;

  data[7] = XangleDegrees;
  data[8] = YangleDegrees;
  data[9] = ZangleDegrees;
}

///*********************************************************************************
//   Time Function
//
//    purpose : Converts UNIX time to dd:hh:mm:ss
//    inputs: Call to action
//    outputs: Time that the action was called (used to know when sensor/camera data was taken or strings sent)
// *********************************************************************************/

void getTime() {
  unix_Time = millis() / 1000 +  unixTimeStamp -  TimeStampInitate / 1000; // record how much time has passed
  data[12] = day(unix_Time);
  data[13] = hour(unix_Time);
  data[14] = minute(unix_Time);
  data[15] = second(unix_Time);
}


///*********************************************************************************
//   Current Sensor Function
//
//    purpose : Measures current
//    inputs: Call to action
//    outputs: current
// *********************************************************************************/

void readSolarCurrent() {
  
  const int chipSelectPin = 9;
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
  
  // return the result:
  data[11] = result;
}
