
#include "main.h"

int CharacterState::descriptorLength() const
{
    return 3 + 3 * (int)worldDerivativeHistory.size() + (int)instance->poseChainReverseDescriptor.size();
}

void CharacterState::makeDescriptor(float *output, int &offset) const
{
    output[offset++] = worldPos.x;
    output[offset++] = worldPos.y;
    output[offset++] = worldPos.z;

    for (vec3f v : worldDerivativeHistory)
    {
        output[offset++] = v.x;
        output[offset++] = v.y;
        output[offset++] = v.z;
    }

    for (float f : instance->poseChainReverseDescriptor)
    {
        output[offset++] = f;
    }
}

void CharacterState::makeHeader(vector<string> &output) const
{
    output.push_back("WorldPos.x");
    output.push_back("WorldPos.y");
    output.push_back("WorldPos.z");

    int index = 0;
    for (vec2f v : worldDerivativeHistory)
    {
        output.push_back("WorldDeriv" + to_string(index) + ".x");
        output.push_back("WorldDeriv" + to_string(index) + ".y");
        output.push_back("WorldDeriv" + to_string(index) + ".z");
        index++;
    }

    index = 0;
    for (float f : instance->poseChainReverseDescriptor)
    {
        output.push_back("pcrd-" + to_string(index));
        index++;
    }
}

int CharacterStatePrediction::descriptorLength() const
{
    return 3 + (int)poseClusters.size();
}

void CharacterStatePrediction::makeDescriptor(float *output, int &offset) const
{
    output[offset++] = worldPosDelta.x;
    output[offset++] = worldPosDelta.y;
    output[offset++] = worldPosDelta.z;

    for (float f : poseClusters)
        output[offset++] = f;
}

void CharacterStatePrediction::makeHeader(vector<string> &output) const
{
    output.push_back("worldPosDelta.x");
    output.push_back("worldPosDelta.y");
    output.push_back("worldPosDelta.z");

    int index = 0;
    for (float f : poseClusters)
    {
        output.push_back("cluster-" + to_string(index));
        index++;
    }
}

int ControllerState::descriptorLength() const
{
    return ControllerButtonCount + 2;
}

void ControllerState::makeDescriptor(float *output, int &offset) const
{
    for (int i = 0; i < ControllerButtonCount; i++)
        output[offset++] = buttons[i];

    output[offset++] = stick.x;
    output[offset++] = stick.y;
}

void ControllerState::makeHeader(const string &prefix, vector<string> &output) const
{
    output.push_back(prefix + "button-A");
    output.push_back(prefix + "button-B");
    output.push_back(prefix + "button-X");
    output.push_back(prefix + "button-Y");
    output.push_back(prefix + "button-L");
    output.push_back(prefix + "button-R");
    output.push_back(prefix + "button-Z");
    output.push_back(prefix + "stick-X");
    output.push_back(prefix + "stick-Y");
}

int ControllerStateHistory::descriptorLength() const
{
    return (int)history.size() * history[0].descriptorLength();
}

void ControllerStateHistory::makeDescriptor(float *output, int &offset) const
{
    for (const ControllerState &controller : history)
        controller.makeDescriptor(output, offset);
}

void ControllerStateHistory::makeHeader(vector<string> &output) const
{
    int index = 0;
    for (const ControllerState &controller : history)
    {
        controller.makeHeader("ch" + to_string(index) + "-", output);
        index++;
    }
}

int GameState::descriptorLength() const
{
    int sum = 0;
    for (int i = 0; i < CharacterCount; i++)
    {
        sum += controllerStateHistory[i].descriptorLength();
        sum += characterState[i].descriptorLength();
        sum += characterPrediction[i].descriptorLength();
    }
    return sum;
}

void GameState::makeDescriptor(float *output) const
{
    int offset = 0;
    for (int i = 0; i < CharacterCount; i++)
    {
        controllerStateHistory[i].makeDescriptor(output, offset);
        characterState[i].makeDescriptor(output, offset);
        characterPrediction[i].makeDescriptor(output, offset);
    }
}

