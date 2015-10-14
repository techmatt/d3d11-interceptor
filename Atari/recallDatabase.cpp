
#include "main.h"

string ObjectSample::toString() const
{
    string result;
    result += frame.toString() + "\n";
    result += "nAlive=" + to_string(transition.nextAlive) + "\n";
    result += "nAnimation=" + to_string(transition.nextAnimation) + "\n";
    result += "nVelocity=" + transition.velocity.toString("x") + "\n";
    result += "nReward=" + to_string(transition.nextReward) + "\n";
    return result;
}

void ObjectSampleDataset::dumpToFile(const string &filename) const
{
    if (util::fileExists(filename))
    {
        cout << "Skipping " << filename << endl;
        return;
    }
    cout << "Generating " << filename << endl;
    ofstream file(filename);
    file << "Sample count: " << allSamples.size() << endl;
    file << "Frame,vHash";

    /*int entryIndex = 0;
    for (auto &e : allSamples[0]->history.history)
    {
        file << ",v" << entryIndex;
        entryIndex++;
    }*/
    file << endl;

    for (auto &sample : allSamples)
    {
        file << sample->frame.toString();
        //file << "," << sample->history.velocityHash;
        //for (auto &e : sample->history.history)
        //    file << "," << e.velocity.toString("_");
        file << endl;
    }
}

void ObjectHistory::computeHash()
{
    hash = util::hash64(velocity) +
           util::hash64(contactStates.data(), (UINT)contactStates.size() * sizeof(BYTE)) +
           util::hash64(lineConstraints.data(), (UINT)lineConstraints.size() * sizeof(BYTE));
}

vector<ObjectSample*> ObjectSampleDataset::getTransitionCandidates(const ObjectHistory &history) const
{
    if (samplesByHash.count(history.hash) > 0)
        return samplesByHash.find(history.hash)->second;
    
    cout << "No samples found for " << objectName << endl;
    // TODO: examine better ways to reduce sample count
    return allSamples;
}

