
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
    void makeFrameMeshes(ApplicationData &app, const FrameObjectData &frame, vector<D3D11TriMesh> &meshes, vector<D3D11TriMesh> &boxMeshes);

    D3D11AssetRenderer assets;

    SignatureColorMap colorMap;
    FrameCollection allFrames;

    vector<D3D11TriMesh> curFrameMeshes;
    vector<D3D11TriMesh> curFrameBoxMeshes;
	
    int frameAObjectIndex;
    FrameObjectData *comparisonFrameA, *comparisonFrameB;
    vector<D3D11TriMesh> comparisonMeshesA;
    vector<D3D11TriMesh> comparisonMeshesB;

    int frameIndex;

    bool bboxMode;

    D3D11Font m_font;
    FrameTimer m_timer;

    Cameraf m_camera;

    mat4f m_world;
};