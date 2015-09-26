
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
    
    state.ale.setBool("color_averaging", false);
    state.ale.loadROM(learningParams().ROMDir + learningParams().ROMName + ".bin");

    state.aleBillboard = D3D11TriMesh(app.graphics, ml::Shapesf::rectangleZ(vec2f(-50.0f, -50.0f), vec2f(50.0f, 50.0f)));

    state.processor.go(state);
    /*const ColourPalette &palette = state.ale.theOSystem->colourPalette();

    string replayDir = learningParams().ROMDatasetDir + "replays/";
    Replay replay;
    replay.load(replayDir + "15414451539455309733.dat");
    //replay.load(replayDir + "1149494339278655681.dat");
    //replay.frames[0]->image.toBmp(palette, state.aleScreenBmp);
    
    for (ReplayFrame *frame : replay.frames)
    {
        if (frame->index % 100 == 0)
            cout << "Frame " << frame->index << " / " << replay.frames.size() << endl;
        state.segmentManager.recordSegments(palette, *frame);
    }
    state.segmentManager.saveAllViz(palette, learningParams().ROMDatasetDir + "viz/");*/

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

    ReplayFrame *frame = new ReplayFrame();

    //frame->action = util::randomElement(legalActions);
    frame->action = AtariUtil::actionFromKeyboard();
    
    frame->reward = state.ale.act(frame->action);

    const ALEScreen &screen = state.ale.getScreen();
    const ColourPalette &palette = state.ale.theOSystem->colourPalette();

    frame->image.fromScreen(screen);
    state.replay.frames.push_back(frame);

    if (state.ale.game_over())
    {
        state.replay.romName = learningParams().ROMName;

        string dir = learningParams().ROMDatasetDir + "replaysRaw/";
        util::makeDirectory(dir);
        state.replay.save(dir + to_string(state.replay.id) + ".dat");
        state.replay = Replay();
        state.ale.reset_game();
    }

    frame->image.toBmp(palette, state.aleScreenBmp);

    state.aleTexture.load(app.graphics, state.aleScreenBmp);
    state.aleTexture.bind();

    state.assets.renderMesh(state.aleBillboard, state.camera.getCameraPerspective());

    //LodePNG::save(state.aleScreenBmp, "debug.png");

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
