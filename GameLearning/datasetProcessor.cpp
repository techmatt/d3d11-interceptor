
#include "main.h"

void DatasetProcessor::go(AppState &state)
{
    state.colorMap.load(learningParams().datasetDir + "signatureColorMap.dat");

    alignFrames(state);
    loadAlignedFrames(state);

    state.analyzer.analyze(state.replays);
    //state.analyzer.dump("logs/segments.txt");

    state.characters.init(state.analyzer);
    state.characters.recordAllFrames(state.replays);
}

void DatasetProcessor::alignFrames(AppState &state)
{
    cout << " *** Aligning frames" << endl;
    util::makeDirectory(learningParams().datasetDir + "alignedFrames/");
    for (const string &rawFilename : Directory::enumerateFilesWithPath(learningParams().datasetDir + "rawFrames/", ".dat"))
    {
        const string alignedFilename = util::replace(rawFilename, "rawFrames/", "alignedFrames/");
        if (!util::fileExists(alignedFilename))
        {
            cout << "Aligning " << rawFilename << endl;
            GameReplay frames;
            frames.load(rawFilename);
            FrameAlignment::alignAllFrames(frames);
            frames.save(alignedFilename);
        }
    }
}

void DatasetProcessor::loadAlignedFrames(AppState &state)
{
    const int maxReplayCount = 1;

    cout << " *** Loading aligned frames" << endl;
    for (const string &alignedFilename : Directory::enumerateFilesWithPath(learningParams().datasetDir + "alignedFrames/", ".dat"))
    {
        if (maxReplayCount <= 0 || state.replays.entries.size() < maxReplayCount)
            state.replays.addEntry(alignedFilename);
    }
}
