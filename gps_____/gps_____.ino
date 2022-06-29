
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

TinyGPSPlus gps;
SoftwareSerial gps_ss(4, 3);

void setup(){
  Serial.begin(38400);
  gps_ss.begin(38400);

}

void loop(){
      while (gps_ss.available()) {
            if (gps.encode(gps_ss.read())) {
                  if (gps.location.isValid()) {
                    Serial.println(gps.location.lat(),6);
                    Serial.println(gps.location.lng(),6);
                  }
            }
      }

}
