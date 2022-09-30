#include <Arduino_FreeRTOS.h>
#include <queue.h>
#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <LiquidCrystal.h>
#define DHTPIN 10
#define DHTTYPE DHT11
#define SOILMOISTUREPIN A3

DHT dht(DHTPIN, DHTTYPE);

int RS = 7, EN = 6, D4 = 5, D5 = 4, D6 = 3, D7 = 2;
LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);

// Define a struct
struct measureRead 
{
  int task; //0 temperature and Humidity, 1 moisture
  float measure1;
  float measure2;
};

QueueHandle_t structQueue;
static void TaskTemperatureHumidity(void *pvParameters);
static void TaskSoilMoisture(void *pvParameters);
static void TaskDisplay(void *pvParameters);

void setup()
{
  Serial.begin(9600);
  dht.begin();
  lcd.begin(16,2); 
  Serial.print(F("Plant monitoring with FreeRTos"));
  structQueue = xQueueCreate( 5, sizeof( struct measureRead ) );
  xTaskCreate(TaskTemperatureHumidity, "ReadTemperatureAndHumidity", 400, NULL, 1, NULL);
  xTaskCreate(TaskDisplay, "DisplayValues", 150, NULL, 2, NULL);
  xTaskCreate(TaskSoilMoisture, "ReadSoilMoisture", 150, NULL, 1, NULL);
  vTaskStartScheduler();
}

static void TaskTemperatureHumidity(void *pvParameters)

{
  while (1)
  {
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    struct measureRead currentMeasure;
    currentMeasure.task = 0;
    currentMeasure.measure1 = dht.readTemperature();  
    currentMeasure.measure2 = dht.readHumidity(); 
    xQueueSend(structQueue, &currentMeasure, portMAX_DELAY == pdPASS); 
    vTaskDelay(2000 / portTICK_PERIOD_MS);
     
  }
}

static void TaskSoilMoisture(void *pvParameters)

{
  int moistureReading;
  while (1)
  {  
    struct measureRead currentMeasure;
    currentMeasure.task = 1;
    currentMeasure.measure1 = analogRead(SOILMOISTUREPIN);  
    xQueueSend(structQueue, &currentMeasure, portMAX_DELAY == pdPASS); 
    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
}

static void TaskDisplay(void *pvParameters)

{
  struct measureRead currentMeasure;
  while (1)
  {
    if(xQueueReceive( structQueue, &currentMeasure, portMAX_DELAY ) == pdPASS){
      if(currentMeasure.task == 0){
        lcd.setCursor(0, 0);
        lcd.print("T:"+String(currentMeasure.measure1,1)+"C H:"+String(currentMeasure.measure2,1)+"%");
      }
      else if(currentMeasure.task == 1){
        lcd.setCursor(0, 1);
        if(currentMeasure.measure1<300){
          lcd.print(F("Soil dry"));
        } else if (currentMeasure.measure1<700){
          lcd.print(F("Soil humid"));
        } else if (currentMeasure.measure1 >950){
          lcd.print(F("In watter"));
        }
      }
    }
    taskYIELD();
  }
}

void loop()
{
}
