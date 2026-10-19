### PINOUT MANUAL & WIRING DIAGRAM

### PROJECT: UNIFIED DECIBEL-SENSING SYSTEM (ESP8266 + INMP441 + MULTIPLEXED SSD VIA 74HC595)

### [SECTION 1] INMP441 MEMS DIGITAL MICROPHONE PIPELINE

---
The INMP441 relies on a high-speed hardware I2S engine. These pin connections
must be wired exactly as specified below. Do not swap or relocate them. 

INMP441 MODULE PINESP8266 PHYSICAL PIN LABELPURPOSE / BUS LINE
**VDD**
3V3Power Input (+3.3V Only!)
**GND**
GNDCommon System Ground Reference
**L/R**
GNDHardware Left Channel Select
**SCK**
D5 (GPIO14)Hardware Bit Clock (BCLK)
**WS**
D8 (GPIO15)Word Select / Clock (LRCK)
**SD**
D6 (GPIO12)Serial Audio Data Out (DIN)

*⚡ DANGER NOTE:* Never link the VDD terminal of the mic to the 5V / VIN pin.
It will immediately fry the delicate internal silicon wafer of the sensor.
*Note:* GPIO15 (D8) requires an external pull-down resistor to ground for the
ESP8266 to successfully boot into its normal execution state. 

### [SECTION 2] SEVEN-SEGMENT DISPLAYS (SSDs) SINGLE REGISTER MULTIPLEX MATRIX

---
To run two individual independent displays while conserving IO ports, both modules
share a single 74HC595 Shift Register via a parallel segment bus. Connect Segment A
of Display 1 to Segment A of Display 2, and trace that junction to the QA pin of
the 74HC595. Repeat for all remaining segments. 

#### A. ESP8266 TO 74HC595 CONTROL BUS

ESP8266 PIN LABEL74HC595 IC TARGET PINSHIFT REGISTER ROLE
**D2 (GPIO4)**
Pin 14 (DS)Serial Data Input
**D7 (GPIO13)**
Pin 11 (SH_CP)Shift Register Clock Line
**D1 (GPIO5)**
Pin 12 (ST_CP)Storage Register Latch Line

#### B. REGISTER OUTPUT PIN SEGMENT MAPPING

Connect the parallel output pins of the single 74HC595 to the tied segment lines: 

74HC595 PIN LABELTARGET SHARED BUS SEGMENTARCHITECTURE ROLE
**QA (Pin 15)**
Segment ATop Horizontal Bar
**QB (Pin 1)**
Segment BUpper Right Vert Bar
**QC (Pin 2)**
Segment CLower Right Vert Bar
**QD (Pin 3)**
Segment DBottom Horizontal Bar
**QE (Pin 4)**
Segment ELower Left Vert Bar
**QF (Pin 5)**
Segment FUpper Left Vert Bar
**QG (Pin 6)**
Segment GMiddle Horizontal Bar
**QH (Pin 7)**
Decimal Point (DP)Decimal Point Indicator

#### C. DYNAMIC DIGIT SELECTION CONTROL

Connect the common pin (Cathode/Anode) of each display to these lines. The ESP8266
will rapidly alternate between these pins to switch the displays on and off. 

MULTIPLEX TARGET SCREENESP8266 PHYSICAL PIN LABELPOWER GATE LOGIC
**LEFT Display Common**
D0 (GPIO16)Active LOW/HIGH Ground Gate
**RIGHT Display Common**
TX (GPIO1)Active LOW/HIGH Ground Gate

### [SECTION 3] CONTROL INTERRUPT PUSH BUTTONS

---
Wire one terminal of the push switch directly to the pin below. Tie the
opposite terminal directly to any common GND plane. Internal software
pull-up configuration handles noise filtration. 

INTERFACE ACTIONESP8266 PHYSICAL PIN LABELEXPECTED STATE VALUE
**BUTTON 1: Cycle Options**
D3 (GPIO0)LOW when held / pressed
**BUTTON 2: Lock / Select**
D4 (GPIO2)LOW when held / pressed

*💡 STRATEGY NOTE:* GPIO0 and GPIO2 act as standard boot configuration inputs.
Using normal pull-up setups on these buttons guarantees the ESP8266 boots cleanly
into normal program execution mode upon receiving power. 

### [SECTION 4] EMERGENCY ALERT ELEMENTS
---
These pins drive warning outputs outward when the room decibel level crosses 
the pre-configured threshold index 't' (set to 60dB).

| PERIPHERAL WARNING UNIT | ESP8266 PHYSICAL PIN LABEL | ACTIVE DRIVING OUTPUT  |
| :---------------------- | :------------------------- | :--------------------- |
| Status Alert LED        | RX (GPIO3)                 | HIGH (+3.3V) = ON      |
| Acoustic Piezo Buzzer   | D9 (GPIO7 / SD2)*          | HIGH (+3.3V) = ON      |

*⚠️ CAUTION:* Remember that since this LED is wired to the RX pin, it might flicker 
very faintly for a split second when the ESP8266 first powers up or when you flash 
new code. Once `Serial.end()` runs in your setup, you have complete digital control.
--------------------------------------------------------------------------------
