
#include "main.h"

void CharacterState::load(const Character &character, const CharacterInstance &instance)
{
    const int animationCount = (int)character.animations.size();
    if (animationStates.size() != animationCount)
        animationStates.resize(animationCount);

    for (int i = 0; i < animationCount; i++)
        animationStates[i] = -1;

    //character. instance.reducedPoseDescriptor
}