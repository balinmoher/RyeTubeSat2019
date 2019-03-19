 /*****************************************************************
   TubeSat Main software
 *****************************************************************/

/********************************************************************************
   Includes
 *******************************************************************************/
///FOR TIME STAMP GO TO http://www.unixtimestamp.com/index.php;
#include <Wire.h>
#include <TimeLib.h> //remember to include this library !!!!//http://playground.arduino.cc/Code/time or http://playground.arduino.cc/Code/DateTime
#include <math.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_VC0706.h>
#include <SoftwareSerial.h>
//#include <HMC5883L.h>
#include <SPI.h>
#include <SparkFunLSM9DS1.h>
#include <avr/sleep.h> //The two sleep libraries
#include <avr/power.h>
#include <avr/wdt.h> //Watchdog
#include <stdlib.h>
#include <SD.h>

File myFile;

//HMC5883L compass;
//LSM9DS1 imu;

/********************************************************************************
  DEFINES
*******************************************************************************/
#define camera_rx 3 // Digital pin 4
#define camera_tx 2 // Digital pin 3

// SDO_XM and SDO_G are both pulled high, so our addresses are:
#define LSM9DS1_M  0x1E // Would be 0x1C if SDO_M is LOW
#define LSM9DS1_AG  0x6B // Would be 0x6A if SDO_AG is LOWg

//int GX, GY, GZ;

#if ARDUINO >= 100 // Determines the version of the arduino IDE being used --> 100 is version 1
#define WIRE_SEND(b) Wire.write((byte) b)
#define WIRE_RECEIVE() Wire.read()
#else
#define WIRE_SEND(b) Wire.send(b)
#define WIRE_RECEIVE() Wire.receive()
#endif

/********************************************************************************
  Declaration of Global Variables
*******************************************************************************/
//float gyro[3];  // makes array of size 3
int num_gyro_errors = 0;
boolean output_errors = false;

// Used to store values from sensors
int sizeData = 14; // Starting at 0
float data[14];

// Data read off eepprom
//float readData[2][14]; // EEPROM1 --> 2x36 matrix

// Create packets
float stringData[14]; // Used to store data
int currentPacket = 1;// Check which string packet it is
float stringData_Vbat; // Used for battery cause want it a decimal
int packetNumber = 0; // Saves packetnumber
int sentHardwarestr = 1; // Used to turn on/off hardware check, used when resending data

// Used for battery voltage
float Vbat; // Battery voltage saved here
double  minVoltageSens = 0; // Minimum voltage of battery that sensor or camera data will be taken
double  minVoltageCam = 0; // Minimum voltage of battery that sensor or camera data will be taken

//record number of cycles
int cycles = 0;
int saveCycle = 0;
int hardwareStringcnt = 0; // Used to check when time to send hardware check string

//Timer
unsigned long currentTime;
unsigned long checkTime = 0;
unsigned long snapshotTime_sens = 0;
unsigned long snapshotTime_cam = 0;
unsigned long SensorActionTime = 2000;  // Interval time for sensors
unsigned long CameraActionTime = 0; // Interval time
int cameraON = 1; // Toggle camera snap
unsigned long unixTimeStamp = 0; // Saves uniix time stamp entered
unsigned long TimeStampInitate = 0; // Get millis when time stamp was entered
time_t unix_Time = 0; // Set up varibales used for time stamp
unsigned long saveLastStamp = 0; // Used to print out time, so time doesnt keep adding up
int camTimeEntered = 0; // Used to change when the snapshotted action time is taken

// Pin for checking battery voltage
int batteryADC = A3; // Analog pin 3

ISR(WDT_vect) {
  wdt_disable();  // Disable watchdog
}

/*********************************************************************************
   Setup function
    effect: initalizes pins and drivers to be used by main loop
          - Also the power off function has lines below, which will safe the number of cycles to a space, which if the tubesat turns off, will read this address
           and write what has been privoulsy saved and "D" user input addedoutage, CODE CURRENTLY COMMENTED OUT becuase it would be annoying for any new users of the code
 *********************************************************************************/

void setup() {
  Serial.begin(9600);
  Wire.begin();
  delay(500);
  //Serial.println("SETUP INITIALIZED");

    //Serial.print("Initializing SD card...");

  //if (!SD.begin(4)) {
  //  Serial.println("initialization failed!");
  //  while (1);
  //}
  //Serial.println("initialization done.");
  
// IMUv2Setup();
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(9, OUTPUT);
  digitalWrite(4, LOW);//4,5,7 must be low for comms to transmit
  digitalWrite(5,LOW);
  digitalWrite(6,HIGH);
  digitalWrite(7,LOW);
  digitalWrite(9,HIGH);
  //digitalWrite(10,LOW);
  pinMode(batteryADC, INPUT); // Used to check battery voltage
  //CameraSetup(); // Set up camera, pseudo-function
  setupComms();
  wdt_enable(WDTO_8S); // Enable watchdog for 8 second timer
  delay(1000);
}

/*********************************************************************************
   loop function

    purpose: Call various functions, track inputs of users, determine when sensor and camera data should be taken
    Notes:
    - minimum voltage function implemented that sensor or camera data will be not be taken if battery voltage is below a certain value
    - minimum sensor and camera interval times that these intervals cant be set too, incase of input error
    - when changing interval times, these new intervals will begin when the interval time is sent and not continue from the previous interval time
    - Letters - D: send data, R: resend data after being sent, SXXX: change sensor interval (currenlty in seconds), CXXX: enter camera interval
               (either unix time stamp or place interval time,  image captured only once per cycle, TXXX: sent time stamp from website listed below)
    - http://www.unixtimestamp.com/index.php :use this website for getting timestamp (dont make changes to the number, code will put it into Eastern time zone. Also
                                              when entering times for camera using CXXX, input timestamp  in eastern time into website in 24 hour time).
 *********************************************************************************/

