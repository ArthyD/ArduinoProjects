#include <SPI.h>
#include <WiFiNINA.h>

void setup() {

  Serial.begin(9600);

  while (!Serial) {

  }

  if (WiFi.status() == WL_NO_MODULE) {

    Serial.println("Communication with WiFi module failed!");

    while (true);

  }

  String fv = WiFi.firmwareVersion();

  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {

    Serial.println("Please upgrade the firmware");

  }

  byte mac[6];

  WiFi.macAddress(mac);

  Serial.print("MAC: ");

  printMacAddress(mac);

  Serial.println();

  Serial.println("Scanning available networks...");

  listNetworks();
}

void loop() {

  delay(10000);

  Serial.println("Scanning available networks...");

  listNetworks();
}

void listNetworks() {

  Serial.println("** Scan Networks **");

  int numSsid = WiFi.scanNetworks();

  if (numSsid == -1)

  {

    Serial.println("Couldn't get a WiFi connection");

    while (true);

  }

  Serial.print("number of available networks: ");

  Serial.println(numSsid);

  for (int thisNet = 0; thisNet < numSsid; thisNet++) {

    Serial.print(thisNet + 1);

    Serial.print(") ");

    Serial.print("Signal: ");

    Serial.print(WiFi.RSSI(thisNet));

    Serial.print(" dBm");

    Serial.print("\tChannel: ");

    Serial.print(WiFi.channel(thisNet));

    byte bssid[6];

    Serial.print("\t\tBSSID: ");

    printMacAddress(WiFi.BSSID(thisNet, bssid));

    Serial.print("\tEncryption: ");

    printEncryptionType(WiFi.encryptionType(thisNet));

    Serial.print("\t\tSSID: ");

    Serial.println(WiFi.SSID(thisNet));

    Serial.flush();

  }

  Serial.println();
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

void print2Digits(byte thisByte) {

  if (thisByte < 0xF) {

    Serial.print("0");

  }

  Serial.print(thisByte, HEX);
}

void printMacAddress(byte mac[]) {

  for (int i = 5; i >= 0; i--) {

    if (mac[i] < 16) {

      Serial.print("0");

    }

    Serial.print(mac[i], HEX);

    if (i > 0) {

      Serial.print(":");

    }

  }

  Serial.println();
}
