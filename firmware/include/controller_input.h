#pragma once

#include <cstdint>

namespace robotic_arm {

// Normalized joystick axes used to drive the arm's 4 main joints, each in
// [-1.0, 1.0]. Mirrors the PS4 controller mapping wired up: left stick
// drives base/shoulder, right stick drives elbow/wrist.
struct StickAxes {
    float leftX = 0.0f;
    float leftY = 0.0f;
    float rightX = 0.0f;
    float rightY = 0.0f;
};

// Normalizes a raw PS4 stick axis (-127..127) to [-1.0, 1.0].
// Raw values inside +/-deadzone are snapped to exactly 0 to absorb joystick
// center drift, so the arm doesn't creep when the sticks are at rest.
float normalizeStickAxis(int8_t rawValue, uint8_t deadzone = 10);

// Normalizes a raw PS4 analog trigger reading (0..255, e.g. PS4.R2()) to
// [0.0, 1.0]. Drives the gripper's micro servo from the R2 trigger.
float normalizeTriggerAxis(uint8_t rawValue, uint8_t deadzone = 5);

}  // namespace robotic_arm
