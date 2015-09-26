
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

    SegmentManager segmentManager;

    DatabaseProcessor processor;

    Replay replay;

    D3D11Texture2D aleTexture;
    D3D11TriMesh aleBillboard;

    Cameraf camera;
};
