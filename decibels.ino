#include <ESP8266WiFi.h>
#include <I2S.h>
#include <math.h> 

// WiFi Configuration
const char* ssid = "name";
const char* password = "bisa1234$"; 

// 74HC595 Shift Register Bus Pins
const int DATA_PIN  = D2; // Pin 14 (DS)
const int CLOCK_PIN = D7; // Pin 11 (SH_CP)
const int LATCH_PIN = D1; // Pin 12 (ST_CP) 

// Digits/SSDs Control Pins (Common Cathode Ground Gates)
const int DIGIT_1_PIN = D0; // Powers left display (GPIO16)
const int DIGIT_2_PIN = TX; // Powers right display (GPIO1) 

// Control Interrupt Push Buttons
const int BTN_NEXT_PIN  = D3; // Button 1: Cycles keys (GPIO0)
const int BTN_SEL_PIN   = D4; // Button 2: Selects key (GPIO2) 

// Emergency Alert Elements
const int RED_LIGHT_PIN = RX; // Decibel trigger light (GPIO3)
const int BUZZER_PIN    = D9; // Decibel trigger piezo (GPIO7 / SD2) 

// Threshold configuration (in Decibels)
const int t = 60; 

// Array containing your exact JSON keys
const int TOTAL_KEYS = 18;
const char* jsonKeys[TOTAL_KEYS] = {
"6a", "6b", "6A", "6B", "6C", "7A", "7B", "7y", "7r",
"8B", "8A", "8g", "8s", "8h", "9", "10", "11", "12"
};
int currentKeyIndex = 0;
bool selectionFinalized = false; 

// Audio Sample Constraints
const int SAMPLE_WINDOW = 50;
const float REFERENCE_AMPLITUDE = 1.0; 

// Segment Lookup Array: Mapping ASCII characters to custom segment bits
// Segment order matching QA->QH map: 0b[DP][G][F][E][D][C][B][A] (1 = Segment ON)
uint8_t getSegmentByte(char c) {
switch(c) {
case '0': return 0b00111111;
case '1': return 0b00000110;
case '2': return 0b01011011;
case '3': return 0b01001111;
case '4': return 0b01100110;
case '5': return 0b01101101;
case '6': return 0b01111101;
case '7': return 0b00000111;
case '8': return 0b01111111;
case '9': return 0b01101111;
case 'A': return 0b01110111;
case 'B': return 0b00110111;
case 'C': return 0b00111001;
case 'a': return 0b01011111;
case 'b': return 0b01111100;
case 'g': return 0b01101111;
case 'h': return 0b01110100;
case 'r': return 0b01010000;
case 's': return 0b01101101;
case 'y': return 0b01101110;
default:  return 0b00000000; // Blank Space
}
} 

// Low-level function to stream out data into the 74HC595 shift register
void writeSegments(uint8_t mask) {
digitalWrite(LATCH_PIN, LOW);
shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, mask);
digitalWrite(LATCH_PIN, HIGH);
} 

// Multiplexes both displays back-and-forth once
void updateDisplay() {
const char* key = jsonKeys[currentKeyIndex];
char leftChar, rightChar; 

// Handle single digit alignment (like "9" -> " 9")
if (key == '\0') {
leftChar = ' ';
rightChar = key;
} else {
leftChar = key;
rightChar = key;
} 

// Flash Left Display
writeSegments(getSegmentByte(leftChar));
digitalWrite(DIGIT_1_PIN, LOW);  // Turn on Left SSD
delay(4);                        // Small window for persistence of vision
digitalWrite(DIGIT_1_PIN, HIGH); // Turn off Left SSD 

// Flash Right Display
writeSegments(getSegmentByte(rightChar));
digitalWrite(DIGIT_2_PIN, LOW);  // Turn on Right SSD
delay(4);                        // Small window for persistence of vision
digitalWrite(DIGIT_2_PIN, HIGH); // Turn off Right SSD
} 

