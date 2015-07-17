
#include "main.h"

void Vizzer::init(ApplicationData &app)
{
    m_meshes.resize(20);

    //const FloatType radius, const ml::vec3<FloatType>& pos, const size_t stacks /*= 10*/, const size_t slices /*= 10*/, const ml::vec4<FloatType>& color
    for (int i = 0; i < 20; i++)
    {
        const vec3f pos = vec3f(util::randomUniform(-1.0f, 1.0f), util::randomUniform(-1.0f, 1.0f), util::randomUniform(-1.0f, 1.0f));
        const vec4f color = vec4f(util::randomUniform(0.5f, 1.0f), util::randomUniform(0.5f, 1.0f), util::randomUniform(0.5f, 1.0f), 1.0f);
        m_meshes[i].load(app.graphics, TriMeshf(Shapesf::sphere(util::randomUniform(0.1f, 0.2f), pos, 10, 10, color)));
    }
	
    const string shaderDir = "../../frameworkD3D11/shaders/";
    m_vsColor.load(app.graphics, shaderDir + "test.shader");
    m_psColor.load(app.graphics, shaderDir + "test.shader");

    m_constants.init(app.graphics);

    vec3f eye(1.0f, 1.0f, 4.5f);
    vec3f worldUp(0.0f, 0.0f, 1.0f);
    m_camera = Cameraf(-eye, worldUp, vec3f::origin, 60.0f, (float)app.window.getWidth() / app.window.getHeight(), 0.01f, 1000.0f, true);

    m_font.init(app.graphics, "Calibri");

}

void Vizzer::render(ApplicationData &app)
{
    m_timer.frame();

    ConstantBuffer constants;
    constants.worldViewProj = m_camera.getCameraPerspective();
    m_constants.update(constants);

    m_vsColor.bind();
    m_psColor.bind();
    m_constants.bindVertexShader(0);

    for (auto &m : m_meshes)
        m.render();

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
    const float distance = 0.1f;
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
    const float distance = 0.002f;
    m_camera.move(distance * wheelDelta);
}

void Vizzer::mouseMove(ApplicationData &app)
{
    const float distance = 0.01f;
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
