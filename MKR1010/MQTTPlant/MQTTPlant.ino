#include <ArduinoMqttClient.h>
#include <WiFiNINA.h>
#include "DHT.h"

#define DHTPIN 2     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11   // DHT 11
#define SOILMOISTUREPIN A3 // Analog pin connected to the moisture sensor

#include "arduino_secrets.h"
char ssid[] = SECRET_SSID;    // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

const char broker[] = "192.168.0.109";
int        port     = 1883;
const char topic[]  = "plant/info";

const long interval = 60000;
unsigned long previousMillis = 0;

// Initialize DHT sensor.
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; 
  }
  Serial.print("Attempting to connect to WPA SSID: ");
  Serial.println(ssid);
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    // failed, retry
    Serial.print(".");
    delay(5000);
  }

  Serial.println("You're connected to the network");
  Serial.println();

  Serial.print("Attempting to connect to the MQTT broker: ");
  Serial.println(broker);

  if (!mqttClient.connect(broker, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());

    while (1);
  }

  Serial.println("You're connected to the MQTT broker!");
  Serial.println();

  dht.begin();
}

void loop() {
  
  mqttClient.poll();

  unsigned long currentMillis = millis();
  
  if (currentMillis - previousMillis >= interval) {
    int moistureReading = analogRead(SOILMOISTUREPIN);
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();

    previousMillis = currentMillis;

    Serial.print("Sending message to topic: ");
    Serial.println(topic);

    mqttClient.beginMessage(topic);
    mqttClient.print("{\"T\": " + String(temperature)+", \"H\": "+ String(humidity)+", \"soil\": "+ String(moistureReading)+"}");
    mqttClient.endMessage();

    Serial.println();

  }
}
