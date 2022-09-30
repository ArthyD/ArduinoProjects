#include <DHT.h>
#include <LiquidCrystal_I2C.h>

#define DHTPIN 2     // Digital pin connected to the DHT sensor

#define DHTTYPE DHT11   // DHT 11

#define SOILMOISTUREPIN A3 // Analog pin connected to the moisture sensor

LiquidCrystal_I2C lcd(0x27, 16, 2);
// Initialize DHT sensor.
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  dht.begin();
  lcd.init(); // initialize the lcd
  lcd.backlight();
}

void loop() {
  delay(2000);
  int moistureReading = analogRead(SOILMOISTUREPIN);
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  // Check if any reads failed
  if (isnan(humidity) || isnan(temperature)) {
    return;
  }
  float heatIndexC = dht.computeHeatIndex(temperature, humidity, false);
  lcd.setCursor(0, 0);
  lcd.print("H:"+ String(humidity,1) +"% T:"+ String(temperature,1) + "C");
  lcd.setCursor(0,1);
  if(moistureReading<300){
    lcd.print("Soil dry");
  } else if (moistureReading<700){
    lcd.print("Soil humid");
  } else if (moistureReading >950){
    lcd.print("In watter");
  }
}
