#include "controller_input.h"

namespace robotic_arm {

float normalizeStickAxis(int8_t rawValue, uint8_t deadzone) {
    int dz = static_cast<int>(deadzone);
    if (rawValue > -dz && rawValue < dz) {
        return 0.0f;
    }

    float v = static_cast<float>(rawValue) / 127.0f;
    if (v > 1.0f) v = 1.0f;
    if (v < -1.0f) v = -1.0f;
    return v;
}

float normalizeTriggerAxis(uint8_t rawValue, uint8_t deadzone) {
    if (rawValue < deadzone) {
        return 0.0f;
    }

    float v = static_cast<float>(rawValue) / 255.0f;
    if (v > 1.0f) v = 1.0f;
    return v;
}

}  // namespace robotic_arm
