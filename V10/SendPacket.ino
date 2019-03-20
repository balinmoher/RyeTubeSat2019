//SENDS PACKET TO SERIAL
//This is what comms will see
//Inputs are pointer to array, and the number of bytes in that array. 
//NOTE: numBytes is the number of total bytes to be read, not necessarily the index. 
//For example a float is 4 bytes and if we pass 3 floats, there are 12 bytes
//float myBytes[3] <-- this is 12 bytes

void sendPacket(int numBytes, byte * packet) {
  Serial.print(F("Symbol"));
  Serial.print(numBytes);
  Serial.write(packet, numBytes);
  }
