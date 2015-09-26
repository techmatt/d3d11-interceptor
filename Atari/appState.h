
struct AppState
{
    AppState()
    {

    }

    ALEInterface ale;
    Bitmap aleScreenBmp;

    //UIConnection ui;
    EventMap eventMap;

    D3D11AssetRenderer assets;

    SegmentManager segmentManager;

    Replay replay;

    D3D11Texture2D aleTexture;
    D3D11TriMesh aleBillboard;

    Cameraf camera;
};
