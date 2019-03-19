// This is a basic snapshot sketch using the VC0706 library.
// On start, the Arduino will find the camera and SD card and
// then snap a photo, saving it to the SD card.
// Public domain.

// If using an Arduino Mega (1280, 2560 or ADK) in conjunction
// with an SD card shield designed for conventional Arduinos
// (Uno, etc.), it's necessary to edit the library file:
//   libraries/SD/utility/Sd2Card.h
// Look for this line:
//   #define MEGA_SOFT_SPI 0
// change to:
//   #define MEGA_SOFT_SPI 1
// This is NOT required if using an SD card breakout interfaced
// directly to the SPI bus of the Mega (pins 50-53), or if using
// a non-Mega, Uno-style board.

//#include <Adafruit_VC0706.h>
//#include <SPI.h>
//#include <SD.h>

// comment out this line if using Arduino V23 or earlier
 #include <SoftwareSerial.h>         

// uncomment this line if using Arduino V23 or earlier
// #include <NewSoftSerial.h>       

// SD card chip select line varies among boards/shields:
// Adafruit SD shields and modules: pin 10
// Arduino Ethernet shield: pin 4
// Sparkfun SD shield: pin 8
// Arduino Mega w/hardware SPI: pin 53
// Teensy 2.0: pin 0
// Teensy++ 2.0: pin 20
//#define chipSelect 10

// Pins for camera connection are configurable.
// With the Arduino Uno, etc., most pins can be used, except for
// those already in use for the SD card (10 through 13 plus
// chipSelect, if other than pin 10).

// Using SoftwareSerial
SoftwareSerial cameraconnection = SoftwareSerial(2, 3);

Adafruit_VC0706 cam = Adafruit_VC0706(&cameraconnection);

// Using hardware serial on Mega: camera TX conn. to RX1,
// camera RX to TX1, no SoftwareSerial object is required:
//Adafruit_VC0706 cam = Adafruit_VC0706(&Serial1);
// When using hardware SPI, the SS pin MUST be set to an
  // output (even if not connected or used).  If left as a
  // floating input w/SPI on, this can cause lockuppage.
//#if !defined(SOFTWARE_SPI)
//#else
//  if(chipSelect != 10) pinMode(10, OUTPUT); // SS on Uno, etc.
//#endif
//

void snapshot() {

  Serial.println("VC0706 Camera snapshot test");

  // Try to locate the camera
  if (cam.begin()) {
    Serial.println("Camera Found:");
  } else {
    Serial.println("No camera found?");
    return;
  }
  // Print out the camera version information (optional)
  char *reply = cam.getVersion();
  if (reply == 0) {
    //Serial.print("Failed to get version");
  } else {
    //Serial.println("-----------------");
    //Serial.print(reply);
    //Serial.println("-----------------");
  }

  // Set the picture size - you can choose one of 640x480, 320x240 or 160x120 
  // Remember that bigger pictures take longer to transmit!
  
  cam.setImageSize(VC0706_640x480);        // biggest
  //cam.setImageSize(VC0706_320x240);        // medium
  //cam.setImageSize(VC0706_160x120);          // small

  // You can read the size back from the camera (optional, but maybe useful?)
  uint8_t imgsize = cam.getImageSize();
//  Serial.print("Image size: ");
//  if (imgsize == VC0706_640x480) Serial.println("640x480");
//  if (imgsize == VC0706_320x240) Serial.println("320x240");
//  if (imgsize == VC0706_160x120) Serial.println("160x120");

  Serial.println("Snap in 1 secs...");
  delay(1000);
  wdt_reset();
  
  if (! cam.takePicture()) {
    Serial.println("Failed to snap!");
  }else{ 
    Serial.println("Picture taken!");
  }
  // Create an image with the name IMAGExx.JPG
  char filename[13];
  strcpy(filename, "IMAGE00.JPG");
  for (int i = 0; i < 100; i++) {
    filename[5] = '0' + i/10;
    filename[6] = '0' + i%10;
    // create if does not exist, do not open existing, write, sync after write
    if (! SD.exists(filename)) {
      break;
    }
  }
  
  // Open the file for writing
  File imgFile = SD.open(filename, FILE_WRITE);

  // Get the size of the image (frame) taken  
  uint16_t jpglen = cam.frameLength();
  Serial.print("Storing ");
  Serial.print(jpglen, DEC);
  Serial.print(" byte image.");

  int32_t time = millis();
  pinMode(8, OUTPUT);
  // Read all the data up to # bytes!
  byte wCount = 0; // For counting # of writes
  while (jpglen > 0) {
    // read 32 bytes at a time;
    uint8_t *buffer;
    uint8_t bytesToRead = min(32, jpglen); // change 32 to 64 for a speedup but may not work with all setups!
    buffer = cam.readPicture(bytesToRead);
    imgFile.write(buffer, bytesToRead);
    
    if(++wCount >= 64) { // Every 2K, give a little feedback so it doesn't appear locked up
      wdt_reset();
      Serial.println(".");
      wCount = 0;
    }
    //Serial.print("Read ");  Serial.print(bytesToRead, DEC); Serial.println(" bytes");
    jpglen -= bytesToRead;
  }
  imgFile.close();

  time = millis() - time;
  Serial.println("done!");
  Serial.print(time); Serial.println(" ms elapsed");

  readSD(filename);
}


void readSD(String filename) {

  // Check if file exists
  if (!SD.exists(filename)) {
    Serial.println("File does not exist");
    return;
   }
  
  // Open the file for reading
  File imgFile = SD.open(filename);
  
    
  // Check if file opened
  if (imgFile) {
    
    unsigned long fileSize = imgFile.size();
    delay(100);
    Serial.println("Reading");
    Serial.println(fileSize);
    delay(20);
    
    
    //Read and print file to serial
     while (imgFile.position() < imgFile.size()) {
        wdt_reset();
        uint8_t mybuffer[32];
        uint8_t bytesToRead = min(32, fileSize);
    
        //Serial.write(imgFile.read()); 
        imgFile.read(mybuffer, bytesToRead); //read to buffer
        Serial.write(mybuffer, bytesToRead); //write to serial
        
        fileSize -= bytesToRead; //update byte count
    }
  } else { 
    Serial.println("Error: could not open file.");
    return;
  }

  Serial.flush(); //send remaining data before continuing
  
  //Notify user
  Serial.println("Finished writing.");
  
  // Close file before exit
  imgFile.close();
}
