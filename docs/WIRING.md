# Wiring Reference

Pin assignments as used by [`firmware/src/main.cpp`](../firmware/src/main.cpp).
Update both this table and the constants at the top of `main.cpp` together if
you change the physical wiring.

## ESP32 pin map

| ESP32 pin | Connects to | Purpose |
|---|---|---|
| GPIO 22 | PCA9685 `SDA` | I2C data (servo driver) |
| GPIO 21 | PCA9685 `SCL` | I2C clock (servo driver) |
| GPIO 25 | L298N `ENA` | Left motor PWM (speed) |
| GPIO 26 | L298N `IN1` | Left motor direction bit 1 |
| GPIO 27 | L298N `IN2` | Left motor direction bit 2 |
| GPIO 14 | L298N `ENB` | Right motor PWM (speed) |
| GPIO 12 | L298N `IN3` | Right motor direction bit 1 |
| GPIO 13 | L298N `IN4` | Right motor direction bit 2 |
| (built-in BT radio) | PS4 controller | Bluetooth, pairs via `PS4.begin(mac)` |
| UART (TBD at bring-up) | DFPlayer Mini `RX`/`TX` | TTS / audio playback |
| I2C or SPI (shared bus or dedicated, TBD) | Display module | Facial expressions |

## PCA9685 servo channels

| Channel | Joint | Safe range (degrees) |
|---|---|---|
| 0 | Base rotation | 0–180 |
| 1 | Shoulder | 20–160 |
| 2 | Elbow | 10–170 |
| 3 | Gripper | 40–140 |

The "safe range" column is enforced in software by
[`ArmJointLimits`](../firmware/include/servo_control.h) — a full joystick
deflection can never command a joint past these values, regardless of what
the controller reports. Tune these once the arm is physically assembled and
you've found its real mechanical stops.

## Known addresses

- PS4 controller Bluetooth address: `AC:FD:93:6A:99:DC`
- ESP32 Bluetooth MAC: `3c:8a:1f:a7:69:c2`

Pass the controller's MAC to `PS4.begin("AC:FD:93:6A:99:DC")` in `setup()` to
pair with that specific controller instead of the first one found.

## Power rails

- Logic (ESP32, PCA9685): 5 V, regulated down from the main battery pack.
- Drive motors: run close to pack voltage (6–12 V) directly through the
  L298N, which has its own onboard regulator for its logic side — do **not**
  feed motor-rail voltage into the ESP32's 5 V pin.
- See [docs/BOM.md](BOM.md) for the specific power supply and regulator parts.
