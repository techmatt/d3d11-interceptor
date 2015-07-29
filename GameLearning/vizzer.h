
struct ConstantBuffer
{
	mat4f worldViewProj;
};

class Vizzer : public ApplicationCallback
{
public:
	void init(ApplicationData &app);
	void render(ApplicationData &app);
	void keyDown(ApplicationData &app, UINT key);
	void keyPressed(ApplicationData &app, UINT key);
	void mouseDown(ApplicationData &app, MouseButtonType button);
	void mouseMove(ApplicationData &app);
	void mouseWheel(ApplicationData &app, int wheelDelta);
	void resize(ApplicationData &app);

private:
    void makeFrameMeshesBox(ApplicationData &app, const FrameObjectData &frame, vector<D3D11TriMesh> &meshes);
    void makeFrameMeshesFull(ApplicationData &app, const FrameObjectData &frame, vector<D3D11TriMesh> &meshes);
    void makeFrameMeshesRigidTransform(ApplicationData &app, const FrameObjectData &frame, vector<D3D11TriMesh> &meshes);

    D3D11AssetRenderer assets;

    GeometryDatabase geoDatabase;
    SignatureColorMap colorMap;
    FrameCollection allFrames;

    vector<D3D11TriMesh> curFrameMeshes;
    vector<D3D11TriMesh> curFrameBoxMeshes;
	
    int frameAObjectIndex;
    int frameBObjectIndex;
    FrameObjectData *comparisonFrameA, *comparisonFrameB;
    vector<D3D11TriMesh> comparisonMeshesA;
    vector<D3D11TriMesh> comparisonMeshesB;

    vector<FrameAlignmentCorrespondence> correspondences;

    int frameIndex;

    bool bboxMode;

    D3D11Font m_font;
    FrameTimer m_timer;

    Cameraf m_camera;

    mat4f m_world;
};