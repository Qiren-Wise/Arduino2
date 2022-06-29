

#include <SoftwareSerial.h>
SoftwareSerial air(4,3);
SoftwareSerial gps(10,11);
#include <TinyGPSPlus.h>
char gpsData;
TinyGPSPlus gpsCls;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(38400);
  air.begin(38400);
  gps.begin(38400);
  Serial.println("started");

}

void loop() {
  // put your main code here, to run repeatedly:
    while (Serial.available()>0){
      air.write(Serial.read());
      }
  
      if (gpsCls.encode(gps.read()))
      {
        displayInfo();
        }
   while (air.available()>0){  //有问题 少步
    byte airData = air.read();
    Serial.write(airData);
    }
  
}
String lastString;
void displayInfo()
{
   if(gpsCls.location.isValid())
   {
      Serial.print("Latitude: ");
      Serial.println(gpsCls.location.lat(),6);    
      Serial.print("Latitude: ");
      Serial.println(gpsCls.location.lng(),6);    
      Serial.print("Latitude: ");
      Serial.println(gpsCls.altitude.meters());

          String latitude = String(gpsCls.location.lat(),6);
          String longitude = String(gpsCls.location.lng(),6);
          String themeters = String(gpsCls.altitude.meters());

          if (latitude + ";" + longitude + ";" + longitude !=lastString){
            lastString = String(latitude + ";" + longitude + ";" + longitude);
            air.println(latitude + ";" + longitude + ";" + longitude);
            }
    }else
      {
        Serial.println("Location:Not Available");
        lastString = String("searching");
        air.println(lastString);
        }
        delay(3000);

   
  }
