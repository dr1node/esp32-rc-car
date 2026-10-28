# ESP32 RC Car

A Bluetooth-controlled RC car concept built around an ESP32 microcontroller, an L298N dual H-bridge motor driver, and four DC motors. This repository documents a reconstructed Arduino IDE implementation because the original source code is no longer available.

> **Project status:** Reconstructed source code — not yet verified on the original physical car. Pin assignments, motor wiring, Bluetooth behavior, and power requirements must be checked against the actual hardware before use.

## Overview

This project demonstrates how an ESP32 can receive commands over Bluetooth and control the movement of a four-wheel RC car. The example firmware supports forward, reverse, left, right, and stop commands.

The implementation uses the Arduino ESP32 Bluetooth Serial API (`BluetoothSerial`) and assumes a **Bluetooth Classic SPP-capable ESP32 board**. Not every ESP32-family board supports Bluetooth Classic; confirm your exact board model before compiling.

## Features

- Bluetooth Classic serial control from a compatible Android phone app
- Forward, reverse, left, right, and stop commands
- Two motor groups: left side and right side
- Adjustable PWM speed control
- Stop motors when no command has been received for a short timeout
- Clear pin configuration section for adapting to the real wiring

## Hardware Assumptions

The source code assumes the following setup:

- An ESP32 board that supports Bluetooth Classic SPP
- One L298N dual H-bridge motor driver module
- Four DC motors, with the two left motors connected as one group and the two right motors as another group
- A motor power supply suitable for the motors and driver
- A compatible phone app that can send single-character Bluetooth serial commands

**Important:** An L298N has two motor channels. This example groups two motors per channel. Check the motor current requirements and the driver's thermal/current limits before connecting two motors to one channel. If the motors draw too much current, use a suitable driver or separate drivers instead.

## Example Wiring

The GPIO assignments below are examples only. Change them to match the actual wiring and avoid pins reserved or unsuitable for your specific ESP32 board.

| ESP32 GPIO | L298N connection | Purpose |
|---|---|---|
| GPIO 26 | IN1 | Left motor direction |
| GPIO 27 | IN2 | Left motor direction |
| GPIO 14 | IN3 | Right motor direction |
| GPIO 13 | IN4 | Right motor direction |
| GPIO 25 | ENA | Left channel speed (PWM) |
| GPIO 33 | ENB | Right channel speed (PWM) |
| GND | GND | Common ground |

### Motor connections

- L298N OUT1 / OUT2: both left-side motors, wired in a suitable parallel configuration if driver capacity permits.
- L298N OUT3 / OUT4: both right-side motors, wired in a suitable parallel configuration if driver capacity permits.
- Motor supply: connect to the L298N motor power input according to the module's specifications.
- ESP32: power it through an appropriate regulated supply or USB.
- **Common ground is required** between the ESP32 and motor driver logic/power system.

If your L298N module has ENA/ENB jumper caps installed, remove them if you intend to control speed through GPIO 25 and GPIO 33. Module layouts can vary.

## Bluetooth Commands

Pair/connect using a Bluetooth Classic serial terminal app and send one character:

| Command | Action |
|---|---|
| `F` | Forward |
| `B` | Backward |
| `L` | Turn left |
| `R` | Turn right |
| `S` | Stop |
| `0`–`9` | Set speed from stop to maximum |

The firmware also accepts lowercase letters. This command protocol is an implementation choice for this reconstructed version, not a verified record of the original project's controls.

## Software Requirements

- Arduino IDE
- ESP32 board support package installed in Arduino IDE
- A Bluetooth Classic serial terminal app on a compatible phone
- No extra Arduino library is required for `BluetoothSerial` when using a compatible ESP32 Arduino core

## Upload Instructions

1. Open `src/main.cpp` in Arduino IDE, or create an Arduino sketch named `esp32_rc_car.ino` and copy the source into it.
2. Select the correct ESP32 board and serial port.
3. Verify the GPIO pin assignments against your actual wiring.
4. Disconnect motor power while checking wiring and preparing the first upload.
5. Upload the firmware.
6. Open a Bluetooth serial terminal app and connect to the device named `ESP32_RC_Car`.
7. Test `S` first, then test each movement command with the wheels lifted clear of the ground.

## Safety and Testing Notes

- Do not connect or disconnect motors while the circuit is powered.
- Verify voltage polarity and the power ratings of the ESP32, driver, and motors.
- Do not power DC motors directly from ESP32 GPIO pins.
- Lift the wheels off the ground during initial movement tests.
- If a motor driver overheats, current draw is excessive, or the ESP32 resets, disconnect power and review the circuit.
- This source has **not been tested on the original physical car**. Validate all wiring and behavior before operating it.

## Repository Structure

```text
esp32-rc-car/
├── README.md
├── src/
│   └── main.cpp
└── images/
    └── rc-car.jpg   # Add your own project photo here
```

## Project Image

Add your own photo of the RC car as `images/rc-car.jpg`. Use an image of the actual project as the main project image. If you add external reference images, clearly label them as references and check their usage permissions.

## Project Status

Documentation and reconstructed example firmware are provided for portfolio purposes. The source code is a starting point and should be updated after the original wiring, control method, and physical behavior can be confirmed.

## License

Choose a license before publishing this repository. If you want others to reuse the code, consider an open-source license such as MIT.
