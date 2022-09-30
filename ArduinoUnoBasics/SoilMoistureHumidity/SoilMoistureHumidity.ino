#include "DHT.h"

#define DHTPIN 2     // Digital pin connected to the DHT sensor

#define DHTTYPE DHT11   // DHT 11

#define SOILMOISTUREPIN A3 // Analog pin connected to the moisture sensor

// Initialize DHT sensor.
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  Serial.println("*** Plant environment measure ***");
  dht.begin();
}

void loop() {
  delay(2000);
  int moistureReading = analogRead(SOILMOISTUREPIN);
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  // Check if any reads failed
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  float heatIndexC = dht.computeHeatIndex(temperature, humidity, false);
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.print("%  Temperature: ");
  Serial.print(temperature);
  Serial.print("°C  Heat index: ");
  Serial.print(heatIndexC);
  Serial.print("°C  Moisture: ");
  Serial.print(moistureReading);
  Serial.println("");
}