#pragma once

#include <cstdint>

namespace robotic_arm {

// Normalized joystick axes used to drive the arm, each in [-1.0, 1.0].
// Mirrors the PS4 controller mapping actually wired up: left stick Y drives
// the base, right stick X/Y drive the shoulder and elbow.
struct StickAxes {
    float leftY = 0.0f;
    float rightX = 0.0f;
    float rightY = 0.0f;
};

// Normalizes a raw PS4 stick axis (-127..127) to [-1.0, 1.0].
// Raw values inside +/-deadzone are snapped to exactly 0 to absorb joystick
// center drift, so the arm doesn't creep when the sticks are at rest.
float normalizeStickAxis(int8_t rawValue, uint8_t deadzone = 10);

}  // namespace robotic_arm
