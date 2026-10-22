#pragma once

#include "controller_input.h"

namespace robotic_arm {

enum class Expression {
    Neutral,
    Happy,
    Sad,
    Angry,
    Surprised,
    Speaking
};

// Pure decision function for the facial-expression display. Priority order:
//   1. Speaking   - overrides everything else so the mouth animates
//                   whenever a TTS clip is playing.
//   2. Button-triggered emotion (Triangle/Square/Circle/Cross).
//   3. Neutral    - resting face when nothing else applies.
Expression determineExpression(const ButtonState& buttons, bool isSpeaking);

}  // namespace robotic_arm
