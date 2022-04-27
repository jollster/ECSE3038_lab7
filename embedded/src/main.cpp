#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <env.h>


#define ONBOARD_LED  2

int getWaterLevel();
int value = 0;
const int potPin = 34;


void setup() {
  Serial.begin(115200);
  pinMode(ONBOARD_LED,OUTPUT);

  WiFi.begin(Wifi_ssid,Wifi_passwd);
  while (WiFi.status()!= WL_CONNECTED){
    delay(500);
    Serial.print("*");
  }

  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  value=getWaterLevel();

  if(value>=80 && value<=100){
    digitalWrite(ONBOARD_LED,HIGH);
  }else{
    digitalWrite(ONBOARD_LED,LOW);
  }

  HTTPClient http;
  // Your IP address with path or Domain name with URL path 
  http.begin("http://192.168.0.10:3000/tank");
  // Send HTTP POST request
  http.addHeader("Content-Type", "application/json");
  char output[128];
  const size_t CAPACITY = JSON_OBJECT_SIZE(4);
  StaticJsonDocument<CAPACITY> doc;

  doc["tank_id"].set(WiFi.macAddress());
  doc["water_level"].set(value);
  
  serializeJson(doc,output);
  int httpResponseCode = http.POST(String(output));
  String payload = "{}"; 
  if (httpResponseCode>0) {
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
    }
  // Free resources
  Serial.println(payload);
  delay(5000);
  http.end();
}

int getWaterLevel(){
  float val = analogRead(potPin);// default 12 bit adc used range 0-4095
  val=round(((val/4095)*5)/0.01);// used lm35 temp sensor 10mV/oC
  return val;
}