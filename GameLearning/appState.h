
struct AppState
{
    AppState()
    {
        curFrameIndex = 0;
        showBBoxes = false;
        showFullMesh = false;
        showSelectionOnly = false;
        showTrackable = false;
        selectedSignature = (UINT64)-1;
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

    UINT64 selectedSignature;

    bool showBBoxes;
    bool showFullMesh;
    bool showSelectionOnly;
    bool showTrackable;
};
