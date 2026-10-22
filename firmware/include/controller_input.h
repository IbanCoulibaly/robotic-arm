#pragma once

#include <cstdint>

namespace robotic_arm {

// Normalized joystick axes, each in the range [-1.0, 1.0].
struct StickAxes {
    float leftX = 0.0f;
    float leftY = 0.0f;
    float rightX = 0.0f;
    float rightY = 0.0f;
};

// Digital button state read from the PS4 controller.
struct ButtonState {
    bool cross = false;
    bool circle = false;
    bool square = false;
    bool triangle = false;
    bool l1 = false;
    bool r1 = false;
    bool l2 = false;
    bool r2 = false;
};

// Normalizes a raw PS4 stick axis (-127..127) to [-1.0, 1.0].
// Raw values inside +/-deadzone are snapped to exactly 0 to absorb joystick
// center drift, so the drivetrain and arm don't creep when the sticks are
// physically at rest.
float normalizeStickAxis(int8_t rawValue, uint8_t deadzone = 10);

}  // namespace robotic_arm
