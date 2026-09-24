/**
 * @file serialLightRead.ino
 * @brief Verifies a GT36537 DDP device (ID 0x0106) on the I2C bus and prints
 *        its raw ADC0 reading and relative percentage over Serial.
 *
 * The GT36537 is an LDR read through PA2 (ADC_IN2, 12 bits). The value is a
 * relative light measurement (0-4095), not lux: the direction of the reading
 * depends on which end of the divider the LDR is connected to.
 */

#include <Arduino.h>
#include <Wire.h>
#include <DevLabDDP.h>
#include <DevLab_I2C_Orchestrator.h>

#if defined(ARDUINO_ARCH_RP2040) || defined(ARDUINO_ARCH_RP2350)
  #define I2C_BUS Wire1
  constexpr int SDA_PIN = 12;
  constexpr int SCL_PIN = 13;
#elif defined(ARDUINO_ARCH_ESP32)
  #define I2C_BUS Wire
  constexpr int SDA_PIN = 6;
  constexpr int SCL_PIN = 7;
#elif defined(ARDUINO_ARCH_AVR)
  // AVR has fixed I2C pins (Uno/Nano: A4/A5, Mega: 20/21, Leonardo: 2/3);
  // SDA/SCL come from the board variant and begin() ignores the pin numbers.
  #define I2C_BUS Wire
  constexpr int SDA_PIN = SDA;
  constexpr int SCL_PIN = SCL;
#else
#error "Use an ESP32, RP2040, RP2350, or AVR master"
#endif

constexpr uint32_t I2C_FREQ = 400000UL;
constexpr uint8_t SENSOR_ADDRESS = 0x26U;
constexpr uint32_t READ_INTERVAL_MS = 250U;
constexpr uint16_t ADC_RESPONSE_DELAY_MS = 2U;

DevLab_I2C_Orchestrator bus(I2C_BUS, I2C_FREQ);
DevLabDDP::Master sensor(bus, DevLabDDP::DEVICE_GT36537);
bool deviceVerified = false;

// The firmware updates PA2 in the background. Read its latest sample directly
// to avoid the longer conversion delay of the generic readAdc() helper.
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
  Serial.begin(9600);
  bus.begin(SDA_PIN, SCL_PIN);

  DevLabDDP::DeviceInfo info;
  deviceVerified = sensor.matchesExpectedDevice(SENSOR_ADDRESS, &info);
  if (!deviceVerified) {
    Serial.println("ERROR: expected GT36537 DDP device ID 0x0106 at 0x26");
    return;
  }
  Serial.println("GT36537 ready: PA2 ADC0 reads the LDR divider");
}

void loop() {
  static uint32_t lastRead = 0U;
  if (millis() - lastRead < READ_INTERVAL_MS) {
    return;
  }
  lastRead = millis();

  uint16_t raw = 0U;
  if (deviceVerified && readLatestLight(raw)) {
#if defined(ARDUINO_ARCH_AVR)
    // AVR Serial has no printf(), and avr-libc printf lacks %f.
    Serial.print("adc0=");
    Serial.print(raw);
    Serial.print(" percent=");
    Serial.println((raw * 100.0f) / 4095.0f, 1);
#else
    Serial.printf("adc0=%u percent=%.1f\n", raw, (raw * 100.0f) / 4095.0f);
#endif
  } else {
    Serial.println("ERROR: light read failed");
  }
}
