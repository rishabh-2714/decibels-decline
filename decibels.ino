#include <ESP8266WiFi.h> // Correct library for your ESP8266 board
#include <I2S.h>         // Required to run the INMP441 digital microphone
#include <math.h>

// WiFi Configuration
const char* ssid = "name";
const char* password = "bisa1234\$";

// Pin Configurations
const int RED_LIGHT_PIN = D1; 
const int BUZZER_PIN = D2; 

// Threshold configuration (in Decibels)
const int t = 60; 

// Sampling constants for I2S audio processing
const int SAMPLE_WINDOW = 50;          // How long to listen before calculating dB (in milliseconds)
const float REFERENCE_AMPLITUDE = 1.0; // Baseline reference for dB scale calibration

void setup() {
  // Initialize Serial at standard 115200 baud (fixed typo from 112500)
  Serial.begin(115200);

  // Initialize digital outputs
  pinMode(RED_LIGHT_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(RED_LIGHT_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);

  // Initialize the I2S Interface for INMP441 (Uses hardware fixed pins: D5, D6, D7)
  if (!I2S.begin(I2S_PHILIPS_MODE, 16000, 16)) {
    Serial.println("Error: Failed to initialize I2S interface!");
    while (1); // Stop execution if microphone fails
  }

  // Connect to WiFi network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void loop() {
  unsigned long startMillis = millis();
  long long sumSquares = 0;
  long sampleCount = 0;

  // 1. Gather digital audio packets over the sample window time
  while (millis() - startMillis < SAMPLE_WINDOW) {
    if (I2S.available()) {
      int16_t sample = I2S.readSample();
      sumSquares += (long long)sample * sample; // Track signal energy
      sampleCount++;
    }
  }

  // 2. Convert raw audio energy into Decibel levels
  int vol = 0; // Storing the volume in an integer variable called 'vol'
  if (sampleCount > 0) {
    double meanSquare = (double)sumSquares / sampleCount;
    double rms = sqrt(meanSquare);
    if (rms < 1) rms = 1; // Prevent mathematical errors in pure silence
    
    vol = (int)(20.0 * log10(rms / REFERENCE_AMPLITUDE)); // Store final dB value
  }

  // 3. Publish variable to Serial Monitor and Python Serial
  // Shaurya's Python script reads this number directly via `ser.readline()`
  Serial.println(vol);

  // 4. Check if volume crosses the decibel threshold 't'
  if (vol > t) {
    digitalWrite(RED_LIGHT_PIN, HIGH); // Changed to HIGH so it turns ON with the buzzer
    digitalWrite(BUZZER_PIN, HIGH);
    delay(2000); // Trigger alert actions for 2 seconds
    digitalWrite(RED_LIGHT_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);
  }

  delay(100);
}
