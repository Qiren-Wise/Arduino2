#include <SoftwareSerial.h>

SoftwareSerial ss(4, 3);  // RX,TX

// 变量声明
String gngga = "";  // 读取到的GNGGA信息
String info[15];  // 用字符数组存储
int commaPosition = -1;

//函数声明
String getTime(); // 获取北京时间
String getLat(); // 获取纬度dd.mmssss
String getLng(); // 获取经度dd.mmssss
String getStatus(); // 获取当前定位状态，0=未定位，1 = 非差分定位，2=差分定位

void setup() {
  Serial.begin(38400);
  ss.begin(38400);
}

void loop() {
  gngga = "";
  while (ss.available() > 0) {
    gngga += char(ss.read());
    delay(10);  // 延时,不延时的话就会跳出while循环
  }

  if (gngga.length() > 0) {
    //Serial.println(gngga);
    //在这里进行数据的解析
    for (int i = 0; i < 15; i++) {
      commaPosition = gngga.indexOf(',');
      if (commaPosition != -1)
      {
        //Serial.println(gngga.substring(0, commaPosition));
        info[i] = gngga.substring(0, commaPosition);
        gngga = gngga.substring(commaPosition + 1, gngga.length());
      }
      else {
        if (gngga.length() > 0) {  // 最后一个会执行这个
          info[i] = gngga.substring(0, commaPosition);
          //Serial.println(gngga);
        }
      }
    }
    Serial.println("time: " + getTime());
    Serial.println("lat: " + getLat());
    Serial.println("lng: " + getLng());
    Serial.println("status: " + getStatus());
  }
}

String getTime(){
   return info[1];
}

String getLat(){
   return info[2];  
}

String getLng(){
   return info[4];  
}

String getStatus(){
   return info[6]; 
}
