#include <SPI.h>
#include <WiFiNINA.h>
#include <WiFiUdp.h>
#include <DHT.h>

//Access point name
char ssid[] = "PlantAP";     

// Define Wifi Client parameters
         
IPAddress gwip(172,128, 128, 1);         // GW fixed IP adress
IPAddress apip(172,128, 128, 100);         // AP fixed IP adress
uint8_t apChannel = 2;                  // AP wifi channel              

int status = WL_IDLE_STATUS;
WiFiServer server(80);

// Define UDP settings for DNS 
#define UDP_PACKET_SIZE 128           // MAX UDP packet size = 512
#define DNSHEADER_SIZE 12             // DNS Header
#define DNSANSWER_SIZE 16             // DNS Answer = standard set with Packet Compression
#define DNSMAXREQUESTS 16             // trigger first DNS requests, to redirect to own web-page
byte packetBuffer[ UDP_PACKET_SIZE];  // buffer to hold incoming and outgoing packets
byte dnsReplyHeader[DNSHEADER_SIZE] = { 
  0x00,0x00,   // ID, to be filled in #offset 0
  0x81,0x80,   // answer header Codes
  0x00,0x01,   //QDCOUNT = 1 question
  0x00,0x01,   //ANCOUNT = 1 answer
  0x00,0x00,   //NSCOUNT / ignore
  0x00,0x00    //ARCOUNT / ignore
  };
byte dnsReplyAnswer[DNSANSWER_SIZE] = {   
  0xc0,0x0c,  // pointer to pos 12 : NAME Labels
  0x00,0x01,  // TYPE
  0x00,0x01,  // CLASS
  0x00,0x00,  // TTL
  0x00,0x3c,  // TLL 1 hour
  0x00,0x04,   // RDLENGTH = 4
  0x00,0x00,  // IP adress octets to be filled #offset 12
  0x00,0x00   // IP adress octeds to be filled
  } ;
byte dnsReply[UDP_PACKET_SIZE];       // buffer to hold the send DNS reply
IPAddress dnsclientIp;
unsigned int dnsclientPort;
unsigned int udpPort = 53;            // local port to listen for UDP packets
WiFiUDP Udp;                          // A UDP instance to let us send and receive packets over UDP
int dnsreqCount=0;

#define DHTPIN 2     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11   // DHT 11
#define SOILMOISTUREPIN A3 // Analog pin connected to the moisture sensor
float h;
float t;
int moisture;
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  //Start serial communication
  Serial.begin(9600);
  Serial.println("Access Point Web Server with CaptivePortal");
  dht.begin();

  //Chek wifi module
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    while (true);
  }


  //Check firmware version
  String fv = WiFi.firmwareVersion();
  if (fv < "1.0.0") {
    Serial.println("Please upgrade the firmware");
  }

  //Creation of access point
  Serial.print("Creating access point named: ");
  Serial.println(ssid);
  WiFi.disconnect();
  //Config access point IPs
  WiFi.config(apip,apip,gwip,IPAddress(255,255,255,0));
  //create access point
  status = WiFi.beginAP(ssid,apChannel); 
  //Check if creation successfull
  if (status != WL_AP_LISTENING) {
    Serial.println("Creating access point failed");
    while (true) ;  
  }
  delay(10000);

  printWiFiStatus();

  //Begin UDP server
  Udp.begin(udpPort);
  server.begin();
}