void setup() {
// Serial initialization (Used during boot config mode)
Serial.begin(115200); 

// Configure shift register logic pins
pinMode(DATA_PIN, OUTPUT);
pinMode(CLOCK_PIN, OUTPUT);
pinMode(LATCH_PIN, OUTPUT); 

// Set up digit switching control lines
pinMode(DIGIT_1_PIN, OUTPUT);
pinMode(DIGIT_2_PIN, OUTPUT);
digitalWrite(DIGIT_1_PIN, HIGH); // Off state (Common Cathode)
digitalWrite(DIGIT_2_PIN, HIGH); // Off state (Common Cathode) 

// Set up buttons with internal pull-ups
pinMode(BTN_NEXT_PIN, INPUT_PULLUP);
pinMode(BTN_SEL_PIN, INPUT_PULLUP); 

// Set up sound action hardware
pinMode(RED_LIGHT_PIN, OUTPUT);
pinMode(BUZZER_PIN, OUTPUT);
digitalWrite(RED_LIGHT_PIN, LOW);
digitalWrite(BUZZER_PIN, LOW); 

// Connect to the WiFi network
WiFi.begin(ssid, password);
while (WiFi.status() != WL_CONNECTED) {
updateDisplay(); // Keep driving display loop while waiting for WiFi
} 

// INTERLOCK CHOICE LOOP: Holds device until selection is locked in
while (!selectionFinalized) {
updateDisplay(); // Keeps displaying the active key choice continuously 

// Read Next Button
if (digitalRead(BTN_NEXT_PIN) == LOW) {
currentKeyIndex = (currentKeyIndex + 1) % TOTAL_KEYS;
while(digitalRead(BTN_NEXT_PIN) == LOW) { updateDisplay(); }
}

// Read Select Button
if (digitalRead(BTN_SEL_PIN) == LOW) {
selectionFinalized = true;

// Output the chosen key down the line directly to Python app.py
Serial.println(jsonKeys[currentKeyIndex]);

writeSegments(0); // Clear shift register outputs completely
while(digitalRead(BTN_SEL_PIN) == LOW) { yield(); }
}
yield();

} 

// Shut down standard serial lines to clear pins TX/RX for active displays and lights
Serial.end(); 

// --- START AUDIO SENSING SUB-ROUTINE ---
if (!I2S.begin(I2S_PHILIPS_MODE, 16000, 16)) {
while (1);
}
} 

void loop() {
unsigned long startMillis = millis();
long long sumSquares = 0;
long sampleCount = 0; 

// Track sound input across the sample window frame
while (millis() - startMillis < SAMPLE_WINDOW) {
if (I2S.available()) {
int16_t sample = I2S.readSample();
sumSquares += (long long)sample * sample;
sampleCount++;
} 

// Continuously cycle displays inside sample window to prevent flickering
updateDisplay();
} 

int vol = 0;
if (sampleCount > 0) {
double meanSquare = (double)sumSquares / sampleCount;
double rms = sqrt(meanSquare);
if (rms < 1) rms = 1; 

vol = (int)(20.0 * log10(rms / REFERENCE_AMPLITUDE));
} 

// Trigger safety warnings if decibel threshold is breached
if (vol > t) {
digitalWrite(RED_LIGHT_PIN, HIGH); 

if (vol > 90) {
// Soft-tone active rule: reduce duty cycle using PWM to lower buzzer intensity
analogWrite(BUZZER_PIN, 45);
} else {
// Standard rule: full power output beep
digitalWrite(BUZZER_PIN, HIGH);
}

// Maintain alert state while keeping screens visible
unsigned long alertStart = millis();
while (millis() - alertStart < 2000) {
updateDisplay();
}

// Clear warning outputs completely
digitalWrite(RED_LIGHT_PIN, LOW);
digitalWrite(BUZZER_PIN, LOW);
analogWrite(BUZZER_PIN, 0);

} 

// Short recovery delay pacing loop
unsigned long recoveryStart = millis();
while (millis() - recoveryStart < 100) {
updateDisplay();
}
}
