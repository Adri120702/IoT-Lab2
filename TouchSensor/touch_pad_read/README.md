| Supported Targets | ESP32-S2 | ESP32-S3 |
| ----------------- | -------- | -------- |

# Touch Pad Read Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

Read and display raw values from capacitive touch pad sensors.

Once configured, ESP32S2/S3 will continuously measure capacitance of touch pad sensors. Measurement is reflected as numeric value inversely related to sensor's capacitance. With a finger touched on a pad, its capacitance will get larger meanwhile the measured value gets smaller, and vice versa.

To detect if a sensor is touched or not, each particular design should be calibrated by obtaining both measurements for each individual sensor. Then a threshold between both values can be established. With specific threshold, API is then able to distinguish whether specific sensor is touched or released. For ESP32-S2/S3, the hardware integrates the edge detection algorithm, which can achieve the purpose of detecting touch actions by configuring appropriate parameters. There is another similar example that demonstrates how to perform simple calibration and trigger an interrupt when a pad is touched - see [touch_pad_interrupt](../touch_pad_interrupt).

## How to use example

### Hardware Required

* A development board with ESP32-S2 or ESP32-S3 chip
* A touch extension board like [esp32-s2-touch-devkit-1](https://docs.espressif.com/projects/espressif-esp-dev-kits/en/latest/esp32s2/esp32-s2-touch-devkit-1/user_guide.html)

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port to use.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Live Sensor Monitor

ESP32-S2/S3 supports up to 14 capacitive touch pads, T1 - T14. T0 is an internal channel used for de-noise and has no corresponding GPIO.

The serial monitor refreshes the readings in place. On startup, the example samples each pad for about two seconds to establish a baseline; do not touch the sensors during calibration. The `CAMBIO` column is the current raw reading minus that baseline. A negative change is common when touching a pad, but the direction and size can vary with the board and wiring.

```text
PAD   BASE     ACTUAL   CAMBIO
T1     6473      6120     -353
T2     6507      6505       -2
```

The display uses ANSI escape sequences to clear the terminal. Use a serial monitor that supports ANSI for the in-place refresh.

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
