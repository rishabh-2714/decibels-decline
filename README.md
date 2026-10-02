# Decibels Decline

A school tech expo project that we use to help teachers' nerves by monitoring noise levels and updating to a website with rewards to incentivise student quietness.

## Features

- **ESP32** - Use the .ino file to code an ESP32 which fully functions on the web
- **Full-stack Flask** - Runs on Python Flask. For permanent tracking, best on PythonAnywhere.
- **Hardware integration** - Requires hardware to run.

## Quick Start

### Prerequisites

- `flask` (Python library)
- ESP32 and supporting Arduino IDE
- MEMS Decibel Sensor, breadboard, common cathode RGB LED, buzzer, battery for ESP32
- PythonAnywhere account

### Installation

1. Clone the repository. On GitHub app/site, just clone. On terminal, 
```bash
  git clone https://github.com
  cd decibels-decline
```
2. Use the code as follows:
## Usage

- In Arduino IDE, download decibels.ino to your ESP32
- Link up all necessary hardware to the ESP32 board
- Link the rest of the repository to PythonAnywhere
- Switch on the ESP32 board. It will start collecting the data and send it to the PythonAnywhere Flask app.

## License

This project is licensed under a Classroom Use License - see the [LICENSE.md](LICENSE.md) file for details.
