#include "DHT.h" 
#include <SoftwareSerial.h>
#include <Wire.h>
#include "DS3231.h"

RTClib RTC;
SoftwareSerial s(2,3); //RX,TX
DHT dht(7, DHT11);
DS3231 Clock;

String datatrans;
int analogV1=0;
int analogI1=1;
int analogV2=2;
int analogI2=3;
float ADCin=0;

//Sensor Tegangan//

float R11 = 15000, R21 = 3300;
float R12 = 82000, R22 = 10000;
float Vin1, V1;
float Vin2, V2;

//Sensor Arus//

float mVperAmp = 66; // use 100 for 20A Module and 66 for 30A Module
float ACSoffset = 2500;
float ArusVin1 = 0;
float Arus1 = 0;
float ArusVin2 = 0;
float Arus2 = 0;
float Vmin = 0;
float Vmax = 13.56;
int soc1;

void setup() {

Serial.begin(9600);
dht.begin();
s.begin(57600);
Wire.begin();
//inisialisasi RTC//
Clock.setSecond(0);
Clock.setMinute(26);
Clock.setHour(13);
Clock.setDoW(5);
Clock.setDate(6);
Clock.setYear(19);
Clock.setMonth(12);
}

void loop() {
  //RTC
delay(1000);
  
    DateTime now = RTC.now();
    
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(' ');
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();
  //RTC

  //DHT11
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  
  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.println(F("°C "));
  //DHT11

  //Voltage Baterai
  ADCin = analogRead(analogV1);
  Vin1 = (ADCin / 1023) * 4.5;
  V1 = Vin1 / (R21 / (R11 + R21));
  Serial.print("V baterai = "); //
  Serial.println(V1);
  ADCin=0;
  //Voltage Baterai


  //Voltage Load perbaiki rangkaian Resistor 1 = 82k, r2 10k?
  ADCin = analogRead(analogV2);
  Vin2 = (ADCin / 1023) * 4.5;
  V2 = Vin2 / (R22 / (R12 + R22));
  Serial.print("V load = "); //
  Serial.println(V2);
  ADCin=0;
  //Voltage Load
  
  //ARUS Bin
  ADCin = analogRead(analogI1);
  ArusVin1  = (ADCin / 1023.0) * 4950; // Gets you mV
  Arus1 = abs((ArusVin1 - ACSoffset) / mVperAmp);
  Serial.print("Amps B1 = "); //
  Serial.println(Arus1, 3); //
  ADCin = 0;
  //Arus Bin
  
  //ARUS Load
  ADCin = analogRead(analogI2);
  ArusVin2  = (ADCin / 1023.0) * 4950; // Gets you mV
  Arus2 = abs((ArusVin2 - ACSoffset) / mVperAmp);
  Serial.print("Amps Load = "); //
  Serial.println(Arus2, 3); //
  ADCin = 0;
  //Arus Load

  //SOC
  if (Arus1<=13) { //discharge current on 0.05C to 0.2C
    Vmin = 10.5;
    soc1 = (V1 - Vmin)/(Vmax - Vmin);
  }
  else if (Arus1<=19.5 && Arus1>13) { //discharge current on 0.3C
    Vmin = 10.2;
    soc1 = (V1 - Vmin)/(Vmax - Vmin);
  }
  else if (Arus1<=195 && Arus1>19.5) { //discharge current on 0.6C to 3C
    Vmin = 9.5;
    soc1 = (V1 - Vmin)/(Vmax - Vmin);
  } else {
    Vmin = 10.5;
    soc1 = (V1 - Vmin)/(Vmax - Vmin);
  }
  Serial.print("SOC Battery= ");
  Serial.print(soc1);

  if (s.available() > 0)
  { datatrans = datatrans + h + "," + t + "," + V1 + "," + Arus1 + "," + V2 + "," + Arus2 + "," + soc1;
    s.println(datatrans);
    delay(1500);
    datatrans = "";
  }
}
