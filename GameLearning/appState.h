
struct AppState
{
    AppState()
    {
        curFrame = FrameID(0, 0);
        gameModelPredictedCharacterFrame = FrameID(0, 0);
        curCharacterIndex = 1;
        anchorAnimationInstanceIndex = 0;
        showBBoxes = false;
        showFullMesh = false;
        showSelectionOnly = false;
        showTrackable = false;
        showCharacterSegments = false;
        showAnimationLabel = false;
        selectedSignature = (UINT64)-1;
        transitionIndex = 0;
    }

    UIConnection ui;
    EventMap eventMap;

    D3D11AssetRenderer assets;

    GeometryDatabase geoDatabase;
    SignatureColorMap colorMap;
    
    vector<D3D11TriMesh> curFrameMeshesBox;
    vector<D3D11TriMesh> curFrameMeshesFull;
    vector<D3D11TriMesh> curFrameMeshesRigidTransform;

    vector<D3D11TriMesh> gameModelFrameMeshesRigidTransform;

    SegmentAnalyzer analyzer;
    CharacterDatabase characters;
    ReplayDatabase replays;

    FrameID curFrame;
    FrameID anchorFrame;
    int curCharacterIndex;
    int anchorAnimationInstanceIndex;
    
    int transitionIndex;
    GameModel gameModel;
    GameState gameModelState;
    GameState gameModelStateStore;
    FrameID gameModelFrame;
    FrameID gameModelPredictedCharacterFrame;
    PredictionEntry gameModelPrediction;

    Cameraf camera;

    UINT64 selectedSignature;

    bool showBBoxes;
    bool showFullMesh;
    bool showSelectionOnly;
    bool showTrackable;
    bool showCharacterSegments;
    bool showAnimationLabel;
};
