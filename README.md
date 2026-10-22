# Robotic Arm

[![CI](https://github.com/IbanCoulibaly/robotic-arm/actions/workflows/ci.yml/badge.svg)](https://github.com/IbanCoulibaly/robotic-arm/actions/workflows/ci.yml)
[![C++17](https://img.shields.io/badge/C%2B%2B-17-00599C?logo=c%2B%2B&logoColor=white)](firmware)
[![Platform: ESP32](https://img.shields.io/badge/platform-ESP32-E7352C?logo=espressif&logoColor=white)](docs/WIRING.md)
[![Status: Finished](https://img.shields.io/badge/status-finished-brightgreen)](docs/ARCHITECTURE.md)
[![License: MIT](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)

**Status: finished.** A 3-servo robot arm, controlled directly by a PS4
DualShock controller through an ESP32 and a PCA9685 PWM driver. That's the
complete, working system — no drivetrain, camera, display, or speaker.

The firmware's control logic — joystick deadzone filtering, axis
normalization, and servo angle mapping with per-joint mechanical safety
limits — is written as plain, hardware-independent C++ and covered by a
native unit test suite: **12 test cases, 28 assertions, 100% line
coverage**, running in CI on every push. See
[Testing & Metrics](#testing--metrics).

## Table of contents

- [How it works](#how-it-works)
- [Repository structure](#repository-structure)
- [Hardware](#hardware)
- [Getting started](#getting-started)
- [Testing & metrics](#testing--metrics)
- [License](#license)

## How it works

A PS4 DualShock controller pairs to the ESP32 over Bluetooth. On every
~20 ms control-loop tick, the firmware reads three stick axes and maps each
one directly to a servo joint: left stick Y → base, right stick X →
shoulder, right stick Y → elbow. Each axis is deadzone-filtered, then
mapped to an angle that is clamped to that joint's configured safe range,
then converted to a PWM pulse and written to the PCA9685 over I2C.

**System architecture** — the complete, finished wiring:

![System architecture](docs/diagrams/system-architecture.svg)

**Software architecture** — the firmware is split into hardware glue
(`main.cpp`, ESP32-only) and pure control logic (everything else), so the
logic can be unit-tested on a laptop with no robot attached:

![Software architecture](docs/diagrams/software-architecture.svg)

**One control-loop tick**, start to finish:

![Control loop sequence](docs/diagrams/control-loop-sequence.svg)

Deeper design rationale (bus choice, why the logic/hardware split exists,
and what was originally scoped but not built) lives in
[docs/ARCHITECTURE.md](docs/ARCHITECTURE.md).

## Repository structure

```
robotic-arm/
├── firmware/
│   ├── include/                   # Public headers for the logic layer
│   │   ├── controller_input.h
│   │   └── servo_control.h
│   └── src/
│       ├── controller_input.cpp   # Deadzone + stick normalization
│       ├── servo_control.cpp      # Angle<->pulse mapping, joint limits
│       └── main.cpp               # ESP32 entry point (hardware glue only)
├── test/
│   ├── minitest.h           # ~80-line dependency-free test framework
│   └── test_main.cpp        # 12 unit tests for the logic layer
├── scripts/
│   └── run_tests.sh         # Build, run, and report coverage locally / in CI
├── docs/
│   ├── ARCHITECTURE.md
│   ├── BOM.md
│   ├── WIRING.md
│   └── diagrams/             # Source SVGs for the diagrams in this README
└── .github/workflows/ci.yml  # Runs scripts/run_tests.sh on every push/PR
```

## Hardware

Full bill of materials: [docs/BOM.md](docs/BOM.md).
Pin map and bus assignments: [docs/WIRING.md](docs/WIRING.md).

Complete component list: ESP32 DevKit · PCA9685 servo driver · 3 servo
motors (base/shoulder/elbow) · PS4 DualShock controller · 3D-printed single
arm ([reference model](https://www.printables.com/model/818975-compact-robot-arm-arduino-3d-printed/files)).

## Getting started

### Flash the firmware (on real hardware)

1. Install the [Arduino IDE](https://www.arduino.cc/en/software) with ESP32
   board support, plus the libraries used in `main.cpp`: `PS4Controller`,
   `Adafruit_PWMServoDriver`, `Wire` (bundled).
2. Open `firmware/src/main.cpp` as a sketch (or load the `firmware/` folder
   in PlatformIO using an `esp32dev` environment).
3. Wire the board per [docs/WIRING.md](docs/WIRING.md) — it's just I2C to
   the PCA9685 and Bluetooth to the controller.
4. Set your PS4 controller's MAC in `PS4.begin("...")` if you want to pair
   to one specific controller.
5. Flash, power on, and pair the controller.

### Run the unit tests (no hardware required)

Everything in `firmware/include` / `firmware/src` except `main.cpp` is plain
C++17 with no Arduino dependency, so the control logic can be built and
tested on any machine with `g++`:

```bash
./scripts/run_tests.sh
```

## Testing & metrics

`scripts/run_tests.sh` compiles the logic layer plus `test/test_main.cpp`
with `g++ -std=c++17 --coverage`, runs the resulting binary, and reports
per-file line coverage via `gcov`. It's also what CI runs on every push —
see the badge at the top of this README.

**Last measured results** (reproduce locally with `./scripts/run_tests.sh`):

| Metric | Result |
|---|---|
| Test cases | 12 |
| Assertions | 28 |
| Failures | 0 |
| `controller_input.cpp` line coverage | 100% (9/9 lines) |
| `servo_control.cpp` line coverage | 100% (16/16 lines) |

What's covered: joystick deadzone edge cases (including a custom deadzone
width), stick-axis normalization at its endpoints and midpoint, servo
angle↔pulse mapping at its endpoints and midpoint, out-of-range input
clamping for both sticks and angles, and that every joint's configured
safety range is internally consistent (`min < max`).

`main.cpp` is intentionally excluded from this suite — it's ESP32-only
hardware glue (`PS4Controller`, `Wire`, `Adafruit_PWMServoDriver`) with no
branching logic of its own to test; its correctness is validated on the
real arm instead.

## License

[MIT](LICENSE)
