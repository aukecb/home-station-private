#include <Arduino.h>
#include <DHT.h>
#include <ESP8266WiFi.h>
// #include <WiFi.h>
#include <PubSubClient.h>
// #include <HTTPClient.h>
#include <WiFiClientSecure.h>

#define DHT_PIN 10
#define HALL_PIN 9
#define LIGHT_PIN A0

// const char* ssid = "Tesla IoT";
// const char* password = "fsL6HgjN";

const char* ssid = "Hutspot";
const char* password = "aukeauke";

// const char* ssid = "TMNL-8949B1";
// const char* password = "bikoiseenlievehond";

const char* mqtt_server = "whub.duckdns.org";
const char* mqtt_user = "minor";
const char* mqtt_pass = "smartthings2023";
const char* api_server = "http://145.24.222.116:8000/dashboard/weather/";

DHT dht(DHT_PIN, DHT11);
WiFiClient wifi_client;
PubSubClient client(wifi_client);

volatile int revolutions;

void ICACHE_RAM_ATTR increase_revolutions();
void callback(char* topic, byte* message, unsigned int length);
void reconnect();
void send_mqtt(float temp, float humid, float rps, float light);

bool mqttConnect(){
  client.setServer(mqtt_server, 8884);
  client.setCallback(callback);
  Serial.print("Connecting to MQTT broker");
  while(!client.connected()){
    if(client.connect("esp32_auke", mqtt_user, mqtt_pass)){
      Serial.println("CONNECTED TO MQTT");
    }else{
      Serial.print("Failed with state ");
      char err_buf[100];
      Serial.println(client.state());
      delay(2000);
    }
    Serial.print(".");
  }
  Serial.println();
  client.subscribe("auke");
  return true;
}

void initWiFi(){
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi...");
  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(500);
  }
  Serial.println(WiFi.localIP());
  mqttConnect();
}

float read_light(){
  float lux = analogRead(LIGHT_PIN);
  Serial.println(lux);
  return 0.142*lux + 144;

}


float rps;
float ms;
unsigned long previousMillis;
unsigned long prevMillis2;
float total;
float read_wind(){
  if(millis() - previousMillis >= 1000){
    float r = static_cast<float>(revolutions) / ((millis() - previousMillis) / 1000.0);
    total += r;
    revolutions = 0;
    previousMillis = millis();
  }
  if(millis() - prevMillis2 >= 10000){
    rps = total / 10;
    total = 0;
    prevMillis2 = millis();
  }
  
  ms = 0.958*rps - 0.201;
  if(ms < 0){
    ms = 0;
  }
  return ms;
}

void increase_revolutions(){
  revolutions++;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  initWiFi();
  dht.begin();
  pinMode(HALL_PIN, INPUT);
  pinMode(LIGHT_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(HALL_PIN), increase_revolutions, RISING);
  delay(500);
}

int counter = 0;
void loop() {
  // put your main code here, to run repeatedly:
  if(counter > 50){
    float temp = dht.readTemperature();
    float humid = dht.readHumidity();
    float light = read_light();
    send_mqtt(temp, humid, ms, light);
    counter = 0;
  }
  client.loop();
  read_wind();
  delay(100);
  counter++;
}

void send_mqtt(float temp, float humid, float wind, float light){
  if(WiFi.status() == WL_CONNECTED){
    if(client.connected()){
      String data = "{\"user\":\"auke\", \"weather_station\": 2, \"data\": {\"temp(°C)\":"+ (String)temp+",\"humid(%)\":"+ (String)humid+",\"wind(m/s)\":"+(String)wind + ",\"light(lux)\":" + (String)light +"}}";
      if(!isnan(temp) && !isnan(humid)){
        Serial.print("Sent: ");
        Serial.println(data);
        const char* d = data.c_str();
        client.publish("weather", d);
      }
    }else{
      mqttConnect();
    }
  }else{
    initWiFi();
  }

}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

}