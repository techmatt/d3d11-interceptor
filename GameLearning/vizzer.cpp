
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

    vec3f eye(1.0f, 1.0f, 4.5f);
    vec3f worldUp(0.0f, 1.0f, 0.0f);
    state.camera = Cameraf(-eye, worldUp, vec3f::origin, 60.0f, (float)app.window.getWidth() / app.window.getHeight(), 0.01f, 10000.0f, true);

    font.init(app.graphics, "Calibri");

    DatasetProcessor processor;
    processor.go(state);

    state.ui.init("GameLearningUI.exe", "GameLearningUI");
    
    registerEventHandlers(app);
}

void Vizzer::registerEventHandlers(ApplicationData& app)
{
    state.eventMap.registerEvent("terminate", [&](const vector<string> &params) {
        PostQuitMessage(0);
    });
    state.eventMap.registerEvent("loadSignature", [&](const vector<string> &params) {
        state.selectedSignature = util::convertTo<UINT64>(params[1]);
    });
    state.eventMap.registerEvent("loadFrame", [&](const vector<string> &params) {
        state.curFrame.replayIndex = util::convertTo<int>(params[1]);
        state.curFrame.replayIndex = math::clamp(state.curFrame.replayIndex, 0, (int)state.replays.entries.size() - 1);
        state.curFrame.frameIndex = util::convertTo<int>(params[2]);
        if (state.curFrame.frameIndex < 0 || state.curFrame.frameIndex >= state.replays.entries[state.curFrame.replayIndex].get().replay->frames.size())
        {
            state.curFrame.frameIndex = 0;
        }
    });
    state.eventMap.registerEvent("showBBoxes", [&](const vector<string> &params) {
        state.showBBoxes = ml::util::convertTo<bool>(params[1]);
    });
    state.eventMap.registerEvent("showFullMesh", [&](const vector<string> &params) {
        state.showFullMesh = ml::util::convertTo<bool>(params[1]);
    });
    state.eventMap.registerEvent("showSelectionOnly", [&](const vector<string> &params) {
        state.showSelectionOnly = ml::util::convertTo<bool>(params[1]);
    });
    state.eventMap.registerEvent("showTrackable", [&](const vector<string> &params) {
        state.showTrackable = ml::util::convertTo<bool>(params[1]);
    });
    state.eventMap.registerEvent("showCharacterSegments", [&](const vector<string> &params) {
        state.showCharacterSegments = ml::util::convertTo<bool>(params[1]);
    });
    state.eventMap.registerEvent("showAnimationLabel", [&](const vector<string> &params) {
        state.showAnimationLabel = ml::util::convertTo<bool>(params[1]);
    });
    state.eventMap.registerEvent("testAnchor", [&](const vector<string> &params) {
        const Character &curCharacter = state.characters.characters[state.curCharacterIndex];
        
        ofstream poseFile("logs/poseAnchorComparison.txt");
        ofstream animationFile("logs/animationAnchorComparison.txt");

        for (CharacterInstance *instance : curCharacter.allInstancesVec)
        {
            float poseDistance = curCharacter.poseDistance(state.anchorFrame, instance->frameID);
            poseFile << instance->frameID.toString() << '\t' << poseDistance << endl;
            
            float animationDistance = curCharacter.poseChainForwardDistance(state.anchorFrame, instance->frameID);
            animationFile << instance->frameID.toString() << '\t' << animationDistance << endl;
        }
    });
}

