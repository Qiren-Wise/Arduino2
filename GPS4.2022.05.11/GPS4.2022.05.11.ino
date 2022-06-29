#define TJC Serial

String comdata = "";
String comdata2 = "";

void setup()
{
  Serial.begin(38400);
  TJC.begin(9600);
  }

  void loop()
  {
    while (Serial.available()>0 )
      {
        comdata +=char(Serial.read());
        delay(2);
        }
        if(comdata.length()>0)
        {
          TJC.print(comdata);
          }

          comdata ="";

          while (TJC.available()>0)
          {
            comdata2 +=char(TJC.read());
            delay(2);
            }
            if(comdata2.length()>0)
            {
              Serial.println(comdata2);
              }
              comdata2 ="";
          
    }
