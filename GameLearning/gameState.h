
struct CharacterState
{
    CharacterState()
    {
        valid = false;
        instance = nullptr;
    }
    int descriptorLength() const;
    void makeDescriptor(float *output, int &offset) const;
    void makeHeader(vector<string> &output) const;

    string describe() const;

    bool valid;

    vec3f worldPos;

    //worldDerivatives[0] = (world_t - world_t-1)
    //worldDerivatives[1] = (world_t-1 - world_t-2) ...
    vector<vec3f> worldDerivativeHistory;
    
    const CharacterInstance *instance;
};

struct CharacterStatePrediction
{
    int descriptorLength() const;
    void makeDescriptor(float *output, int &offset) const;
    void makeHeader(vector<string> &output) const;

    vec3f worldPosDelta;
    vector<float> poseClusters;
};

struct ControllerState
{
    void LoadGamecube(const GCPadStatus &pad);

    int descriptorLength() const;
    void makeDescriptor(float *output, int &offset) const;
    void makeHeader(const string &prefix, vector<string> &output) const;

    static const int ControllerButtonCount = 7;
    float buttons[ControllerButtonCount];

    vec2f stick;
};

struct ControllerStateHistory
{
    int descriptorLength() const;
    void makeDescriptor(float *output, int &offset) const;
    void makeHeader(vector<string> &output) const;

    //history[0] = current frame
    vector<ControllerState> history;
};

struct GameState
{
    void load(const FrameID &frameID, const ReplayDatabase &replays, const CharacterDatabase &characters);
    
    int descriptorLength() const;
    void makeDescriptor(float *output) const;
    vector<string> makeHeader() const;

    static const int CharacterCount = 1;

    //
    // input state
    //
    CharacterState characterState[CharacterCount];
    ControllerStateHistory controllerStateHistory[CharacterCount];

    //
    // predicted output
    //
    CharacterStatePrediction characterPrediction[CharacterCount];
};