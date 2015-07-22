
#include "mLibCore.h"

#include "mLibLodePNG.h"

using namespace std;
using namespace ml;

typedef ColorImageR8G8B8A8 Bitmap;

#include <windows.h>
#include <d3d11.h>

#include "util.h"

#include "../shared/gameAIInterface.h"

#include "localizedObject.h"
#include "objectCollection.h"

#include "globals.h"

#include "assets.h"
#include "d3d11Device.h"
#include "d3d11DeviceContext.h"
#include "DXGISwapChain.h"
