
struct AppState
{
    AppState()
    {

    }

    const ColourPalette& getPalette() const
    {
        return ale.theOSystem->colourPalette();
    }

    ALEInterface ale;
    Bitmap aleScreenBmp;

    //UIConnection ui;
    EventMap eventMap;

    D3D11AssetRenderer assets;

    SegmentDatabase SegmentDatabase;
    SegmentAnalyzer segmentAnalyzer;
    ObjectAnalyzer objectAnalyzer;

    Game::Model model;
    Game::ModelLearner modelLearner;

    DatabaseProcessor processor;

    Replay replay;
    ReplayDatabase replayDatabase;

    D3D11Texture2D aleTexture;
    D3D11TriMesh aleBillboard;

    Cameraf camera;
};
