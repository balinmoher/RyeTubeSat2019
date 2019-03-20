/*********************************************************************************
    Hardware Check Function

    purpose : implements the main procedure of checking the hardware of the sensors
    inputs: Prompt to check over sensors
    outputs: A string that lets the ground station know that all sensors are operating properlly
 *********************************************************************************/

// Create sensor check string
void hardwareTest() {
  String checkMss = "M"; // Hardware check
  String delim = ":"; // Used to distinguish between sensor values
  String betweenTime = ";"; // Used to distinguish between time values
  if (sentHardwarestr == 1) { // Used to know when resending data since dont want to redo check when resending data
    packetNumber++;
      
    String sensorCheck = pinTesting(); // Call function to check over pins to ensure they are functional

    unix_Time = millis() / 1000 +  unixTimeStamp -  TimeStampInitate / 1000; // Record how much time has passed since last time
    String mday = String(day(unix_Time));
    String mhour = String(hour(unix_Time));
    String mmin = String(minute(unix_Time));
    String msec = String(second(unix_Time));

    Vbat = analogRead(batteryADC) * 4.2 / 1023; // Check battery voltge

    Serial.print(packetNumber); Serial.print(delim); Serial.print(checkMss); Serial.print(delim); // Header
    Serial.print(currentPacket + 1); Serial.print("-"); Serial.print(saveCycle + 1); Serial.print(delim);

    Serial.print(mday); Serial.print(betweenTime); Serial.print(mhour); Serial.print(betweenTime); // Current time
    Serial.print(mmin); Serial.print(betweenTime); Serial.print(msec); Serial.print(delim);

    Serial.print(Vbat); Serial.print(delim); // Batter and panel voltages
    Serial.print(sensorCheck); Serial.write(13); Serial.println(" ");

    hardwareStringcnt = 0;
    currentPacket = 1; //Reset packet numbers
  }
}

String pinTesting() {
  // Declare Pins of various sensors
  int PTT = 5; // digital pin 5
  int TRCVR = 6; // digital pin 6
  int AMP = 7; // digital pin 7
  int PIC = 9; // digital pin 9
  // 5 and 7 need to be HIGH to receive
  int batteryADC = A3; // analog pin 3
  int IMU_SDA = A4; // analog pin 4
  int IMU_SCL = A5; // analog pin 5
  int light_pin = A2; // analog pin 2
  int temp_pin1 = A1; // analog pin 1
  int temp_pin2 = A0; // analog pin 0

  //payload pins become inputs
  pinMode(camera_rx, INPUT);
  pinMode(camera_tx, OUTPUT);
  pinMode(light_pin, INPUT);
  pinMode(temp_pin1, INPUT);
  pinMode(temp_pin2, INPUT);
  pinMode(batteryADC, INPUT);

  // Radio pins
  pinMode(TRCVR, OUTPUT); //Chip select
  pinMode(PIC, OUTPUT);
  pinMode(AMP, OUTPUT);

  // create variables to use in the hardware check
  bool vall = LOW;
  bool valt = LOW;
  bool valt2 = LOW;
  bool valr = LOW;
  bool vali = LOW;

  // Variables to use for loops
  int m = 0;
  int n = 0;
  int o = 0;
  int p = 0;
  int q = 0;
  int r = 0;

  if (m < 2) { // light pin check this loop checks if the pin is on
    if (digitalRead (light_pin) == LOW) { // if the pin is on vall becomes high
      vall = HIGH; // this value is checked later. When vall is read as high it means the pin did turn on
    }
    else {
      digitalWrite(light_pin, LOW); //if the pin was not on this should trun the pin on
    }
    m = m + 1; // loops to check the pin a second time
  }

  if (n < 2) { //temp pin check
    if (digitalRead(temp_pin1) == LOW) {
      valt = HIGH;
    }
    else {
      digitalWrite(temp_pin1, LOW);
    }
    n = n + 1;
  }

  if (o < 2) { // temp pin 2 check
    if (digitalRead (temp_pin2) == LOW) {
      valt2 = HIGH;
    }
    else {
      digitalWrite(temp_pin2, LOW);
    }
    o = o + 1;
  }

  //Radio check and turn off transmitter
  if (digitalRead (TRCVR) == HIGH) { // if the transmitter is on turn it off
    digitalWrite(TRCVR, LOW);
  }
  else {
    valr = LOW;
  }

  // IMU sensor check
  if (p < 2) { //this loop works similar to the previous loop for light pin
    if (digitalRead (IMU_SDA) == HIGH) {
      vali = HIGH;
    }
    else {
      digitalWrite(IMU_SDA, HIGH);
    }
    p = p + 1;
  }

  if (q < 2) {// Imu sensor check
    if (digitalRead (IMU_SCL) == HIGH) {
      vali = HIGH;
    }
    else  {
      digitalWrite(IMU_SCL, HIGH);
    }
    q = q + 1;
  }
  // Camera check
  bool valrx = LOW;
  bool valtx = HIGH;
  if (r < 2) { // loop to check if camera is working
    if (digitalRead(camera_rx) == HIGH) { // if camera is on valrx represents that it is on
      valrx = HIGH;
    }
    else { // if camera was off turn it on
      digitalWrite(camera_rx, HIGH);
    }
    r = r + 1; // iterate again to make sure the camera turned on
  }
  if (digitalRead(camera_tx) == HIGH) {
    digitalWrite(camera_tx, LOW);
  }
  else {
    valtx = LOW;
  }

  // Faulty Sensor Check
  int val_check[5] = {vall, valt, valt2, vali, valrx};
  String val_sensor[5] = {"P", "T1", "T2", "IMU", "C"};
  String sensorCheck = ""; // sensor used to create sensor check string
  String val_msg = ""; // string used to determine if the sensors are working or not
  for (int i = 0; i < 5; i++) { // iterate through the 5 variables
    if (val_check[i] = LOW) {
      val_msg = val_sensor[i] + String("PR"); // PR for problem. If the sensors are not on at this point there was a problem. PR defines which sesnors are not working
    }
    else {
      val_msg = val_sensor[i] + String("WO"); // WO for working order. if the sensors did turn on then WO will show the user that the sesnor is properly working
    }
    sensorCheck = sensorCheck + val_msg;
  }
  return (sensorCheck); //return created string
}
