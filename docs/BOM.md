# Bill of Materials

Sourced from the original project plan. Status reflects what the team had
on hand as of the last planning pass; update the checkboxes as the build
progresses.

## Core electronics

| Component | Role | Status |
|---|---|---|
| ESP32 DevKit (or compatible Arduino board) | Microcontroller / system brain | ✅ owned |
| PCA9685 16-channel PWM driver | Drives the 3–4 arm servos over I2C | — |
| L298N (or TB6612FNG) motor driver | Drives the drivetrain DC motors | — |
| Servo motors ×3–4 | Arm base / shoulder / elbow / gripper | — |
| DC motors ×2 | Left/right drivetrain | — |
| DFPlayer Mini + speaker | Text-to-speech / audio playback | — |
| LCD/OLED display | Facial expressions | ✅ owned |
| PS4 DualShock controller | Driver input (Bluetooth) | — |
| ESP32-CAM or OV7670 camera module | Vision (Part B) | — |

## Power

| Component | Role | Status |
|---|---|---|
| SHNITPWR 4–12 V power supply | Bench power during bring-up | ✅ owned |
| Battery pack (LiPo / NiMH / 9V) | Portable power | — |
| Voltage regulator (e.g. LM7805) | Steps pack voltage down to the 5 V logic rail | — |

## Mechanical / structural

| Component | Role | Status |
|---|---|---|
| Chassis (tank-track or car-style base) | Robot body | — |
| Wheels | Drivetrain | ✅ owned (Amazon) |
| 3D-printed arm (compact Arduino robot arm) | Single arm, per [Printables model](https://www.printables.com/model/818975-compact-robot-arm-arduino-3d-printed/files) | in progress |
| DOBTIM adhesive foam padding | Vibration damping / mounting | ✅ owned |
| Gorilla clear glue | Assembly | ✅ owned |
| AMUU rubber bands (500 pcs) | Temporary fastening during assembly | ✅ owned |
| ETopLike M1/M1.2/M1.4/M1.6 screw assortment | Fastening electronics + 3D-printed parts | ✅ owned |

## Wiring / small parts

| Component | Role | Status |
|---|---|---|
| USB-C adapter | Programming / charging | ✅ owned |
| IWISS 2.54 mm Dupont connector kit | Servo / Arduino jumper wiring | ✅ owned |
| Chanzon 3 mm LED assortment (100 pcs, 10 colors) | Status indicators / face accents | ✅ owned |
| Remote controller customizer | PS4 controller prep | — |
| Offline voice-recognition sensor (micro:) | Future offline voice command input | ✅ owned |

## Software tools

- Arduino IDE
- `Servo.h`, `Adafruit_PWMServoDriver.h`, `PS4Controller.h`, `DFPlayerMini.h`
- CAD tool for enclosure/arm tweaks (e.g. Shapr3D)
- 3D printing slicer

See [docs/ARCHITECTURE.md](ARCHITECTURE.md) for how these parts fit together and
[docs/WIRING.md](WIRING.md) for the ESP32 pin map used by the firmware in
[`firmware/`](../firmware).
