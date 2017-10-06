
#include <Wire.h>
#include "RTClib.h"  // Credit: Adafruit
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

const char *ssid     = "UFAL";
const char *password = "";


int rele = 16;
int rele2 = 13;


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

void setup(){
  Serial.begin(115200);

  WiFi.begin(ssid, password);

  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
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

  //RTC.adjust(DateTime(__DATE__, __TIME__));
/*  if (now.unixtime() < compiled.unixtime()) {
    //Serial.println("RTC is older than compile time! Updating");
    RTC.adjust(DateTime(__DATE__, __TIME__));

  }*/


}

void forceUpdate(void) {
  Serial.println(timeClient.getDay());
  timeClient.forceUpdate();
  Serial.println(timeClient.getDay());

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

  if (hora>=18 && hora <21)

  {
    digitalWrite(rele, HIGH);

  }
  else
  {
   digitalWrite(rele, LOW);
  }
  if (hora>=10 && hora <16)

  {
    digitalWrite(rele2, HIGH);

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

    Serial.print(' ');
    Serial.print(dia);
    Serial.print(" - ");
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
