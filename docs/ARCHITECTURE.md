# Architecture

## Status: finished

This project is complete as scoped: a single 3-servo arm, controlled
directly by a PS4 controller through an ESP32. There is no drivetrain,
camera, facial-expression display, or speaker — those appeared in the
original brainstorm but were never built, and are not part of this
repository.

## Overview

One ESP32 reads a PS4 controller's left/right stick axes over Bluetooth and
turns them directly into three servo angles — base, shoulder, elbow — via a
PCA9685 PWM driver over I2C. That's the whole system.

See the system block diagram:
[`docs/diagrams/system-architecture.svg`](diagrams/system-architecture.svg).

## Hardware architecture

| Subsystem | Bus | Why |
|---|---|---|
| PCA9685 (servo driver) | I2C | Offloads PWM generation for 3 servos so the ESP32's own timers aren't contended |
| PS4 controller | Bluetooth (ESP32's built-in radio) | No extra communication module needed |

Full pin map: [`docs/WIRING.md`](WIRING.md).

### Arm construction

The arm itself is the 3D-printed
["compact robot arm"](https://www.printables.com/model/818975-compact-robot-arm-arduino-3d-printed/files)
design, assembled with the screws/glue/padding listed in
[`docs/BOM.md`](BOM.md), driven by 3 servos (base rotation, shoulder,
elbow).

## Software architecture

See [`docs/diagrams/software-architecture.svg`](diagrams/software-architecture.svg).

The firmware is split into two kinds of code:

1. **Hardware glue** — [`firmware/src/main.cpp`](../firmware/src/main.cpp).
   Owns `setup()`/`loop()`, talks to `PS4Controller`, `Wire`, and
   `Adafruit_PWMServoDriver`. It contains no decision logic — it just reads
   the controller, calls into the logic layer, and writes the servo driver.
2. **Decision logic** — `firmware/include/*.h` + `firmware/src/*.cpp`
   (everything except `main.cpp`). Plain, portable C++ with zero Arduino
   dependencies:
   - `controller_input` — deadzone filtering and axis normalization.
   - `servo_control` — angle↔PWM-pulse mapping and per-joint mechanical
     safety clamps.

Because the logic layer never includes `Arduino.h`, it compiles and runs on
a laptop with a plain C++ compiler. That's what makes the arm's control
behavior unit-testable without any hardware attached — see
[`docs/diagrams/control-loop-sequence.svg`](diagrams/control-loop-sequence.svg)
for how one `loop()` tick moves through both halves, and the root
[README](../README.md#testing--metrics) for how to run the suite and what it
reports.

### Why this split matters

The original reference sketches (PS4 input reading, PCA9685 servo control)
mixed hardware I/O and control math directly in `loop()`, with only a flat
0–180° clamp on every servo. That's fine for a quick bring-up test, but it
means the only way to check "does full-stick-left actually keep the
shoulder joint inside its mechanical range" is to power on the real arm and
watch it. This repo's split moves that question into a unit test
(`stick_to_joint_angle_respects_mechanical_limits` in
[`test/test_main.cpp`](../test/test_main.cpp)) that runs in under a second,
on every commit, before any servo ever moves — and lets each joint have its
own safety range instead of one shared 0–180° limit.

## What was scoped but not built

The original planning document described a larger robot: a two-wheel
drivetrain, an ESP32-CAM for vision, a facial-expression display, and a
DFPlayer Mini for text-to-speech, with a second ("Part B") phase for
autonomous navigation. None of that was built. This repository documents
and tests only the arm + PS4 controller system described above.
