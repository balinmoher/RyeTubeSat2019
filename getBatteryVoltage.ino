/*********************************************************************************
   Battery Voltage Function
 *********************************************************************************/

void getVBat() {
  Vbat = analogRead(batteryADC) *  3.7 / 1023;  //gets battery voltage
//  data[10] = Vbat; //inserts battery voltage into data array
//  data[0] = 0;
}
