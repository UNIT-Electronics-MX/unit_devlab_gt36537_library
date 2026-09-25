# DevLab_GT36537

Arduino library for the GT36537 light sensor (LDR) using the DevLab Device
Protocol (DDP) over I2C.

The sensor node is a PY32F003 that reads the midpoint of an LDR voltage
divider on PA2 (`ADC_IN2`) and exposes the 12-bit result through I2C. This
library gives Arduino sketches the DDP master needed to discover, validate and
read it.

Compatible with ESP32, RP2040/RP2350 and Arduino-compatible platforms.

---

# Features

- DDP 1.0 master through the `DevLabDDP` dependency
- Device identification (Device ID `0x0106`) before any read or write
- 12-bit light reading (`0` to `4095`), updated by the firmware every 20 ms
- Raw and inverted readings, each as a raw count or a `0-100%` percentage
- I2C address scan and reassignment (`0x08` to `0x77`)
- Bus recovery (`beginRecovered`) for a slave left mid-transaction
- Ready-to-use examples for Serial output, the serial plotter, and address management
- Supports custom I2C pins

---

# Supported Interfaces

| Interface | Support Status |
|---|---|
| I2C | Supported |
| SPI | Not available |

---

# Installation

## Manual Installation

1. Open Arduino IDE
2. Go to:

```text
Sketch -> Library Manager -> Search DevLab_GT36537...
```

3. Click on Install (the `DevLabDDP` dependency is installed with it)

4. Compile and upload the examples for the sensor

---

# Quick Start Example

```cpp
#include <Arduino.h>
#include <Wire.h>
#include <DevLab_GT365xx.h>

#define SDA_PIN 6
#define SCL_PIN 7

constexpr uint8_t SENSOR_ADDRESS = 0x26U;

DevLab_GT365xx sensor(Wire, SENSOR_ADDRESS);

void setup() {
  Serial.begin(115200);

  sensor.beginRecovered(SDA_PIN, SCL_PIN);
  if (!sensor.busReady() || !sensor.isConnected()) {
    Serial.println("GT36537 initialization failed.");
    while (1);
  }
}

void loop() {
  uint16_t raw;

  if (sensor.readRaw(raw)) {
    Serial.print("ADC0: ");
    Serial.print(raw);

    Serial.print(" (");
    Serial.print((raw * 100.0f) / 4095.0f);
    Serial.println(" %)");
  }

  delay(250);
}
```

`DevLab_GT365xx` owns its own `DevLab_I2C_Orchestrator` internally, so construct it from a `TwoWire` bus (as above) rather than wiring up the orchestrator by hand — passing a bus straight into `DevLabDDP::Master` does not compile, since that constructor takes the orchestrator by non-const reference.

---

# Wiring Example

| GT36537 node | MCU |
|---|---|
| SDA | SDA (GPIO6 on ESP32, GPIO12 on RP2040) |
| SCL | SCL (GPIO7 on ESP32, GPIO13 on RP2040) |
| VCC | 3.3V |
| GND | GND |

---

# Compatibility

| MCU Platform | Status |
|---|---|
| ESP32 | Compatible |
| ESP32-S3 | Compatible |
| RP2040 / RP2350 | Compatible |
| AVR (Uno, Nano, Mega, Leonardo) | Compatible (fixed I2C pins per board variant) |

---

# Notes

The reading is a relative light measurement, not lux. The GT36537 is an LDR
(20 to 30 kOhm at 10 lux, up to 2 MOhm in darkness, typical response 20 ms
rising and 30 ms falling). Converting counts to lux needs the fixed resistor,
the reference voltage, the optical geometry and a calibration against a lux
meter.

The direction of the reading depends on which end of the divider carries the
LDR. Cover and uncover the sensor before defining thresholds, and use two
thresholds (on/off) to add hysteresis and avoid noise near the limit.

The firmware answers two equivalent read commands, both returning the same
little-endian `uint16`:

| Command | Name |
|---:|---|
| `0x60` | `CMD_READ_ADC0` |
| `0x80` | `CMD_GT36537_RAW` |

The factory I2C address is `0x26` and can be changed with the
`i2c/changeAddress` example.

---

# Folder Structure

```text
DevLab_GT36537/
├── examples/
│   ├── i2c/
│   │   ├── changeAddress/
│   │   └── i2c_scanner/
│   └── light/
│       ├── serialLightRead/
│       ├── serialLightReadInverted/
│       └── serialLightRawPlot/
├── src/
│   ├── DevLab_GT365xx.h
│   └── DevLab_GT365xx.cpp
├── library.properties
├── README.md
└── LICENSE
```

---

# Version

| Parameter | Value |
|---|---|
| Library Name | DevLab_GT36537 |
| Version | 1.0.0 |
| Communication | I2C (DDP 1.0) |
| Device ID | `0x0106` |
| Architecture | Cross-platform |

---

# Author

UNIT Electronics - DevLab Ecosystem

---

# License

MIT License
