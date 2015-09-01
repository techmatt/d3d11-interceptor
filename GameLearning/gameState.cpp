
#include "main.h"

void CharacterState::load(const Character &character, const CharacterInstance &instance)
{
    /*const int animationCount = (int)character.animations.size();
    
    bestPoseDistSq = numeric_limits<float>::max();
    activeAnimationsByIndex.clear();

    for (AnimationFrame frame : character.findAnimationFramesRadius(instance, (float)learningParams().maxPoseDistSq))
    {
        if (activeAnimationsByIndex.count(frame.animation->index) == 0)
        {
            AnimationFrame &newFrame = activeAnimationsByIndex[frame.animation->index];
            newFrame.animation = frame.animation;
            newFrame.poseDistSq = numeric_limits<float>::max();
            newFrame.offset = -1;
        }

        AnimationFrame &stateFrame = activeAnimationsByIndex[frame.animation->index];
        if (frame.poseDistSq < stateFrame.poseDistSq)
        {
            stateFrame.offset = frame.offset;
            stateFrame.poseDistSq = frame.poseDistSq;
            bestPoseDistSq = min(bestPoseDistSq, stateFrame.poseDistSq);
        }
    }

    vector<int> animationsToRemove;
    for (auto &entry : activeAnimationsByIndex)
    {
        if (entry.second.poseDistSq > bestPoseDistSq * 2.0f)
            animationsToRemove.push_back(entry.first);
    }
    for (int animation : animationsToRemove)
        activeAnimationsByIndex.erase(animation);*/
}

string CharacterState::describe() const
{
    /*string result;
    if (activeAnimationsByIndex.size() == 0) return "<none>";
    for (auto &animation : activeAnimationsByIndex)
    {
        string s = "a" + to_string(animation.first) + "-" +
            to_string(animation.second.offset) + ":" + to_string(animation.second.poseDistSq) + " | ";
        result += s;
    }
    return result;*/
    return "<none>";
}

void GameState::load(const ProcessedFrame &frame, const CharacterDatabase &characters)
{
    for (int characterIndex = 0; characterIndex < characters.characters.size(); characterIndex++)
    {
        const Character &character = characters.characters[characterIndex];
        CharacterInstance instance;
        if (character.makeInstance(frame, instance))
        {
            characterState[characterIndex].load(character, instance);
        }
    }
}
