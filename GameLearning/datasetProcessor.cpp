
#include "main.h"

void DatasetProcessor::go(AppState &state)
{
    state.colorMap.load(learningParams().datasetDir + "signatureColorMap.dat");

    alignFrames(state);
    loadAlignedFrames(state);

    state.analyzer.analyze(state.allFrames);
    //state.analyzer.dump("logs/segments.txt");

    const int characterCount = (int)state.analyzer.characterSegments.size();
    for (int characterIndex = 0; characterIndex < characterCount; characterIndex++)
    {
        state.characters.resize(characterCount);
        state.characters[characterIndex].load(state.allFrames, state.analyzer.characterSegments[characterIndex], characterIndex);
    }
}

void DatasetProcessor::alignFrames(AppState &state)
{
    for (const string &rawFilename : Directory::enumerateFilesWithPath(learningParams().datasetDir + "rawFrames/", ".dat"))
    {
        const string alignedFilename = util::replace(rawFilename, "rawFrames/", "alignedFrames/");
        if (!util::fileExists(alignedFilename))
        {
            cout << "Aligning " << rawFilename << endl;
            FrameCollection frames;
            frames.load(rawFilename);
            FrameProcessing::alignAllFrames(frames);
            frames.save(alignedFilename);
        }
    }
}

void DatasetProcessor::loadAlignedFrames(AppState &state)
{
    for (const string &alignedFilename : Directory::enumerateFilesWithPath(learningParams().datasetDir + "alignedFrames/", ".dat"))
    {
        FrameCollection frames;

        cout << "Loading " << alignedFilename << endl;
        frames.load(alignedFilename);

        util::push_back(state.allFrames.frames, frames.frames);
    }
}
