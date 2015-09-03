
struct CharacterState
{
    CharacterState()
    {
        valid = false;
    }
    int descriptorLength() const;
    void makeDescriptor(float *output, int &offset) const;
    void makeHeader(vector<string> &output) const;

    string describe() const;

    bool valid;

    vec3f worldPos;

    //worldDerivatives[0] = (world_t - world_t-1)
    //worldDerivatives[1] = (world_t-1 - world_t-2) ...
    deque<vec3f> worldDerivativeHistory;
    
    // poseHistory[0] = cluster_t
    // poseHistory[1] = cluster_t-1 ...
    deque<const PoseCluster*> poseHistory;
};

struct CharacterStateTransition
{
    int descriptorLength() const;
    void makeDescriptor(float *output, int &offset) const;
    void makeHeader(vector<string> &output) const;

    vec3f worldPosDelta;
    const PoseCluster *newCluster;
};

struct ControllerState
{
    void LoadGamecube(const ProcessedFrame &frame);
    void LoadGamecube(const GCPadStatus &pad, int controllerIndex);

    int descriptorLength() const;
    void makeDescriptor(float *output, int &offset) const;
    void makeHeader(vector<string> &output) const;

    static const int ControllerButtonCount = 7;
    static const int ControllerCount = 1;

    struct Controller
    {
        float buttons[ControllerButtonCount];
        vec2f stick;
    };

    Controller controllers[ControllerCount];
};

struct ControllerHistory
{
    ControllerHistory() {}
    ControllerHistory(const FrameID &frameID, const ReplayDatabase &replays);
    deque<ControllerState> history;
};

struct GameState
{
    
    void load(const FrameID &frameID, const ReplayDatabase &replays, const CharacterDatabase &characterDatabase);
    
    int descriptorLength() const;
    void makeDescriptor(float *output) const;
    vector<string> makeHeader() const;

    static const int CharacterCount = 2;

    //
    // input state
    //
    CharacterState characters[CharacterCount];
    ControllerHistory controllers;
};

struct StateTransition
{
    CharacterStateTransition character[GameState::CharacterCount];
};