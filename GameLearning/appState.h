
struct AppState
{
    AppState()
    {
        curFrameIndex = 0;
        showBBoxes = false;
        showFullMesh = false;
    }

    UIConnection ui;
    EventMap eventMap;

    D3D11AssetRenderer assets;

    GeometryDatabase geoDatabase;
    SignatureColorMap colorMap;
    FrameCollection allFrames;

    vector<D3D11TriMesh> curFrameMeshesBox;
    vector<D3D11TriMesh> curFrameMeshesFull;
    vector<D3D11TriMesh> curFrameMeshesRigidTransform;

    SegmentAnalyzer analyzer;

    int curFrameIndex;

    Cameraf camera;

    bool showBBoxes;
    bool showFullMesh;
};
