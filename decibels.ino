#include <ESP8266WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Wi-Fi configuration
const char* ssid = "name";
const char* password = "YOUR_WIFI_PASSWORD";

// OLED configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_ADDRESS 0x3C

Adafruit_SSD1306 display(
  SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET
);

// Pin configuration
const int OLED_SDA_PIN = D2;
const int OLED_SCL_PIN = D1;
const int MIC_PIN = A0;

const int BTN_NEXT_PIN = D3;
const int BTN_SEL_PIN = D4;

const int RED_LIGHT_PIN = D5;
const int BUZZER_PIN = D6;

// Inverted raw ADC threshold: 0–1023.
const int SOUND_THRESHOLD = 750;

const unsigned long ALERT_DURATION = 2000;
const unsigned long DISPLAY_INTERVAL = 100;
const unsigned long DEBOUNCE_TIME = 30;

const int TOTAL_KEYS = 18;
const char* jsonKeys[TOTAL_KEYS] = {
  "6a", "6b", "6A", "6B", "6C",
  "7A", "7B", "7y", "7r",
  "8B", "8A", "8g", "8s", "8h",
  "9", "10", "11", "12"
};

int currentKeyIndex = 0;
bool selectionFinalized = false;

bool alertActive = false;
unsigned long lastLoudTime = 0;
unsigned long lastDisplayTime = 0;

// Variables for averaging every 10 readings
int sampleCount = 0;
long sampleSum = 0;

// Debounced button handling
struct Button {
  int pin;
  bool lastReading;
  bool stableState;
  unsigned long lastChange;
};

Button nextButton = {BTN_NEXT_PIN, HIGH, HIGH, 0};
Button selectButton = {BTN_SEL_PIN, HIGH, HIGH, 0};

bool wasPressed(Button& button) {
  bool reading = digitalRead(button.pin);
  unsigned long now = millis();

  if (reading != button.lastReading) {
    button.lastChange = now;
    button.lastReading = reading;
  }

  if (now - button.lastChange >= DEBOUNCE_TIME &&
      reading != button.stableState) {
    button.stableState = reading;
    return button.stableState == LOW;
  }

  return false;
}

void showSelection() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Select classroom");

  display.setTextSize(3);
  display.setCursor(40, 18);
  display.print(jsonKeys[currentKeyIndex]);

  display.setTextSize(1);
  display.setCursor(0, 48);
  display.print("D3: Next");

  display.setCursor(0, 56);
  display.print("D4: Select");

  display.display();
}

void showMicrophone(int micValue) {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Class: ");
  display.print(jsonKeys[currentKeyIndex]);

  display.setCursor(0, 12);
  display.print("Mic inverted raw");

  display.setTextSize(2);
  display.setCursor(0, 26);
  display.print("Val:");
  display.print(micValue);

  display.setTextSize(1);
  display.setCursor(0, 48);
  display.print("Threshold: ");
  display.print(SOUND_THRESHOLD);

  display.setCursor(0, 56);
  display.print(alertActive ? "ALERT" : "Monitoring");

  display.display();
}

void setup() {
  Serial.begin(115200);

  pinMode(MIC_PIN, INPUT);

  pinMode(BTN_NEXT_PIN, INPUT_PULLUP);
  pinMode(BTN_SEL_PIN, INPUT_PULLUP);

  pinMode(RED_LIGHT_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  digitalWrite(RED_LIGHT_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);

  Wire.begin(OLED_SDA_PIN, OLED_SCL_PIN);

  if (!display.begin(
        SSD1306_SWITCHCAPVCC,
        OLED_ADDRESS,
        true,
        false)) {
    Serial.println("OLED initialization failed");

    while (true) {
      delay(100);
    }
  }

  display.setTextWrap(false);
  showSelection();

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
}

void loop() {
  bool nextPressed = wasPressed(nextButton);
  bool selectPressed = wasPressed(selectButton);

  // Select classroom before starting microphone monitoring.
  if (!selectionFinalized) {
    if (nextPressed) {
      currentKeyIndex = (currentKeyIndex + 1) % TOTAL_KEYS;
      showSelection();
    }

    if (selectPressed) {
      selectionFinalized = true;
      Serial.println("Selected room: " + String(jsonKeys[currentKeyIndex]));
      lastDisplayTime = millis() - DISPLAY_INTERVAL;
    }

    delay(1);
    return;
  }

  unsigned long now = millis();

  if (now - lastDisplayTime >= DISPLAY_INTERVAL) {
    lastDisplayTime = now;

    // Invert reading: 0 becomes 1023, 1023 becomes 0.
    int micValue = 1023 - analogRead(MIC_PIN);

    // Accumulate for mean calculation
    sampleSum += micValue;
    sampleCount++;

    // Check threshold immediately for responsive alerts
    if (micValue > SOUND_THRESHOLD) {
      alertActive = true;
      lastLoudTime = now;
    }

    if (alertActive &&
        now - lastLoudTime >= ALERT_DURATION) {
      alertActive = false;
    }

    digitalWrite(RED_LIGHT_PIN, alertActive ? HIGH : LOW);
    digitalWrite(BUZZER_PIN, alertActive ? HIGH : LOW);

    // Send only the mean of every 10 readings to Serial
    if (sampleCount >= 10) {
      int meanVal = sampleSum / 10;

      // Print just the mean number
      Serial.println(meanVal);

      // Reset accumulators
      sampleSum = 0;
      sampleCount = 0;
    }

    showMicrophone(micValue);
  }

  delay(1);
}
