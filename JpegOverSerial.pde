import processing.serial.*;  
   
Serial myPort;  
OutputStream output;  
   
   
void setup() {  
   
  size(640, 480);  
   
  myPort = new Serial( this, Serial.list()[1], 9600);  
  myPort.clear();  
   
  output = createOutput("newImage13.jpg");  
}  
   
   
void draw() {  
   
  try {  
   while ( myPort.available () > 0 ) {  
    output.write(myPort.read());  
   }  
   println(".");
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
