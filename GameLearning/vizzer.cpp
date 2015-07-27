
#include "main.h"

const float sizeThreshold = 30.0f;
//const float sizeThreshold = numeric_limits<float>::max();

void Vizzer::makeFrameMeshes(ApplicationData &app, const FrameObjectData &frame, vector<D3D11TriMesh> &meshes, vector<D3D11TriMesh> &boxMeshes)
{
    boxMeshes.clear();
    boxMeshes.resize(frame.objects.size());
    int objectIndex = 0;
    for (auto &o : frame.objects)
    {
        const vec3f extent = o.boundingBox.getExtent();
        const float maxDim = max(max(extent.x, extent.y), extent.z);
        if (maxDim > sizeThreshold)
        {
            continue;
        }

        boxMeshes[objectIndex] = D3D11TriMesh(app.graphics, Shapesf::box(o.boundingBox, vec4f(colorMap.getColor(o.signature), 1.0f)));
        objectIndex++;
    }

    meshes.clear();
    meshes.resize(frame.objectMeshes.size());
    for (int objectIndex = 0; objectIndex < frame.objectMeshes.size(); objectIndex++)
    {
        const auto &o = frame.objectMeshes[objectIndex];
        TriMeshf mesh;
        o.toMesh(colorMap, mesh);
        meshes[objectIndex] = D3D11TriMesh(app.graphics, mesh);
    }
}

void Vizzer::init(ApplicationData &app)
{	
    const string shaderDir = "../../frameworkD3D11/shaders/";
    
    assets.init(app.graphics);

    vec3f eye(1.0f, 1.0f, 4.5f);
    vec3f worldUp(0.0f, 1.0f, 0.0f);
    m_camera = Cameraf(-eye, worldUp, vec3f::origin, 60.0f, (float)app.window.getWidth() / app.window.getHeight(), 0.01f, 10000.0f, true);

    m_font.init(app.graphics, "Calibri");

    m_world = mat4f::identity();

    //singleCaptureObjects.load(R"(C:\Code\d3d11-interceptor\Dolphin-x64\d3d11Logs\objects.txt)");
    allFrames.load(R"(C:\Code\d3d11-interceptor\Dolphin-x64\d3d11Logs\allFrames.dat)");

    colorMap.load(R"(C:\Code\d3d11-interceptor\Dolphin-x64\signatureColorMap.dat)");

    frameIndex = 0;

    bboxMode = true;

    comparisonFrameA = allFrames.frames[498];
    comparisonFrameB = allFrames.frames[598];

    makeFrameMeshes(app, *comparisonFrameA, comparisonMeshesA, curFrameBoxMeshes);
    makeFrameMeshes(app, *comparisonFrameB, comparisonMeshesB, curFrameBoxMeshes);

    FrameProcessing::alignFrames(*comparisonFrameA, *comparisonFrameB);
}

void Vizzer::render(ApplicationData &app)
{
    m_timer.frame();

    m_world = m_world * mat4f::rotationZ(1.0f);

    if (bboxMode)
    {
        for (auto &m : curFrameBoxMeshes)
        {
            assets.renderMesh(m, m_camera.getCameraPerspective());
        }
    }
    else
    {
        for (auto &m : curFrameMeshes)
        {
            assets.renderMesh(m, m_camera.getCameraPerspective());
        }
    }

    m_font.drawString(app.graphics, "FPS: " + convert::toString(m_timer.framesPerSecond()), vec2i(10, 5), 24.0f, RGBColor::Red);
    m_font.drawString(app.graphics, "Frame " + to_string(frameIndex) + " / " + to_string(allFrames.frames.size()), vec2i(10, 30), 24.0f, RGBColor::Red);

    const FrameObjectData &frame = *allFrames.frames[frameIndex];
    m_font.drawString(app.graphics, "Object count: " + to_string(frame.objects.size()), vec2i(10, 55), 24.0f, RGBColor::Red);
}

void Vizzer::resize(ApplicationData &app)
{
    m_camera.updateAspectRatio((float)app.window.getWidth() / app.window.getHeight());
}

void Vizzer::keyDown(ApplicationData &app, UINT key)
{
    if (key == KEY_F) app.graphics.castD3D11().toggleWireframe();
    if (key == KEY_TAB) bboxMode = !bboxMode;
}

void Vizzer::keyPressed(ApplicationData &app, UINT key)
{
    const float distance = 1.0f;
    const float theta = 5.0f;

    if(key == KEY_S) m_camera.move(-distance);
    if(key == KEY_W) m_camera.move(distance);
    if(key == KEY_A) m_camera.strafe(-distance);
    if(key == KEY_D) m_camera.strafe(distance);
	if(key == KEY_E) m_camera.jump(distance);
	if(key == KEY_Q) m_camera.jump(-distance);

    if(key == KEY_UP) m_camera.lookUp(theta);
    if(key == KEY_DOWN) m_camera.lookUp(-theta);
    if(key == KEY_LEFT) m_camera.lookRight(theta);
    if(key == KEY_RIGHT) m_camera.lookRight(-theta);

    int frameDelta = 0;
    if (key == KEY_O) frameDelta = -1;
    if (key == KEY_P) frameDelta = 1;

    if (frameDelta != 0)
    {
        frameIndex = math::mod(frameIndex + frameDelta, allFrames.frames.size());
        if (GetAsyncKeyState(VK_SHIFT))
        {
            while (allFrames.frames[frameIndex]->objectMeshes.size() == 0)
                frameIndex = math::mod(frameIndex + frameDelta, allFrames.frames.size());
        }
        const FrameObjectData &frame = *allFrames.frames[frameIndex];

        makeFrameMeshes(app, frame, curFrameMeshes, curFrameBoxMeshes);
    }
}

void Vizzer::mouseDown(ApplicationData &app, MouseButtonType button)
{

}

void Vizzer::mouseWheel(ApplicationData &app, int wheelDelta)
{
    const float distance = 0.1f;
    m_camera.move(distance * wheelDelta);
}

void Vizzer::mouseMove(ApplicationData &app)
{
    const float distance = 1.0f;
    const float theta = 0.5f;

    vec2i posDelta = app.input.mouse.pos - app.input.prevMouse.pos;

    if(app.input.mouse.buttons[MouseButtonRight])
    {
        m_camera.strafe(-distance * posDelta.x);
        m_camera.jump(distance * posDelta.y);
    }

    if(app.input.mouse.buttons[MouseButtonLeft])
    {
        m_camera.lookRight(-theta * posDelta.x);
        m_camera.lookUp(theta * posDelta.y);
    }

}
