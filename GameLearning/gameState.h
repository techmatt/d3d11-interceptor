
struct CharacterState
{
    void load(const Character &character, const CharacterInstance &instance);

    string describe() const;

    float worldX, worldY;
    //map<int, AnimationFrame> activeAnimationsByIndex;

    float bestPoseDistSq;
};

struct GameState
{
    void load(const ProcessedFrame &frame, const CharacterDatabase &characters);

    CharacterState characterState[2];

    // previous frames?
};