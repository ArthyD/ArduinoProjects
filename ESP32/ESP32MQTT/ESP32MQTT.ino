#include <WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"
#include <LiquidCrystal_I2C.h>
#include "secrets.h" 

const char* ssid = SECRET_SSID;
const char* password =  SECRET_PASS;
const char* mqttServer = "192.168.0.106";
const int mqttPort = 1883;
const char* MQTTUSER = "esp32";
#define MQTT_SERIAL_PUBLISH_CH "plant/info"

const long interval = 60000;
unsigned long previousMillis = 0;

WiFiClient wifiClient;

PubSubClient client(wifiClient);

#define DHT11PIN 16
#define HumidPin 33

DHT dht(DHT11PIN, DHT11);
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup_wifi() {
    delay(10);
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    randomSeed(micros());
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(),MQTTUSER,"")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
void setup() {
  Serial.begin(115200);
  Serial.print("Starting");
  Serial.setTimeout(500);
  setup_wifi();
  client.setServer(mqttServer, mqttPort);
  reconnect();
  dht.begin();
  lcd.init();
  lcd.backlight();
}

void publishSerialData(char *serialData){
  if (!client.connected()) {
    reconnect();
  }
  client.publish(MQTT_SERIAL_PUBLISH_CH, serialData);
}


void loop() {
  unsigned long currentMillis = millis();

  
  if (currentMillis - previousMillis >= interval) {
    int moistureReading = analogRead(HumidPin);
    int humi = dht.readHumidity();
    float temp = dht.readTemperature();
    lcd.setCursor(0,0);
    lcd.print("T:"+String(temp)+"C H:"+String(humi)+"%");
    lcd.setCursor(0,1);
    lcd.print("Soil : ("+String(moistureReading)+")");
    
    String message = "{\"ID\":1, \"T\": " + String(temp)+", \"H\": "+ String(humi)+", \"soil\": "+ String(moistureReading)+"}";
    char mqttmessage[message.length()+1];
    message.toCharArray(mqttmessage, message.length()+1);
    previousMillis = currentMillis;

    Serial.print("Sending message to topic ");
    
    publishSerialData(mqttmessage);


    Serial.println();
  }
}
