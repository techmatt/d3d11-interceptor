
#include "mLibCore.h"

#include "mLibLodePNG.h"

using namespace std;
using namespace ml;

typedef ColorImageR8G8B8A8 Bitmap;

const int frameDumpIndex = 150;
const bool capturingAllBuffers = true;
const int keyFrameCaptureRate = 1;

#include <windows.h>
#include <d3d11.h>

#include "../shared/gameAIInterface.h"
#include "../shared/localizedObject.h"
#include "../shared/objectCollection.h"
#include "../shared/signatureColorMap.h"
#include "../shared/geometryDatabase.h"

#include "globals.h"

#include "assets.h"
#include "d3d11Device.h"
#include "d3d11DeviceContext.h"
#include "DXGISwapChain.h"

#include "util.h"