void Vizzer::render(ApplicationData &app)
{
    const float targetFPS = 20.0f;
    //Sleep(DWORD(1000.0f / targetFPS));

    timer.frame();

    state.eventMap.dispatchEvents(state.ui);

    const GameReplay &replay = *state.replays.entries[state.curFrame.replayIndex].get().replay;
    const FrameObjectData &frame = *state.replays.getFrame(state.curFrame)->frame;

    if (frameDirty)
        reloadFrame(app);

    vector<D3D11TriMesh> *meshesPtr = &state.curFrameMeshesRigidTransform;
    if (state.showBBoxes)
    {
        meshesPtr = &state.curFrameMeshesBox;
    }
    if (state.showFullMesh)
    {
        meshesPtr = &state.curFrameMeshesFull;
    }
    auto &meshes = *meshesPtr;

    const bool useSignatureCorrespondenceDebugColoring = false;

    for (int meshIndex = 0; meshIndex < meshes.size(); meshIndex++)
    {
        const UINT64 signature = frame.objectData[meshIndex].signature;
        const SegmentStats &segmentInfo = state.analyzer.segments[signature];
        vec3f color(1.0f, 1.0f, 1.0f);
        if (useSignatureCorrespondenceDebugColoring)
        {
                
            const int r = util::hash32(signature) & 255;
            const int g = util::hash32(signature * 458 + 58) & 255;
            const int b = util::hash32(signature * 127 + 13) & 255;
            const vec3f sigColor(r / 255.0f, g / 255.0f, b / 255.0f);
            color = math::lerp(sigColor, vec3f(1.0f, 1.0f, 1.0f), 0.5f);
        }
        if (state.showTrackable)
            if (segmentInfo.trackableSegment())
                color = vec3f(0.3f, 0.3f, 1.3f);
            else
                color = vec3f(1.3f, 0.3f, 0.3f);

        if (signature == state.selectedSignature)
            color = vec3f(1.0f, 0.0f, 1.0f);

        bool selectedCharacter = false;
        if (state.showCharacterSegments)
        {
            if (segmentInfo.characterLabel == state.curCharacterIndex)
            {
                selectedCharacter = true;
                color = vec3f(0.3f, 0.3f, 1.3f);
            }
            else if (segmentInfo.characterLabel == -1)
                color = vec3f(0.5f, 0.5f, 0.5f);
            else
                color = vec3f(1.3f, 0.3f, 0.3f);
        }

        if (state.showAnimationLabel)
        {
            color = vec3f(1.0f, 1.0f, 1.0f);
            for (const Character &c : state.characters.characters)
            {
                if (c.allSegmentsSet.count(signature) > 0)
                {
                    const CharacterInstance *instance = c.findInstanceAtFrame(state.curFrame);
                    if (instance != nullptr && instance->animation.sequence != nullptr)
                    {
                        color = instance->animation.sequence->color;
                    }
                }
            }
        }

        if (!state.showSelectionOnly || signature == state.selectedSignature || selectedCharacter)
            state.assets.renderMesh(meshes[meshIndex], state.camera.getCameraPerspective(), color);
    }

    const bool showModelState = true;
    if (showModelState)
    {
        auto &meshes = state.gameModelFrameMeshesRigidTransform;
        const FrameObjectData &frame = *state.replays.getFrame(state.gameModelPredictedCharacterFrame)->frame;
        for (int meshIndex = 0; meshIndex < meshes.size(); meshIndex++)
        {
            const UINT64 signature = frame.objectData[meshIndex].signature;
            const SegmentStats &segmentInfo = state.analyzer.segments[signature];
            vec3f color(0.5f, 1.0f, 0.5f);
            
            mat4f worldTransform = mat4f::identity();
            for (const Character &c : state.characters.characters)
            {
                if (c.characterIndex == state.curCharacterIndex && c.allSegmentsSet.count(signature) > 0)
                {
                    const CharacterInstance *predictedInstance = c.findInstanceAtFrame(state.gameModelPredictedCharacterFrame);
                    if (predictedInstance != nullptr)
                    {
                        const vec3f worldPosDiff = state.gameModelState.characters[c.characterIndex].worldPos - predictedInstance->worldCentroid;
                        worldTransform = mat4f::translation(worldPosDiff);
                    }

                    state.assets.renderMesh(meshes[meshIndex], state.camera.getCameraPerspective() * worldTransform, color);
                }
            }
        }
    }

    const Character &curCharacter = state.characters.characters[state.curCharacterIndex];

    const CharacterInstance *anchorInstance = curCharacter.findInstanceAtFrame(state.anchorFrame);
    const CharacterInstance *curInstance = curCharacter.findInstanceAtFrame(state.curFrame);
    int anchorAnimationInstanceCount = 0;
    /*if (anchorInstance != nullptr && anchorInstance->animation.animation > 0)
    {
        anchorAnimationInstanceCount = (int)anchorInstance->animation.animation->instances.size();
    }*/

    /*if (GetAsyncKeyState(VK_F4))
    {
        GameState gameState;
        gameState.load(state.curFrame, state.replays, state.characters);

        ofstream file("state.csv");
        for (string s : gameState.makeHeader())
            file << s << ',';
        file << "empty";
        file << endl;

        vector<float> descriptor(gameState.descriptorLength());

        for (CharacterInstance *instance : state.characters.characters[1].allInstancesVec)
        {
            gameState.load(instance->frameID, state.replays, state.characters);
            gameState.makeDescriptor(descriptor.data());
            for (float f : descriptor)
                file << f << ',';
            file << "0";
            file << endl;
        }
    }*/

    vector<string> text;
    text.push_back("FPS: " + convert::toString(timer.framesPerSecond()));
    text.push_back("Replay " + to_string(state.curFrame.replayIndex) + " / " + to_string(state.replays.entries.size()));
    text.push_back("Frame " + to_string(state.curFrame.frameIndex) + " / " + to_string(replay.frames.size()));
    text.push_back("Object count: " + to_string(frame.objectData.size()));
    text.push_back("Selected character: " + to_string(state.curCharacterIndex) + " / " + to_string(state.analyzer.characterSegments.size()));
    text.push_back("Anchor forward chain dist: " + to_string(curCharacter.poseChainForwardDistance(state.anchorFrame, state.curFrame)));
    text.push_back("Anchor reverse chain dist: " + to_string(curCharacter.poseChainReverseDistance(state.anchorFrame, state.curFrame)));
    text.push_back("Anchor pose dist: " + to_string(curCharacter.poseDistance(state.anchorFrame, state.curFrame)));

    auto &predictedPoseHistory = state.gameModelState.characters[state.curCharacterIndex].poseHistory;
    text.push_back("Predicted reverse chain dist: " + to_string(curCharacter.poseChainReverseDistance(predictedPoseHistory, state.curFrame)));

    if (anchorInstance)
        text.push_back("Anchor pose index: " + to_string(anchorInstance->poseCluster->index));

    if (curInstance)
    {
        text.push_back("Current pose index: " + to_string(curInstance->poseCluster->index));
        text.push_back("Current pose seed: " + to_string(curInstance->poseCluster->seedFrame.frameIndex));
    }

    //text.push_back("Character state: " + gameState.characterState[state.curCharacterIndex].describe());
    text.push_back("Controller 0: " + frame.padState[0].toString());
    text.push_back("Controller 1: " + frame.padState[1].toString());

    text.push_back("Prediction index: " + to_string(state.predictionIndex));
    text.push_back("Prediction pose chain dist: " + to_string(state.gameModelPrediction.poseChainDistSq));
    text.push_back("Controller dist: " + to_string(state.gameModelPrediction.controllerDist));
    text.push_back("Velocity dist: " + to_string(state.gameModelPrediction.velocityDist));

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

void Vizzer::reloadFrame(ApplicationData &app)
{
    FrameObjectData *curFrame = state.replays.getFrame(state.curFrame)->frame;
    FrameObjectData *modelFrame = state.replays.getFrame(state.gameModelPredictedCharacterFrame)->frame;

    //makeFrameMeshesBox(app, *curFrame, state.curFrameMeshesBox);
    makeFrameMeshesFull(app, *curFrame, state.curFrameMeshesFull);
    makeFrameMeshesRigidTransform(app, *curFrame, state.curFrameMeshesRigidTransform);

    if (modelFrame)
    {
        cout << "predicted frame: " << state.gameModelPredictedCharacterFrame.toString() << endl;
        makeFrameMeshesRigidTransform(app, *modelFrame, state.gameModelFrameMeshesRigidTransform);
    }
    else
        cout << "ModelFrame == nullptr" << endl;

    frameDirty = false;
}

void Vizzer::keyDown(ApplicationData &app, UINT key)
{
    if (key == KEY_F) app.graphics.castD3D11().toggleWireframe();

    if (key == KEY_K) state.curCharacterIndex = math::mod(state.curCharacterIndex - 1, state.analyzer.characterSegments.size());
    if (key == KEY_L) state.curCharacterIndex = math::mod(state.curCharacterIndex + 1, state.analyzer.characterSegments.size());
    
    if (key == KEY_J)
    {
        state.anchorFrame = state.curFrame;
        state.gameModelState.load(state.anchorFrame, state.replays, state.characters);
        state.gameModelFrame = state.curFrame;

        state.gameModelPredictedCharacterFrame = state.gameModelState.characters[state.curCharacterIndex].poseHistory[0]->seedFrame;

        frameDirty = true;
    }

    bool advanceModel = (key == KEY_B || key == KEY_V);
    
    if (advanceModel)
    {
        state.gameModelFrame = state.gameModelFrame.delta(1);

        ControllerState controller;
        const ProcessedFrame *nextFrame = state.replays.getFrame(state.gameModelFrame);

        if (nextFrame != nullptr)
            controller.LoadGamecube(*nextFrame);

        StateTransition transition;
        state.gameModel.predictTransition(state.replays, state.characters, state.gameModelState, transition, 0);
        state.gameModel.advanceGameState(state.gameModelState, transition, controller);

        state.gameModelPredictedCharacterFrame = state.gameModelState.characters[state.curCharacterIndex].poseHistory[0]->seedFrame;

        state.gameModelStateStore = state.gameModelState;

        frameDirty = true;
    }

    int predictionDelta = 0;
    if (key == KEY_Z) predictionDelta = -1;
    if (key == KEY_X) predictionDelta = 1;

    if (predictionDelta)
    {
        state.predictionIndex += predictionDelta;

        state.gameModelState = state.gameModelStateStore;

        ControllerState controller;
        const ProcessedFrame *nextFrame = state.replays.getFrame(state.gameModelFrame.delta(1));

        if (nextFrame != nullptr)
            controller.LoadGamecube(*nextFrame);

        StateTransition transition;
        state.gameModelPrediction = state.gameModel.predictTransition(state.replays, state.characters, state.gameModelState, transition, state.predictionIndex);
        state.gameModel.advanceGameState(state.gameModelState, transition, controller);

        state.gameModelPredictedCharacterFrame = state.gameModelState.characters[state.curCharacterIndex].poseHistory[0]->seedFrame;

        frameDirty = true;
    }

    int animationInstanceDelta = 0;
    if (key == KEY_N) animationInstanceDelta = -1;
    if (key == KEY_M) animationInstanceDelta = 1;

    if (animationInstanceDelta != 0)
    {
        const Character &curCharacter = state.characters.characters[state.curCharacterIndex];
        const CharacterInstance *anchorInstance = curCharacter.findInstanceAtFrame(state.anchorFrame);
        if (anchorInstance != nullptr && anchorInstance->animation.sequence != nullptr)
        {
            const AnimationSequence &curAnimation = *anchorInstance->animation.sequence;
            if (curAnimation.instances.size() > 0)
            {
                state.anchorAnimationInstanceIndex = math::mod(state.anchorAnimationInstanceIndex + animationInstanceDelta, curAnimation.instances.size());
                state.curFrame = curAnimation.instances[state.anchorAnimationInstanceIndex];
                frameDirty = true;
            }
        }
    }

    int frameDelta = 0;
    if (key == KEY_O) frameDelta = -1;
    if (key == KEY_P || key == KEY_V) frameDelta = 1;

    if (GetAsyncKeyState(VK_CONTROL)) frameDelta *= 10;

    if (frameDelta != 0)
    {
        GameReplay &replay = *state.replays.entries[state.curFrame.replayIndex].get().replay;
        state.curFrame.frameIndex = math::mod(state.curFrame.frameIndex + frameDelta, replay.frames.size());

        if (GetAsyncKeyState(VK_SHIFT))
        {
            while (replay.frames[state.curFrame.frameIndex]->objectMeshes.size() == 0)
                state.curFrame.frameIndex = math::mod(state.curFrame.frameIndex + frameDelta, replay.frames.size());
        }

        frameDirty = true;
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

    if(key == KEY_UP) state.camera.lookUp(theta);
    if(key == KEY_DOWN) state.camera.lookUp(-theta);
    if(key == KEY_LEFT) state.camera.lookRight(theta);
    if(key == KEY_RIGHT) state.camera.lookRight(-theta);
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

void Vizzer::makeFrameMeshesBox(ApplicationData &app, const FrameObjectData &frame, vector<D3D11TriMesh> &meshes)
{
    meshes.resize(frame.objectData.size());
    int objectIndex = 0;
    for (auto &o : frame.objectData)
    {
        const vec3f extent = o.bbox.getExtent();
        const float maxDim = max(max(extent.x, extent.y), extent.z);
        
        const float sizeThreshold = numeric_limits<float>::max();
        if (maxDim > sizeThreshold)
        {
            continue;
        }

        meshes[objectIndex] = D3D11TriMesh(app.graphics, Shapesf::box(o.bbox, vec4f(state.colorMap.getColor(o.signature), 1.0f)));
        objectIndex++;
    }
}

void Vizzer::makeFrameMeshesRigidTransform(ApplicationData &app, const FrameObjectData &frame, vector<D3D11TriMesh> &meshes)
{
    meshes.resize(frame.objectData.size());
    int objectIndex = 0;
    for (auto &o : frame.objectData)
    {
        const LocalizedObject *geometry = state.geoDatabase.loadGeometry(o.signature);
        if (geometry != nullptr)
        {
            TriMeshf mesh;
            geometry->toMesh(state.colorMap, mesh);
            mesh.transform(FrameAlignment::alignObjects(geometry->data, o));
            meshes[objectIndex].load(app.graphics, mesh);
        }
        objectIndex++;
    }
}

void Vizzer::makeFrameMeshesFull(ApplicationData &app, const FrameObjectData &frame, vector<D3D11TriMesh> &meshes)
{
    meshes.resize(frame.objectMeshes.size());
    for (int objectIndex = 0; objectIndex < frame.objectMeshes.size(); objectIndex++)
    {
        const auto &o = frame.objectMeshes[objectIndex];
        TriMeshf mesh;
        o.toMesh(state.colorMap, mesh);
        meshes[objectIndex] = D3D11TriMesh(app.graphics, mesh);
    }
}
