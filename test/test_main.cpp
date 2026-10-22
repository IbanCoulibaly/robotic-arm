// Native unit tests for the hardware-independent firmware logic.
//
// These compile and run on the host (no ESP32, no Arduino toolchain) via
// scripts/run_tests.sh, and in CI on every push (see
// .github/workflows/ci.yml).

#include "minitest.h"

#include "controller_input.h"
#include "expression_engine.h"
#include "motion_control.h"
#include "servo_control.h"

using namespace robotic_arm;

// ------------------------------- controller_input ---------------------------
TEST_CASE(deadzone_zeroes_small_noise) {
    ASSERT_EQ(normalizeStickAxis(0), 0.0f);
    ASSERT_EQ(normalizeStickAxis(5), 0.0f);
    ASSERT_EQ(normalizeStickAxis(-5), 0.0f);
    ASSERT_EQ(normalizeStickAxis(9), 0.0f);
    ASSERT_EQ(normalizeStickAxis(-9), 0.0f);
}

TEST_CASE(deadzone_boundary_is_exclusive) {
    ASSERT_TRUE(normalizeStickAxis(10) != 0.0f);
    ASSERT_TRUE(normalizeStickAxis(-10) != 0.0f);
}

TEST_CASE(stick_axis_normalizes_to_unit_range) {
    ASSERT_NEAR(normalizeStickAxis(127), 1.0f, 0.001);
    ASSERT_NEAR(normalizeStickAxis(-127), -1.0f, 0.001);
    ASSERT_NEAR(normalizeStickAxis(64), 0.5f, 0.01);
}

TEST_CASE(stick_axis_respects_custom_deadzone) {
    ASSERT_EQ(normalizeStickAxis(20, /*deadzone=*/30), 0.0f);
    ASSERT_TRUE(normalizeStickAxis(40, /*deadzone=*/30) != 0.0f);
}

// ------------------------------- motion_control ------------------------------
TEST_CASE(arcade_drive_full_forward) {
    MotorOutput out = mixArcadeDrive(1.0f, 0.0f);
    ASSERT_EQ(out.left, 255);
    ASSERT_EQ(out.right, 255);
}

TEST_CASE(arcade_drive_full_reverse) {
    MotorOutput out = mixArcadeDrive(-1.0f, 0.0f);
    ASSERT_EQ(out.left, -255);
    ASSERT_EQ(out.right, -255);
}

TEST_CASE(arcade_drive_in_place_turn) {
    MotorOutput out = mixArcadeDrive(0.0f, 1.0f);
    ASSERT_EQ(out.left, 255);
    ASSERT_EQ(out.right, -255);
}

TEST_CASE(arcade_drive_idle_sticks_stop_the_robot) {
    MotorOutput out = mixArcadeDrive(0.0f, 0.0f);
    ASSERT_EQ(out.left, 0);
    ASSERT_EQ(out.right, 0);
}

TEST_CASE(arcade_drive_scales_instead_of_clipping) {
    // Full throttle + full turn would be (2.0, 0.0) unscaled; the mixer
    // should scale both sides down together rather than saturating the
    // outer wheel at max forward and losing the turn entirely.
    MotorOutput out = mixArcadeDrive(1.0f, 1.0f);
    ASSERT_EQ(out.left, 255);
    ASSERT_EQ(out.right, 0);
}

TEST_CASE(arcade_drive_never_exceeds_pwm_range) {
    for (float throttle = -1.0f; throttle <= 1.0f; throttle += 0.25f) {
        for (float turn = -1.0f; turn <= 1.0f; turn += 0.25f) {
            MotorOutput out = mixArcadeDrive(throttle, turn);
            ASSERT_TRUE(out.left >= -255 && out.left <= 255);
            ASSERT_TRUE(out.right >= -255 && out.right <= 255);
        }
    }
}

