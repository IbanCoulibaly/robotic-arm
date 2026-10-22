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

That's the entire wiring — the finished build has no other peripherals.

## PCA9685 servo channels

| Channel | Joint | Controller axis | Safe range (degrees) |
|---|---|---|---|
| 0 | Base | Left stick Y | 0–180 |
| 1 | Shoulder | Right stick X | 20–160 |
| 2 | Elbow | Right stick Y | 10–170 |

The "safe range" column is enforced in software by
[`ArmJointLimits`](../firmware/include/servo_control.h) — a full joystick
deflection can never command a joint past these values, regardless of what
the controller reports. Tune these to the physical arm's actual mechanical
stops if you rebuild it.

## Known addresses

- PS4 controller Bluetooth address: `AC:FD:93:6A:99:DC`
- ESP32 Bluetooth MAC: `3c:8a:1f:a7:69:c2`

Pass the controller's MAC to `PS4.begin("AC:FD:93:6A:99:DC")` in `setup()` to
pair with that specific controller instead of the first one found.

## Power

- Logic (ESP32, PCA9685): 5 V, regulated down from the bench/battery supply.
- Servos: powered from the PCA9685's dedicated V+ rail, not the ESP32's 5 V
  pin — three servos can draw more current than the ESP32's onboard
  regulator is rated for.

See [docs/BOM.md](BOM.md) for the specific power supply and regulator parts.
