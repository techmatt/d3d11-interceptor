
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
    void registerEventHandlers(ApplicationData& app);

    void makeFrameMeshesBox(ApplicationData &app, const FrameObjectData &frame, vector<D3D11TriMesh> &meshes);
    void makeFrameMeshesFull(ApplicationData &app, const FrameObjectData &frame, vector<D3D11TriMesh> &meshes);
    void makeFrameMeshesRigidTransform(ApplicationData &app, const FrameObjectData &frame, vector<D3D11TriMesh> &meshes);

    D3D11Font font;
    FrameTimer timer;

    AppState state;
};