// ------------------------------- servo_control -------------------------------
TEST_CASE(angle_to_pulse_endpoints) {
    ASSERT_EQ(angleToPulse(0), kServoMinPulse);
    ASSERT_EQ(angleToPulse(180), kServoMaxPulse);
}

TEST_CASE(angle_to_pulse_midpoint) {
    ASSERT_EQ(angleToPulse(90), (kServoMinPulse + kServoMaxPulse) / 2);
}

TEST_CASE(angle_to_pulse_clamps_out_of_range_input) {
    ASSERT_EQ(angleToPulse(-45), kServoMinPulse);
    ASSERT_EQ(angleToPulse(270), kServoMaxPulse);
}

TEST_CASE(clamp_angle_respects_joint_limits) {
    ASSERT_EQ(clampAngle(5, 20, 160), 20);
    ASSERT_EQ(clampAngle(175, 20, 160), 160);
    ASSERT_EQ(clampAngle(90, 20, 160), 90);
}

TEST_CASE(stick_to_joint_angle_centers_at_rest) {
    ArmJointLimits fullRange{0, 180};
    ASSERT_EQ(stickToJointAngle(0.0f, fullRange), 90);
}

TEST_CASE(stick_to_joint_angle_respects_mechanical_limits) {
    ArmJointLimits shoulder{20, 160};
    ASSERT_EQ(stickToJointAngle(1.0f, shoulder), 160);
    ASSERT_EQ(stickToJointAngle(-1.0f, shoulder), 20);
}

TEST_CASE(stick_to_joint_angle_clamps_out_of_range_stick_input) {
    // A malformed upstream reading outside [-1, 1] must still produce a
    // mechanically safe angle, never an out-of-bounds pulse downstream.
    ArmJointLimits fullRange{0, 180};
    ASSERT_EQ(stickToJointAngle(5.0f, fullRange), 180);
    ASSERT_EQ(stickToJointAngle(-5.0f, fullRange), 0);
}

// ------------------------------- expression_engine ----------------------------
TEST_CASE(speaking_overrides_all_button_expressions) {
    ButtonState buttons;
    buttons.triangle = true;
    ASSERT_EQ(determineExpression(buttons, /*isSpeaking=*/true), Expression::Speaking);
}

TEST_CASE(triangle_maps_to_happy) {
    ButtonState buttons;
    buttons.triangle = true;
    ASSERT_EQ(determineExpression(buttons, false), Expression::Happy);
}

TEST_CASE(square_maps_to_sad) {
    ButtonState buttons;
    buttons.square = true;
    ASSERT_EQ(determineExpression(buttons, false), Expression::Sad);
}

TEST_CASE(circle_maps_to_angry) {
    ButtonState buttons;
    buttons.circle = true;
    ASSERT_EQ(determineExpression(buttons, false), Expression::Angry);
}

TEST_CASE(cross_maps_to_surprised) {
    ButtonState buttons;
    buttons.cross = true;
    ASSERT_EQ(determineExpression(buttons, false), Expression::Surprised);
}

TEST_CASE(no_input_is_neutral) {
    ButtonState buttons;
    ASSERT_EQ(determineExpression(buttons, false), Expression::Neutral);
}

// ------------------------------------- runner ---------------------------------
int main() {
    auto& tests = minitest::registry();
    std::cout << "Running " << tests.size() << " test cases...\n\n";

    int passed = 0;
    for (auto& t : tests) {
        minitest::currentTestName = t.name;
        minitest::currentTestFailed = false;
        t.fn();
        if (!minitest::currentTestFailed) passed++;
    }

    std::cout << "\n" << minitest::assertionCount << " assertions across "
               << tests.size() << " test cases, " << minitest::failureCount
               << " assertion failures.\n";

    if (minitest::failureCount == 0) {
        std::cout << "ALL TESTS PASSED (" << passed << "/" << tests.size() << ")\n";
        return 0;
    }

    std::cout << "TESTS FAILED (" << passed << "/" << tests.size() << " test cases passed)\n";
    return 1;
}