ObjectTransition ObjectSampleDataset::predictTransitionSingleton(AppState &state, const ReplayDatabase &replays, int testReplayIndex, const vector<Game::StateInst> &states, int baseFrameIndex, int action, const HistoryMetricWeights &metric) const
{
    if (objectName == "unnamed")
        return ObjectTransition();

    HistorySlotInfo slotInfo;
    for (auto &it : state.model.stateSpec.objects)
        slotInfo.objectNames.push_back(it.name);
    slotInfo.lines = HistoryMetricWeights().lines;
    
    ObjectHistory history = RecallDatabase::computeObjectHistorySingleton(state, states, baseFrameIndex, objectName, slotInfo);
    vector<ObjectSample*> candidates = getTransitionCandidates(history);

    if (state.dumpAllTransitions)
    {
        candidates = allSamples;
    }

    //cout << objectName << " candidates: " << candidates.size() << endl;
    if (candidates.size() == 1)
    {
        cout << "Singular candidate -- get more training data" << endl;
        candidates = allSamples;
    }

    ObjectTransition blankTransition;
    blankTransition.nextAlive = true;
    blankTransition.nextAnimation = 0;
    blankTransition.nextReward = 0;
    blankTransition.velocity = vec2s(0, 0);

    if (candidates.size() == 0)
    {
        cout << "No candidates found for " << objectName << endl;
        return blankTransition;
    }

    vector<ObjectSample*> bestSamples;
    double bestSamplesDist = std::numeric_limits<double>::max();

    ofstream dumpFile;
    if (state.dumpAllTransitions)
    {
        dumpFile.open(learningParams().ROMDatasetDir + "dump" + objectName + ".csv", ios::out);
        dumpFile << "selected action: " << action << endl;
        for (int history = 0; history <= 10; history++)
            dumpFile << "action" << history << ": " << states[max(0, (int)states.size() - 1 - history)].action << endl;
        dumpFile << "frame,action,padBPos,ballPos,trans-vy,barrier,velDist,animDist,actionDist,positionDist,contactPadADist,offsetPadADist,contactPadBDist,offsetPadBDist,lineConstraints,dist" << endl;
    }

    for (ObjectSample *sample : candidates)
    {
        /*if (sample->frame.toString() == "r1-f185")
        {
            int a = 5;
        }*/

        const vector<Game::StateInst> &candidateStates = replays.replays[sample->frame.replayIndex]->states;

        const double animationDist = AtariUtil::compareAnimationDescriptorDistSingleton(states, baseFrameIndex, candidateStates, sample->frame.frameIndex, objectName, learningParams().historyFrames);
        
        double actionDist = AtariUtil::compareActionDescriptorDist(states, baseFrameIndex, candidateStates, sample->frame.frameIndex, learningParams().historyFrames);
        if (sample->nextAction != action)
            actionDist++;

        const double positionDist = AtariUtil::comparePositionDescriptorDistSingleton(states, baseFrameIndex, candidateStates, sample->frame.frameIndex, objectName, learningParams().historyFrames);

        const double velocityDist = AtariUtil::compareVelocityDescriptorDistSingleton(states, baseFrameIndex, candidateStates, sample->frame.frameIndex, objectName, learningParams().historyFrames);

        const double contactPadADist = AtariUtil::compareContactDescriptorDistSingleton(state.segmentDatabase, states, baseFrameIndex, candidateStates, sample->frame.frameIndex, objectName, "padA");
        const double offsetPadADist = AtariUtil::compareOffsetDescriptorDistSingleton(state.segmentDatabase, states, baseFrameIndex, candidateStates, sample->frame.frameIndex, objectName, "padA");

        const double contactPadBDist = AtariUtil::compareContactDescriptorDistSingleton(state.segmentDatabase, states, baseFrameIndex, candidateStates, sample->frame.frameIndex, objectName, "padB");
        const double offsetPadBDist = AtariUtil::compareOffsetDescriptorDistSingleton(state.segmentDatabase, states, baseFrameIndex, candidateStates, sample->frame.frameIndex, objectName, "padB");
        
        const double lineConstraints = AtariUtil::compareLineConstraintsSingleton(states, baseFrameIndex, candidateStates, sample->frame.frameIndex, objectName, metric.lines);

        const double dist = velocityDist * metric.velocity +
                           animationDist * metric.animation +
                           actionDist * metric.action +
                           positionDist * metric.position +
                           contactPadADist * 20 +
                           offsetPadADist +
                           contactPadBDist * 20 +
                           offsetPadBDist +
                           lineConstraints;

        if (state.dumpAllTransitions)
        {
            dumpFile << sample->frame.toString() << ",";

            const Game::StateInst &stateInst = candidateStates[sample->frame.frameIndex];

            dumpFile << stateInst.action << ",";

            if (stateInst.objects.find("padB")->second.size() == 0)
                dumpFile << "dead,";
            else
                dumpFile << stateInst.objects.find("padB")->second[0].origin.toString("x") << ",";

            if (stateInst.objects.find("ball")->second.size() == 0)
                dumpFile << "dead,";
            else
                dumpFile << stateInst.objects.find("ball")->second[0].origin.toString("x") << ",";

            dumpFile << sample->transition.velocity.y << ",x,";
            
            dumpFile << velocityDist << ",";
            dumpFile << animationDist << ",";
            dumpFile << actionDist << ",";
            dumpFile << positionDist << ",";
            dumpFile << contactPadADist << ",";
            dumpFile << offsetPadADist << ",";
            dumpFile << contactPadBDist << ",";
            dumpFile << offsetPadBDist << ",";
            dumpFile << lineConstraints << ",";
            dumpFile << dist << endl;
        }
        
        if (sample->frame.replayIndex == testReplayIndex)
            continue;

        if (dist < bestSamplesDist)
            bestSamples.clear();
        
        if (dist <= bestSamplesDist)
        {
            bestSamples.push_back(sample);
            bestSamplesDist = dist;
        }
    }

    if (bestSamples.size() == 0)
    {
        cout << "No samples found for " << objectName << endl;
        return blankTransition;
    }

    if (state.dumpAllTransitions)
    {
        dumpFile << "Best samples:" << endl;
        for (int i = 0; i < bestSamples.size(); i++)
        {
            dumpFile << i << "," << bestSamples[i]->frame.toString() << "," << bestSamples[i]->toString() << endl;
        }
    }

    if (Constants::transitionDebugging)
    {
        cout << "Best samples: " << bestSamples.size() << endl;
        for (int sampleIndex = 0; sampleIndex < min(2, (int)bestSamples.size()); sampleIndex++)
            cout << bestSamples[sampleIndex]->toString() << endl;
    }

    return bestSamples[0]->transition;
}

