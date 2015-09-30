
#pragma once

#include "mLibInclude.h"

using namespace ml;
using namespace std;

typedef ColorImageR8G8B8A8 Bitmap;

#include "constants.h"
#include "learningParams.h"

#include "C:\Code\Arcade-Learning-Environment-0.5.0\src\ale_interface.hpp"

#include "LSH.h"

struct AppState;
struct SegmentAnimation;
struct ReplayFrame;
class ColourPalette;
struct SegmentDatabase;
struct ObjectAnalyzer;

namespace Game
{
    struct StateInst;
}

#include "atariUtil.h"
#include "replay.h"
#include "segmentDatabase.h"
#include "replayDatabase.h"
#include "segmentAnalyzer.h"
#include "objectAnalyzer.h"
#include "recallDatabase.h"
#include "databaseProcessor.h"

#include "gameModel.h"
#include "modelLearner.h"

#include "appState.h"
#include "vizzer.h"
