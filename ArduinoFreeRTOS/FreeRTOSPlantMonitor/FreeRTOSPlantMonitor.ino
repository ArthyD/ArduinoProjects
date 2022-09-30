#include <Arduino_FreeRTOS.h>
#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>


#define DHTPIN 2
#define DHTTYPE DHT11
#define SOILMOISTUREPIN A3

DHT dht(DHTPIN, DHTTYPE);
float h;
float t;
int moistureReading;

static void TaskHumidityTemperature(void *pvParameters);
static void TaskSoilMoisture(void *pvParameters);
static void TaskDisplay(void *pvParameters);
static void idleTask(void *pvParameters);

void setup()
{
  Serial.begin(9600);
  dht.begin();
  Serial.print(F("Plant monitoring with FreeRTos"));
  xTaskCreate(TaskHumidityTemperature, "ReadTemperatureAndHumidity", 500, NULL, 1, NULL);
  xTaskCreate(TaskDisplay, "DisplayValues", 100, NULL, 2, NULL);
  xTaskCreate(TaskSoilMoisture, "ReadSoilMoisture", 100, NULL, 1, NULL);
  xTaskCreate(idleTask, "Task0", 100, NULL, 0, NULL);
}

static void TaskHumidityTemperature(void *pvParameters)

{
  while (1)
  {
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    h = dht.readHumidity();
    t = dht.readTemperature();
  }
}

static void TaskSoilMoisture(void *pvParameters)

{
  while (1)
  {
    
    moistureReading = analogRead(SOILMOISTUREPIN);
    
    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
}

static void TaskDisplay(void *pvParameters)

{
  while (1)
  {
    vTaskDelay(2200 / portTICK_PERIOD_MS);
    Serial.print(F("Humidity: "));
    Serial.print(h);
    Serial.print(F("%  Temperature: "));
    Serial.print(t);
    Serial.println(F("°C "));
    if(moistureReading<300){
      Serial.println(F("Soil dry"));
    } else if (moistureReading<700){
      Serial.println(F("Soil humid"));
    } else if (moistureReading >950){
      Serial.println(F("In watter"));
    }
  }
}

static void idleTask(void *pvParameters)

{
  while (1)
  {
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}

void loop()
{
}
