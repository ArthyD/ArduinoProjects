#include "arduino_secrets.h" 
#include "WiFiNINA.h"

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS; 
int status = WL_IDLE_STATUS;

void setup() {
  String firmVersion;
  Serial.begin(9600);
  while(!Serial){
    ;
  }
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    while (true);
  }
  firmVersion = WiFi.firmwareVersion();
  if (firmVersion < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
    Serial.print("Current version :");
    Serial.print(firmVersion);
    Serial.print(" Lattest version:");
    Serial.println(WIFI_FIRMWARE_LATEST_VERSION);
  }

  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(ssid);              
    status = WiFi.begin(ssid, pass);
    delay(10000);
  }                           
  printWifiStatus();
}

void loop() {
  // put your main code here, to run repeatedly:

}

void printWifiStatus() {
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
}
