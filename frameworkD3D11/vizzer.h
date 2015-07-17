
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
	vector<D3D11TriMesh> m_meshes;
	
    D3D11VertexShader m_vsColor;
	D3D11PixelShader m_psColor;
    D3D11VertexShader m_vsPointCloud;
    D3D11PixelShader m_psPointCloud;

    D3D11Font m_font;
    FrameTimer m_timer;

	D3D11ConstantBuffer<ConstantBuffer> m_constants;
	Cameraf m_camera;
};