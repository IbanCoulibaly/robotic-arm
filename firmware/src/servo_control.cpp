#include "servo_control.h"

#include <cmath>

namespace robotic_arm {

int clampAngle(int angleDegrees, int minAngle, int maxAngle) {
    if (angleDegrees < minAngle) return minAngle;
    if (angleDegrees > maxAngle) return maxAngle;
    return angleDegrees;
}

int angleToPulse(int angleDegrees) {
    int angle = clampAngle(angleDegrees, kServoMinAngle, kServoMaxAngle);
    long span = kServoMaxPulse - kServoMinPulse;
    long pulse = (static_cast<long>(angle) * span) / (kServoMaxAngle - kServoMinAngle) + kServoMinPulse;
    return static_cast<int>(pulse);
}

int stickToJointAngle(float normalizedStick, const ArmJointLimits& limits) {
    float clamped = normalizedStick;
    if (clamped > 1.0f) clamped = 1.0f;
    if (clamped < -1.0f) clamped = -1.0f;

    float angle = 90.0f + clamped * 90.0f;
    return clampAngle(static_cast<int>(std::lround(angle)), limits.minAngle, limits.maxAngle);
}

}  // namespace robotic_arm
