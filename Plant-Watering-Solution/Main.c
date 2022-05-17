#include "ThingSpeak.h"
#include <ESP8266WiFi.h>

#include <Wire.h>
#include "SSD1306.h"
SSD1306  display(0x3c, D1, D2);

//----------- Enter you Wi-Fi Details---------//
char ssid[] = "SSID"; //SSID
char pass[] = "Pass"; // Password
//-------------------------------------------//

// --------------- Tank details --------------//
const int total_height = 30; // Tank height in CM
const int hold_height = 25;// Water hold height in CM
//-------------------------------------------//

//----- minutes -----//
float minute = 0.1; // Data update in min.
//------------------//

//----------- Channel Details -------------//
unsigned long Channel_ID = ----; // Channel ID
const int Field_number = 1; // To which field to write data (don't change)
const char * WriteAPIKey = "----"; // Your write API Key

const char* server = "api.thingspeak.com";
String apiKey = "-------";
// ----------------------------------------//

const int trigger = 16;
const int echo = 5;
long Time;
int x;
int i;
int distanceCM;
int resultCM;
int tnk_lvl = 0;
int sensr_to_wtr = 0;
WiFiClient  client;

void setup()
{
  Serial.begin(115200);
  pinMode(trigger, OUTPUT);
  pinMode(echo, INPUT);
  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);
  sensr_to_wtr = total_height - hold_height;
  Serial.begin(115200);
  pinMode(A0,INPUT);
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  delay(10);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
}

void loop()
{
  internet();
  for (i = 0; i < minute; i++)
  {
    Serial.println("System Standby....");
    Serial.print(i);
    Serial.println(" Minutes elapsed.");
    delay(20000);
    delay(20000);
    delay(20000);
  }
  measure();
  Serial.print("Tank Level:");
  Serial.print(tnk_lvl);
  Serial.println("%");
  upload();
  display.clear();
  int temp = analogRead(A0);
  if (client.connect(server,80)) {  //   "184.106.153.149" or api.thingspeak.com
    String postStr = apiKey;
           postStr +="&field1=";
           postStr += String(temp);
           postStr += "\r\n\r\n";
     client.print("POST /update HTTP/1.1\n");
     client.print("Host: api.thingspeak.com\n");
     client.print("Connection: close\n");
     client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
     client.print("Content-Type: application/x-www-form-urlencoded\n");
     client.print("Content-Length: ");
     client.print(postStr.length());
     client.print("\n\n");
     client.print(postStr);
     Serial.print("Moisture Sensor Value: ");
     Serial.print(temp);
     display.drawString(0, 20,"Moisture: ");
     display.drawString(0, 30,String(temp));
     Serial.println(" send to Thingspeak");
     display.drawString(0, 10, "Send to Thingspeak");  
     display.display();
  }

  client.stop();
   Serial.println("Waiting...");    

  // thingspeak needs minimum 15 sec delay between updates
  // Here I set upload every 60 seconds

  for(unsigned int i = 0; i < 20; i++)
  {
    delay(2000);                        
  }                        
}
void upload()
{
  internet();
  x = ThingSpeak.writeField(Channel_ID, Field_number, tnk_lvl, WriteAPIKey);
  if (x == 200)Serial.println("Data Updated.");
  if (x != 200)
  {
    Serial.println("Data upload failed, retrying....");
    delay(15000);
    upload();
  }
}

void measure()
{
  delay(100);
  digitalWrite(trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger, LOW);
  Time = pulseIn(echo, HIGH);
  distanceCM = Time * 0.034;
  resultCM = distanceCM / 2;

  tnk_lvl = map(resultCM, sensr_to_wtr, total_height, 100, 0);
  if (tnk_lvl > 100) tnk_lvl = 100;
  if (tnk_lvl < 0) tnk_lvl = 0;
}

void internet()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    while (WiFi.status() != WL_CONNECTED)
    {
      WiFi.begin(ssid, pass);
      Serial.print(".");
      delay(5000);
    }
    Serial.println("\nConnected.");
  }
}
