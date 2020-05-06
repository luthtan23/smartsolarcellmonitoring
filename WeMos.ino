#include <PZEM004T.h>

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>
#include <FirebaseArduino.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include <WiFiClient.h>
#include <ThingSpeak.h>

#define FIREBASE_HOST "chargingstation-5a5a3.firebaseio.com"
#define FIREBASE_AUTH "LZv4R149gWuiM2RWDeCun5Gmi7uGpPMsX9Z4yCh5"

const char* ssid     = "UnpadWiFi";
const char* host = "pintas.unpad.ac.id";

String myString;
char rdata; // received charactors
float Val1, Val2, Val3, Val4, Val5, Val6, Val8, Val9, Val10, Val11, Val12, PB; //Val13,Val14,Val15;
int Val7;

SoftwareSerial s(D2, D3); //RX, TX
WiFiClient client;
unsigned long myChannelNumber = 927292; //Your Channel Number (Without Brackets)
const char * myWriteAPIKey = "Z30R008BHXB3O0WF"; //Your Write API Key
int statusCode = 0;

void setup() {
  Serial.begin(9600);
  s.begin(57600);
  delay(100);

  // We start by connecting to a WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  HTTPClient http;  //Declare an object of class HTTPClient
  http.begin("http://pintas.unpad.ac.id/ac_portal/login.php");              //Specify request destination
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  String postData;
  postData = "opr=pwdLogin&userName=muhammad16029&pwd=tantowi2307&rememberPwd=0";
  int httpCode = http.POST(postData);   //Send the request
  String payload = http.getString();    //Get the response payload

  Serial.println(httpCode);   //Print HTTP return code
  Serial.println(payload);    //Print request response payload
  http.end();

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  ThingSpeak.begin(client);
}

void loop() {
  if (s.available() > 0 )
  {
    rdata = s.read();
    myString = myString + rdata;
    if ( rdata == '\n') {
      String l = getValue(myString, ',', 0);
      String m = getValue(myString, ',', 1);
      String n = getValue(myString, ',', 2);
      String o = getValue(myString, ',', 3);
      String p = getValue(myString, ',', 4);
      String q = getValue(myString, ',', 5);
      String r = getValue(myString, ',', 6);
      String s = getValue(myString, ',', 7);
      String t = getValue(myString, ',', 8);
      String u = getValue(myString, ',', 9);
      String v = getValue(myString, ',', 10);
      String w = getValue(myString, ',', 11);
      String x = getValue(myString, ',', 12);
      /*String y = getValue(myString, ',', 13);
        String z = getValue(myString, ',', 14);*/
      myString = "";
      ////////////////////////////////
      Val1 = l.toFloat();
      Firebase.setFloat("humidity/Value", Val1);//sementara buat VMPPT
      Serial.print("hum= "); Serial.println(Val1);
      ThingSpeak.writeField(myChannelNumber, 5, Val1, myWriteAPIKey);
      statusCode = ThingSpeak.getLastReadStatus();
      Val2 = m.toFloat();
      Firebase.setFloat("temp/Value", Val2);
      Serial.print("temp= "); Serial.println(Val2);
      ThingSpeak.writeField(myChannelNumber, 6, Val2, myWriteAPIKey);
      statusCode = ThingSpeak.getLastReadStatus();
      Val3 = n.toFloat();
      Firebase.setFloat("Vbat/Value", Val3);
      Serial.print("Vb= "); Serial.println(Val3);
      ThingSpeak.writeField(myChannelNumber, 1, Val3, myWriteAPIKey);
      statusCode = ThingSpeak.getLastReadStatus();
      Val4 = o.toFloat();
      Firebase.setFloat("Arus1/Value", Val4);
      Serial.print("Ib= "); Serial.println(Val4);
      ThingSpeak.writeField(myChannelNumber, 2, Val4, myWriteAPIKey);
      statusCode = ThingSpeak.getLastReadStatus();
      //POWER B1
      PB = Val3 * Val4;
      Firebase.setFloat("power/Value", PB);
      Serial.print("power= "); Serial.println(PB);
      ThingSpeak.writeField(myChannelNumber, 3, PB, myWriteAPIKey);
      statusCode = ThingSpeak.getLastReadStatus();
      //POWERB
      Val5 = p.toFloat();
      Firebase.setFloat("Vload/Value", Val5);
      Serial.print("Vload= "); Serial.println(Val5);
      Val6 = q.toFloat();
      Firebase.setFloat("Iload/Value", Val6);
      Serial.print("Iload= "); Serial.println(Val6);
      Val7 = x.toInt();
      Firebase.setInt("soc", Val7);
      Serial.print("SOC= "); Serial.print(Val7);
      ThingSpeak.writeField(myChannelNumber, 4, Val7, myWriteAPIKey);
      statusCode = ThingSpeak.getLastReadStatus();
    }
  }
}
void sensorvalue1()
{
  float sdata = Val1;
}
void sensorvalue2()
{
  float sdata = Val2;
}
void sensorvalue3()
{
  float sdata = Val3;
}
void sensorvalue4()
{
  float sdata = Val4;
}
void sensorvalue5()
{
  float sdata = Val5;
}
void sensorvalue6()
{
  float sdata = Val6;
}
void sensorvalue13()
{
  int sdata = Val7;
}
/*void sensorvalue14()
  {
  float sdata = Val14;
  }
  void sensorvalue15()
  {
  float sdata = Val15;
  } */
String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = { 0, -1 };
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}
