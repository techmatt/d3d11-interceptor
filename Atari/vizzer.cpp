
#include "main.h"

void Vizzer::init(ApplicationData &app)
{
    //
    // initialize parameters
    //
    ParameterFile params;
    const string file = "learningParams.txt";
    if (!util::fileExists(file))
    {
        cerr << "Parameter file not found: " << file << ", cwd=" << util::workingDirectory() << endl;
        exit(-1);
        return;
    }
    params.addParameterFile(file);
    initLearningParams(params);
    
    state.assets.init(app.graphics);

    vec3f eye(0.0f, 0.0f, 150.0f);
    vec3f worldUp(0.0f, 1.0f, 0.0f);
    state.camera = Cameraf(-eye, worldUp, vec3f::origin, 60.0f, (float)app.window.getWidth() / app.window.getHeight(), 0.01f, 10000.0f, true);

    font.init(app.graphics, "Calibri");

    //state.ui.init("GameLearningUI.exe", "GameLearningUI");
    
    state.ale.setBool("color_averaging", true);
    state.ale.loadROM(R"(C:\Code\Arcade-Learning-Environment-0.5.0\roms\ms_pacman.bin)");

    state.aleBillboard = D3D11TriMesh(app.graphics, ml::Shapesf::rectangleZ(vec2f(-50.0f, -50.0f), vec2f(50.0f, 50.0f)));

    registerEventHandlers(app);
}

void Vizzer::registerEventHandlers(ApplicationData& app)
{
    state.eventMap.registerEvent("terminate", [&](const vector<string> &params) {
        PostQuitMessage(0);
    });
}

void Vizzer::render(ApplicationData &app)
{
    //const float targetFPS = 20.0f;
    //Sleep(DWORD(1000.0f / targetFPS));

    timer.frame();

    //state.eventMap.dispatchEvents(state.ui);

    ActionVect legalActions = state.ale.getMinimalActionSet();

    bool left = GetAsyncKeyState(VK_LEFT) != 0;
    bool right = GetAsyncKeyState(VK_RIGHT) != 0;
    bool up = GetAsyncKeyState(VK_UP) != 0;
    bool down = GetAsyncKeyState(VK_DOWN) != 0;
    bool fire = GetAsyncKeyState(VK_SPACE) != 0;

    //Action a = util::randomElement(legalActions);

    Action a = PLAYER_A_NOOP;

    if (fire) a = PLAYER_A_FIRE;
    if (up) a = PLAYER_A_UP;
    if (right) a = PLAYER_A_RIGHT;
    if (left) a = PLAYER_A_LEFT;
    if (down) a = PLAYER_A_DOWN;
    if (right && up) a = PLAYER_A_UPRIGHT;
    if (left && up) a = PLAYER_A_UPLEFT;
    if (right && down) a = PLAYER_A_DOWNRIGHT;
    if (left && down) a = PLAYER_A_DOWNLEFT;
    if (up && fire) a = PLAYER_A_UPFIRE;
    if (right && fire) a = PLAYER_A_RIGHTFIRE;
    if (left && fire) a = PLAYER_A_LEFTFIRE;
    if (down && fire) a = PLAYER_A_DOWNFIRE;
    if (left && up && fire) a = PLAYER_A_UPLEFTFIRE;
    if (right && up && fire) a = PLAYER_A_UPRIGHTFIRE;
    if (down && right && fire) a = PLAYER_A_DOWNRIGHTFIRE;
    if (left && down && fire) a = PLAYER_A_DOWNLEFTFIRE;

    float reward = state.ale.act(a);

    if (state.ale.game_over())
    {
        state.ale.reset_game();
    }

    const ALEScreen &screen = state.ale.getScreen();
    const ColourPalette &palette = state.ale.theOSystem->colourPalette();

    if (state.aleScreen.getWidth() != screen.width())
        state.aleScreen.allocate(screen.width(), screen.height());

    for (auto &p : state.aleScreen)
    {
        int r, g, b;
        palette.getRGB(screen.get(state.aleScreen.getHeight() - 1 - p.y, p.x), r, g, b);
        p.value = vec4uc(r, g, b, 255);

        if (p.x == 0 || p.y == 0 || p.x == state.aleScreen.getWidth() - 1 || p.y == state.aleScreen.getHeight() - 1)
            p.value = vec4uc(255, 255, 255, 255);
    }

    state.aleTexture.load(app.graphics, state.aleScreen);
    state.aleTexture.bind();

    state.assets.renderMesh(state.aleBillboard, state.camera.getCameraPerspective());

    //LodePNG::save(state.aleScreen, "debug.png");

    vector<string> text;
    text.push_back("FPS: " + convert::toString(timer.framesPerSecond()));
    
    const bool useText = true;
    if (useText)
        drawText(app, text);
}

void Vizzer::resize(ApplicationData &app)
{
    state.camera.updateAspectRatio((float)app.window.getWidth() / app.window.getHeight());
}

void Vizzer::drawText(ApplicationData &app, vector<string> &text)
{
    int y = 0;
    for (auto &entry : text)
    {
        font.drawString(app.graphics, entry, vec2i(10, 5 + y++ * 25), 24.0f, RGBColor::Red);
    }
}

void Vizzer::keyDown(ApplicationData &app, UINT key)
{
    if (key == KEY_F) app.graphics.castD3D11().toggleWireframe();
    if (key == KEY_R) state.ale.reset_game();
}

void Vizzer::keyPressed(ApplicationData &app, UINT key)
{
    const float distance = 1.0f;
    const float theta = 5.0f;

    if(key == KEY_S) state.camera.move(-distance);
    if(key == KEY_W) state.camera.move(distance);
    if(key == KEY_A) state.camera.strafe(-distance);
    if(key == KEY_D) state.camera.strafe(distance);
	if(key == KEY_E) state.camera.jump(distance);
	if(key == KEY_Q) state.camera.jump(-distance);

    /*if(key == KEY_UP) state.camera.lookUp(theta);
    if(key == KEY_DOWN) state.camera.lookUp(-theta);
    if(key == KEY_LEFT) state.camera.lookRight(theta);
    if(key == KEY_RIGHT) state.camera.lookRight(-theta);*/
}

void Vizzer::mouseDown(ApplicationData &app, MouseButtonType button)
{

}

void Vizzer::mouseWheel(ApplicationData &app, int wheelDelta)
{
    const float distance = 0.1f;
    state.camera.move(distance * wheelDelta);
}

void Vizzer::mouseMove(ApplicationData &app)
{
    const float distance = 1.0f;
    const float theta = 0.5f;

    vec2i posDelta = app.input.mouse.pos - app.input.prevMouse.pos;

    if(app.input.mouse.buttons[MouseButtonRight])
    {
        state.camera.strafe(-distance * posDelta.x);
        state.camera.jump(distance * posDelta.y);
    }

    if(app.input.mouse.buttons[MouseButtonLeft])
    {
        state.camera.lookRight(-theta * posDelta.x);
        state.camera.lookUp(theta * posDelta.y);
    }

}
