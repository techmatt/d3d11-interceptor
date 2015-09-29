
#include "main.h"

void DatabaseProcessor::go(AppState &state)
{
    state.SegmentDatabase.init();

    processRawReplays(state);

    state.replayDatabase.loadAnnotatedReplays();

    computeObjects(state);
    annotateObjects(state);

    dumpDebugFrames(state, FrameID(0, 1505), 20);
    //dumpDebugFrames(state, FrameID(0, 5600), 100);

    const bool dumpViz = true;
    if (dumpViz)
    {
        state.SegmentDatabase.saveVizColors(state.getPalette(), learningParams().ROMDatasetDir + "vizColors/");
        state.SegmentDatabase.saveVizObjects(state.getPalette(), learningParams().ROMDatasetDir + "vizObjects/");
        state.objectAnalyzer.outputViz(learningParams().ROMDatasetDir + "statsObjects/");
    }

    state.model.initStateSpec(state.objectAnalyzer, learningParams().ROMDatasetDir + "modelSpec/variableSpec.txt");
    state.model.describeModel(learningParams().ROMDatasetDir + "model.csv");

    dumpReplayStateGraphs(state);
}

void DatabaseProcessor::dumpDebugFrames(AppState &state, FrameID startFrame, int length)
{
    const string replayAnnotatedDir = learningParams().ROMDatasetDir + "replaysAnnotated/";
    const string outDir = learningParams().ROMDatasetDir + "debugViz/";

    util::makeDirectory(outDir);

    Replay &replay = *state.replayDatabase.replays[startFrame.replayIndex];
    cout << "Dumping frames in replay " << replay.index << endl;
    for (int frameOffset = 0; frameOffset < length; frameOffset++)
    {
        FrameID frameID = startFrame.delta(frameOffset);
        const ReplayFrame &frame = *replay.frames[frameID.frameIndex];
        const Bitmap imageA = AtariUtil::makeFrameObjectImage(state.SegmentDatabase, state.getPalette(), state.objectAnalyzer, frameID.replayIndex, frame);
        const Bitmap imageB = frame.image.toBmp(state.getPalette());
        LodePNG::save(imageA, outDir + frameID.toString() + "_A.png");
        LodePNG::save(imageB, outDir + frameID.toString() + "_B.png");
    }
}

void DatabaseProcessor::processRawReplays(AppState &state)
{
    state.SegmentDatabase.load(learningParams().ROMDatasetDir + "segments.dat");

    const string replayRawDir = learningParams().ROMDatasetDir + "replaysRaw/";
    const string replayAnnotatedDir = learningParams().ROMDatasetDir + "replaysAnnotated/";
    util::makeDirectory(replayAnnotatedDir);

    for (const string &filename : Directory::enumerateFiles(replayRawDir, ".dat"))
    {
        if (state.SegmentDatabase.processedReplays.count(filename) == 0 ||
            !util::fileExists(replayAnnotatedDir + filename))
        {
            Replay replay;
            cout << "Loading segments in " << filename << endl;
            replay.load(replayRawDir + filename);
            for (ReplayFrame *frame : replay.frames)
            {
                if (frame->index % 1000 == 0)
                    cout << "Frame " << frame->index << " / " << replay.frames.size() << endl;
                state.SegmentDatabase.recordAndAnnotateSegments(state.getPalette(), *frame);
            }
            state.SegmentDatabase.processedReplays.insert(filename);

            //replay.clearImages();
            replay.save(replayAnnotatedDir + filename);
        }
    }

    cout << "Segment manager has " << state.SegmentDatabase.segmentCount() << " segments" << endl;
    state.SegmentDatabase.save(learningParams().ROMDatasetDir + "segments.dat");
    state.SegmentDatabase.load(learningParams().ROMDatasetDir + "segments.dat");
}

void DatabaseProcessor::computeObjects(AppState &state)
{
    state.segmentAnalyzer.init(state.SegmentDatabase);

    for (const Replay *replay : state.replayDatabase.replays)
    {
        cout << "Computing objects in replay " << replay->index << endl;
        
        for (int frameIndex = 0; frameIndex < replay->frames.size() - 1; frameIndex++)
        {
            if (frameIndex % 1000 == 0)
                cout << "Frame " << frameIndex << " / " << replay->frames.size() << endl;
            const ReplayFrame &frameA = *replay->frames[frameIndex + 0];
            const ReplayFrame &frameB = *replay->frames[frameIndex + 1];
            state.segmentAnalyzer.recordFramePair(state.SegmentDatabase, frameA, frameB);
        }
    }

    state.SegmentDatabase.objects = state.segmentAnalyzer.extractObjects();
    state.objectAnalyzer.init(state.SegmentDatabase, vec2i((int)state.ale.getScreen().width(), (int)state.ale.getScreen().height()));
}

void DatabaseProcessor::annotateObjects(AppState &state)
{
    int replayIndex = 0;
    for (Replay *replay : state.replayDatabase.replays)
    {
        cout << "Annotating objects in replay " << replay->index << endl;
        
        for (int frameIndex = 0; frameIndex < replay->frames.size(); frameIndex++)
        {
            if (frameIndex % 1000 == 0)
                cout << "Frame " << frameIndex << " / " << replay->frames.size() << endl;
            ReplayFrame &frame = *replay->frames[frameIndex];
            state.segmentAnalyzer.annotateObjects(frame);
        }

        replay->updateObjectIDs(state.SegmentDatabase);

        cout << "Computing object statistics in replay " << replay->index << endl;
        for (int frameIndex = 0; frameIndex < replay->frames.size(); frameIndex++)
        {
            const ReplayFrame &frame = *replay->frames[frameIndex];
            state.objectAnalyzer.AnalyzeFrame(state.SegmentDatabase, frame);
        }

        cout << "Computing object tracks in replay " << replay->index << endl;
        state.objectAnalyzer.startTracking(*replay->frames[0], replayIndex);
        for (int frameIndex = 1; frameIndex < replay->frames.size(); frameIndex++)
        {
            state.objectAnalyzer.trackFrames(*replay->frames[frameIndex - 1], *replay->frames[frameIndex]);
        }

        if (replayIndex == 0)
        {
            cout << "Outputting segment blacklist" << endl;
            state.objectAnalyzer.outputSegmentBlacklist(state, *replay);
        }
        
        //replay.clearImages();
        //replay->save(filename);
        replayIndex++;
    }

    state.objectAnalyzer.finalizeTracks();
    state.objectAnalyzer.assignObjectNames();
}

void DatabaseProcessor::dumpReplayStateGraphs(AppState &state)
{
    const string replayAnnotatedDir = learningParams().ROMDatasetDir + "replaysAnnotated/";
    const string graphDir = learningParams().ROMDatasetDir + "stateGraphs/";

    util::makeDirectory(graphDir);

    for (Replay *replay : state.replayDatabase.replays)
    {
        cout << "Dumping replay graphs in replay " << replay->index << endl;
        
        replay->updateObjectIDs(state.SegmentDatabase);

        vector<Game::StateInst> states;
        state.modelLearner.loadReplayStates(state, *replay, state.model, states);
        AtariUtil::saveStateGraph(states, graphDir + to_string(replay->index) + ".csv");
    }
}
