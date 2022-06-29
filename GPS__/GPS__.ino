#include <DMXSerial2.h>
#include <rdm.h>

#include <ACAN_ESP32.h>
#include <ACAN_ESP32_AcceptanceFilters.h>
#include <ACAN_ESP32_Buffer16.h>
#include <ACAN_ESP32_CANRegisters.h>
#include <ACAN_ESP32_Settings.h>
#include <CANMessage.h>

  #include <TimerOne.h>

#define GPSSerial Serial1
#define DebugSerial Serial

const unsigned int bufferLength = 600;
char RxBuffer[bufferLength];
unsigned int ii = 0;

[4] = {"$GPRMC,,V,,,,,,,,,,N*7F"};

struct
{
    unsigned char CMD;
    unsigned int LengthA;
    char fixed[2];
    unsigned char id_length;
    char id[2];
    unsigned char LengthB1;
    unsigned char LengthB2;
    char api_key[5];
} login;

struct
{
    unsigned char CMD;
    unsigned char LengthA;
    unsigned char mark;
    unsigned char type;
    unsigned char LengthB1;
    unsigned char LengthB2;
    char streamA[4];
    char id[20];
    char streamB[7];
    char GPS_Buffer[150];
    char streamC[7];
} Save_Data;

void setup(){
    GPSSerial.begin(9600);
    DebugSerial.begin(9600);


    Timer1.initialize(1000);
    Timer1.attachInterrupt(Timer1_handler);

    DebugSerial.println("setup end!");
}

void loop(){
    DebugSerial.println("loop start!");
    getGPSData();
    DebugSerial.println("loop end!");
}

unsigned long  Time_Cont = 0;
void Timer1_handler(void)
{
    Time_Cont++;
}

void getGPSData(){
    char *GPS_BufferHead, *GPS_BufferTail;

    clrRxBuffer();

    Time_Cont = 0;
    while (Time_Cont < 2000)
    {
        readBuffer();

        if ((GPS_BufferHead = strstr(RxBuffer, "$GPRMC,")) != NULL)
        {
            if ((GPS_BufferTail = strstr(GPS_BufferHead, "\r\n")) != NULL)
            {
                if(GPS_BufferTail > GPS_BufferHead)
                {
                    memcpy(Save_Data.GPS_Buffer, GPS_BufferHead, GPS_BufferTail - GPS_BufferHead);

                    DebugSerial.println("-----1------");
                    DebugSerial.println(Save_Data.GPS_Buffer);
                    break;
                    clrRxBuffer();
                }
            }

        }       
    }
    if(Time_Cont >= 2000)
    {
        memcpy(Save_Data.GPS_Buffer, EER_GPS_Buffer, 4);   //输出错误信息    
    }
    DebugSerial.println("-----2------");
    DebugSerial.println(Save_Data.GPS_Buffer);

    memset(Save