void loop() {
  wdt_reset(); // Reset the watchdog every loop iteration or when command is issued
  unsigned long currentCounter = millis(); //grab current time

  //used for sensors
  if ((unsigned long)(currentCounter - snapshotTime_sens ) >= SensorActionTime) { // Check for rollover --> if the timer has reset because it resets every 49.7 days or so
    snapshotTime_sens = floor(millis() / 1000); // Grab time action ended - the floor is used to fix any errors that might constantly add up
    snapshotTime_sens = snapshotTime_sens * 1000;
  getVBat();  // Check battery voltage

    if (Vbat > minVoltageSens) { // Nake sure that the battery wont get discharged due to battery being to low
      cycles = cycles + 1; // Increment number of cycles

      T1Write(); // Get data from various sensors
      T2Write();
      P1Write();
//      IMUv2Call();
      getVBat();
//      getTime();  // Get time action commenced
      
      
//WRITE TO SD HERE
    }
  }

  //used for Camera
  currentCounter = millis(); // Grab current time
  if (cameraON == 1) { // Check if want to snap photo --> When first run this is already set to one
    if ((unsigned long)(currentCounter - snapshotTime_cam) >= CameraActionTime) { // Check for rollover
    getVBat();  // Check battery voltage
      if (Vbat > minVoltageCam) { // Make sure that the battery wont get discharged due to battery being to low
        //CAMERA CODE
        
        readSolarCurrent(); //TODO REMOVE THIS LATER
        
        snapshot(); // Placeholder function for taking a picture, tbd
        cameraON = 0;  // Snap only 1 photo per cycle
        wdt_reset();
      }
    }
  }



  if (Serial.available()) { //check if anything to read
    String peraction = Serial.readString(); //look for inputs
    String getNum = peraction;
    getNum.remove(0, 1); // Grab the number send with number if one
    peraction.remove(1); // Grab letter that was sent

    // Read data, send packets
    if (peraction == "D") {
      saveCycle = cycles; // save number of increments of cycles
      int address_to_read = packetNumber * 4 * sizeData;
      for (int i = 0; i <= saveCycle; i++) {
        // Something here turns the radio off
//        readEEPROM(address_to_read); // Read data from EEPROM
//        combineDat(i);
//        Serial.write(10);
        wdt_reset();
      }

//      hardwareTest();
      cycles = 0; //reset data

      // if you want the camera packet printed
      // createCameraPacket();

      wdt_reset(); //watchdog timer
    }

    // Resend data
    if (peraction == "R") {
      sentHardwarestr = 0; // Turn off hardware check since data already saved
      packetNumber = packetNumber - saveCycle - 1; // Reset packet numer to what it was previously
      int address_to_read = saveCycle * sizeData;
      for (int i = 0; i <= saveCycle; i++) {
//        readEEPROM(address_to_read); // Read data from EEPROM
//        combineDat(i); //interpuret data to be sent
        wdt_reset();
      }

      hardwareTest();
      sentHardwarestr = 1; // Turn hardware check back on
      hardwareStringcnt = 0;
      packetNumber++;
    }

    // Change sensor timer
    //SXXX:  put in SXX to change sensor timer, xxx are numbers
    if (peraction == "S") {
      if (getNum.toInt() * 1000 > 1000) {
        SensorActionTime =  getNum.toInt() * 1000; // Get num is time put after, convert to milliseconds
        snapshotTime_sens = millis(); // Next sensor reading will take place once this is done
        Serial.println("Hello World");
        wdt_reset();
      }
    }

    // Change camera timer
    // Cxxx to change camera,xxx are numbers, xxx are numbers
    if (peraction == "C") {
      // Determine if lower or higher
      if (getNum.toInt() * 1000 > 1000 && getNum.toInt() < 1490998951) { // To ensure that if changing above a certain value CHANGE IF CHANGING IMPUT NUMER
        // The 1490998951 is a unix time stamp of the day this was done, time stamp cant be lower
        CameraActionTime = getNum.toInt() * 1000; // Get num is time put after, convert to milliseconds
        snapshotTime_cam = millis(); // Grab time action ended
        wdt_reset();
      } else if (getNum.toInt() > 1490998951) {
        unsigned long currentT = millis() - TimeStampInitate  +  unixTimeStamp * 1000; // Record how much time has passed since last time
        CameraActionTime = (getNum.toInt() - now()) * 1000; // Get action time if unix time stamp
        snapshotTime_cam = millis(); // Grab time action was completed
        wdt_reset();
      }
      camTimeEntered = 1; //Set interval time
    }

    // Add or change the time stamp
    if (peraction == "T") {
      //-14400 used to put into Eastern Time
      unix_Time = getNum.toInt() - 14400; // Get time stamp
      setTime(unix_Time); // Set the time using the inputting time stamp
      unixTimeStamp = getNum.toInt() - 14400; // Save time stamp to use for interval times
      TimeStampInitate = millis(); // Record when timestamp was entered
      Serial.println("T");
      wdt_reset();
    }
  }
}