vector<string> GameState::makeHeader() const
{
    vector<string> result;
    for (int i = 0; i < CharacterCount; i++)
    {
        controllerStateHistory[i].makeHeader(result);
        characterState[i].makeHeader(result);
        characterPrediction[i].makeHeader(result);
    }
    return result;
}

void ControllerState::LoadGamecube(const GCPadStatus &pad)
{
    auto toFloat = [](int x) { if (x == 0) return 0.0f; return 1.0f; };

    int buttonIndex = 0;
    buttons[buttonIndex++] = toFloat(pad.button & PAD_BUTTON_A);
    buttons[buttonIndex++] = toFloat(pad.button & PAD_BUTTON_B);
    buttons[buttonIndex++] = toFloat(pad.button & PAD_BUTTON_X);
    buttons[buttonIndex++] = toFloat(pad.button & PAD_BUTTON_Y);
    buttons[buttonIndex++] = toFloat(pad.button & PAD_TRIGGER_L);
    buttons[buttonIndex++] = toFloat(pad.button & PAD_TRIGGER_R);
    buttons[buttonIndex++] = toFloat(pad.button & PAD_TRIGGER_Z);

    stick.x = pad.stickX / 255.0f;
    stick.y = pad.stickY / 255.0f;
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

void GameState::load(const FrameID &frameID, const ReplayDatabase &replays, const CharacterDatabase &characters)
{
    const ProcessedFrame *baseFrame = replays.getFrame(frameID);
    
    for (int characterIndex = 0; characterIndex < CharacterCount; characterIndex++)
    {
        //
        // ControllerStateHistory
        //
        ControllerStateHistory &controllerHistory = controllerStateHistory[characterIndex];
        controllerHistory.history.resize(learningParams().controllerHistorySize);
        for (int historyOffset = 0; historyOffset < learningParams().controllerHistorySize; historyOffset++)
        {
            const ProcessedFrame *historyFrame = replays.getFrame(frameID.delta(-historyOffset));
            if (historyFrame == nullptr)
                controllerHistory.history[historyOffset].LoadGamecube(baseFrame->frame->padState[characterIndex]);
            else
                controllerHistory.history[historyOffset].LoadGamecube(historyFrame->frame->padState[characterIndex]);
        }

        //
        // CharacterState
        //
        const Character &character = characters.characters[1];
        const CharacterInstance *instance = character.findInstanceAtFrame(frameID);
        const CharacterInstance *fallbackInstance = character.findInstanceAtFrame(frameID);


        if (instance == nullptr)
        {
            characterState[characterIndex].valid = false;
            characterState[characterIndex].instance = fallbackInstance;
            characterState[characterIndex].worldPos = vec3f::origin;
        }
        else
        {
            characterState[characterIndex].valid = true;
            characterState[characterIndex].instance = instance;
            characterState[characterIndex].worldPos = instance->worldCentroid;
        }

        characterState[characterIndex].worldDerivativeHistory.resize(learningParams().worldPosHistorySize);
        for (int historyOffset = 0; historyOffset < learningParams().controllerHistorySize; historyOffset++)
        {
            const CharacterInstance *instanceA = character.findInstanceAtFrame(frameID.delta(-historyOffset));
            const CharacterInstance *instanceB = character.findInstanceAtFrame(frameID.delta(-historyOffset - 1));
            if (instanceA != nullptr && instanceB != nullptr)
            {
                characterState[characterIndex].worldDerivativeHistory[historyOffset] = instanceA->worldCentroid - instanceB->worldCentroid;
            }
        }

        //
        // CharacterStatePrediction
        //
        const CharacterInstance *nextInstance = character.findInstanceAtFrame(frameID.delta(1));
        CharacterStatePrediction &prediction = characterPrediction[characterIndex];
        prediction.poseClusters.resize(character.poseClusters.size());
        for (float &x : prediction.poseClusters)
            x = 0;

        if (instance != nullptr && nextInstance != nullptr)
        {
            prediction.worldPosDelta = nextInstance->worldCentroid - instance->worldCentroid;

            vector< pair<PoseCluster*, float> > candidates = character.findPoseClustersRadius(*nextInstance, (float)learningParams().poseDistSqThreshold);
            for (auto &cluster : candidates)
                prediction.poseClusters[cluster.first->index] = 1;
        }
    }
}