void RecallDatabase::init(AppState &state)
{
    for (auto &o : state.model.stateSpec.objects)
    {
        if (o.name == "unnamed")
            continue;

        objectSamples[o.name] = makeObjectSampleDataset(state, o.name);
        objectSamples[o.name]->dumpToFile(learningParams().ROMDatasetDir + "samples-" + o.name + ".csv");

        for (auto &replay : state.replayDatabase.replays)
        {
            const string predictionDir = learningParams().ROMDatasetDir + "predictions/";
            util::makeDirectory(predictionDir);
            //predictAllTransitions(state.replayDatabase, replay->states, o.name, predictionDir + "replay" + to_string(replay->replay->index) + "-" + o.name + ".csv");
        }
    }
}

ObjectTransition RecallDatabase::computeObjectTransitionSingleton(const vector<Game::StateInst> &states, int baseFrameIndex, const string &objectName)
{
    const Game::StateInst &curState = states[baseFrameIndex];
    const Game::StateInst &nextState = states[baseFrameIndex + 1];

    const Game::ObjectInst *mostRecentInst = nullptr;
    for (int frame = baseFrameIndex; !mostRecentInst && frame >= 0; frame--)
    {
        const auto &instances = states[frame].objects.find(objectName)->second;
        if (instances.size() > 0)
            mostRecentInst = &instances[0];
    }

    const vector<Game::ObjectInst> &nextInstances = nextState.objects.find(objectName)->second;

    ObjectTransition result;

    result.nextReward = nextState.reward;
    if (nextInstances.size() == 0)
    {
        result.nextAnimation = mostRecentInst ? mostRecentInst->segmentHash : 0;
        result.nextAlive = 0;
        result.velocity = vec2s(0, 0);
    }
    else //if (nextInstances.size() >= 1)
    {
        result.nextAnimation = nextInstances[0].segmentHash;
        result.nextAlive = 1;
        if (mostRecentInst == nullptr)
            result.velocity = vec2s(0, 0);
        else
            result.velocity = nextInstances[0].origin - mostRecentInst->origin;
    }
    return result;
}

ObjectSampleDataset* RecallDatabase::makeObjectSampleDataset(AppState &state, const string &objectName)
{
    cout << "Making object sample dataset for " << objectName << endl;
    ObjectSampleDataset *result = new ObjectSampleDataset(objectName);

    HistorySlotInfo slotInfo;
    for (auto &it : state.model.stateSpec.objects)
        slotInfo.objectNames.push_back(it.name);
    slotInfo.lines = HistoryMetricWeights().lines;

    for (auto &replay : state.replayDatabase.replays)
    {
        for (int baseFrameIndex = learningParams().historyFrames + 30; baseFrameIndex < replay->states.size() - 30 - learningParams().historyFrames; baseFrameIndex++)
        {
            ObjectSample *newSample = new ObjectSample;
            newSample->frame = FrameID(replay->replay->index, baseFrameIndex);
            newSample->history = computeObjectHistorySingleton(state, replay->states, baseFrameIndex, objectName, slotInfo);
            newSample->transition = computeObjectTransitionSingleton(replay->states, baseFrameIndex, objectName);
            newSample->nextAction = replay->states[baseFrameIndex + 1].action;
            
            vector<ObjectSample*> &hashList = result->samplesByHash[newSample->history.hash];
            hashList.push_back(newSample);

            result->allSamples.push_back(newSample);
        }
    }

    return result;
}

