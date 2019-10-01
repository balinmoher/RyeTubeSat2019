// Using SoftwareSerial
SoftwareSerial cameraconnection = SoftwareSerial(2, 3);

//Cam
Adafruit_VC0706 cam = Adafruit_VC0706(&cameraconnection);

/*********************************************************************************
   Snapshot Function

    purpose : takes a picture and saves it to SD card
    inputs: Call to action
    outputs: None

 *********************************************************************************/
void snapshot() {

  // Try to locate the camera
  if (!cam.begin()) {
    return;
  }

  // Set the picture size - you can choose one of 640x480, 320x240 or 160x120 
  // Remember that bigger pictures take longer to transmit!
  
  cam.setImageSize(VC0706_640x480);        // biggest
  //cam.setImageSize(VC0706_320x240);        // medium
  //cam.setImageSize(VC0706_160x120);          // small

  // You can read the size back from the camera (optional, but maybe useful?)
  uint8_t imgsize = cam.getImageSize();
  delay(1000);
  wdt_reset();

  //Take the picture
  if (cam.takePicture()) {
    Serial.println(F("Picture taken!"));
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
  delay(200);
  if (!imgFile) {
    Serial.println(F("File did not open"));
    }
    
  // Get the size of the image (frame) taken  
  uint16_t jpglen = cam.frameLength();
  Serial.print(F("Storing "));
  Serial.print(jpglen, DEC);
  Serial.print(F(" byte image."));

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
      Serial.println(F("."));
      wCount = 0;
    }
    //Serial.print("Read ");  Serial.print(bytesToRead, DEC); Serial.println(" bytes");
    jpglen -= bytesToRead;
  }
  imgFile.close();

  time = millis() - time;
  Serial.println(F("done!"));
  Serial.print(time); Serial.println(F(" ms elapsed"));

}

/*********************************************************************************
   Read SD Function

    purpose : Reads picture from SD card over serial
    inputs: File name
    outputs: None

 *********************************************************************************/
void readSD(String filename) {

  // Check if file exists
  if (!SD.exists(filename)) {
    Serial.println(F("File does not exist"));
    return;
   }
  
  // Open the file for reading
  File imgFile = SD.open(filename);
  
    
  // Check if file opened
  if (imgFile) {
    
    unsigned long fileSize = imgFile.size(); //get size of file
    delay(100);
    Serial.println(F("Reading"));
    Serial.println(fileSize);
    delay(20);
    
    
    //Read and print file to serial
     while (imgFile.position() < imgFile.size()) {
        wdt_reset();
        uint8_t mybuffer[64];
        uint8_t bytesToRead = min(64, fileSize);
    
        //Serial.write(imgFile.read()); 
        imgFile.read(mybuffer, bytesToRead); //read to buffer
        //Serial.write(mybuffer, bytesToRead); //write to serial
        sendPacket(sizeof(mybuffer), (byte*)mybuffer);
        fileSize -= bytesToRead; //update byte count
    }
  } else { 
    Serial.println(F("Error: could not open file."));
    return;
  }

  Serial.flush(); //send remaining data before continuing
  
  //Notify user
  Serial.println(F("Finished writing."));
  
  // Close file before exit
  imgFile.close();
}
