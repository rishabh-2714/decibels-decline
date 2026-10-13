# PINOUT MANUAL & WIRING DIAGRAM
## PROJECT: UNIFIED DECIEL-SENSING SYSTEM (ESP8266 + INMP441 + MULTIPLEXED SSDs)
--------------------------------------------------------------------------------

### [SECTION 1] INMP441 MEMS DIGITAL MICROPHONE PIPELINE
================================================================================
The INMP441 relies on a high-speed hardware I2S engine. These pin connections 
must be wired exactly as specified below. Do not swap or relocate them.

| INMP441 MODULE PIN | ESP8266 PHYSICAL PIN LABEL | PURPOSE / BUS LINE          |
| :----------------- | :------------------------- | :-------------------------- |
| VDD                | 3V3                        | Power Input (+3.3V Only!)   |
| GND                | GND                        | Common System Ground Reference|
| L/R                | GND                        | Hardware Left Channel Select |
| SCK                | D7                         | Hardware Bit Clock (BCLK)   |
| WS                 | D5                         | Word Select / Clock (LRCK)  |
| SD                 | D6                         | Serial Audio Data Out (DIN) |

*⚡ DANGER NOTE:* Never link the VDD terminal of the mic to the 5V / VIN pin. 
It will immediately fry the delicate internal silicon wafer of the sensor.


### [SECTION 2] SEVEN-SEGMENT DISPLAYS (SSDs) PARALLEL BUS & DIGIT LINES
================================================================================
To run two individual independent displays without running out of IO ports, 
both modules are wired in a shared parallel matrix bus. Connect Segment A of 
Display 1 to Segment A of Display 2, and trace that junction to the D0 pin. 
Repeat for all remaining segments.

#### A. SHARED SEGMENT BUS DATA MAP
| SEVEN-SEGMENT TERMINAL | ESP8266 PHYSICAL PIN LABEL | ARCHITECTURE ROLE     |
| :--------------------- | :------------------------- | :--------------------- |
| Segment A              | D0                         | Top Horizontal Bar     |
| Segment B              | D8                         | Upper Right Vert Bar   |
| Segment C              | RX                         | Lower Right Vert Bar   |
| Segment D              | TX                         | Bottom Horizontal Bar  |
| Segment E              | SD3                        | Lower Left Vert Bar    |
| Segment F              | SD2                        | Upper Left Vert Bar    |
| Segment G              | D9                         | Middle Horizontal Bar  |

#### B. DYNAMIC DIGIT SELECTION CONTROL
Connect the common pin of each display to these lines. This permits the ESP8266 
to rapidly switch between the Left and Right screens to create an illusion 
of both digits lit at once.

| MULTIPLEX TARGET SCREEN | ESP8266 PHYSICAL PIN LABEL | POWER GATE LOGIC      |
| :---------------------- | :------------------------- | :--------------------- |
| LEFT Display Common     | D7                         | Active LOW Ground Gate |
| RIGHT Display Common    | D6                         | Active LOW Ground Gate |

*⚙️ TIME-MULTIPLEX NOTES:* During the selection menu loop, D6 and D7 handle screen 
switching. Once you press select, the screen clears out and releases these lines 
instantly so the high-resolution I2S microphone engine can safely hook into them.


### [SECTION 3] CONTROL INTERRUPT PUSH BUTTONS
================================================================================
Wire one terminal of the push switch directly to the pin below. Tie the 
opposite terminal directly to any common GND plane. Internal software 
pull-up configuration handles noise filtration.

| INTERFACE ACTION        | ESP8266 PHYSICAL PIN LABEL | EXPECTED STATE VALUE   |
| :---------------------- | :------------------------- | :--------------------- |
| BUTTON 1: Cycle Options | D3                         | LOW when held / pressed|
| BUTTON 2: Lock / Select | D4                         | LOW when held / pressed|


### [SECTION 4] EMERGENCY ALERT ELEMENTS
================================================================================
These pins drive warning outputs outward when the room decibel level crosses 
the pre-configured threshold index 't' (set to 60dB).

| PERIPHERAL WARNING UNIT | ESP8266 PHYSICAL PIN LABEL | ACTIVE DRIVING OUTPUT  |
| :---------------------- | :------------------------- | :--------------------- |
| Large Red Alert Lamp    | D1                         | HIGH (+3.3V) = ON      |
| Acoustic Piezo Buzzer   | D2                         | HIGH (+3.3V) = ON      |

--------------------------------------------------------------------------------
EOF - END OF FILE
