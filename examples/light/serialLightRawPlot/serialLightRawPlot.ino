/**
 * @file serialLightRawPlot.ino
 * @brief Streams the raw GT36537 ADC0 count (0-4095), one integer per line, at
 *        115200 baud for python/serial_plotter.py or the Arduino Serial Plotter.
 */

#include <Arduino.h>
#include <Wire.h>
#include <DevLabDDP.h>

#if defined(ARDUINO_ARCH_RP2040) || defined(ARDUINO_ARCH_RP2350)
TwoWire &sensorBus = Wire1;
constexpr int SDA_PIN = 12;
constexpr int SCL_PIN = 13;
#elif defined(ARDUINO_ARCH_ESP32)
TwoWire &sensorBus = Wire;
constexpr int SDA_PIN = 6;
constexpr int SCL_PIN = 7;
#else
#error "Use an ESP32, RP2040, or RP2350 master"
#endif

constexpr uint8_t SENSOR_ADDRESS = 0x26U;
constexpr uint32_t READ_INTERVAL_MS = 20U;
constexpr uint16_t ADC_RESPONSE_DELAY_MS = 2U;

DevLabDDP::Master sensor(sensorBus, DevLabDDP::DEVICE_GT36537);
bool deviceVerified = false;

// Output is intentionally one decimal integer per line so it can be consumed
// directly by python/serial_plotter.py without choosing a field.
bool readLatestLight(uint16_t &value) {
  uint8_t bytes[2];
  if (!sensor.readCommand(SENSOR_ADDRESS, CMD_READ_ADC0, bytes, 2U,
                          ADC_RESPONSE_DELAY_MS)) {
    return false;
  }
  value = (uint16_t)bytes[0] | ((uint16_t)bytes[1] << 8);
  return true;
}

void setup() {
  Serial.begin(115200);
  delay(500U);
  sensorBus.begin(SDA_PIN, SCL_PIN);
  sensorBus.setClock(400000U);

  DevLabDDP::DeviceInfo info;
  deviceVerified = sensor.matchesExpectedDevice(SENSOR_ADDRESS, &info);
  if (!deviceVerified) {
    Serial.println("ERROR: expected GT36537 DDP device ID 0x0106 at 0x26");
  }
}

void loop() {
  uint16_t raw = 0U;
  if (deviceVerified && readLatestLight(raw)) {
    Serial.println(raw);
  } else {
    Serial.println("ERROR: light read failed");
  }
  delay(READ_INTERVAL_MS);
}
