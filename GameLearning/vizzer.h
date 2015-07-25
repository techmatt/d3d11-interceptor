
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
    D3D11AssetRenderer assets;

    ObjectCollection objects;

    vector<D3D11TriMesh> objectMeshes;
    vector<D3D11TriMesh> objectBoxMeshes;
	
    bool bboxMode;

    D3D11Font m_font;
    FrameTimer m_timer;

    Cameraf m_camera;

    mat4f m_world;
};