#pragma once

namespace robotic_arm {

constexpr int kServoMinPulse = 150;  // PCA9685 pulse count at 0 degrees
constexpr int kServoMaxPulse = 600;  // PCA9685 pulse count at 180 degrees
constexpr int kServoMinAngle = 0;
constexpr int kServoMaxAngle = 180;

// Maps a servo angle (0-180 degrees) to a PCA9685 12-bit PWM pulse count
// (150-600 @ 50 Hz). Out-of-range angles are clamped first so a bad upstream
// value can't command the servo past its mechanical travel.
int angleToPulse(int angleDegrees);

// Clamps an angle into a joint's configured safe range.
int clampAngle(int angleDegrees, int minAngle, int maxAngle);

// Per-joint mechanical safety limits, in degrees.
struct ArmJointLimits {
    int minAngle;
    int maxAngle;
};

// Converts a normalized stick axis in [-1.0, 1.0] into a servo angle
// centered at 90 degrees, respecting the joint's configured limits. This is
// the single choke point that keeps a full-deflection joystick input from
// ever commanding a joint past its mechanical stop.
int stickToJointAngle(float normalizedStick, const ArmJointLimits& limits);

}  // namespace robotic_arm
