
struct CharacterState
{
    void load(const Character &character, const CharacterInstance &instance);
    float worldX, worldY;
    vector<short> animationStates;
};

struct GameState
{
    CharacterState characters[2];

    // previous frames?
};