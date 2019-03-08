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
  float tempT1Pin = 1;
  float voltage1;
  float T1 = analogRead(tempT1Pin);
  float T2 = analogRead(tempT1Pin);
  float T3 = analogRead(tempT1Pin);
  float T4 = analogRead(tempT1Pin);
  T1 = (T1 + T2 + T3 + T4) / 4; // final temperature stored is the average of 4 temperature readings
  voltage1 = T1 * 5 / 1.0240;
//  data[1] = 25 - ((750 - voltage1) / 10);
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
  float tempT2Pin = 0;
  float T2 = analogRead(tempT2Pin);
  float T1 = analogRead(tempT2Pin);
  float T3 = analogRead(tempT2Pin);
  float T4 = analogRead(tempT2Pin);
  T2 = (T1 + T2 + T3 + T4) / 4;
  voltage2 = T2 * 5 / 1.0240;
//  data[2] = 25 - ((750 - voltage2) / 10);
}