ObjectHistory RecallDatabase::computeObjectHistorySingleton(AppState &appState, const vector<Game::StateInst> &states, int baseFrameIndex, const string &objectName, const HistorySlotInfo &slotInfo)
{
    ObjectHistory result;

    const auto &curInstances = states[baseFrameIndex].getInstances(objectName);
    if (curInstances.size() == 0)
    {
        result.alive = false;
        result.hash = 0;
        return result;
    }
    result.alive = true;
    const vec2s curLocation = curInstances[0].origin;

    vec2s prevLocation;
    for (int stateIndex = baseFrameIndex - 1; stateIndex >= 0; stateIndex--)
    {
        const vector<Game::ObjectInst> &instances = states[stateIndex].getInstances(objectName);
        if (instances.size() > 0)
        {
            prevLocation = instances[0].origin;
            break;
        }
    }

    result.velocity = curLocation - prevLocation;
    
    const int objectCount = (int)slotInfo.objectNames.size();
    result.contactStates.resize(objectCount);

    for (int objectIndex = 0; objectIndex < objectCount; objectIndex++)
    {
        const bool contact = AtariUtil::objectsInContactSingleton(appState.segmentDatabase, states[baseFrameIndex], objectName, slotInfo.objectNames[objectIndex]);
        result.contactStates[objectIndex] = contact ? 1 : 0;
    }

    const int lineCount = (int)slotInfo.lines.size();
    result.lineConstraints.resize(lineCount);
    for (int lineIndex = 0; lineIndex < lineCount; lineIndex++)
    {
        const LineConstraint &line = slotInfo.lines[lineIndex];
        const bool onLine = line.onLine(curLocation);
        result.lineConstraints[lineIndex] = onLine ? 1 : 0;
    }
    
    result.computeHash();
    return result;
}

void RecallDatabase::predictAllTransitions(AppState &state, const ReplayDatabase &replays, int testReplayIndex, const vector<Game::StateInst> &states, const string &objectName, const string &filename)
{
    /*cout << "Predicting all transitions for " << objectName << endl;

    ofstream file(filename);

    for (int entry = 0; entry < learningParams().historyFrames; entry++)
    {
        file << "h" << entry << "-ox,";
        file << "h" << entry << "-oy,";
        file << "h" << entry << "-vx,";
        file << "h" << entry << "-vy,";
        file << "h" << entry << "-alive,";
    }

    file << "ac t-vx,";
    file << "ac t-vy,";
    file << "ac t-animation,";
    file << "ac t-alive,";

    file << "pr t-vx,";
    file << "pr t-vy,";
    file << "pr t-animation,";
    file << "pr t-alive,";

    file << endl;

    for (int baseFrameIndex = learningParams().historyFrames + 30; baseFrameIndex < states.size() - 30 - learningParams().historyFrames; baseFrameIndex++)
    {
        const ObjectHistory history = computeObjectHistorySingleton(states, baseFrameIndex, objectName);
        const ObjectTransition actualTransition = computeObjectTransitionSingleton(states, baseFrameIndex, objectName);

        for (int entry = 0; entry < learningParams().historyFrames; entry++)
        {
            file << history.history[entry].origin.x << ",";
            file << history.history[entry].origin.y << ",";
            file << history.history[entry].velocity.x << ",";
            file << history.history[entry].velocity.y << ",";
            file << history.history[entry].alive << ",";
        }

        //const vector<ObjectSample*> transitionCandidates = objectSamples[objectName]->getTransitionCandidates(history);
        //for (const ObjectSample *sample : transitionCandidates)
        //{
        //    file << sample->transition.velocity.x << ",";
        //    file << sample->transition.velocity.y << ",";
        //}

        file << actualTransition.velocity.x << ",";
        file << actualTransition.velocity.y << ",";
        file << actualTransition.nextAnimation << ",";
        file << actualTransition.nextAlive << ",";

        HistoryMetricWeights metric;
        ObjectTransition predictedTransition = objectSamples[objectName]->predictTransitionSingleton(state, replays, testReplayIndex, states, baseFrameIndex, states[baseFrameIndex].variables.find("action")->second, objectName, metric);
        file << predictedTransition.velocity.x << ",";
        file << predictedTransition.velocity.y << ",";
        file << predictedTransition.nextAnimation << ",";
        file << predictedTransition.nextAlive << ",";

        file << endl;
    }*/
}
