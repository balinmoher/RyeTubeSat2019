/*********************************************************************************
//   Time Function
//
//    purpose : Converts UNIX time to dd:hh:mm:ss
//    inputs: Call to action
//    outputs: Time that the action was called (used to know when sensor/camera data was taken or strings sent)
// *********************************************************************************/

void getTime() {
  unix_Time = millis() / 1000 +  unixTimeStamp -  TimeStampInitate / 1000; // record how much time has passed
  data[0] = day(unix_Time);
  data[1] = hour(unix_Time);
  data[2] = minute(unix_Time);
  data[3] = second(unix_Time);
}



/*********************************************************************************
   Battery Voltage Function

    purpose : gets voltage of the battery
    inputs: Call to action
    outputs: battery voltage
    
 *********************************************************************************/

void getVBat() {
  Vbat = analogRead(batteryADC) *  3.7 / 1023;  //gets battery voltage
  data[4] = Vbat; //inserts battery voltage into data array
}

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
  
  voltage1 = T1 * 5 / 1.0240;
  data[5] = 25 - ((750 - voltage1) / 100);
}

///*********************************************************************************
//   Temperature Sensor 2 Function
//
//    purpose : implements the main procedure of taking 4 consecutive temperature readings
//    and taking the average of these 4 temperatures to give a more accurate temperature
//    reading.
//    inputs: Call to action
//    outputs: Temperature reading for the temperature sensor one
//    Units: Celsius
//
// *********************************************************************************/
//
//void T2Write() { // reads and stores the temperature from sensor 2
//  float voltage2;
//  int tempT2Pin = A1;
//  int T2 = analogRead(tempT2Pin);
//  
//  voltage2 = T2 * 5 / 1024;
//  data[6] = 25 - ((750 - voltage2) / 100);
//}

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

  data[6] = lux_1;

}

/*********************************************************************************
   IMU Function

    purpose : Get gyroscope and magnetometer data
    inputs: Call to action
    outputs: Gyroscope and magnetometer sensor values

 *********************************************************************************/

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
    //Serial.println(F("Failed to communicate with LSM9DS1."));
  }
}

void IMUv2Call() {

  //Read Gyroscope and store to data array
  imu.readGyro();
  float GX = (float)((imu.gx));
  float GY = (float)((imu.gy));
  float GZ = (float)((imu.gz));
  data[7] = GX;
  data[8] = GY;
  data[9] = GZ;

  //Read magnetometer
  imu.readMag();
  float MX = (float)(imu.mx);
  float MY = (float)(imu.my);
  float MZ = (float)(imu.mz);

  data[10] = MX;
  data[11] = MY;
  data[12] = MZ;

//  // Calculate the attitude (rad)
//  float Zanglerad = atan2(MX, MY);
//  float Xanglerad = atan2(MY, MZ);
//  float Yanglerad = atan2(MZ, MX);
//
//  // Correction angles
//  if (Zanglerad < 0) {
//    Zanglerad += 2 * PI;
//  }
//  if (Zanglerad > 2 * PI) {
//    Zanglerad -= 2 * PI;
//  }
//  //X angles correction
//  if (Xanglerad < 0) {
//    Xanglerad += 2 * PI;
//  }
//  if (Xanglerad > 2 * PI) {
//    Xanglerad -= 2 * PI;
//  }
//  //Y angle corrections
//  if (Yanglerad < 0) {
//    Yanglerad += 2 * PI;
//  }
//  if (Yanglerad > 2 * PI) {
//    Yanglerad -= 2 * PI;
//  }
//
//  //Calculate angles and store in data array
//  float ZangleDegrees = Zanglerad * 180 / M_PI;
//  float YangleDegrees = Yanglerad * 180 / M_PI;
//  float XangleDegrees = Xanglerad * 180 / M_PI;
//  data[14] = XangleDegrees;
//  data[15] = YangleDegrees;
//  data[16] = ZangleDegrees;

  //Read Accelerometer
  imu.readAccel();
  float AX = (imu.calcAccel(imu.ax));
  float AY = (imu.calcAccel(imu.ay));
  float AZ = (imu.calcAccel(imu.az));

  data[13] = AX;
  data[14] = AY;
  data[15] = AZ;

  
}


///*********************************************************************************
//   Current Sensor Function
//
//    purpose : Measures current
//    inputs: Call to action
//    outputs: current
// *********************************************************************************/

void readSolarCurrent() {
  
 // Received Bytes
unsigned int RB1;
unsigned int RB2;
unsigned int RB3;

// Command Bytes
// MSB, SEL2, SEL1, SEL0, SGL/DIF, UNI/BIP, PD1, PD0
const byte TB1[] = {B10001111, B11001111, B10011111, B11011111, B10101111, B11101111, B10111111, B11111111};
const byte TB2 = B00000000;
  

    //disable SPI device to start with
  digitalWrite(9, HIGH);
  // start the SPI library:
  SPI.begin();


//for (int i = 0; i < 8; i++) {
          // SPI conversation
          SPI.beginTransaction(SPISettings(50000, MSBFIRST, SPI_MODE0));
          digitalWrite(9, LOW);          // CS low
      //    RB1 = SPI.transfer(TB1[i]);
         RB1 = SPI.transfer(TB1[0]);
          RB2 = SPI.transfer(TB2);
          RB3 = SPI.transfer(TB2);
          digitalWrite(9, HIGH);         // CS high
          SPI.endTransaction();

          // Conversation results
          uint16_t RB_ans = (RB2 * 256 + RB3) >> 6;
          double V_sense = (double)RB_ans / 255.0 * 5 / 50.0;
          double current = V_sense / 1.5;
          //int y = i + 20;
          //data[y] = current;
          data[16] = current;
          delay(1);
//}
}
