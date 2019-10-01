/*********************************************************************************
   Send Packets Function

    purpose : send fata packets over serial
    inputs: number of bytes to be sent and a pointer to the first byte in the array/buffer
    outputs: None

    NOTE: numBytes is the number of total bytes to be read, not necessarily the index. 
    For example a float is 4 bytes and if we pass 3 floats, there are 12 bytes
    float myBytes[3] <-- this is 12 bytes
 *********************************************************************************/
void sendPacket(int numBytes, byte * packet) {
  Serial.write(packet, numBytes);
  Serial.println(F(""));
  delay(15);
  }

/*********************************************************************************
   Send Data File Function

    purpose : send sensor data file over serial
    inputs: name of file
    outputs: None
    
 *********************************************************************************/
void sendDataFile(String filename) {

  //check if file exists
  if (! SD.exists(filename)) {
      Serial.println(F("Doesnt Exist"));
      return;
    }

  //open the file
  File datFile = SD.open(filename, FILE_READ);
  
      if (datFile) {
    
        //Read and print data file
        while (datFile.position() < datFile.size()) {
            wdt_reset();
            uint8_t mybuffer[64];
            int i = 0;
            
            //read until newline or until buffer is full
            while (datFile.available() && datFile.peek() != '\r' && i < sizeof(mybuffer)) { 
              mybuffer[i] = datFile.read();
              i++;
              if (datFile.peek() == '\r') { //read eol char so this loop begins on new line
                datFile.read(); //read carriage return
                datFile.read(); //read line feed
                delay(1000);
                }
            }
            sendPacket(sizeof(mybuffer), (byte*)mybuffer); //send the data
        }
      //close file and move on to new data file
      datFile.close();
      
      } else {
        Serial.println(F("File did not open"));
        }
      

  }


/*********************************************************************************
    Get Data Function

    purpose : read sensor data
    inputs: call to action
    outputs: None
    
 *********************************************************************************/
void getData() {
      T1Write(); 
      P1Write();
      IMUv2Call();
      getVBat();
      getTime();  // Get time action commenced
      readSolarCurrent();
      Serial.println(F("Data Collected"));
}

/*********************************************************************************
    Save Data File Function

    purpose : save sensor data to file
    inputs: File number
    outputs: None
    
 *********************************************************************************/
void saveData(int fileNum) {

      //Concatenate a string based on the number input - save in PROGMEM to avoid memory issues
      PROGMEM String fileToWrite = "D" + String(fileNum) + ".txt";
      Serial.println(fileToWrite);

      //Open the file
      File dataFile = SD.open(fileToWrite, FILE_WRITE);
      delay(200);
      
      if (!dataFile) {
        Serial.println(F("File did not open"));
      
      } else {
        //write data to file -- data array is made of floats - each float is 4 bytes
        for (int i = 0; i < sizeof(data)/ 4; i++) {
          dataFile.print(data[i]);
          dataFile.print(F(" "));
          }
          dataFile.println(F(""));
        dataFile.close(); //close file;
      }
  }
