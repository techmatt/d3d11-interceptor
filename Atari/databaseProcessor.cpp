
#include "main.h"

void DatabaseProcessor::go(AppState &state)
{
    state.segmentManager.init();

    processRawReplays(state);
    computeObjects(state);

    state.segmentManager.saveVizColors(state.getPalette(), learningParams().ROMDatasetDir + "vizColors/");
    state.segmentManager.saveVizObjects(state.getPalette(), learningParams().ROMDatasetDir + "vizObjects/");
}

void DatabaseProcessor::processRawReplays(AppState &state)
{
    state.segmentManager.load(learningParams().ROMDatasetDir + "segments.dat");

    const string replayRawDir = learningParams().ROMDatasetDir + "replaysRaw/";
    const string replayAnnotatedDir = learningParams().ROMDatasetDir + "replaysAnnotated/";
    util::makeDirectory(replayAnnotatedDir);

    for (const string &filename : Directory::enumerateFiles(replayRawDir, ".dat"))
    {
        if (state.segmentManager.processedReplays.count(filename) == 0 ||
            !util::fileExists(replayAnnotatedDir + filename))
        {
            Replay replay;
            cout << "Loading segments in " << filename << endl;
            replay.load(replayRawDir + filename);
            for (ReplayFrame *frame : replay.frames)
            {
                if (frame->index % 100 == 0)
                    cout << "Frame " << frame->index << " / " << replay.frames.size() << endl;
                state.segmentManager.recordAndAnnotateSegments(state.getPalette(), *frame);
            }
            state.segmentManager.processedReplays.insert(filename);

            replay.clearImages();
            replay.save(replayAnnotatedDir + filename);
        }
    }

    cout << "Segment manager has " << state.segmentManager.segmentCount() << " segments" << endl;
    state.segmentManager.save(learningParams().ROMDatasetDir + "segments.dat");
}

void DatabaseProcessor::computeObjects(AppState &state)
{
    state.segmentAnalyzer.init(state.segmentManager);

    const string replayAnnotatedDir = learningParams().ROMDatasetDir + "replaysAnnotated/";
    for (const string &filename : Directory::enumerateFilesWithPath(replayAnnotatedDir, ".dat"))
    {
        Replay replay;
        cout << "Computing objects in " << filename << endl;
        replay.load(filename);

        for (int frameIndex = 0; frameIndex < replay.frames.size() - 1; frameIndex++)
        {
            if (frameIndex % 100 == 0)
                cout << "Frame " << frameIndex << " / " << replay.frames.size() << endl;
            const ReplayFrame &frameA = *replay.frames[frameIndex + 0];
            const ReplayFrame &frameB = *replay.frames[frameIndex + 1];
            state.segmentAnalyzer.recordFramePair(state.segmentManager, frameA, frameB);
        }
    }

    state.segmentManager.objects = state.segmentAnalyzer.extractObjects();
}