
#include "main.h"

void DatabaseProcessor::go(AppState &state)
{
    state.segmentDatabase.init();

    processRawReplays(state);

    state.replayDatabase.loadAnnotatedReplays();

    computeObjects(state);
    annotateObjects(state);

    dumpDebugFrames(state, FrameID(0, 1505), 20);

    const bool dumpViz = false;
    if (dumpViz)
    {
        state.segmentDatabase.saveVizColors(state.getPalette(), learningParams().ROMDatasetDir + "vizColors/");
        state.segmentDatabase.saveVizObjects(state.getPalette(), learningParams().ROMDatasetDir + "vizObjects/");
        state.objectAnalyzer.outputViz(learningParams().ROMDatasetDir + "statsObjects/");
    }

    state.model.initStateSpec(state.objectAnalyzer, learningParams().ROMDatasetDir + "modelSpec/variableSpec.txt");
    state.model.describeModel(learningParams().ROMDatasetDir + "model.csv");

    dumpReplayStateGraphs(state);

    state.replayDatabase.loadGameStates(state);
    state.recallDatabase.init(state);
}

void DatabaseProcessor::dumpDebugFrames(AppState &state, FrameID startFrame, int length)
{
    const string replayAnnotatedDir = learningParams().ROMDatasetDir + "replaysAnnotated/";
    const string outDir = learningParams().ROMDatasetDir + "debugViz/";

    util::makeDirectory(outDir);

    Replay &replay = *state.replayDatabase.replays[startFrame.replayIndex]->replay;
    cout << "Dumping frames in replay " << replay.index << endl;
    for (int frameOffset = 0; frameOffset < length; frameOffset++)
    {
        FrameID frameID = startFrame.delta(frameOffset);
        const ReplayFrame &frame = *replay.frames[frameID.frameIndex];
        const Bitmap imageA = AtariUtil::makeFrameObjectImage(state.segmentDatabase, state.getPalette(), state.objectAnalyzer, frameID.replayIndex, frame);
        const Bitmap imageB = frame.image.toBmp(state.getPalette());
        LodePNG::save(imageA, outDir + frameID.toString() + "_A.png");
        LodePNG::save(imageB, outDir + frameID.toString() + "_B.png");
    }
}

void DatabaseProcessor::processRawReplays(AppState &state)
{
    state.segmentDatabase.load(learningParams().ROMDatasetDir + "segments.dat");

    const string replayRawDir = learningParams().ROMDatasetDir + "replaysRaw/";
    const string replayAnnotatedDir = learningParams().ROMDatasetDir + "replaysAnnotated/";
    util::makeDirectory(replayAnnotatedDir);

    for (const string &filename : Directory::enumerateFiles(replayRawDir, ".dat"))
    {
        if (state.segmentDatabase.processedReplays.count(filename) == 0 ||
            !util::fileExists(replayAnnotatedDir + filename))
        {
            Replay replay;
            cout << "Loading segments in " << filename << endl;
            replay.load(replayRawDir + filename);
            for (ReplayFrame *frame : replay.frames)
            {
                if (frame->index % 1000 == 0)
                    cout << "Frame " << frame->index << " / " << replay.frames.size() << endl;
                state.segmentDatabase.recordAndAnnotateSegments(state.getPalette(), *frame);
            }
            state.segmentDatabase.processedReplays.insert(filename);

            //replay.clearImages();
            replay.save(replayAnnotatedDir + filename);
        }
    }

    cout << "Segment manager has " << state.segmentDatabase.segmentCount() << " segments" << endl;
    state.segmentDatabase.save(learningParams().ROMDatasetDir + "segments.dat");
    state.segmentDatabase.load(learningParams().ROMDatasetDir + "segments.dat");
}

void DatabaseProcessor::computeObjects(AppState &state)
{
    state.segmentAnalyzer.init(state.segmentDatabase);

    for (const AnnotatedReplay *replayA : state.replayDatabase.replays)
    {
        const Replay &replay = *replayA->replay;
        cout << "Computing objects in replay " << replay.index << endl;
        
        for (int frameIndex = 0; frameIndex < replay.frames.size() - 1; frameIndex++)
        {
            if (frameIndex % 1000 == 0)
                cout << "Frame " << frameIndex << " / " << replay.frames.size() << endl;
            const ReplayFrame &frameA = *replay.frames[frameIndex + 0];
            const ReplayFrame &frameB = *replay.frames[frameIndex + 1];
            state.segmentAnalyzer.recordFramePair(state.segmentDatabase, frameA, frameB);
        }
    }

    state.segmentDatabase.objects = state.segmentAnalyzer.extractObjects();
    state.objectAnalyzer.init(state.segmentDatabase, vec2i((int)state.ale.getScreen().width(), (int)state.ale.getScreen().height()));
}

void DatabaseProcessor::annotateObjects(AppState &state)
{
    int replayIndex = 0;
    for (AnnotatedReplay *replayA : state.replayDatabase.replays)
    {
        Replay *replay = replayA->replay;
        cout << "Annotating objects in replay " << replay->index << endl;
        
        for (int frameIndex = 0; frameIndex < replay->frames.size(); frameIndex++)
        {
            if (frameIndex % 1000 == 0)
                cout << "Frame " << frameIndex << " / " << replay->frames.size() << endl;
            ReplayFrame &frame = *replay->frames[frameIndex];
            state.segmentAnalyzer.annotateObjects(frame);
        }

        replay->updateObjectIDs(state.segmentDatabase);

        cout << "Computing object statistics in replay " << replay->index << endl;
        for (int frameIndex = 0; frameIndex < replay->frames.size(); frameIndex++)
        {
            const ReplayFrame &frame = *replay->frames[frameIndex];
            state.objectAnalyzer.AnalyzeFrame(state.segmentDatabase, frame);
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

    for (AnnotatedReplay *replay : state.replayDatabase.replays)
    {
        cout << "Dumping replay graphs in replay " << replay->replay->index << endl;
        
        replay->replay->updateObjectIDs(state.segmentDatabase);

        vector<Game::StateInst> states;
        state.modelLearner.loadReplayStates(state, *replay->replay, state.model, states);
        AtariUtil::saveStateGraph(states, graphDir + to_string(replay->replay->index) + ".csv");
    }
}
