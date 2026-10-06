//make variables for pin names (see setup) and import necessary libraries. You need to send data to Python (written by Shaurya) via WiFi or IoT.
#include <WiFi.h> // Use <ESP8266WiFi.h> if you are using an ESP8266 board
const char* ssid = "name";
const char* password = "bisa1234$";
const int INMP441 = A0; 
const int RED_LIGHT_PIN = D1; 
const int BUZZER_PIN = D2; 
const int t = 60;
void setup() {
  //pinMode for a noise sensor, a large red light and a buzzer. Figure out which models.
  serial.begin(112500);
  pinMode(INMP441, INPUT);
  pinMode(RED_LIGHT_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(RED_LIGHT_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);
  Serial.begin(112500);
  WiFi.begin(name, bisa1234$);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
}

void loop() {
  //Over here, you need to make the actual code.
  //Start with storing the volume in a variable called `vol`
  int vol = analogRead(NOISE_SENSOR_PIN);
  // Publish variable to Serial Monitor and Python Serial
  // Shaurya's Python script can read this via `ser.readline()`
  Serial.println(vol);

  //Then, publish that variable to Serial Monitor and Python Serial

  //Then, assuming a variable t (for the volume threshold in deciBels),
  if(vol>t) {
  digitalWrite(RED_LIGHT_PIN, LOW);
    digitalWrite(BUZZER_PIN, HIGH);
    delay(2000);
    digitalWrite(RED_LIGHT_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);
  }
  delay(100);
}
