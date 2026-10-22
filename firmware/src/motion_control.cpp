#include "motion_control.h"

#include <algorithm>
#include <cmath>

namespace robotic_arm {

namespace {

int toPwm(float normalized) {
    float scaled = normalized * 255.0f;
    if (scaled > 255.0f) scaled = 255.0f;
    if (scaled < -255.0f) scaled = -255.0f;
    return static_cast<int>(std::lround(scaled));
}

}  // namespace

MotorOutput mixArcadeDrive(float throttle, float turn) {
    float left = throttle + turn;
    float right = throttle - turn;

    // Scale both sides down together (instead of clipping) whenever the sum
    // would exceed +/-1.0, so a turn command at full throttle still steers
    // instead of just saturating both wheels at max forward speed.
    float maxMagnitude = std::max({std::fabs(left), std::fabs(right), 1.0f});
    left /= maxMagnitude;
    right /= maxMagnitude;

    return MotorOutput{toPwm(left), toPwm(right)};
}

}  // namespace robotic_arm
