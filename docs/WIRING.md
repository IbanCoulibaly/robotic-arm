# Wiring Reference

Pin assignments as used by [`firmware/src/main.cpp`](../firmware/src/main.cpp).
Update both this table and the constants at the top of `main.cpp` together if
you change the physical wiring.

## ESP32 pin map

| ESP32 pin | Connects to | Purpose |
|---|---|---|
| GPIO 22 | PCA9685 `SDA` | I2C data |
| GPIO 21 | PCA9685 `SCL` | I2C clock |
| (built-in BT radio) | PS4 controller | Bluetooth, pairs via `PS4.begin(mac)` |

That's the entire ESP32 wiring — the finished build has no other peripherals.

## PCA9685 servo channels

| Channel | Joint | Servo | Controller axis | Safe range (degrees) |
|---|---|---|---|---|
| 0 | Base | 20kg digital | Left stick X | 0–180 |
| 1 | Shoulder | 20kg digital | Left stick Y | 20–160 |
| 2 | Elbow | 20kg digital | Right stick Y | 10–170 |
| 3 | Wrist | 20kg digital | Right stick X | 0–180 |
| 4 | Gripper | 9g micro | R2 trigger | 40–140 |

The "safe range" column is enforced in software by
[`ArmJointLimits`](../firmware/include/servo_control.h) — a full joystick
deflection (or full trigger pull, for the gripper) can never command a
joint past these values, regardless of what the controller reports. Tune
these to the physical arm's actual mechanical stops if you rebuild it.

The gripper uses `triggerToJointAngle()` instead of `stickToJointAngle()`:
the R2 trigger has a natural rest position at 0 (released), not a centered
one, so it maps linearly across the joint's full range instead of
±90° around a 90° center.

## Known addresses

- PS4 controller Bluetooth address: `AC:FD:93:6A:99:DC`
- ESP32 Bluetooth MAC: `3c:8a:1f:a7:69:c2`

Pass the controller's MAC to `PS4.begin("AC:FD:93:6A:99:DC")` in `setup()` to
pair with that specific controller instead of the first one found.

## Power

- Logic (ESP32, PCA9685 control side): 5 V, regulated down from the
  bench/battery supply.
- Servos: powered from the PCA9685's dedicated V+ rail, not the ESP32's 5 V
  pin. The four 20kg digital servos can draw well over an amp each under
  load — size this rail (and its wiring gauge) for worst-case stall
  current across all four simultaneously, not just typical draw. The 9g
  micro gripper servo draws far less and can share the same rail.
- Use the T-connector battery leads for the servo rail specifically, kept
  separate from the ESP32's USB/regulator power path.

See [docs/BOM.md](BOM.md) for the specific power supply, regulator, and
connector parts.
