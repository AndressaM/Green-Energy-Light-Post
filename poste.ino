#include <Wire.h>
#include "RTClib.h"  // Credit: Adafruit
#include <NTPClient.h>
#include "ESP8266WiFi.h"
#include <WiFiUdp.h>
#include <math.h> 

//BH1750 IIC Mode

int BH1750address = 0x23; //setting i2c address
byte buff[2];

const char *ssid     = "UFAL";
const char *password = NULL;

int off_hour = 21;
int off_minute = 0;
int rele = 16;
int rele2 = 13;
int lamp_on = 0;

RTC_DS1307 RTC;

WiFiUDP ntpUDP;

int16_t utc = -3; //UTC -3:00 Brazil
uint32_t currentMillis = 0;
uint32_t previousMillis = 0;

NTPClient timeClient(ntpUDP, "a.st1.ntp.br", utc*3600, 60000);


int dia  =0;
int  mes = 0;
int ano = 0;
int hora =0;
int  minuto = 0;
int segundo =0;

void forceUpdate(void) {
  Serial.println(timeClient.getDay());
  timeClient.forceUpdate();
  Serial.println(timeClient.getDay());

}
void setup(){
  Serial.begin(115200);

  WiFi.begin(ssid, password);

  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "Não conectou" );
  }

  timeClient.begin();
  timeClient.update();



 pinMode(rele, OUTPUT);
 pinMode(rele2, OUTPUT);
 
  // Instantiate the RTC
  Wire.begin();
  RTC.begin();

  // Check if the RTC is running.
  if (! RTC.isrunning()) {
    Serial.println("RTC is NOT running");
  }

  // This section grabs the current datetime and compares it to
  // the compilation time.  If necessary, the RTC is updated.
  DateTime now = RTC.now();
  DateTime compiled = DateTime(__DATE__, __TIME__);
  forceUpdate();

  //RTC.adjust(DateTime(_DATE, __TIME_));

  //Instantiate the Sensor Ligth

  



/*  if (now.unixtime() < compiled.unixtime()) {
    //Serial.println("RTC is older than compile time! Updating");
    RTC.adjust(DateTime(_DATE, __TIME_));
  }*/


}



void checkOST(void) {

  int x = minuto - timeClient.getMinutes();

  if ( abs(x) > 1) {
    Serial.println("ATUALIZANDO");
    Serial.println("HORA ANTIGA DO RTC");
    printTime();

    Serial.println("HORA ANTIGA DO NTP");
    Serial.println(timeClient.getDay());
    Serial.println(timeClient.getHours());
    Serial.println(timeClient.getMinutes());
    Serial.println(timeClient.getSeconds());

    forceUpdate();


    Serial.println("HORA ATUAL DO NTP");
    Serial.println(timeClient.getDay());
    Serial.println(timeClient.getHours());
    Serial.println(timeClient.getMinutes());
    Serial.println(timeClient.getSeconds());

     RTC.adjust(DateTime(ano, mes, timeClient.getDay(), timeClient.getHours(), timeClient.getMinutes(), timeClient.getSeconds()));

     Serial.println("HORA ATUAL DO RTC");
     printTime();






    /*previousMillis = currentMillis;    // Salva o tempo atual
    printf("Time Epoch: %d: ", timeClient.getEpochTime());
    Serial.println(timeClient.getFormattedTime());*/
  }
}

void loop() {

  printTime();
  //Controle de acionamento da lâmpada através da hora do sistema e do sensor de luz.
  uint16_t val=120;
  int its_dark = 0;
  BH1750_Init(BH1750address);
  delay(200);
 
  if(2==BH1750_Read(BH1750address))
  {
     val=((buff[0]<<8)|buff[1])/1.2;
     Serial.print(val,DEC);     
     Serial.println("[lx]"); 
  }
  delay(150);

  if (hora == 17 && minuto == 0)
  {
    lamp_on = 0;
  }
  if (val <= 60 && lamp_on == 0)
  {
    int expire_min = (minuto+5)%60;
    while(minuto != expire_min)
    {
      if(2==BH1750_Read(BH1750address))
      {
        val=((buff[0]<<8)|buff[1])/1.2;
        Serial.print(val,DEC);     
        Serial.println("[lx]"); 
      }
      delay(150);

      if(val > 60)
      {
        its_dark = 0;
        break; 
      }
      else
      {
        its_dark = 1;
      }
    }
  }

  if(its_dark == 1)
  {
    off_hour = hora+4;
    off_minute = minuto;
    Serial.print("Desligando às");
    Serial.print(off_hour);
    Serial.print(":");
    Serial.print(off_minute);
    Serial.print("\n");
    lamp_on = 1;
  }
  
  if (lamp_on == 1 && (hora < off_hour || ( hora == off_hour && minuto < off_minute)))
  {
    digitalWrite(rele, HIGH);
    Serial.print("Ligando a Lâmpada.\n");

  }
  else
  {
   digitalWrite(rele, LOW);
   Serial.print("Lâmpada desligada\n");
  }

  //Acionamento do cooler
  
  if (hora>=9 && hora <16)

  {
    digitalWrite(rele2, HIGH);
    Serial.print("Ligando o coller.\n");
 

  }
  else
  {
   digitalWrite(rele2, LOW);
  }




  checkOST();
  delay (1000);
}

void printTime(){
    DateTime now = RTC.now();//Recuperando a data e hora atual

     dia = now.day();
     mes = now.month();
     ano = now.year();
     hora = now.hour();
     minuto = now.minute();
     segundo = now.second();

    //Serial.print(' ');
    Serial.print(dia);
    Serial.print("/");
    Serial.print(mes);
    Serial.print('/');
    Serial.print(ano);
    Serial.print(' ');
    Serial.print(hora);
    Serial.print(':');
    Serial.print(minuto) ;
    Serial.print(':');
    Serial.print(segundo);
    Serial.println(' ');

}

 
int BH1750_Read(int address)
{
   int i=0;
   Wire.beginTransmission(address);
   Wire.requestFrom(address, 2);
   while(Wire.available()) 
   {
      buff[i] = Wire.read();  // receive one byte
      i++;
   }
   Wire.endTransmission();  
   return i;
}
 
void BH1750_Init(int address) 
{
   Wire.beginTransmission(address);
   Wire.write(0x10); //1lx reolution 120ms
   Wire.endTransmission();
}
