String setupMessage = "Hi I am the setup";
String helloMessage = "Hello world";
int loopCounter = 0;

void setup() {
  // put your setup code here, to run once:
  //Start serial communication
  Serial.begin(9600);
  String localSetUpMessage = "It also works here and I am local";
  Serial.println(setupMessage);
  Serial.println(localSetUpMessage);
}

void loop() {
  // put your main code here, to run repeatedly:
  String messageToSend = helloMessage + " " + String(loopCounter);
  loopCounter += 1;
  Serial.println(messageToSend);
  delay(5000);
}
