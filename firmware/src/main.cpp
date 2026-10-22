// Robotic Arm - ESP32 firmware
//
// What's actually built: a single 3-servo arm (base/shoulder/elbow) driven
// by a PCA9685 PWM driver over I2C, controlled directly by a PS4 DualShock
// controller over Bluetooth. There is no drivetrain, camera, display, or
// speaker in this build.
//
// This file is intentionally "thin": it only talks to hardware. The
// decisions (deadzone filtering, servo angle mapping, per-joint safety
// clamps) live in controller_input / servo_control, which are plain,
// hardware-independent C++ covered by the native unit tests in test/. That
// split is what makes the logic testable on a laptop without an ESP32 or
// the physical arm attached.

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <PS4Controller.h>

#include "controller_input.h"
#include "servo_control.h"

using namespace robotic_arm;

// ---- Pin / channel map ----------------------------------------------------
constexpr int kI2cSda = 22;
constexpr int kI2cScl = 21;

constexpr int kServoChannelBase = 0;
constexpr int kServoChannelShoulder = 1;
constexpr int kServoChannelElbow = 2;

// Mechanical safety margins so a full joystick deflection can never drive a
// joint into its physical stop.
const ArmJointLimits kBaseLimits{0, 180};
const ArmJointLimits kShoulderLimits{20, 160};
const ArmJointLimits kElbowLimits{10, 170};

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

void onPs4Connect() { Serial.println("PS4 controller connected."); }
void onPs4Disconnect() { Serial.println("PS4 controller disconnected."); }

void setup() {
    Serial.begin(115200);

    Wire.begin(kI2cSda, kI2cScl);
    pwm.begin();
    pwm.setPWMFreq(50);

    PS4.attachOnConnect(onPs4Connect);
    PS4.attachOnDisconnect(onPs4Disconnect);
    PS4.begin();  // pass a MAC string, e.g. PS4.begin("AC:FD:93:6A:99:DC"),
                  // to pair to one specific controller.

    Serial.println("Robotic Arm ready.");
}

void loop() {
    if (!PS4.isConnected()) {
        return;
    }

    // 1. Read + normalize controller input.
    StickAxes sticks;
    sticks.leftY = normalizeStickAxis(PS4.LStickY());
    sticks.rightX = normalizeStickAxis(PS4.RStickX());
    sticks.rightY = normalizeStickAxis(PS4.RStickY());

    // 2. Map each stick axis to a mechanically clamped joint angle, then to
    //    a PCA9685 pulse, and write it out over I2C.
    int baseAngle = stickToJointAngle(sticks.leftY, kBaseLimits);
    int shoulderAngle = stickToJointAngle(sticks.rightX, kShoulderLimits);
    int elbowAngle = stickToJointAngle(sticks.rightY, kElbowLimits);

    pwm.setPWM(kServoChannelBase, 0, angleToPulse(baseAngle));
    pwm.setPWM(kServoChannelShoulder, 0, angleToPulse(shoulderAngle));
    pwm.setPWM(kServoChannelElbow, 0, angleToPulse(elbowAngle));

    delay(20);  // ~50 Hz control loop
}
