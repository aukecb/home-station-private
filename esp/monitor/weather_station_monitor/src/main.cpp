#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_RESET -1

void callback(char* topic, byte* payload, unsigned int length);
void print_display(StaticJsonDocument<200> msg);

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// const char* ssid = "TMNL-8949B1";
// const char* password = "bikoiseenlievehond";

const char* ssid = "Hutspot";
const char* password = "aukeauke";

const char* mqtt_server = "whub.duckdns.org";
const char* mqtt_username = "minor";
const char* mqtt_password = "smartthings2023";

WiFiClient espClient;
PubSubClient mqtt(mqtt_server, 8884, callback, espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

void wifi_connect(){
  WiFi.begin(ssid, password);
  Serial.print("Connecting to ");
  Serial.print(ssid);

  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(100);
  }
  Serial.println();
  Serial.print("Connection established: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length){
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.println("] ");
  char message[length];
  for(int i = 0; i < length; i++){
    Serial.print((char)payload[i]);
    message[i] = (char)payload[i];
  }
  StaticJsonDocument<200> msg;
  deserializeJson(msg, message);
  if(msg["user"] == "auke"){
    Serial.println("Message from auke!");
    print_display(msg);
  }
  mqtt.publish("auke", message);
  Serial.println();
}

void print_display(StaticJsonDocument<200> msg){
  display.clearDisplay();
  display.setCursor(0,0);
  display.setTextSize(1.6);
  display.print("Temp (");
  display.print(char(247));
  display.print("C): ");
  display.println((String)msg["data"]["temperature"]);

  display.println();
  display.print("Humid (%): ");
  display.println((String)msg["data"]["humidity (%)"]);
  
  display.println();
  display.print("Wind speed (m/s): ");
  display.println((String)msg["data"]["wind_speed"]);
  display.display();
}

void setup() {
  Serial.begin(115200);

  wifi_connect();

  while(!mqtt.connected()){
    if(mqtt.connect("esp8266_monitor", mqtt_username, mqtt_password)){
      Serial.println("Connected to mqtt broker");
      mqtt.subscribe("weather");
    }else{
      Serial.println("MQTT connection attempt failed");
      delay(200);
    }

  }

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)){
    Serial.println(F("SSD1306 allocation failed..."));
    for(;;);
  }
  display.clearDisplay();
  delay(500);
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.display();
}

void loop() {
  // put your main code here, to run repeatedly:
  mqtt.loop();
}
