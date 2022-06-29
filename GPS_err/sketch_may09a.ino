#include <SoftwareSerial.h>
 
// The serial connection to the GPS module
SoftwareSerial ss(4, 3);  // RX,TX
 
void setup(){
  Serial.begin(38400);
  ss.begin(38400);
}
 
void loop(){
  if (ss.available() > 0){
    // get the byte data from the GPS
    byte gpsData = ss.read();  //read是剪切，所以不加延迟。加延迟反而会影响数据读取
    Serial.write(gpsData);
  }
}
