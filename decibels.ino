#include <ESP8266WiFi.h>
#include <I2S.h>
#include <math.h>

// WiFi Configuration
const char* ssid = "name";
const char* password = "bisa1234\$";

// Hardwired Pin Assignments
const int RED_LIGHT_PIN = D1; // Decibel trigger light
const int BUZZER_PIN    = D2; // Decibel trigger alert
const int BTN_NEXT_PIN  = D3; // Button 1: Cycles keys
const int BTN_SEL_PIN   = D4; // Button 2: Selects key

// Shared Segment Bus Pins (A, B, C, D, E, F, G)
const int SEG_PINS[7] = {D0, D8, RX, TX, SD3, SD2, D9}; 

// Digits/SSDs Control Pins (Common Cathode)
const int DIGIT_1_PIN = D7; // Powers left display
const int DIGIT_2_PIN = D6; // Powers right display

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
// Segment order in byte: 0b0[G][F][E][D][C][B][A] (1 = Segment ON)
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
    case 'B': return 0b00110111; // DISTINCT: Uppercase 'B' (Top, bottom, sides open middle - backwards C)
    case 'C': return 0b00111001;
    case 'a': return 0b01011111;
    case 'b': return 0b01111100; // DISTINCT: Lowercase 'b' (Traditional 6-without-top-bar style)
    case 'g': return 0b01101111;
    case 'h': return 0b01110100;
    case 'r': return 0b01010000;
    case 's': return 0b01101101;
    case 'y': return 0b01101110;
    default:  return 0b00000000; // Blank Space
  }
}

// Low-level function to illuminate physical segment pins
void writeSegments(uint8_t mask) {
  for (int i = 0; i < 7; i++) {
    digitalWrite(SEG_PINS[i], (mask & (1 << i)) ? HIGH : LOW);
  }
}

// Multiplexes both displays back-and-forth once
void updateDisplay() {
  const char* key = jsonKeys[currentKeyIndex];
  char leftChar, rightChar;

  // Handle single digit alignment (like "9" -> " 9")
  if (key[1] == '\0') {
    leftChar = ' ';
    rightChar = key[0];
  } else {
    leftChar = key[0];
    rightChar = key[1];
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
  Serial.begin(115200);

  // Set up pins for segment data bus
  for(int i = 0; i < 7; i++) {
    pinMode(SEG_PINS[i], OUTPUT);
  }

  // Set up digit switching control lines
  pinMode(DIGIT_1_PIN, OUTPUT);
  pinMode(DIGIT_2_PIN, OUTPUT);
  digitalWrite(DIGIT_1_PIN, HIGH); // Off state
  digitalWrite(DIGIT_2_PIN, HIGH); // Off state

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
      
      writeSegments(0); // Clear lines out entirely
      while(digitalRead(BTN_SEL_PIN) == LOW) { yield(); }
    }
    yield(); 
  }

  // --- START AUDIO SENSING SUB-ROUTINE ---
  if (!I2S.begin(I2S_PHILIPS_MODE, 16000, 16)) {
    Serial.println("Error: Failed to initialize I2S interface!");
    while (1); 
  }
}

void loop() {
  unsigned long startMillis = millis();
  long long sumSquares = 0;
  long sampleCount = 0;

  while (millis() - startMillis < SAMPLE_WINDOW) {
    if (I2S.available()) {
      int16_t sample = I2S.readSample();
      sumSquares += (long long)sample * sample; 
      sampleCount++;
    }
  }

  int vol = 0; 
  if (sampleCount > 0) {
    double meanSquare = (double)sumSquares / sampleCount;
    double rms = sqrt(meanSquare);
    if (rms < 1) rms = 1; 
    
    vol = (int)(20.0 * log10(rms / REFERENCE_AMPLITUDE)); 
  }

  // Send volume streaming updates to python script tracker
  Serial.println(vol);

  if (vol > t) {
    digitalWrite(RED_LIGHT_PIN, HIGH); 
    digitalWrite(BUZZER_PIN, HIGH);
    delay(2000); 
    digitalWrite(RED_LIGHT_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);
  }

  delay(100);
}
