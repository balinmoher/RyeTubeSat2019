import processing.serial.*;  
   
Serial myPort;  
OutputStream output;  
boolean contacted = false;
String mybuff;
String comparison = "Reading";
int bytesToRead;

void setup() {  
   
  size(640, 480);  
   
  //println( Serial.list() );  
  myPort = new Serial( this, Serial.list()[1], 9600);  
  myPort.clear();  
   
  output = createOutput("JimbosPimple.jpg");  
}  
   //TODO implement timeout protocol, where if get stuck in while loop, update timer
   
void draw() {  
   
  try {  
    while ( myPort.available () > 0 ) { 
     if (!contacted) {
       mybuff = myPort.readStringUntil(13);
       if (mybuff != null) {
         mybuff = trim(mybuff);
         println(mybuff);
       
         if (mybuff.equals(comparison)) {
           contacted = true;
           while (myPort.available() == 0) {} //stay here until byte number received
           
           mybuff = myPort.readStringUntil(13);
           mybuff = trim(mybuff);
           bytesToRead = int(mybuff);
           print(bytesToRead); println(" Bytes");
           myPort.clear();
         }
       }
     } else {

       while (bytesToRead > 0) {
         if (myPort.available() > 0) {
          output.write(myPort.read());
          bytesToRead--;
        }
       }
         
      output.flush(); // Writes the remaining data to the file  
      output.close(); // Finishes the file
      contacted = false; //resets condition
      
    }
   }
  }   
  catch (IOException e) {  
   e.printStackTrace();  
  }  
}  
   
   
void keyPressed() {  
   
  try {   
   output.flush(); // Writes the remaining data to the file  
   output.close(); // Finishes the file  
  }   
   
  catch (IOException e) {  
   e.printStackTrace();  
  }  
}   
