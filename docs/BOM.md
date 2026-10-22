# Bill of Materials

Final parts list for the finished build: a 5-servo arm driven by a PCA9685,
controlled directly by a PS4 controller through an ESP32. Nothing else is
in this build.

## Electronics

| Component | Role |
|---|---|
| ESP32 DevKit | Microcontroller — reads the PS4 controller, drives the PCA9685 |
| PCA9685 16-channel PWM driver | Generates the PWM signal for each servo over I2C |
| 20kg digital servo ×4 | Base, shoulder, elbow, and wrist joints |
| 9g micro servo ×1 | Gripper, driven by the R2 analog trigger |
| PS4 DualShock controller | Driver input, paired over Bluetooth |

## Power

| Component | Role |
|---|---|
| SHNITPWR 4–12 V power supply | Bench power |
| Voltage regulator (e.g. LM7805) | Steps supply voltage down to the 5 V logic rail used by the ESP32 and PCA9685 |
| T-connector (Deans-style) battery leads | Power distribution from the pack to the servo rail |
| Panel pushbutton | Main power switch |

The 20kg digital servos can draw significant current under load — size the
servo-rail power supply for 4× that stall current, not just logic-rail
current. See [docs/WIRING.md](WIRING.md).

## Mechanical / structural

| Component | Role |
|---|---|
| 3D-printed single arm | [Compact robot arm model](https://www.printables.com/model/818975-compact-robot-arm-arduino-3d-printed/files) |
| Acrylic mounting plates ×2 | Electronics mounting |
| Nylon idler/drive gears ×3 | Arm mechanism |
| DOBTIM adhesive foam padding | Mounting / vibration damping |
| Gorilla clear glue | Assembly |
| M1 / M1.2 / M1.4 / M1.6 / M2 screw & washer assortment | Fastening the 3D-printed parts and electronics |

## Wiring / small parts

| Component | Role |
|---|---|
| USB-C adapter | Programming power |
| IWISS 1550pcs 2.54 mm Dupont connector kit | Servo / ESP32 jumper wiring |
| Fermerry multi-color hookup wire spools | Point-to-point wiring |
| Heat-shrink tubing assortment | Wire strain relief / insulation |
| Status LED | Power/connection indicator |

## Software tools

- Arduino IDE (or PlatformIO) with ESP32 board support
- `PS4Controller`, `Adafruit_PWMServoDriver`, `Wire` libraries
- `g++` (C++17) for the native unit test suite

## In the parts kit but not used in this build

The physical kit also includes an Arduino Leonardo and 4 potentiometers.
Per the finished architecture, control is PS4-controller-only (see
[docs/ARCHITECTURE.md](ARCHITECTURE.md)) — the Leonardo and potentiometers
are spare parts from early prototyping, not part of the shipped firmware.

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
