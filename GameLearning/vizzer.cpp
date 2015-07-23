
#include "main.h"

void Vizzer::init(ApplicationData &app)
{	
    const string shaderDir = "../../frameworkD3D11/shaders/";
    
    assets.init(app.graphics);

    vec3f eye(1.0f, 1.0f, 4.5f);
    vec3f worldUp(0.0f, 1.0f, 0.0f);
    m_camera = Cameraf(-eye, worldUp, vec3f::origin, 60.0f, (float)app.window.getWidth() / app.window.getHeight(), 0.01f, 10000.0f, true);

    m_font.init(app.graphics, "Calibri");

    m_world = mat4f::identity();

    objects.load(R"(C:\Code\d3d11-interceptor\Dolphin-x64\d3d11Logs\capture000843\objects.txt)");
}

void Vizzer::render(ApplicationData &app)
{
    m_timer.frame();

    m_world = m_world * mat4f::rotationZ(1.0f);

    int index = 0;
    for (auto &o : objects.objects)
    {
        if (index == 312)
            int a = 5;
        index++;
        for (auto &v : o.vertices)
        {
            vec4i material = o.materials[2];
            assets.renderSphere(m_camera.getCameraPerspective(), v.worldPos, 0.2f, vec3f(material.getVec3()) / 255.0f);
        }
    }

    m_font.drawString(app.graphics, "FPS: " + convert::toString(m_timer.framesPerSecond()), vec2i(10, 5), 24.0f, RGBColor::Red);
}

void Vizzer::resize(ApplicationData &app)
{
    m_camera.updateAspectRatio((float)app.window.getWidth() / app.window.getHeight());
}

void Vizzer::keyDown(ApplicationData &app, UINT key)
{

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
