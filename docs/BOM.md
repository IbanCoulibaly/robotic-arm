# Bill of Materials

Final parts list for the finished build: a 3-servo arm driven by a PCA9685,
controlled directly by a PS4 controller through an ESP32. Nothing else is
in this build.

## Electronics

| Component | Role |
|---|---|
| ESP32 DevKit | Microcontroller — reads the PS4 controller, drives the PCA9685 |
| PCA9685 16-channel PWM driver | Generates the PWM signal for each servo over I2C |
| Servo motors ×3 | Base, shoulder, and elbow joints |
| PS4 DualShock controller | Driver input, paired over Bluetooth |

## Power

| Component | Role |
|---|---|
| SHNITPWR 4–12 V power supply | Bench power |
| Voltage regulator (e.g. LM7805) | Steps supply voltage down to the 5 V logic rail used by the ESP32 and PCA9685 |

## Mechanical / structural

| Component | Role |
|---|---|
| 3D-printed single arm | [Compact robot arm model](https://www.printables.com/model/818975-compact-robot-arm-arduino-3d-printed/files) |
| DOBTIM adhesive foam padding | Mounting / vibration damping |
| Gorilla clear glue | Assembly |
| ETopLike M1/M1.2/M1.4/M1.6 screw assortment | Fastening the 3D-printed parts and electronics |

## Wiring / small parts

| Component | Role |
|---|---|
| USB-C adapter | Programming power |
| IWISS 2.54 mm Dupont connector kit | Servo / ESP32 jumper wiring |

## Software tools

- Arduino IDE (or PlatformIO) with ESP32 board support
- `PS4Controller`, `Adafruit_PWMServoDriver`, `Wire` libraries
- `g++` (C++17) for the native unit test suite

---

The original planning document scoped a larger robot — a drivetrain,
camera, facial-expression display, and speaker — see
[docs/ARCHITECTURE.md](ARCHITECTURE.md) for what was actually built versus
that initial scope. Everything not listed above (motor driver, drive
motors, wheels/chassis, camera, display, audio module) was not built and is
not part of this repository's firmware.

See [docs/ARCHITECTURE.md](ARCHITECTURE.md) for how these parts fit together
and [docs/WIRING.md](WIRING.md) for the ESP32 pin map used by the firmware in
[`firmware/`](../firmware).