void loop() {
  int t;
  char c;
  // compare the previous AP status to the current status
  IPAddress ip = WiFi.localIP();
  if (status != WiFi.status()) {
    // it has changed update the variable
    status = WiFi.status();

    if (status == WL_AP_CONNECTED) {
      // a device has connected to the AP
      Serial.println("Device connected to AP");
      dnsreqCount=0; 
      udpScan(); // scan DNS request for redirect
    }
    else {
      // a device has disconnected from the AP, and we are back in listening mode
      Serial.println("Device disconnected from AP");
    }
  }
  // Wifi Server check
  WiFiClient client = server.available();   // listen for incoming clients
  if (client) {     
    measureData();                        // if you get a client,
    Serial.println("new client");           
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        c = client.read();             // read a byte, then
        Serial.write(c);                    
        if (c == '\n') {                    // if the byte is a newline character
          // That's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // the content of the HTTP response follows the header:
            
            client.print("<h1>Data measure from the plant </h1>");
            client.print("<p>Temperature is "+ String(t)+"&deg;C</p>");
            client.print("<p>Humidity is " + String(h) +"%</p>");
            client.print("<h2>Soil humidity</h2>");
            if(moisture<300){
              client.print("Soil dry ");
            } else if (moisture<700){
              client.print("Soil humid ");
            } else if (moisture >950){
              client.print("In watter ");
            }
            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          }
          else {      // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        }
        else if (c != '\r') {    // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      } // end loop client avaialbe    
    } // end loo client connected
    
    // close the connection:
    client.stop();
    Serial.println("**client disconnected");
  }
  else udpScan();
}



void printWiFiStatus() {
  IPAddress ip;
  // print the SSID of the network you're attached to:
 
  Serial.print("Nina W10 firmware: ");Serial.println(WiFi.firmwareVersion());
  Serial.print("SSID: "); Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  ip = WiFi.localIP();
  Serial.print("IP Address  : ");  Serial.println(ip);
  // print your WiFi shield's gateway:
  ip = WiFi.gatewayIP();
  Serial.print("IP Gateway  : ");  Serial.println(ip);
  // print your WiFi shield's gateway:
  ip = WiFi.subnetMask();
  Serial.print("Subnet Mask : ");  Serial.println(ip);
  // print where to go in a browser:
  ip = WiFi.localIP();
  Serial.print("To see this page in action, open a browser to http://");  Serial.println(ip);
}

// UIDP port 53 - DNS - Scan
void udpScan(){
  int t=0;  // generic loop counter
  int r,p;  // reply and packet counters
  unsigned int packetSize=0;
  unsigned int replySize=0;
  packetSize = Udp.parsePacket();
  if ( (packetSize!=0) && (packetSize<UDP_PACKET_SIZE) ){  //only packets with small size
    // We've received a packet, read the data from it
    Udp.read(packetBuffer, packetSize); // read the packet into the buffer
    dnsclientIp = Udp.remoteIP();
    dnsclientPort = Udp.remotePort();
    if ( (dnsclientIp != apip) && (dnsreqCount<=DNSMAXREQUESTS) ){ // only non-local IP and only the first few DNSMAXREQUESTS x times
      Serial.print("DNS-packets (");Serial.print(packetSize);
      Serial.print(") from ");Serial.print(dnsclientIp);
      Serial.print(" port ");Serial.println(dnsclientPort);
      for (t=0;t<packetSize;++t){
        Serial.print(packetBuffer[t],HEX);Serial.print(":");
      }
      Serial.println(" ");
      for (t=0;t<packetSize;++t){
        Serial.print( (char) packetBuffer[t]);//Serial.print("");
      }
      Serial.println("");
      //Copy Packet ID and IP into DNS header and DNS answer
      dnsReplyHeader[0] = packetBuffer[0];dnsReplyHeader[1] = packetBuffer[1]; // Copy ID of Packet offset 0 in Header
      dnsReplyAnswer[12] = apip[0];dnsReplyAnswer[13] = apip[1];dnsReplyAnswer[14] = apip[2];dnsReplyAnswer[15] = apip[3]; // copy AP Ip adress offset 12 in Answer
      r=0; // set reply buffer counter
      p=12; // set packetbuffer counter @ QUESTION QNAME section
      // copy Header into reply
      for (t=0;t<DNSHEADER_SIZE;++t) dnsReply[r++]=dnsReplyHeader[t];
      // copy Qusetion into reply:  Name labels till octet=0x00
      while (packetBuffer[p]!=0) dnsReply[r++]=packetBuffer[p++];
      // copy end of question plus Qtype and Qclass 5 octets
      for(t=0;t<5;++t)  dnsReply[r++]=packetBuffer[p++];
      //copy Answer into reply
      for (t=0;t<DNSANSWER_SIZE;++t) dnsReply[r++]=dnsReplyAnswer[t];
      replySize=r;
      Serial.print("DNS-Reply (");Serial.print(replySize);
      Serial.print(") from ");Serial.print(apip);
      Serial.print(" port ");Serial.println(udpPort);
      for (t=0;t<replySize;++t){
        Serial.print(dnsReply[t],HEX);Serial.print(":");
      }
      Serial.println(" ");
      for (t=0;t<replySize;++t){
        Serial.print( (char) dnsReply[t]);//Serial.print("");
      }
      Serial.println("");  
      // Send DSN UDP packet
      Udp.beginPacket(dnsclientIp, dnsclientPort); //reply DNSquestion
      Udp.write(dnsReply, replySize);
      Udp.endPacket();
      dnsreqCount++;
    } // end loop correct IP
  } // end loop received packet
}

void measureData(){
  delay(2000);
  moisture = analogRead(SOILMOISTUREPIN);
  h = dht.readHumidity();
  t = dht.readTemperature();
}
