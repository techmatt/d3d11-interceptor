#include "main.h"

void main()
{
	Vizzer callback;
	ApplicationWin32 app(nullptr, 1280, 1024, "D3D11 Test", GraphicsDeviceTypeD3D11, callback);
	app.messageLoop();
}
