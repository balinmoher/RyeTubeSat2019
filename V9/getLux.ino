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

  Serial.println("LUX:");
  Serial.println(data[3]);
}
