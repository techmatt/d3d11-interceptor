
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
    void drawText(ApplicationData &app, vector< string > &text);

    D3D11Font font;
    FrameTimer timer;

    AppState state;

    bool frameDirty;
};