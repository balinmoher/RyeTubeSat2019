#include <SD.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include <stdlib.h>
#include <Adafruit_VC0706.h>
#include <avr/wdt.h> //Watchdog
#include <TimeLib.h>
#include <SparkFunLSM9DS1.h>
#include <SPI.h>

//These are the memory addresses of the IMU
//////////////////////////////////////////
#define LSM9DS1_M  0x1E // Would be 0x1C if SDO_M is LOW
#define LSM9DS1_AG  0x6B // Would be 0x6A if SDO_AG is LOWg

//Data variables
//////////////////////////////////////////
float data[16];
float Vbat;
int batteryADC = A3; // Analog pin 3
int dataFileNum = 0; //counts which data file is current

//Flags for command checks
//////////////////////////////////////////
bool requestData = false;
bool requestOldData = false;
bool requestPicture = false;
bool changeSensorTimer = false;
bool changeCamTimer = false;
String commandNum = "0"; //used to store numbers entered in command check

//Time variables
//////////////////////////////////////////
unsigned long currentTime;
unsigned long snapshotTime_sens = 0; //the last time the sensors were read
unsigned long snapshotTime_cam = 0; //the last time a pic was taken
unsigned long SensorActionTime = 2000;  // Interval time for sensors
unsigned long unixTimeStamp = 0; // Saves uniix time stamp entered
unsigned long TimeStampInitate = 0; // Get millis when time stamp was entered
unsigned long CameraActionTime = 60000; // Interval time for camera
time_t unix_Time = 0; // Set up varibales used for time stamp
LSM9DS1 imu;

/********************************************************************************
  DEFINES
*******************************************************************************/
//PIN DEFINITION
//DIGITAL PINS:
//      1 = EMPTY
//      2 = RX (ARDUINO) TO TX (CAMERA) (softwareserial, defined in Snapshot.ino)
//      3 = TX (ARDUINO) TO RX (CAMERA) (softwareserial, defined in Snapshot.ino)
//      4 = EMPTY
//      5 = EMPTY
//      6 = EMPTY
//      7 = EMPTY
//      8 = CHIPSELECT FOR SD CARD
//      9 = CHIPSELECT FOR CURRENT SENSOR     
//      10 = EMPTY  
//      11 = SDI FOR SD CARD AND CURRENT SENSOR
//      12 = SDO FOR SD CARD AND CURRENT SENSOR
//      13 = SCK FOR SD CARD AND CURRENT SENSOR
//ANALOG PINS:
//      A0 = INSIDE TEMPERATURE SENSOR
//      A1 = OUTSIDE TEMPERATURE SENSOR
//      A2 = LUX SENSOR
//      A3 = EMPTY
//      A4 = EMPTY
//      A5 = EMPTY
//      A6 = EMPTY
//      A7 = EMPTY

void setup() {

  Serial.begin(9600);
  Wire.begin();

  //Call IMU setup function before running
  IMUv2Setup();

  //Set up SD card
  Serial.print(F("Initializing SD card..."));
  if (!SD.begin(8)) {
    Serial.println(F("initialization failed!"));
  }
  Serial.println(F("initialization done."));
  
  //this is disabled because we are using Nano with old bootloader
  //wdt_enable(WDTO_8S); // Enable watchdog for 8 second timer 
  delay(1000);

}


