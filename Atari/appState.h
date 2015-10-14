
struct AppState
{
    AppState()
    {
        replayFramesSkipsLeft = 10;
        dumpAllTransitions = false;
        gamePaused = false;
        automatePlay = false;
        mostRecentFrame = nullptr;
    }

    const ColourPalette& getPalette() const
    {
        return ale.theOSystem->colourPalette();
    }

    ALEInterface ale;
    Bitmap sceenBmp;

    UIConnection ui;
    EventMap eventMap;

    D3D11AssetRenderer assets;

    SegmentDatabase segmentDatabase;
    SegmentAnalyzer segmentAnalyzer;
    ObjectAnalyzer objectAnalyzer;

    FrameID curFrame;
    FrameID anchorFrame;
    FrameID gameModelFrame;

    vector<Game::StateInst> modelStateHistory;
    Game::Model model;
    Game::ModelLearner modelLearner;
    RecallDatabase recallDatabase;

    DatabaseProcessor processor;

    Replay liveReplay;
    ReplayDatabase replayDatabase;

    D3D11Texture2D aleTexture;
    D3D11TriMesh aleBillboard;

    int replayFramesSkipsLeft;

    Cameraf camera;

    bool dumpAllTransitions;
    bool gamePaused;
    bool automatePlay;

    ReplayFrame *mostRecentFrame;
};
