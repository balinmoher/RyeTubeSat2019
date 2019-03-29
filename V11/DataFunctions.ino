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
  Serial.print(F("Symbol"));
  Serial.print(numBytes);
  Serial.write(packet, numBytes);
  //TODO -- possible delay??
  delay(15);
  }

/*********************************************************************************
   Send Data File Function

    purpose : send sensor data file over serial
    inputs: name of file
    outputs: None
    
 *********************************************************************************/
void sendDataFile(String filename) {
  
  File datFile = SD.open(filename);

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
                }
            }
            sendPacket(sizeof(mybuffer), mybuffer);
        }
      }
      //close file and move on to new data file
      datFile.close();
  }


/*********************************************************************************
    Get Data Function

    purpose : read sensor data
    inputs: call to action
    outputs: None
    
 *********************************************************************************/
void getData() {
      T1Write(); 
      T2Write();
      P1Write();
      IMUv2Call();
      getVBat();
      getTime();  // Get time action commenced
      readSolarCurrent();
      wdt_reset();
}

/*********************************************************************************
    Save Data File Function

    purpose : save sensor data to file
    inputs: call to action
    outputs: None
    
 *********************************************************************************/
void saveData() {
      //read data to file delimited by spaces on a single line
      String datFileName = "DATA" + String(dataFileNum) + ".txt";
      File datFile = SD.open(datFileName, FILE_WRITE);
      for (int i = 0; i < sizeof(data); i++) {
        datFile.print(data[i]);
        }
      datFile.println("");
      datFile.close();
  }