/*********************************
  1: Take sensor readings and save to SD if the time is over the interval
  2: Take a picture and save it to the SD if time is over the interval
  3: Check for command inputs and set flags to execute them
  4: Execute commands
**********************************/
void loop() {
  wdt_reset(); // Reset the watchdog every loop iteration or when command is issued
  unsigned long currentCounter = millis(); //grab current time to be compared against intervals

  //**************
  //  Update sensor info if its been over the interval time
  //  Save it to the SD card
  //***************
  if ((unsigned long)(currentCounter - snapshotTime_sens ) >= SensorActionTime) { // Check for rollover --> if the timer has reset because it resets every 49.7 days or so
    snapshotTime_sens = floor(millis() / 1000); // Grab time action ended - the floor is used to fix any errors that might constantly add up
    snapshotTime_sens = snapshotTime_sens * 1000;
    getVBat();  // Check battery voltage

    if (Vbat > 0) { // Make sure that the battery wont get discharged due to battery being to low
      
      //read sensors and store data
      getData();
      saveData(dataFileNum);
      wdt_reset();
    }
  }

  //**************
  //  Take a picture if its been over the interval time
  //  Save it to the SD card
  //***************
  currentCounter = millis(); // Grab current time
    if ((unsigned long)(currentCounter - snapshotTime_cam) >= CameraActionTime) { // Check for rollover
        snapshotTime_cam = floor(millis() / 1000); // Grab time action ended - the floor is used to fix any errors that might constantly add up
        snapshotTime_cam = snapshotTime_cam * 1000;
      
      getVBat();      // Check battery voltage
      if (Vbat > 0) { // Make sure that the battery wont get discharged due to battery being to low
        snapshot();   // Take a picture and save it
        wdt_reset(); 
      }
    }



   /*******************************************************
  Command check
  
    Check for inputs from ground station and perform actions based on what was entered
      DXX`= send data file denoted by XX where XX are numbers
      SXX = change sensor timer to XX where XX are numbers
      CXX = change camera timer to XX where XX are numbers
      PXX = send picture denoted by XX where XX are numbers

  IMPORTANT: This runs asynchronously - meaning it interrupts anything going on to
             run, including when a file is in the middle of writing.
             For this reason we set flags here only for later execution.
  *******************************************************/
  if (Serial.available()) { //check if anything to read

    //**************
    //  Get input string
    //  Get keyword letter at beginning
    //  Get number associated with it
    //  Remove newline at end of number
    //***************
    String peraction = Serial.readString(); //look for inputs
    String getNum = peraction;
    getNum.remove(0, 1); // Grab the number send with number if one
    getNum.remove(getNum.length() - 1);
    peraction.remove(1); // Grab letter that was sent
    

    //**************
    //  Send data file - DXX
    //  XX is number of file
    //  Set Flag
    //***************
    if (peraction == "D") {
      requestOldData = true;
      commandNum = getNum;
      wdt_reset(); //watchdog timer
    }
  
    //**************
    //  Send photo file - PXX
    //  XX is number of file
    //  Set Flag
    //***************
    if (peraction == "P") {
      //XX is number of photo on SD
      requestPicture = true;
      commandNum = getNum;
      wdt_reset();
    }


    //**************
    //  Change frequency the sensors are read - SXX
    //  XX is number of seconds between readings
    //  Set Flag
    //***************
    if (peraction == "S") {
      changeSensorTimer = true;
      commandNum = getNum;
      wdt_reset();
    }

    //**************
    //  Change frequency the photos are taken - CXX
    //  XX is number of seconds between photos
    //  Set Flag
    //***************
    if (peraction == "C") {
      changeCamTimer = true;
      commandNum = getNum;
      wdt_reset();
    }
    
  }

    //**************
    //  If a data file is to be read: 
    //    Concatenate a string based on the number entered in command
    //    Store this string in PROGMEM so as not to take up local space
    //    Send the data file
    //    Increment the number of the file which is being written to ONLY IF
    //        the file just read was the current file
    //***************
  if (requestOldData) {
    wdt_reset();
    PROGMEM String oldfileToSend = "D" + String(commandNum) + ".txt"; //concat name of file
    Serial.println(oldfileToSend);
    
    sendDataFile(oldfileToSend);             //send the data file
    requestOldData = false;                  //reset flag
    
    if (commandNum.toInt() == dataFileNum) { //increment file to be written to
      dataFileNum++;
    }
 }


    //**************
    //  If a photo file is to be read: 
    //    Concatenate a string based on the number entered in command
    //    Send the data file
    //***************
  if (requestPicture) {
    wdt_reset();
     char fileToRead[13] = "IMAGE00.JPG"; //Set file name
     fileToRead[5] = commandNum.charAt(0);
     fileToRead[6] = commandNum.charAt(1);
     fileToRead[12] = 0;                  //Set null char at end of string
     Serial.println(fileToRead);

     readSD(fileToRead);                  //Send the file
     requestPicture = false;              //reset the flag
    }


    //**************
    //  If the sensor timer is to be changed: 
    //    Set the new interval
    //    Update the snapshot time - time since last reading
    //    This is done to take a measurement after XX seconds have passed instead of less
    //***************
  if (changeSensorTimer) {
    if (commandNum.toInt() * 1000 > 1000) {
        SensorActionTime =  commandNum.toInt() * 1000; // Get num is time put after, convert to milliseconds
        snapshotTime_sens = millis();                  // Next sensor reading will take place once this is done
        wdt_reset();
      }
     changeSensorTimer = false;                        //reset flag
    }

    //**************
    //  If the camera timer is to be changed: 
    //    Set the new interval
    //    Update the snapshot time - time since last photo
    //    This is done to take a photo after XX seconds have passed instead of less
    //***************
  if (changeCamTimer) {
    
    if (commandNum.toInt() * 1000 > 1000) {
        CameraActionTime =  commandNum.toInt() * 1000; // Get num is time put after, convert to milliseconds       
        snapshotTime_cam = millis();                   // Next sensor reading will take place once this is done
        wdt_reset();
      }
     changeCamTimer = false;                           //reset flag
    }  
} 
