
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

    state.ui.init("atariUI.exe", "atariUI");
    
    state.ale.setBool("color_averaging", false);
    state.ale.setFloat("repeat_action_probability", 0.0f);
    
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
        state.segmentDatabase.recordSegments(palette, *frame);
    }
    state.segmentDatabase.saveAllViz(palette, learningParams().ROMDatasetDir + "viz/");*/

    state.curFrame = FrameID(0, 0);

    registerEventHandlers(app);
}

void Vizzer::registerEventHandlers(ApplicationData& app)
{
    state.eventMap.registerEvent("terminate", [&](const vector<string> &params) {
        PostQuitMessage(0);
    });
    state.eventMap.registerEvent("loadFrame", [&](const vector<string> &params) {
        state.curFrame.replayIndex = util::convertTo<int>(params[1]);
        state.curFrame.replayIndex = math::clamp(state.curFrame.replayIndex, 0, (int)state.replayDatabase.replays.size() - 1);
        state.curFrame.frameIndex = util::convertTo<int>(params[2]);
        if (state.curFrame.frameIndex < 0 || state.curFrame.frameIndex >= state.replayDatabase.replays[state.curFrame.replayIndex]->replay->frames.size())
        {
            state.curFrame.frameIndex = 0;
        }
    });
    state.eventMap.registerEvent("dumpAllTransitions", [&](const vector<string> &params) {
        state.dumpAllTransitions = ml::util::convertTo<bool>(params[1]);
    });
}

void Vizzer::render(ApplicationData &app)
{
    //const float targetFPS = 20.0f;
    //Sleep(DWORD(1000.0f / targetFPS));

    timer.frame();

    state.eventMap.dispatchEvents(state.ui);

    const bool livePlayMode = false;
    ReplayFrame *frameToRender = nullptr;
    const ColourPalette &palette = state.getPalette();
    if (livePlayMode)
    {
        ActionVect legalActions = state.ale.getMinimalActionSet();

        ReplayFrame *frame = new ReplayFrame();

        //frame->action = util::randomElement(legalActions);
        frame->action = AtariUtil::actionFromKeyboard();
        frame->reward = state.ale.act(frame->action);

        const ALEScreen &screen = state.ale.getScreen();
        
        frame->image.fromScreen(screen);

        //state.segmentDatabase.recordAndAnnotateSegments(state.getPalette(), *frame);
        //state.segmentAnalyzer.annotateObjects(*frame);
        //frame->updateObjectIDs(state.segmentDatabase);

        if (state.replayFramesSkipsLeft)
            state.replayFramesSkipsLeft--;
        else
            state.liveReplay.frames.push_back(frame);

        if (state.ale.game_over())
        {
            state.liveReplay.romName = learningParams().ROMName;

            string dir = learningParams().ROMDatasetDir + "replaysRaw/";
            util::makeDirectory(dir);
            //state.replay.save(dir + to_string(state.liveReplay.id) + ".dat");
            state.liveReplay = Replay();
            state.ale.reset_game();
        }

        if (GetAsyncKeyState(KEY_C))
        {
            string dir = learningParams().ROMDatasetDir + "replaysRaw/";
            util::makeDirectory(dir);
            state.liveReplay.save(dir + to_string(state.liveReplay.id) + ".dat");
        }
        frameToRender = frame;
    }
    else
    {
        frameToRender = &state.replayDatabase.getFrame(state.curFrame);
    }
    
    frameToRender->image.toBmp(palette, state.sceenBmp);

    if (state.modelStateHistory.size() > 0)
    {
        AtariUtil::overlayModelFrame(state, state.modelStateHistory.back(), state.sceenBmp);
    }

    state.aleTexture.load(app.graphics, state.sceenBmp);
    state.aleTexture.bind();

    state.assets.renderMesh(state.aleBillboard, state.camera.getCameraPerspective());

    Game::StateInst gameStateInst;
    state.model.loadObjects(state, state.objectAnalyzer, *frameToRender, gameStateInst);
    state.model.readVariables(state.segmentDatabase, gameStateInst);

    //LodePNG::save(state.aleScreenBmp, "debug.png");

    vector<string> text;
    text.push_back("FPS: " + convert::toString(timer.framesPerSecond()));
    text.push_back("frame: " + state.curFrame.toString());
    text.push_back("action: " + to_string(frameToRender->action));

    const bool displayVariables = true;
    if (displayVariables)
    {
        for (auto &v : gameStateInst.variables)
        {
            text.push_back(v.first + " = " + to_string(v.second));
        }

        if (gameStateInst.objects.find("padB")->second.size() > 0)
            text.push_back("padB-y, actual = " + to_string(gameStateInst.objects.find("padB")->second[0].origin.y));

        if (state.modelStateHistory.size() > 0)
        {
            auto &objects = state.modelStateHistory.back().objects;
            if (objects.count("padB") > 0 && objects.find("padB")->second.size() > 0)
                text.push_back("padB-y, predicted = " + to_string(objects.find("padB")->second[0].origin.y));
        }
    }
    
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

    if (key == KEY_J)
    {
        state.anchorFrame = state.curFrame;
        //state.gameModelState.load(state.anchorFrame, state.replays, state.characters);
        state.gameModelFrame = state.curFrame;

        state.modelStateHistory = state.replayDatabase.replays[state.gameModelFrame.replayIndex]->states;
        state.modelStateHistory.resize(state.gameModelFrame.frameIndex + 1);
    }

    bool advanceModel = (key == KEY_B || key == KEY_V);

    if (advanceModel)
    {
        int action = state.replayDatabase.getFrame(state.gameModelFrame).action;
        //cout << "Selected action = " << action << " drawn from frame " << state.gameModelFrame.toString() << endl;
        state.gameModelFrame = state.gameModelFrame.delta(1);
        
        Game::StateInst nextState;
        state.model.advance(state, state.gameModelFrame.replayIndex, state.modelStateHistory, action, nextState);
        state.modelStateHistory.push_back(nextState);

        /*ControllerState controller;
        const ProcessedFrame *nextFrame = state.replays.getFrame(state.gameModelFrame);

        if (nextFrame != nullptr)
            controller.LoadGamecube(*nextFrame);

        StateTransition transition;
        state.gameModel.predictTransition(state.replays, state.characters, state.gameModelState, transition, 0);
        state.gameModel.advanceGameState(state.gameModelState, transition, controller);

        state.gameModelPredictedCharacterFrame = state.gameModelState.characters[state.curCharacterIndex].poseHistory[0]->seedFrame;

        state.gameModelStateStore = state.gameModelState;

        frameDirty = true;*/
    }

    int frameDelta = 0;
    if (key == KEY_O) frameDelta = -1;
    if (key == KEY_P || key == KEY_V) frameDelta = 1;

    if (GetAsyncKeyState(VK_CONTROL)) frameDelta *= 10;

    if (frameDelta != 0)
    {
        Replay &replay = *state.replayDatabase.replays[state.curFrame.replayIndex]->replay;
        state.curFrame.frameIndex = math::mod(state.curFrame.frameIndex + frameDelta, replay.frames.size());
    }
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
