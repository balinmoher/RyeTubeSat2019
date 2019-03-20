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
