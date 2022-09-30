#include "WiFiNINA.h"

void setup() {
  byte mac[6];
  String firmVersion;
  // put your setup code here, to run once:
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
  WiFi.macAddress(mac); // Get the mac address
  Serial.print("MAC address : ");
  printMacAddress(mac);
  Serial.println();
}

void loop() {
  Serial.println("*** Scanning available networks ***");
  listNetworks();
  delay(10000); // We san every 10 seconds
}

void listNetworks(){
  int numSsid = WiFi.scanNetworks(); // Get the number of available networks
  if (numSsid == -1){
    Serial.println("Couldn't get a WiFi connection");
    while (true); //stop
  } else {
    Serial.println("There are " + String(numSsid) +" wifi networks available");
  }
  for (int thisNet = 0; thisNet < numSsid; thisNet++) {
    //Print all the information
    Serial.print(thisNet + 1);
    Serial.print(") ");
    Serial.print("Signal: ");
    Serial.print(WiFi.RSSI(thisNet));
    Serial.print(" dBm");
    Serial.print("\tChannel: ");
    Serial.print(WiFi.channel(thisNet));
    Serial.print("\tEncryption: ");
    printEncryptionType(WiFi.encryptionType(thisNet));
    byte bssid[6];
    Serial.print("\tBSSID: ");
    printMacAddress(WiFi.BSSID(thisNet, bssid));
    Serial.print("\tSSID: ");
    Serial.print(WiFi.SSID(thisNet));
    Serial.println();
    Serial.flush();
  }
  Serial.println();
}

void printMacAddress(byte mac[]){
  for (int i = 5; i >= 0; i--) {
    if (mac[i] < 16) {
      Serial.print("0");
    }
    Serial.print(mac[i], HEX);
    if (i > 0) {
      Serial.print(":");
    }
  }  
}

void printEncryptionType(int thisType) {
  switch (thisType) {
    case ENC_TYPE_WEP:
      Serial.print("WEP");
      break;
    case ENC_TYPE_TKIP:
      Serial.print("WPA");
      break;
    case ENC_TYPE_CCMP:
      Serial.print("WPA2");
      break;
    case ENC_TYPE_NONE:
      Serial.print("None");
      break;
    case ENC_TYPE_AUTO:
      Serial.print("Auto");
      break;
    case ENC_TYPE_UNKNOWN:
    default:
      Serial.print("Unknown");
      break;
  }
}
