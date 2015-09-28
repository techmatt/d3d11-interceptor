
#include "main.h"

void DatabaseProcessor::go(AppState &state)
{
    state.segmentManager.init();

    processRawReplays(state);
    computeObjects(state);
    annotateObjects(state);

    //dumpDebugFrames(state, FrameID(0, 3000), 500);
    //dumpDebugFrames(state, FrameID(0, 5600), 100);

    state.segmentManager.saveVizColors(state.getPalette(), learningParams().ROMDatasetDir + "vizColors/");
    state.segmentManager.saveVizObjects(state.getPalette(), learningParams().ROMDatasetDir + "vizObjects/");
    state.objectAnalyzer.outputViz(learningParams().ROMDatasetDir + "statsObjects/");
    
}

void DatabaseProcessor::dumpDebugFrames(AppState &state, FrameID startFrame, int length)
{
    const string replayAnnotatedDir = learningParams().ROMDatasetDir + "replaysAnnotated/";
    const string outDir = learningParams().ROMDatasetDir + "debugViz/";

    util::makeDirectory(outDir);

    int curReplayIndex = 0;
    for (const string &filename : Directory::enumerateFilesWithPath(replayAnnotatedDir, ".dat"))
    {
        curReplayIndex++;
        if (curReplayIndex - 1 != startFrame.replayIndex)
            continue;

        Replay replay;
        cout << "Dumping frames in " << filename << endl;
        replay.load(filename);
        for (int frameOffset = 0; frameOffset < length; frameOffset++)
        {
            FrameID frameID = startFrame.delta(frameOffset);
            const ReplayFrame &frame = *replay.frames[frameID.frameIndex];
            const Bitmap imageA = AtariUtil::makeFrameObjectImage(state.segmentManager, state.getPalette(), state.objectAnalyzer, frameID.replayIndex, frame);
            const Bitmap imageB = frame.image.toBmp(state.getPalette());
            LodePNG::save(imageA, outDir + frameID.toString() + "_A.png");
            LodePNG::save(imageB, outDir + frameID.toString() + "_B.png");
        }
    }
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
                if (frame->index % 1000 == 0)
                    cout << "Frame " << frame->index << " / " << replay.frames.size() << endl;
                state.segmentManager.recordAndAnnotateSegments(state.getPalette(), *frame);
            }
            state.segmentManager.processedReplays.insert(filename);

            //replay.clearImages();
            replay.save(replayAnnotatedDir + filename);
        }
    }

    cout << "Segment manager has " << state.segmentManager.segmentCount() << " segments" << endl;
    state.segmentManager.save(learningParams().ROMDatasetDir + "segments.dat");
    state.segmentManager.load(learningParams().ROMDatasetDir + "segments.dat");
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
            if (frameIndex % 1000 == 0)
                cout << "Frame " << frameIndex << " / " << replay.frames.size() << endl;
            const ReplayFrame &frameA = *replay.frames[frameIndex + 0];
            const ReplayFrame &frameB = *replay.frames[frameIndex + 1];
            state.segmentAnalyzer.recordFramePair(state.segmentManager, frameA, frameB);
        }
    }

    state.segmentManager.objects = state.segmentAnalyzer.extractObjects();
    state.objectAnalyzer.init(state.segmentManager, vec2i((int)state.ale.getScreen().width(), (int)state.ale.getScreen().height()));
}

void DatabaseProcessor::annotateObjects(AppState &state)
{
    const string replayAnnotatedDir = learningParams().ROMDatasetDir + "replaysAnnotated/";
    int replayIndex = 0;
    for (const string &filename : Directory::enumerateFilesWithPath(replayAnnotatedDir, ".dat"))
    {
        Replay replay;
        cout << "Annotating objects in " << filename << endl;
        replay.load(filename);

        for (int frameIndex = 0; frameIndex < replay.frames.size(); frameIndex++)
        {
            if (frameIndex % 1000 == 0)
                cout << "Frame " << frameIndex << " / " << replay.frames.size() << endl;
            ReplayFrame &frame = *replay.frames[frameIndex];
            state.segmentAnalyzer.annotateObjects(frame);
        }

        replay.updateObjectIDs(state.segmentManager);

        cout << "Computing object statistics in " << filename << endl;
        for (int frameIndex = 0; frameIndex < replay.frames.size(); frameIndex++)
        {
            const ReplayFrame &frame = *replay.frames[frameIndex];
            state.objectAnalyzer.AnalyzeFrame(state.segmentManager, frame);
        }

        cout << "Computing object tracks in " << filename << endl;
        state.objectAnalyzer.startTracking(*replay.frames[0], replayIndex);
        for (int frameIndex = 1; frameIndex < replay.frames.size(); frameIndex++)
        {
            state.objectAnalyzer.trackFrames(*replay.frames[frameIndex - 1], *replay.frames[frameIndex]);
        }

        if (replayIndex == 0)
        {
            cout << "Outputting segment blacklist" << endl;
            state.objectAnalyzer.outputSegmentBlacklist(replay);
        }
        
        //replay.clearImages();
        replay.save(filename);
        replayIndex++;
    }

    state.objectAnalyzer.finalizeTracks();
}
