
struct AppState
{
    AppState()
    {
        curFrameIndex = 0;
        curCharacterIndex = 0;
        showBBoxes = false;
        showFullMesh = false;
        showSelectionOnly = false;
        showTrackable = false;
        showCharacterSegments = false;
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
    vector<Character> characters;

    int curFrameIndex;
    int curCharacterIndex;
    int animationAnchorFrame;

    Cameraf camera;

    UINT64 selectedSignature;

    bool showBBoxes;
    bool showFullMesh;
    bool showSelectionOnly;
    bool showTrackable;
    bool showCharacterSegments;
};
