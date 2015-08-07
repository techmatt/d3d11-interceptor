
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
        state.curFrame.frameIndex = util::convertTo<int>(params[1]);
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
            
            float animationDistance = curCharacter.animationDistance(state.anchorFrame, instance->frameID);
            animationFile << instance->frameID.toString() << '\t' << animationDistance << endl;
        }
    });
}

void Vizzer::render(ApplicationData &app)
{
    timer.frame();

    state.eventMap.dispatchEvents(state.ui);

    GameReplay &replay = *state.replays.entries[state.curFrame.replayIndex].get().replay;
    const FrameObjectData &frame = *replay.frames[state.curFrame.frameIndex];

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

    for (int meshIndex = 0; meshIndex < meshes.size(); meshIndex++)
    {
        const UINT64 signature = frame.objectData[meshIndex].signature;
        const SegmentStats &segmentInfo = state.analyzer.segments[signature];
        vec3f color(1.0f, 1.0f, 1.0f);
        /*if (useSignatureCorrespondenceDebugColoring)
        {
                
            const int r = util::hash32(signature) & 255;
            const int g = util::hash32(signature * 458 + 58) & 255;
            const int b = util::hash32(signature * 127 + 13) & 255;
            const vec3f sigColor(r / 255.0f, g / 255.0f, b / 255.0f);
            color = math::lerp(sigColor, vec3f(1.0f, 1.0f, 1.0f), 0.5f);
        }*/
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
                    if (instance != nullptr && instance->sequences.size() > 0)
                    {
                        color = c.sequences[instance->sequences[0].sequenceIndex].color;
                    }
                }
            }
        }

        if (!state.showSelectionOnly || signature == state.selectedSignature || selectedCharacter)
            state.assets.renderMesh(meshes[meshIndex], state.camera.getCameraPerspective(), color);
    }

    const Character &curCharacter = state.characters.characters[state.curCharacterIndex];

    const CharacterInstance *anchorInstance = curCharacter.findInstanceAtFrame(state.anchorFrame);
    int anchorAnimationInstanceCount = 0;
    if (anchorInstance != nullptr && anchorInstance->sequences.size() > 0)
    {
        const AnimationSequence &curAnimation = curCharacter.sequences[anchorInstance->sequences[0].sequenceIndex];
        anchorAnimationInstanceCount = (int)curAnimation.instances.size();
    }

    vector<string> text;
    text.push_back("FPS: " + convert::toString(timer.framesPerSecond()));
    text.push_back("Frame " + to_string(state.curFrame.frameIndex) + " / " + to_string(replay.frames.size()));
    text.push_back("Object count: " + to_string(frame.objectData.size()));
    text.push_back("Selected character: " + to_string(state.curCharacterIndex) + " / " + to_string(state.analyzer.characterSegments.size()));
    text.push_back("Anchor animation dist: " + to_string(curCharacter.animationDistance(state.anchorFrame, state.curFrame)));
    text.push_back("Anchor animation: " + to_string(state.anchorAnimationInstanceIndex) + " / " + to_string(anchorAnimationInstanceCount));
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
    bool frameDirty = false;

    if (key == KEY_F) app.graphics.castD3D11().toggleWireframe();

    if (key == KEY_K) state.curCharacterIndex = math::mod(state.curCharacterIndex - 1, state.analyzer.characterSegments.size());
    if (key == KEY_L) state.curCharacterIndex = math::mod(state.curCharacterIndex + 1, state.analyzer.characterSegments.size());
    
    if (key == KEY_J) state.anchorFrame = state.curFrame;

    int animationInstanceDelta = 0;
    if (key == KEY_N) animationInstanceDelta = -1;
    if (key == KEY_M) animationInstanceDelta = 1;

    if (animationInstanceDelta != 0)
    {
        const Character &curCharacter = state.characters.characters[state.curCharacterIndex];
        const CharacterInstance *anchorInstance = curCharacter.findInstanceAtFrame(state.anchorFrame);
        if (anchorInstance != nullptr && anchorInstance->sequences.size() > 0)
        {
            const AnimationSequence &curAnimation = curCharacter.sequences[anchorInstance->sequences[0].sequenceIndex];
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
    if (key == KEY_P) frameDelta = 1;

    if (GetAsyncKeyState(VK_CONTROL)) frameDelta *= 10;

    if (frameDirty || frameDelta != 0)
    {
        GameReplay &replay = *state.replays.entries[state.curFrame.replayIndex].get().replay;
        state.curFrame.frameIndex = math::mod(state.curFrame.frameIndex + frameDelta, replay.frames.size());
        if (GetAsyncKeyState(VK_SHIFT))
        {
            while (replay.frames[state.curFrame.frameIndex]->objectMeshes.size() == 0)
                state.curFrame.frameIndex = math::mod(state.curFrame.frameIndex + frameDelta, replay.frames.size());
        }
        const FrameObjectData &frame = *replay.frames[state.curFrame.frameIndex];

        makeFrameMeshesBox(app, frame, state.curFrameMeshesBox);
        makeFrameMeshesFull(app, frame, state.curFrameMeshesFull);
        makeFrameMeshesRigidTransform(app, frame, state.curFrameMeshesRigidTransform);
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
    meshes.clear();
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
    meshes.clear();
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
            meshes[objectIndex] = D3D11TriMesh(app.graphics, mesh);
        }
        objectIndex++;
    }
}

void Vizzer::makeFrameMeshesFull(ApplicationData &app, const FrameObjectData &frame, vector<D3D11TriMesh> &meshes)
{
    meshes.clear();
    meshes.resize(frame.objectMeshes.size());
    for (int objectIndex = 0; objectIndex < frame.objectMeshes.size(); objectIndex++)
    {
        const auto &o = frame.objectMeshes[objectIndex];
        TriMeshf mesh;
        o.toMesh(state.colorMap, mesh);
        meshes[objectIndex] = D3D11TriMesh(app.graphics, mesh);
    }
}
