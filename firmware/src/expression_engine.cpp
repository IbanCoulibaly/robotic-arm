#include "expression_engine.h"

namespace robotic_arm {

Expression determineExpression(const ButtonState& buttons, bool isSpeaking) {
    if (isSpeaking) return Expression::Speaking;
    if (buttons.triangle) return Expression::Happy;
    if (buttons.square) return Expression::Sad;
    if (buttons.circle) return Expression::Angry;
    if (buttons.cross) return Expression::Surprised;
    return Expression::Neutral;
}

}  // namespace robotic_arm
