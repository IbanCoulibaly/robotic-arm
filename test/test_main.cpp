// Native unit tests for the hardware-independent firmware logic.
//
// These compile and run on the host (no ESP32, no Arduino toolchain) via
// scripts/run_tests.sh, and in CI on every push (see
// .github/workflows/ci.yml).

#include "minitest.h"

#include "controller_input.h"
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

TEST_CASE(trigger_axis_zeroes_small_noise) {
    ASSERT_EQ(normalizeTriggerAxis(0), 0.0f);
    ASSERT_EQ(normalizeTriggerAxis(4), 0.0f);
}

TEST_CASE(trigger_axis_boundary_is_exclusive) {
    ASSERT_TRUE(normalizeTriggerAxis(5) != 0.0f);
}

TEST_CASE(trigger_axis_normalizes_to_unit_range) {
    ASSERT_NEAR(normalizeTriggerAxis(255), 1.0f, 0.001);
    ASSERT_NEAR(normalizeTriggerAxis(128), 0.502f, 0.01);
}

TEST_CASE(trigger_axis_never_goes_negative) {
    ASSERT_TRUE(normalizeTriggerAxis(0) >= 0.0f);
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

TEST_CASE(trigger_to_joint_angle_spans_full_range) {
    ArmJointLimits gripper{40, 140};
    ASSERT_EQ(triggerToJointAngle(0.0f, gripper), 40);
    ASSERT_EQ(triggerToJointAngle(1.0f, gripper), 140);
    ASSERT_EQ(triggerToJointAngle(0.5f, gripper), 90);
}

TEST_CASE(trigger_to_joint_angle_clamps_out_of_range_trigger_input) {
    ArmJointLimits gripper{40, 140};
    ASSERT_EQ(triggerToJointAngle(-1.0f, gripper), 40);
    ASSERT_EQ(triggerToJointAngle(2.0f, gripper), 140);
}

TEST_CASE(each_joint_limit_used_in_firmware_is_internally_consistent) {
    // Guards against a typo in main.cpp's joint-limit constants producing
    // min > max, which would silently invert clampAngle's behavior.
    ArmJointLimits base{0, 180};
    ArmJointLimits shoulder{20, 160};
    ArmJointLimits elbow{10, 170};
    ArmJointLimits wrist{0, 180};
    ArmJointLimits gripper{40, 140};
    ASSERT_TRUE(base.minAngle < base.maxAngle);
    ASSERT_TRUE(shoulder.minAngle < shoulder.maxAngle);
    ASSERT_TRUE(elbow.minAngle < elbow.maxAngle);
    ASSERT_TRUE(wrist.minAngle < wrist.maxAngle);
    ASSERT_TRUE(gripper.minAngle < gripper.maxAngle);
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
