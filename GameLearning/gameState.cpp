
#include "main.h"

int CharacterState::descriptorLength() const
{
    return 3 + 3 * (int)worldDerivativeHistory.size() + Constants::poseChainReverseLength;
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

    for (const PoseCluster* cluster : poseHistory)
    {
        output[offset++] = (float)cluster->index;
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
    for (const PoseCluster* cluster : poseHistory)
    {
        output.push_back("cluster" + to_string(index));
    }
}

int ControllerState::descriptorLength() const
{
    return ControllerCount * (ControllerButtonCount + 2);
}

void ControllerState::makeDescriptor(float *output, int &offset) const
{
    for (const Controller &c : controllers)
    {
        for (int i = 0; i < ControllerButtonCount; i++)
            output[offset++] = c.buttons[i];

        output[offset++] = c.stick.x;
        output[offset++] = c.stick.y;
    }
}

void ControllerState::makeHeader(vector<string> &output) const
{
    int controllerIndex = 0;
    for (const Controller &c : controllers)
    {
        string prefix = "c" + to_string(controllerIndex++) + "-";
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
}

int GameState::descriptorLength() const
{
    int sum = 0;
    for (int i = 0; i < CharacterCount; i++)
        sum += characters[i].descriptorLength();

    for (int i = 0; i < controllerHistory.size(); i++)
        sum += controllerHistory[i].descriptorLength();
    return sum;
}

void GameState::makeDescriptor(float *output) const
{
    int offset = 0;
    for (int i = 0; i < CharacterCount; i++)
        characters[i].makeDescriptor(output, offset);

    for (int i = 0; i < controllerHistory.size(); i++)
        controllerHistory[i].makeDescriptor(output, offset);
}

vector<string> GameState::makeHeader() const
{
    vector<string> result;
    for (int i = 0; i < CharacterCount; i++)
        characters[i].makeHeader(result);

    for (int i = 0; i < controllerHistory.size(); i++)
        controllerHistory[i].makeHeader(result);
    return result;
}

void ControllerState::LoadGamecube(const ProcessedFrame &frame)
{
    for (int controllerIndex = 0; controllerIndex < ControllerState::ControllerCount; controllerIndex++)
    {
        LoadGamecube(frame.frame->padState[controllerIndex], controllerIndex);
    }
}

void ControllerState::LoadGamecube(const GCPadStatus &pad, int controllerIndex)
{
    Controller &c = controllers[controllerIndex];
    auto toFloat = [](int x) { if (x == 0) return 0.0f; return 1.0f; };

    int buttonIndex = 0;
    c.buttons[buttonIndex++] = toFloat(pad.button & PAD_BUTTON_A);
    c.buttons[buttonIndex++] = toFloat(pad.button & PAD_BUTTON_B);
    c.buttons[buttonIndex++] = toFloat(pad.button & PAD_BUTTON_X);
    c.buttons[buttonIndex++] = toFloat(pad.button & PAD_BUTTON_Y);
    c.buttons[buttonIndex++] = toFloat(pad.button & PAD_TRIGGER_L);
    c.buttons[buttonIndex++] = toFloat(pad.button & PAD_TRIGGER_R);
    c.buttons[buttonIndex++] = toFloat(pad.button & PAD_TRIGGER_Z);

    c.stick.x = pad.stickX / 255.0f;
    c.stick.y = pad.stickY / 255.0f;
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

void GameState::load(const FrameID &frameID, const ReplayDatabase &replays, const CharacterDatabase &characterDatabase)
{
    const ProcessedFrame *baseFrame = replays.getFrame(frameID);
    
    //
    // controllerHistory
    //
    controllerHistory.resize(learningParams().controllerHistorySize);
    for (int historyOffset = 0; historyOffset < learningParams().controllerHistorySize; historyOffset++)
    {
        const ProcessedFrame *historyFrame = replays.getFrame(frameID.delta(-historyOffset));

        if (historyFrame == nullptr)
            controllerHistory[historyOffset].LoadGamecube(*baseFrame);
        else
            controllerHistory[historyOffset].LoadGamecube(*historyFrame);
    }

    //
    // characters
    //
    for (int characterIndex = 0; characterIndex < CharacterCount; characterIndex++)
    {
        //
        // CharacterState
        //
        const Character &character = characterDatabase.characters[characterIndex];
        CharacterState &characterState = characters[characterIndex];

        const CharacterInstance *instance = character.findInstanceAtFrame(frameID);
        const CharacterInstance *fallbackInstance = character.allInstancesVec.front();

        if (instance == nullptr)
        {
            characterState.valid = false;
            characterState.worldPos = vec3f::origin;
        }
        else
        {
            characterState.valid = true;
            characterState.worldPos = instance->worldCentroid;
        }

        characterState.worldDerivativeHistory.resize(learningParams().worldPosHistorySize);
        for (int historyOffset = 0; historyOffset < learningParams().worldPosHistorySize; historyOffset++)
        {
            const CharacterInstance *instanceA = character.findInstanceAtFrame(frameID.delta(-historyOffset));
            const CharacterInstance *instanceB = character.findInstanceAtFrame(frameID.delta(-historyOffset - 1));
            if (instanceA != nullptr && instanceB != nullptr)
            {
                characterState.worldDerivativeHistory[historyOffset] = instanceA->worldCentroid - instanceB->worldCentroid;
            }
        }

        characterState.poseHistory.resize(Constants::poseChainReverseLength);
        for (int historyOffset = 0; historyOffset < learningParams().worldPosHistorySize; historyOffset++)
        {
            const CharacterInstance *instance = character.findInstanceAtFrame(frameID.delta(-historyOffset));
            if (instance == nullptr)
                characterState.poseHistory[historyOffset] = character.poseClusters.front();
            else
                characterState.poseHistory[historyOffset] = instance->poseCluster;
        }
    }
}
