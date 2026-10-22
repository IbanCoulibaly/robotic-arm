# Architecture

## Overview

Robotic Arm is a two-part build:

- **Part A (this repo's firmware target):** a manually operated robot — PS4
  controller in, drivetrain + 4-DOF arm + facial-expression display + TTS
  out. No autonomy; every action is a direct response to a human operator.
- **Part B (roadmap):** adds a camera and onboard perception so the robot can
  navigate and interact without a human driving every motion.

See the system block diagram:
[`docs/diagrams/system-architecture.svg`](diagrams/system-architecture.svg).

## Hardware architecture

One ESP32 DevKit is the single point of control. It talks to four peripheral
subsystems, each over a different bus, chosen to match what each peripheral
actually needs:

| Subsystem | Bus | Why |
|---|---|---|
| PCA9685 (servo driver) | I2C | Offloads PWM generation for 4+ servos so the ESP32's own timers aren't contended |
| L298N (motor driver) | GPIO + PWM | Simple H-bridge, no addressing needed |
| DFPlayer Mini (audio/TTS) | UART | Self-contained MP3 decoder; ESP32 just sends play commands |
| Display (face) | I2C or SPI | Cheap, well-supported by common OLED/TFT libraries |

The PS4 controller connects over Bluetooth using the ESP32's built-in radio
— no extra communication module needed for Part A. Part B adds an ESP32-CAM
over Wi-Fi for vision, kept as a separate module so a camera failure can't
take down drivetrain/arm control.

Full pin map: [`docs/WIRING.md`](WIRING.md).

### Chassis material trade-off (from the original design notes)

Three chassis materials were evaluated:

- **Plywood** — cheap, easy to shape with hand tools, but warps over time.
- **Acrylic/polycarbonate** — insulating (good for mounting electronics),
  lightweight, inexpensive, but also warps under heat; best used as a
  secondary, non-load-bearing layer.
- **Steel** — heaviest and most expensive, but immune to warping and gives
  the most structural confidence for an outer frame.

Decision: wood/acrylic for the primary deck (cheap iteration while the
electronics layout is still changing), steel reserved for a structural outer
frame once the design stabilizes.

Two drivetrain layouts were considered: continuous track ("tank") wheels
(better load distribution and terrain handling, at the cost of speed,
assembly complexity, and price) versus a car-style 4-wheel chassis (simpler,
cheaper, swappable tires, but can't pivot in place and has limited load
distribution). The current BOM targets a tank-style base
([Thingiverse reference](https://www.thingiverse.com/thing:971568/files)) for
better indoor maneuverability.

## Software architecture

See [`docs/diagrams/software-architecture.svg`](diagrams/software-architecture.svg).

The firmware is deliberately split into two kinds of code:

1. **Hardware glue** — [`firmware/src/main.cpp`](../firmware/src/main.cpp).
   Owns `setup()`/`loop()`, talks to `PS4Controller`, `Wire`,
   `Adafruit_PWMServoDriver`, and raw GPIO. It contains no decision logic —
   it just reads sensors, calls into the logic layer, and writes actuators.
2. **Decision logic** — `firmware/include/*.h` + `firmware/src/*.cpp`
   (everything except `main.cpp`). Plain, portable C++ with zero Arduino
   dependencies:
   - `controller_input` — deadzone filtering and axis normalization.
   - `motion_control` — arcade-style differential-drive mixing for the two
     drive motors.
   - `servo_control` — angle↔PWM-pulse mapping and per-joint mechanical
     safety clamps.
   - `expression_engine` — the button/speech → facial-expression state
     machine.

Because the logic layer never includes `Arduino.h`, it compiles and runs on
a laptop with a plain C++ compiler. That's what makes the robot's control
behavior unit-testable without any hardware — see
[`docs/diagrams/control-loop-sequence.svg`](diagrams/control-loop-sequence.svg)
for how one `loop()` tick moves through both halves, and the root
[README](../README.md#testing--metrics) for how to run the suite and what it
currently reports.

### Why this split matters

The original reference sketches (PS4 input reading, PCA9685 servo control)
mixed hardware I/O and control math directly in `loop()`, with no clamping
on servo angles. That's fine for a quick bring-up test, but it means the
only way to check "does full-stick-left actually keep the shoulder joint
inside its mechanical range" is to power on the real arm and watch it. This
repo's split moves that question into a unit test
(`stick_to_joint_angle_respects_mechanical_limits` in
[`test/test_main.cpp`](../test/test_main.cpp)) that runs in under a second,
on every commit, before any servo ever moves.

## Facial expression state machine

See [`docs/diagrams/expression-state-machine.svg`](diagrams/expression-state-machine.svg).
Priority order, evaluated every tick:

1. `isSpeaking == true` → **Speaking** (mouth animation), regardless of any
   button state — a TTS clip that gets interrupted by an emotion face would
   look broken.
2. Otherwise, the first true emotion button: Triangle → Happy, Square → Sad,
   Circle → Angry, Cross → Surprised.
3. Otherwise → **Neutral**.

## Roadmap

- [x] Part A — manual control: drivetrain, 4-DOF arm, facial expressions,
      TTS hook, unit-tested control logic.
- [ ] Part A — hardware bring-up: wire real PCA9685/L298N/DFPlayer/display,
      tune per-joint limits in `firmware/include/servo_control.h` to the
      physical arm's actual mechanical stops.
- [ ] Part B — camera-based perception (ESP32-CAM) for obstacle awareness.
- [ ] Part B — autonomous navigation loop, replacing/augmenting direct
      controller input.
- [ ] Part B — AI-driven speech interaction (beyond canned TTS clips).
