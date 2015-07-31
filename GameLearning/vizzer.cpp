
#include "main.h"

//const float sizeThreshold = 30.0f;
const float sizeThreshold = numeric_limits<float>::max();

void Vizzer::makeFrameMeshesBox(ApplicationData &app, const FrameObjectData &frame, vector<D3D11TriMesh> &meshes)
{
    meshes.clear();
    meshes.resize(frame.objectData.size());
    int objectIndex = 0;
    for (auto &o : frame.objectData)
    {
        const vec3f extent = o.bbox.getExtent();
        const float maxDim = max(max(extent.x, extent.y), extent.z);
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
            mesh.transform(FrameProcessing::alignObjects(geometry->data, o));
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

void Vizzer::init(ApplicationData &app)
{	
    const string shaderDir = "../../frameworkD3D11/shaders/";
    
    state.assets.init(app.graphics);

    vec3f eye(1.0f, 1.0f, 4.5f);
    vec3f worldUp(0.0f, 1.0f, 0.0f);
    state.camera = Cameraf(-eye, worldUp, vec3f::origin, 60.0f, (float)app.window.getWidth() / app.window.getHeight(), 0.01f, 10000.0f, true);

    font.init(app.graphics, "Calibri");

    state.ui.init("GameLearningUI.exe", "GameLearningUI");

    //singleCaptureObjects.load(R"(C:\Code\d3d11-interceptor\Dolphin-x64\d3d11Logs\objects.txt)");
    state.allFrames.load(R"(C:\Code\d3d11-interceptor\Dolphin-x64\d3d11Logs\allFrames.dat)");
    //state.allFrames.frames.resize(500);

    state.colorMap.load(R"(C:\Code\d3d11-interceptor\Dolphin-x64\signatureColorMap.dat)");

    //comparisonFrameB->transform(mat4f::translation(vec3f(0.0f, 0.0f, 100.0f)));

    //auto alignment = FrameProcessing::alignFrames(*comparisonFrameA, *comparisonFrameB);

    //if (alignment.size > 0)
    //    comparisonFrameB->transform(alignment.transform.getInverse());

    //auto alignment2 = FrameProcessing::alignFrames(*comparisonFrameA, *comparisonFrameB);

    //correspondences = FrameProcessing::getCorrespondences(*comparisonFrameA, *comparisonFrameB);

    FrameProcessing::alignAllFrames(state.allFrames);

    state.analyzer.analyze(state.allFrames);
    //state.analyzer.dump("segments.txt");

    const int characterCount = (int)state.analyzer.characterSegments.size();
    for (int characterIndex = 0; characterIndex < characterCount; characterIndex++)
    {
        state.characters.resize(characterCount);
        state.characters[characterIndex].load(state.allFrames, state.analyzer.characterSegments[characterIndex], characterIndex);
    }
    
    //makeFrameMeshes(app, *comparisonFrameA, comparisonMeshesA, curFrameBoxMeshes);
    //makeFrameMeshes(app, *comparisonFrameB, comparisonMeshesB, curFrameBoxMeshes);

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
}

void Vizzer::render(ApplicationData &app)
{
    timer.frame();

    state.eventMap.dispatchEvents(state.ui);

    const FrameObjectData &frame = *state.allFrames.frames[state.curFrameIndex];

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

        if (!state.showSelectionOnly || signature == state.selectedSignature || selectedCharacter)
            state.assets.renderMesh(meshes[meshIndex], state.camera.getCameraPerspective(), color);

        /*if (meshIndex == frameAObjectIndex)
        {
            const auto &v0 = frame.objectMeshes[meshIndex];
            for (const vec3f &v : frame.objectMeshes[meshIndex].data.vertices)
            {
                assets.renderSphere(state.camera.getCameraPerspective(), v, 0.4f, color);
            }
        }*/
    }

    /*const UINT64 targetSignature = comparisonFrameA->objects[frameAObjectIndex].signature;

    for (int meshIndex = 0; meshIndex < comparisonMeshesA.size(); meshIndex++)
    {
        const D3D11TriMesh &m = comparisonMeshesA[meshIndex];
        vec3f color(1.0f, 1.0f, 1.0f);
        if (comparisonFrameA->objects[meshIndex].signature == targetSignature)
            color = vec3f(1.0f, 0.0f, 0.0f);
        if (meshIndex == frameAObjectIndex)
            color = vec3f(0.0f, 1.0f, 0.0f);

        assets.renderMesh(m, state.camera.getCameraPerspective(), color);
    }

    for (int meshIndex = 0; meshIndex < comparisonMeshesB.size(); meshIndex++)
    {
        const D3D11TriMesh &m = comparisonMeshesB[meshIndex];
        vec3f color(1.0f, 1.0f, 1.0f);
        if (comparisonFrameB->objects[meshIndex].signature == targetSignature)
            color = vec3f(0.0f, 0.0f, 1.0f);
        if (meshIndex == frameBObjectIndex)
            color = vec3f(0.0f, 0.75f, 0.0f);
        assets.renderMesh(m, state.camera.getCameraPerspective(), color);
    }*/

    /*for (auto &c : correspondences)
    {
        const vec3f color = vec3f(0.8f, 0.8f, 0.8f);
        assets.renderSphere(state.camera.getCameraPerspective(), c.source->centroid, 2.0f, color);
        assets.renderSphere(state.camera.getCameraPerspective(), c.dest->centroid, 2.0f, color);
        assets.renderCylinder(state.camera.getCameraPerspective(), c.source->centroid, c.dest->centroid, 2.0f, color);
    }*/

    int y = 0;
    font.drawString(app.graphics, "FPS: " + convert::toString(timer.framesPerSecond()), vec2i(10, 5 + y++ * 25), 24.0f, RGBColor::Red);
    font.drawString(app.graphics, "Frame " + to_string(state.curFrameIndex) + " / " + to_string(state.allFrames.frames.size()), vec2i(10, 5 + y++ * 25), 24.0f, RGBColor::Red);
    font.drawString(app.graphics, "Selected signature: " + to_string(state.selectedSignature), vec2i(10, 5 + y++ * 25), 24.0f, RGBColor::Red);
    font.drawString(app.graphics, "Object count: " + to_string(frame.objectData.size()), vec2i(10, 5 + y++ * 25), 24.0f, RGBColor::Red);
    font.drawString(app.graphics, "Selected character: " + to_string(state.curCharacterIndex) + " / " + to_string(state.analyzer.characterSegments.size()), vec2i(10, 5 + y++ * 25), 24.0f, RGBColor::Red);
    
    double animationDistL2 = -1.0;
    double animationDistMaxError = -1.0;
    if (state.curCharacterIndex >= 0 && state.curCharacterIndex < state.characters.size())
    {
        const auto &animationInstanceA = state.characters[state.curCharacterIndex].findInstanceAtFrame(state.animationAnchorFrame);
        const auto &animationInstanceB = state.characters[state.curCharacterIndex].findInstanceAtFrame(state.curFrameIndex);
        if (animationInstanceA != nullptr && animationInstanceB != nullptr)
        {
            animationDistL2 = Character::frameInstanceDistL2(*animationInstanceA, *animationInstanceB);
            animationDistMaxError = Character::frameInstanceDistMaxError(*animationInstanceA, *animationInstanceB);
        }
    }
    font.drawString(app.graphics, "DistL2: " + to_string(animationDistL2), vec2i(10, 5 + y++ * 25), 24.0f, RGBColor::Red);
    font.drawString(app.graphics, "DistMax: " + to_string(animationDistMaxError), vec2i(10, 5 + y++ * 25), 24.0f, RGBColor::Red);

    //font.drawString(app.graphics, "Target object A index: " + to_string(frameAObjectIndex) + " / " + to_string(comparisonFrameA->objectData.size()) + " sig=" + to_string(comparisonFrameA->objectData[frameAObjectIndex].signature), vec2i(10, 80), 24.0f, RGBColor::Red);
    //font.drawString(app.graphics, "Target object B index: " + to_string(frameBObjectIndex) + " / " + to_string(comparisonFrameB->objectData.size()) + " sig=" + to_string(comparisonFrameB->objectData[frameBObjectIndex].signature), vec2i(10, 105), 24.0f, RGBColor::Red);
}

void Vizzer::resize(ApplicationData &app)
{
    state.camera.updateAspectRatio((float)app.window.getWidth() / app.window.getHeight());
}

void Vizzer::keyDown(ApplicationData &app, UINT key)
{
    if (key == KEY_F) app.graphics.castD3D11().toggleWireframe();

    if (key == KEY_K) state.curCharacterIndex = math::mod(state.curCharacterIndex - 1, state.analyzer.characterSegments.size());
    if (key == KEY_L) state.curCharacterIndex = math::mod(state.curCharacterIndex + 1, state.analyzer.characterSegments.size());
    

    if (key == KEY_J) state.animationAnchorFrame = state.curFrameIndex;

    /*if (key == KEY_K) frameAObjectIndex = math::mod(frameAObjectIndex - 1, comparisonFrameA->objectData.size());
    if (key == KEY_L) frameAObjectIndex = math::mod(frameAObjectIndex + 1, comparisonFrameA->objectData.size());

    if (key == KEY_N) frameBObjectIndex = math::mod(frameBObjectIndex - 1, comparisonFrameB->objectData.size());
    if (key == KEY_M) frameBObjectIndex = math::mod(frameBObjectIndex + 1, comparisonFrameB->objectData.size());

    if (key == KEY_C)
    {
        comparisonFrameA->objectMeshes[frameAObjectIndex].saveDescription("objA.txt");
        comparisonFrameB->objectMeshes[frameBObjectIndex].saveDescription("objB.txt");
    }*/

    int frameDelta = 0;
    if (key == KEY_O) frameDelta = -1;
    if (key == KEY_P) frameDelta = 1;

    if (frameDelta != 0)
    {
        state.curFrameIndex = math::mod(state.curFrameIndex + frameDelta, state.allFrames.frames.size());
        if (GetAsyncKeyState(VK_SHIFT))
        {
            while (state.allFrames.frames[state.curFrameIndex]->objectMeshes.size() == 0)
                state.curFrameIndex = math::mod(state.curFrameIndex + frameDelta, state.allFrames.frames.size());
        }
        const FrameObjectData &frame = *state.allFrames.frames[state.curFrameIndex];

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
