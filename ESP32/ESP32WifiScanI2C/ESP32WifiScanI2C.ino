#include "WiFi.h"
#include <LiquidCrystal_I2C.h>

int lcdColumns = 16;
int lcdRows = 2;

const int incButton = 18;
const int decButton = 19;

int incrPress = 0;
int decrPress = 0;
int ind = 0;

char *SSIDs[10];

LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);

void IRAM_ATTR isrIncr() {
  Serial.println("Button 1");
    incrPress += 1;
}

void IRAM_ATTR isrDecr() {
  Serial.println("Button 2");
    decrPress += 1;
}

void setup()
{
    Serial.begin(115200);

    // Set WiFi to station mode and disconnect from an AP if it was previously connected
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    lcd.init();
    lcd.backlight();
    delay(100);
        
    pinMode(incButton, INPUT_PULLUP);
    attachInterrupt(incButton, isrIncr, RISING);
    pinMode(decButton, INPUT_PULLUP);
    attachInterrupt(decButton, isrDecr, RISING);

    Serial.println("Setup done");
}

void loop()
{
    Serial.println("scan start");

    // WiFi.scanNetworks will return the number of networks found
    int n = WiFi.scanNetworks();
    Serial.println("scan done");
    if (n == 0) {
        Serial.println("no networks found");
    } else if(n>10){
      n=10;
    } 
    
    else {
        Serial.print(n);
        Serial.println(" networks found");
        for (int i = 0; i < n; ++i) {
            // Print SSID and RSSI for each network found
            Serial.print(i + 1);
            Serial.print(": ");
            Serial.print(WiFi.SSID(i));
            Serial.print(" (");
            Serial.print(WiFi.RSSI(i));
            Serial.print(")");
            Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
            delay(10);
        }
        if(incrPress > 0){
          ind++;
          incrPress = 0;
        }
        if(decrPress > 0){
          ind--;
          decrPress = 0;
        }
        if(ind > n-1){
          ind = 0;
        }
        if(ind <0){
          ind = 0;
        }
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(WiFi.SSID(ind));
        lcd.setCursor(0, 1);
        lcd.print(WiFi.RSSI(ind));
    }
    Serial.println("");

    // Wait a bit before scanning again
    delay(100);
}
