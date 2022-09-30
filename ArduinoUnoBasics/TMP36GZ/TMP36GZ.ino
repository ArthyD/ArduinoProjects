#define sensorPin A3

void setup() {
  // Start serial communication
  Serial.begin(9600);
  Serial.println("*** Temperature measure ***");
  pinMode(sensorPin, INPUT);
}

void loop() {
  // Get a reading from the temperature sensor
  int reading = analogRead(sensorPin);

  // Convert the reading into voltage
  float voltage = reading * (5000 / 1024.0);

  // Convert the voltage into the temperature in Celsius
  float temperature = (voltage - 500) / 10;
  
  Serial.print(temperature);
  Serial.print(" \xC2\xB0"); // shows degree symbol
  Serial.println("C");

  delay(1000);
}
