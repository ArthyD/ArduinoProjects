#include "SPI.h"
#include "WiFiNINA.h"
#include "DHT.h"
#include "arduino_secrets.h" 
char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS; 

int status = WL_IDLE_STATUS;
WiFiServer server(80);


#define DHTPIN 2     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11   // DHT 11
#define SOILMOISTUREPIN A3 // Analog pin connected to the moisture sensor
float h;
float t;
int moisture;
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  String firmVersion;
  Serial.begin(9600);
  while(!Serial){
    ;
  }
  dht.begin();
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
  server.begin();                           
  printWifiStatus();
}


void loop() {
  WiFiClient client = server.available();   
  if (client) {  
    measureData();                           
    Serial.println("new client");       
    String currentLine = ""; //buffer for user incoming data
    while (client.connected()) {     
      if (client.available()) {  //check if there's bytes to read from the client,
        char c = client.read();  // read it
        Serial.write(c);                    
        if (c == '\n') {    // if the byte is a newline character
          // two newline characters in a row = end of client request so response
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
            // the content of the HTTP response follows the header:
            client.print("<h1>Data measure from the plant </h1>");
            client.print("<p>Temperature is "+ String(t)+"°C</p>");
            client.print("<p>Humidity is " + String(h) +"%</p>");
            client.print("<h2>Soil humidity</h2>");
            if(moisture<300){
              client.print("Soil dry");
            } else if (moisture<700){
              client.print("Soil humid");
            } else if (moisture >950){
              client.print("In watter");
            }
            // The HTTP response ends with another blank line:
            client.println();
            break;
          } else { 
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
        if (currentLine.endsWith("GET /H")) {
          digitalWrite(LED_BUILTIN, HIGH);             
        }
        if (currentLine.endsWith("GET /L")) {
          digitalWrite(LED_BUILTIN, LOW);
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
}

void printWifiStatus() {
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
}

void measureData(){
  delay(2000);
  moisture = analogRead(SOILMOISTUREPIN);
  h = dht.readHumidity();
  t = dht.readTemperature();
}
