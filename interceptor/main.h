
#include "mLibCore.h"

#include "mLibLodePNG.h"

using namespace std;
using namespace ml;

typedef ColorImageR8G8B8A8 Bitmap;

const bool capturingAllBuffers = true;

#include <windows.h>
#include <d3d11.h>

#include "util.h"

#include "../shared/gameAIInterface.h"
#include "../shared/localizedObject.h"
#include "../shared/objectCollection.h"

#include "globals.h"

#include "assets.h"
#include "d3d11Device.h"
#include "d3d11DeviceContext.h"
#include "DXGISwapChain.h"
