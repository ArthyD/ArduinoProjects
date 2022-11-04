#include <Arduino_FreeRTOS.h>
#include <queue.h>
#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <LiquidCrystal_I2C.h>
#define DHTPIN 22
#define DHTTYPE DHT11
#define SOILMOISTUREPIN A0

DHT dht(DHTPIN, DHTTYPE);

LiquidCrystal_I2C lcd(0x27,20,4);

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
  lcd.init(); 
  lcd.backlight();
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
          lcd.print("Soil dry ("+String(currentMeasure.measure1,0)+")");
        } else if (currentMeasure.measure1<700){
          lcd.print("Soil humid ("+String(currentMeasure.measure1,0)+")");
        } else if (currentMeasure.measure1 >950){
          lcd.print("In watter ("+String(currentMeasure.measure1,0)+")");
        }
      }
    }
    taskYIELD();
  }
}

void loop()
{
}
