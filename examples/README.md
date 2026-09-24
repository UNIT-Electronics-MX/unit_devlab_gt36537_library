# GT36537 DDP examples

| Example | Purpose |
|---|---|
| `light/serialLightRead` | Verify Device ID `0x0106` and print `adc0=<counts>` plus the relative percentage through Serial. |
| `light/serialLightRawPlot` | Print only one ADC count per line for `python/serial_plotter.py` or the Arduino Serial Plotter. |
| `i2c/changeAddress` | Interactively change the address of a GT36537 DDP node. |

The value is a 12-bit ADC count (`0` to `4095`) of the LDR divider on PA2, not
lux. Its direction depends on which end of the divider carries the LDR: cover
and uncover the sensor to check it before defining thresholds.

`serialLightRawPlot` uses 115200 baud and produces output such as `2652`, with
no headers or extra fields during acquisition.

The address example verifies Device ID `0x0106` before modifying anything. It
scans the bus at startup and scans again after a successful address change.
Open Serial at 115200 baud and enter, for example:

```text
scan
change 26 30
```

ESP32 uses SDA GPIO6 and SCL GPIO7. RP2040/RP2350 uses `Wire1`, SDA GPIO12
and SCL GPIO13. The factory I2C address is `0x26`.
