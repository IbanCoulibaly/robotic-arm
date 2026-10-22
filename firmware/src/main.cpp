// Robotic Arm - ESP32 firmware (Part A: manual control)
//
// Hardware: ESP32 DevKit, PCA9685 16-channel PWM driver (I2C), PS4
// DualShock controller (Bluetooth), L298N dual H-bridge motor driver,
// DFPlayer Mini (TTS / audio playback), and an I2C/SPI display that renders
// facial expressions.
//
// This file is intentionally "thin": it only talks to hardware. Every
// decision (deadzone filtering, drive-mixing, servo angle mapping, which
// expression to show) lives in controller_input / motion_control /
// servo_control / expression_engine, which are plain, hardware-independent
// C++ covered by the native unit tests in test/. That split is what makes
// the logic testable on a laptop without an ESP32 attached.

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <PS4Controller.h>

#include "controller_input.h"
#include "expression_engine.h"
#include "motion_control.h"
#include "servo_control.h"

using namespace robotic_arm;

// ---- Pin / channel map ----------------------------------------------------
constexpr int kI2cSda = 22;
constexpr int kI2cScl = 21;

constexpr int kServoChannelBase = 0;      // arm base rotation
constexpr int kServoChannelShoulder = 1;
constexpr int kServoChannelElbow = 2;
constexpr int kServoChannelGripper = 3;

constexpr int kMotorLeftPwmPin = 25;   // L298N ENA
constexpr int kMotorLeftDirPin1 = 26;  // L298N IN1
constexpr int kMotorLeftDirPin2 = 27;  // L298N IN2
constexpr int kMotorRightPwmPin = 14;  // L298N ENB
constexpr int kMotorRightDirPin1 = 12; // L298N IN3
constexpr int kMotorRightDirPin2 = 13; // L298N IN4

// Mechanical safety margins so a full joystick deflection can never drive a
// joint into its physical stop.
const ArmJointLimits kBaseLimits{0, 180};
const ArmJointLimits kShoulderLimits{20, 160};
const ArmJointLimits kElbowLimits{10, 170};
const ArmJointLimits kGripperLimits{40, 140};

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// Set by the TTS/audio layer while a DFPlayer Mini clip is playing, so the
// expression engine can animate the mouth in sync with speech.
bool isSpeaking = false;

void driveMotor(int pwmPin, int dirPin1, int dirPin2, int signedSpeed) {
    bool forward = signedSpeed >= 0;
    digitalWrite(dirPin1, forward ? HIGH : LOW);
    digitalWrite(dirPin2, forward ? LOW : HIGH);
    analogWrite(pwmPin, abs(signedSpeed));
}

void applyExpression(Expression e) {
    // Hardware bring-up hook: wire this switch to whatever display driver
    // ends up on the board (OLED face sprites, LED matrix, etc). Keeping it
    // as a single switch means swapping display hardware only touches this
    // function, never the decision logic in expression_engine.cpp.
    switch (e) {
        case Expression::Happy:     /* render happy face */     break;
        case Expression::Sad:       /* render sad face */       break;
        case Expression::Angry:     /* render angry face */     break;
        case Expression::Surprised: /* render surprised face */ break;
        case Expression::Speaking:  /* animate mouth */         break;
        case Expression::Neutral:
        default:                    /* render resting face */   break;
    }
}

void onPs4Connect() { Serial.println("PS4 controller connected."); }
void onPs4Disconnect() { Serial.println("PS4 controller disconnected."); }

void setup() {
    Serial.begin(115200);

    Wire.begin(kI2cSda, kI2cScl);
    pwm.begin();
    pwm.setPWMFreq(50);

    pinMode(kMotorLeftPwmPin, OUTPUT);
    pinMode(kMotorLeftDirPin1, OUTPUT);
    pinMode(kMotorLeftDirPin2, OUTPUT);
    pinMode(kMotorRightPwmPin, OUTPUT);
    pinMode(kMotorRightDirPin1, OUTPUT);
    pinMode(kMotorRightDirPin2, OUTPUT);

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
    sticks.leftX = normalizeStickAxis(PS4.LStickX());
    sticks.leftY = normalizeStickAxis(PS4.LStickY());
    sticks.rightX = normalizeStickAxis(PS4.RStickX());
    sticks.rightY = normalizeStickAxis(PS4.RStickY());

    ButtonState buttons;
    buttons.cross = PS4.Cross();
    buttons.circle = PS4.Circle();
    buttons.square = PS4.Square();
    buttons.triangle = PS4.Triangle();
    buttons.l1 = PS4.L1();
    buttons.r1 = PS4.R1();
    buttons.l2 = PS4.L2();
    buttons.r2 = PS4.R2();

    // 2. Drivetrain: left stick -> arcade mix -> L298N.
    MotorOutput motors = mixArcadeDrive(sticks.leftY, sticks.leftX);
    driveMotor(kMotorLeftPwmPin, kMotorLeftDirPin1, kMotorLeftDirPin2, motors.left);
    driveMotor(kMotorRightPwmPin, kMotorRightDirPin1, kMotorRightDirPin2, motors.right);

    // 3. Arm: right stick (base/shoulder) + shoulder buttons (elbow/gripper)
    //    -> per-joint clamped angles -> PCA9685.
    int baseAngle = stickToJointAngle(sticks.rightX, kBaseLimits);
    int shoulderAngle = stickToJointAngle(sticks.rightY, kShoulderLimits);
    int elbowAngle = buttons.r1 ? kElbowLimits.maxAngle
                                 : (buttons.l1 ? kElbowLimits.minAngle : 90);
    int gripperAngle = buttons.r2 ? kGripperLimits.minAngle : kGripperLimits.maxAngle;

    pwm.setPWM(kServoChannelBase, 0, angleToPulse(baseAngle));
    pwm.setPWM(kServoChannelShoulder, 0, angleToPulse(shoulderAngle));
    pwm.setPWM(kServoChannelElbow, 0, angleToPulse(elbowAngle));
    pwm.setPWM(kServoChannelGripper, 0, angleToPulse(gripperAngle));

    // 4. Face: expression state machine -> display.
    applyExpression(determineExpression(buttons, isSpeaking));

    delay(20);  // ~50 Hz control loop
}
