#include <Arduino.h>
#include <DHT.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

#define DHT_PIN 26

static const char CA_KEY[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIFtTCCA52gAwIBAgIUAlgOH6qArdXEEjp+5RzSpYkwwrQwDQYJKoZIhvcNAQEN
BQAwajEXMBUGA1UEAwwOMTQ1LjI0LjIyMi4xMTYxFjAUBgNVBAoMDU93blRyYWNr
cy5vcmcxFDASBgNVBAsMC2dlbmVyYXRlLUNBMSEwHwYJKoZIhvcNAQkBFhJub2Jv
ZHlAZXhhbXBsZS5uZXQwHhcNMjMwOTI5MjI1MzQ1WhcNMzIwOTI2MjI1MzQ1WjBq
MRcwFQYDVQQDDA4xNDUuMjQuMjIyLjExNjEWMBQGA1UECgwNT3duVHJhY2tzLm9y
ZzEUMBIGA1UECwwLZ2VuZXJhdGUtQ0ExITAfBgkqhkiG9w0BCQEWEm5vYm9keUBl
eGFtcGxlLm5ldDCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBALcAhZjl
a5BEhw20U3lni4Y+r1di0QmZL/mBHgBkUVDzrKCCsH+niwCMb4VEHikaz8oIY3IO
Xx1MvDYk3lofmVqMUmAdzfd89nsOxo0BIW0bfSgU6c+TR4Pj4Gl4Cl5FQhWlsBQD
ezdHQpjOLkiW2tyRPKfoVWqtrPdse9w6qt9p4qS29hibptcQS23dOUX2x/Pa38Rw
lu0rHozexNiOVPogfrCOCnPjSkH1A93LQ31O1+4bHJKogcbGraFd+B6+x/VM5Tji
HE+f/k37rVwVJDJFZVmxsZ9c+fDnC3PMZd4wVPMJznhmPSdCU37O5FZ/No83Yjoe
MH0/pUao0yNtxktzryV8R67VDHDXPwFmUQSayGIJud5UJ8kWMZkNpzoUynUT7f3w
0FWjqOLOb6NR6vF0rCLjLZmzhYzLNaZN1zly35QFI18ap6aGFp9Aap2+vcOrayEn
4TA+nb0QnUGWh/j0itFjIjwfXU7i0V31SMjuRetrzRjinvLV/Pqs6xFSyTZoVBOv
EnG6Q8V151sORGU7Q8F0Wv+Z1VuD9RvGdG8p+Fma44LBI8b0ijLCjqD4GFWAoZ0i
4qd5gBkyi4A0LtiYlNWmY/Hb6DmVM2UXO3pqD1vtnmeYdIK4Dh+Z8G3g4uWyYveP
hDERojHodgwWrADfZvkmryHOL0PecwMuhj4VAgMBAAGjUzBRMB0GA1UdDgQWBBRZ
5AKlJj3E5M87tYDZVFP7OjFiZjAfBgNVHSMEGDAWgBRZ5AKlJj3E5M87tYDZVFP7
OjFiZjAPBgNVHRMBAf8EBTADAQH/MA0GCSqGSIb3DQEBDQUAA4ICAQBS560qvGwn
IryDzMomPe0FGNMa8l+Ylf1bSythD51zwALGrWP+pFRaWpcIvQKiiciUqLMR3mfE
IB5GaSvVs6gqT10OFXDiH4tZp1f7S3mR/jcwx78NycRxQ8rKYZXROTItmBpoYQTy
yhzMN+UsoUpuiSTy/tn0Qj1ByG7zNoomnsL/djvDyy1pKpo7wmfT1x4JGqiSkCej
8pNbw9xCxnp/jCn2QfPiVASpm3Q8ooDdDmLHbrvSLDkB9Ac+L9Z5tADAXDvjfEwz
FnLex/RPrccqje8fnDGXmaYAlolNTvO7J+/8si1ty4EZG5L4nLefe3GAZrYK+LcF
GJjELcNMg9J49lXYv+V8rbk0TVDOQ9iJplcqMfW3GtpRIXNLDwf1D3pFzOrh1zJ/
lacLBt06ihnlE1lufkDbAeS3fGrMuKtjKMPzHi2U4ZiP064PDcMpYrJoykx8ZKxa
oGIXbdIZTYjRn7elcGbOjKb3BYBbHdQdp9fHLWbB6LiQD8ir6yeAjw6uDYwYHluS
SGyYSBDqe2EHQeSzBLaP7aMYe3rwUmYnKrQ0ZN2G2moE5H/SvFmObKtXu8gpjSv7
Alr+8ssM2jyoBVy9a4WK22DIB7I9ZuMyT6lqux5M1uJlMMV07JW2ujQbGJfvwLwq
sYeLNLx2sCm9LrvCLrxjTDmf1rK7KDJIRQ==
-----END CERTIFICATE-----)EOF";



const char* ssid = "Tesla IoT";
const char* password = "fsL6HgjN";

// const char* ssid = "Hutspot";
// const char* password = "aukeauke";

// 7C:77:16:89:49:B1
// 124:119:22:137:73:177

// 54:83:3a:77:84:86
// 84, 131, 58, 119, 132, 134
// const uint8_t bssid[6] = {124, 119, 22, 137, 73, 177};
// const uint8_t bssid[6] = {84, 131, 58, 119, 132, 134};
// const char* ssid = "TMNL-8949B1";
// const char* password = "bikoiseenlievehond";

// const char* mqtt_server = "192.168.82.206";
const char* mqtt_server = "145.24.222.116";
const char* mqtt_user = "minor";
const char* mqtt_pass = "smartthings2023";
const char* api_server = "http://145.24.222.116:8000/dashboard/weather/";

DHT dht(DHT_PIN, DHT11);
WiFiClient wifi_client;
// WiFiClientSecure wifi_client;
PubSubClient client(wifi_client);

long last_msg = 0;
char msg[50];
int value = 0;

void callback(char* topic, byte* message, unsigned int length);
void reconnect();

bool mqttConnect(){
  client.setServer(mqtt_server, 8884);
  client.setCallback(callback);
  // Serial.println(CA_KEY);
  Serial.print("Connecting to MQTT broker");
  while(!client.connected()){
    if(client.connect("esp32", mqtt_user, mqtt_pass)){
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
  client.subscribe("incoming");
  return true;
}

void initWiFi(){
  WiFi.mode(WIFI_STA);
  // WiFi.begin(ssid, password, 11, bssid, true);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi...");
  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(500);
  }
  Serial.println(WiFi.localIP());
  // wifi_client.setCACert(CA_KEY);
  mqttConnect();
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  initWiFi();
  dht.begin();
  delay(500);
}

void loop() {
  // put your main code here, to run repeatedly:
  float temp = dht.readTemperature();
  float humid = dht.readHumidity();
  Serial.println(temp);
  Serial.println(humid);
  if(WiFi.status() == WL_CONNECTED){
    if(client.connected()){
      String data = "{\"user\":\"auke\", \"weather_station\": 2, \"data\": {\"temperature\":\""+ (String)temp+"\",\"humidity\":\""+ (String)humid+"\",\"wind_speed\":4.0 ,\"light_intensity\":40}}";
      if(!isnan(temp) && !isnan(humid)){
        Serial.print("Sent: ");
        Serial.println(data);
        const char* d = data.c_str();
        client.publish("test", d);
      }
    }else{
      mqttConnect();
    }
  }else{
    initWiFi();
  }
  client.loop();
  delay(1000);
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