#pragma once

namespace robotic_arm {

// Signed PWM duty-cycle targets for the two drive motors, in [-255, 255].
// Positive drives a wheel forward, negative drives it in reverse.
struct MotorOutput {
    int left = 0;
    int right = 0;
};

// Arcade-style differential-drive mixing: combines a forward/back throttle
// axis and a left/right turn axis (both normalized to [-1.0, 1.0]) into
// independent left/right motor commands, scaling down proportionally
// instead of hard-clipping so the robot still turns predictably at full
// throttle.
MotorOutput mixArcadeDrive(float throttle, float turn);

}  // namespace robotic_arm
