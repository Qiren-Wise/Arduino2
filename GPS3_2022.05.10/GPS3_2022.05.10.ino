//VCC接5V，GND接GND，RX接D18，TX接D19。

#define TJC Serial
#define  TJC Serial1

String data="";
int mark = 0;
boolean Mark_Start=false;
boolean valid=false;
String GGAUTCtime,GGAlatitude,GGAlongitude,GPStatus,SatelliteNum,HDOPfactor,Height,
PositionValid,RMCUTCtime,RMClatitude,RMClongitude,Speed,Direction,Date,Declination,Mode;
void setup(){ 
  Serial.begin(38400);
  Serial1.begin(98400);
  delay(1000);
}

void loop(){
  while (Serial1.available()> 0){
    if(Mark_Start){
      data=reader();
      Serial.println(data);
      if(data.equals("GPGGA")){
        //Serial.println(1);
        GGAUTCtime=reader();
        GGAlatitude=reader();
        GGAlatitude+=reader();
        GGAlongitude=reader();
        GGAlongitude+=reader();
        GPStatus=reader();
        SatelliteNum=reader();
        HDOPfactor=reader();
        Height=reader();
        Mark_Start=false;
        valid=true;
        data="";

      }
      else if(data.equals("GPGSA")){
        Serial.println(2);
        Mark_Start=false;
        data="";
      }
      else if(data.equals("GPGSV")){
        Serial.println(3);
        Mark_Start=false;
        data="";
      }
      else if(data.equals("GPRMC")){
        //Serial.println(4);
        RMCUTCtime=reader();
        PositionValid=reader();
        RMClatitude=reader();
        RMClatitude+=reader();
        RMClongitude=reader();
        RMClongitude+=reader();
        Speed=reader();
        Direction=reader();
        Date=reader();
        Declination=reader();
        Declination+=reader();
        Mode=reader();
        valid=true;
        Mark_Start=false;
        data="";
      }
      else if(data.equals("GPVTG")){
        Serial.println(5);
        Mark_Start=false;
        data="";
      }
      else{
        Serial.println(6);
        Mark_Start=false;
        data="";
      }
    }
    if(valid){
      if(PositionValid=="A"){
        Serial.println("Position Valid");
      }
      else{
        Serial.println("Your position is not valid.");
      }
      Serial.print("Date:");//日期
      Serial.println(Date);
      Serial.print("UTCtime:");//世界协调时间
      Serial.print(RMCUTCtime);
      Serial.print("   ");
      Serial.println(GGAUTCtime);
      Serial.print("Latitude:");//纬度
      Serial.print(RMClatitude);
      Serial.print("   ");
      Serial.println(GGAlatitude);
      Serial.print("Longitude:");//经度
      Serial.print(RMClongitude);
      Serial.print("   ");
      Serial.println(GGAlongitude);
      Serial.print("GPStatus:");//GP地位
      Serial.println(GPStatus);
      Serial.print("SatelliteNum:");//卫星数
      Serial.println(SatelliteNum);
      Serial.print("HDOPfactor:");
      Serial.println(HDOPfactor);
      Serial.print("Height:");//高度
      Serial.println(Height);
      Serial.print("Speed:");//速度
      Serial.println(Speed);
      Serial.print("Direction:");//方向
      Serial.println(Direction);
      Serial.print("Declination:");//赤纬
      Serial.println(Declination);
      Serial.print("Mode:");//模式
      Serial.println(Mode);     
      valid=false;
    }
    if(Serial1.find("$")){
      Serial.println("capture");
      Mark_Start=true;
    }
  }

}

String reader(){
  String value="";
  int temp;
startover:
 while (Serial1.available() > 0){
    delay(2);
    temp=Serial1.read();
        if((temp==',')||(temp=='*')){
                  if(value.length()){
                    //Serial.println("meaningful message");
                    return value;
                  }
                            else {
                              //Serial.println("empty");
                              return "";
                            }     
        }
              else if(temp=='$'){
                //Serial.println("failure");
                Mark_Start=false;
              }
                    else{
                      //Serial.println("add");
                      value+=char(temp);
                    }
  }
 while (!(Serial1.available() > 0)){
  }
 goto startover; 
}
