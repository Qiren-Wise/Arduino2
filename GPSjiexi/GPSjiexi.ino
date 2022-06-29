/***
 * The circuit:
 * RX is digital pin 10 (connect to TX of other device)
 * TX is digital pin 11 (connect to RX of other device)
***/
//#include <TinyGPS++.h>
#include <SoftwareSerial.h>

SoftwareSerial mySerial(10, 11);
 
char rmc_flag = 0;
char command_flag = 0;
 
char gps_data_nema[220] = {0};
char gps_data_rmc[72] = {0};
char gps_data_anal[20] = {'0','0','0','0','0','.','0','0','0','0',
                        '0','0','0','0','0','.','0','0','0','0'};//经纬度分
char gps_data_byte[4] = {0};
 
char *gprmc = "$GPRMC";
char *command = "Ad";
char command_code[2] = {0};
 
void memcpy(char *dest, char *src, int n)
{
  int i = 0;
  for(i = 0; i < n; i++)
  {
    *dest = *src;
    dest++;
    src++;
  }
}
 
int memcmp(char *s1, char *s2, int n)
{
  int i = 0;
  for(i = 0; i < n; i++)
  {
    if(*s1 != *s2)return 0;
    s1++;
    s2++;
  }
  return 1;
}
 
void setup() 
{
  Serial.begin(57600);
  mySerial.begin(4800);
}
 
void loop() {
  static char i = 0;
  static char j = 0;
  char k = 0;
  char l = 0;
  char comma[12] = {0}; //一共有12个逗号
  // put your main code here, to run repeatedly:
  if(Serial.available())
  {
    gps_data_nema[i] = Serial.read();
    Serial.print(gps_data_nema[i]);
 
  if(i == 0)
  {
      if(gps_data_nema[i] != '$')
            Serial.println("Warning:the data input is wrong!");
  }
 
  if(gps_data_nema[i] == '$')
  {
    i = 0;
    gps_data_nema[i] = '$';
  }
 
  if(gps_data_nema[i] == 0x0A) //if the end of string
  {
      if(memcmp(gps_data_nema, gprmc, 6))
      {
        memcpy(gps_data_rmc, gps_data_nema, i);
        rmc_flag = 1;
      };
      i = 0;
   }
  
    if(i > 210) //if the data is out range
      Serial.println("Warning:the data formation is false!");
  
    i++;
  }
  
  if(rmc_flag == 1)
  {
    l = 0;
    for(k = 0; k < 72; k++)
    {
    if(gps_data_rmc[k] == ',')
    {
      comma[l] = k;
      l ++;
    }
  }
  
  if((comma[3]-comma[2]) == 10)
  {
    k = comma[2] + 1;
    memcpy(&gps_data_anal[11], &gps_data_rmc[k], 9);
  }
    
  if((comma[5]-comma[4]) == 11)
  {
    l = comma[4] + 1;
    memcpy(&gps_data_anal[0], &gps_data_rmc[l], 10);
  }
 
    gps_data_byte[0] = gps_data_anal[8];
    gps_data_byte[1] = gps_data_anal[9];
    gps_data_byte[2] = gps_data_anal[18];
    gps_data_byte[3] = gps_data_anal[19];
 
    rmc_flag = 0;
  }
 
  if(mySerial.available()&&(command_flag == 0))
  {
    command_code[j] = mySerial.read();
    mySerial.print(command_code[j]);
    j = j + 1;
    if(j == 2)
    {
      mySerial.print("AddressWHR");
      command_flag = 1;
      j = 0;
    }
  }
 
  if(command_flag == 1)
  {
    if(memcmp(command, command_code, 2))
    {
      mySerial.print("AddressFLT");
      for(k = 0; k < 20; k++)mySerial.print(gps_data_anal[k]);
      mySerial.print("CRC");
 
      mySerial.print("AddressLCT");
      for(l = 0; l < 4; l++)mySerial.print(gps_data_byte[l]);
      mySerial.print("CRC");
    }
    command_flag = 0;
  }
}
