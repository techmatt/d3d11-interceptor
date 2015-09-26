
#include "main.h"

void DatabaseProcessor::go(AppState &state)
{
    initSegmentManager(state);
}

void DatabaseProcessor::initSegmentManager(AppState &state)
{
    state.segmentManager.load(learningParams().ROMDatasetDir + "segments.dat");

    const string replayDir = learningParams().ROMDatasetDir + "replaysRaw/";
    for (const string &filename : Directory::enumerateFiles(replayDir))
    {
        if (state.segmentManager.processedReplays.count(filename) == 0)
        {
            Replay replay;
            cout << "Loading segments in " << filename << endl;
            replay.load(replayDir + filename);
            for (ReplayFrame *frame : replay.frames)
            {
                if (frame->index % 100 == 0)
                    cout << "Frame " << frame->index << " / " << replay.frames.size() << endl;
                state.segmentManager.recordSegments(state.getPalette(), *frame);
            }
            state.segmentManager.processedReplays.insert(filename);

            state.segmentManager.saveAllViz(state.getPalette(), learningParams().ROMDatasetDir + "viz/");
        }
    }

    cout << "Segment manager has " << state.segmentManager.segmentCount() << " segments" << endl;
    state.segmentManager.save(learningParams().ROMDatasetDir + "segments.dat");
}

void DatabaseProcessor::processRawReplays(AppState &state)
{

}