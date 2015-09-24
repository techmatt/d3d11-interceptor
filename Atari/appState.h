
struct AppState
{
    AppState()
    {

    }

    ALEInterface ale;
    Bitmap aleScreen;

    //UIConnection ui;
    EventMap eventMap;

    D3D11AssetRenderer assets;

    D3D11Texture2D aleTexture;
    D3D11TriMesh aleBillboard;

    Cameraf camera;
};
