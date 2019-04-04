///*********************************************************************************
//   Time Function
//
//    purpose : Converts UNIX time to dd:hh:mm:ss
//    inputs: Call to action
//    outputs: Time that the action was called (used to know when sensor/camera data was taken or strings sent)
// *********************************************************************************/
//
//void getTime() {
//  unix_Time = millis() / 1000 +  unixTimeStamp -  TimeStampInitate / 1000; // record how much time has passed
//  data[11] = day(unix_Time);
//  data[12] = hour(unix_Time);
//  data[13] = minute(unix_Time);
//  data[14] = second(unix_Time);
//}
