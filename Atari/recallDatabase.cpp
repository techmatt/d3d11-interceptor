
#include "main.h"

string ObjectSample::toString() const
{
    string result;
    result += frame.toString() + "\n";
    result += "nAlive=" + to_string(transition.nextAlive) + "\n";
    result += "nAnimation=" + to_string(transition.nextAnimation) + "\n";
    result += "nVelocity=" + transition.velocity.toString(",") + "\n";
    return result;
}

void ObjectHistory::computeHashes()
{
    velocityHash = computeVelocityAliveHash();
    animationHash = computeAnimationHash();
}

UINT64 ObjectHistory::computeVelocityAliveHash() const
{
    UINT64 result = 0;
    int factor = 7;
    for (const Entry &e : history)
    {
        result += util::hash64(e.velocity) * factor + e.alive * factor * 541;
        factor = factor * 77 + 911;
    }
    return result;
}

UINT64 ObjectHistory::computeAnimationHash() const
{
    UINT64 result = 0;
    int factor = 7;
    for (const Entry &e : history)
    {
        result += e.animation * factor;
        factor = factor * 76 + 9213;
    }
    return result;
}

vector<ObjectSample*> ObjectSampleDataset::getTransitionCandidates(const ObjectHistory &history) const
{
    vector<ObjectSample*> result;
    const UINT64 combinedHash = history.combinedHash();
    if (historyByCombinedHash.count(combinedHash) > 0)
    {
        for (auto &sample : historyByCombinedHash.find(combinedHash)->second)
            result.push_back(sample);

        if (Constants::transitionDebugging)
            cout << "combined hash matches: " << result.size() << endl;
    }
    if (result.size() == 0 && historyByVelocityHash.count(history.velocityHash) > 0)
    {
        for (auto &sample : historyByVelocityHash.find(history.velocityHash)->second)
            result.push_back(sample);

        if (Constants::transitionDebugging)
            cout << "velocity hash matches: " << result.size() << endl;
    }
    return result;
}

ObjectTransition ObjectSampleDataset::predictTransitionSingleton(const ReplayDatabase &replays, const vector<Game::StateInst> &states, int baseFrameIndex, int action, const string &objectName, const HistoryMetricWeights &metric) const
{
    if (objectName == "unnamed")
        return ObjectTransition();

    ObjectHistory history = RecallDatabase::computeObjectHistorySingleton(states, baseFrameIndex, objectName);
    vector<ObjectSample*> candidates = getTransitionCandidates(history);

    if (Constants::dumpAllTransitions)
    {
        candidates = allSamples;
    }

    ObjectTransition blankTransition;
    blankTransition.nextAlive = true;
    blankTransition.nextAnimation = history.history[0].animation;
    blankTransition.velocity = vec2s(0, 0);

    if (candidates.size() == 0)
    {
        cout << "No candidates found" << endl;
        return blankTransition;
    }

    vector<ObjectSample*> bestSamples;
    float bestSamplesDist = std::numeric_limits<float>::max();

    
    ofstream dumpFile;
    if (Constants::dumpAllTransitions)
    {
        dumpFile.open("dump.csv", ios::out);
        dumpFile << "selected action: " << action << endl;
        for (int history = 0; history <= 10; history++)
            dumpFile << "action" << history << ": " << states[max(0, (int)states.size() - 1 - history)].variables.find("action")->second << endl;
        dumpFile << "frame,action,padB-y,trans-vy,animDist,actionDist,positionDist,dist" << endl;
    }

    for (ObjectSample *sample : candidates)
    {
        const vector<Game::StateInst> &candidateStates = replays.replays[sample->frame.replayIndex]->states;

        const int animationDist = AtariUtil::compareAnimationDescriptorDistSingleton(states, baseFrameIndex, candidateStates, sample->frame.frameIndex, objectName, learningParams().historyFrames);
        
        int actionDist = AtariUtil::compareActionDescriptorDist(states, baseFrameIndex, candidateStates, sample->frame.frameIndex, learningParams().historyFrames);
        if (sample->nextAction != action)
            actionDist++;

        const int positionDist = AtariUtil::comparePositionDescriptorDistSingleton(states, baseFrameIndex, candidateStates, sample->frame.frameIndex, objectName, learningParams().historyFrames);

        const float dist = animationDist * metric.animation + actionDist * metric.action + positionDist * metric.position;

        if (Constants::dumpAllTransitions)
        {
            dumpFile << sample->frame.toString() << ",";

            const Game::StateInst &stateInst = candidateStates[sample->frame.frameIndex];

            dumpFile << stateInst.variables.find("action")->second << ",";
            dumpFile << stateInst.objects.find("padB")->second[0].origin.y << ",";
            dumpFile << sample->transition.velocity.y << ",";
            
            dumpFile << animationDist << ",";
            dumpFile << actionDist << ",";
            dumpFile << positionDist << ",";
            dumpFile << dist << endl;
        }
        
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
        cout << "No samples found" << endl;
        return blankTransition;
    }

    

    if (Constants::transitionDebugging)
    {
        cout << "Best samples: " << bestSamples.size() << endl;
        for (int sampleIndex = 0; sampleIndex < min(2, (int)bestSamples.size()); sampleIndex++)
            cout << bestSamples[sampleIndex]->toString() << endl;
    }

    return bestSamples[0]->transition;
}

float RecallDatabase::compareVelocityHistory(const ObjectHistory &a, const ObjectHistory &b)
{
    float sum = 0.0f;
    for (int i = 0; i < a.history.size(); i++)
    {
        const ObjectHistory::Entry &aEntry = a.history[i];
        const ObjectHistory::Entry &bEntry = b.history[i];

        sum += vec2f::distSq(vec2f(aEntry.velocity), vec2f(bEntry.velocity));
    }
    return sum;
}

void RecallDatabase::init(AppState &state)
{
    for (auto &o : state.model.stateSpec.objects)
    {
        if (o.name == "unnamed")
            continue;

        objectSamples[o.name] = makeObjectSampleDataset(state, o.name);

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

    if (nextInstances.size() == 0)
    {
        result.nextAnimation = 0;
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
    ObjectSampleDataset *result = new ObjectSampleDataset();

    for (auto &replay : state.replayDatabase.replays)
    {
        for (int baseFrameIndex = learningParams().historyFrames + 30; baseFrameIndex < replay->states.size() - 30 - learningParams().historyFrames; baseFrameIndex++)
        {
            ObjectSample *newSample = new ObjectSample;
            newSample->frame = FrameID(replay->replay->index, baseFrameIndex);
            newSample->history = computeObjectHistorySingleton(replay->states, baseFrameIndex, objectName);
            newSample->transition = computeObjectTransitionSingleton(replay->states, baseFrameIndex, objectName);
            newSample->nextAction = replay->states[baseFrameIndex + 1].variables.find("action")->second;
            
            vector<ObjectSample*> &combinedHashList = result->historyByCombinedHash[newSample->history.combinedHash()];
            combinedHashList.push_back(newSample);

            vector<ObjectSample*> &velocityHashList = result->historyByVelocityHash[newSample->history.velocityHash];
            velocityHashList.push_back(newSample);

            result->allSamples.push_back(newSample);
        }
    }

    return result;
}

ObjectHistory RecallDatabase::computeObjectHistorySingleton(const vector<Game::StateInst> &states, int baseFrameIndex, const string &objectName)
{
    ObjectHistory result;
    result.history.resize(learningParams().historyFrames);

    vec2s lastKnownLocation;
    for (int stateIndex = baseFrameIndex; stateIndex >= 0; stateIndex--)
    {
        const vector<Game::ObjectInst> &instances = states[stateIndex].objects.find(objectName)->second;
        if (instances.size() > 0)
        {
            lastKnownLocation = instances[0].origin;
            break;
        }
    }

    for (int historyIndex = 0; historyIndex < result.history.size(); historyIndex++)
    {
        ObjectHistory::Entry &entry = result.history[historyIndex];
        
        const Game::StateInst &curState = states[max(0, baseFrameIndex - historyIndex)];
        const Game::StateInst &prevState = states[max(0, baseFrameIndex - historyIndex - 1)];

        const vector<Game::ObjectInst> &instances = curState.objects.find(objectName)->second;

        if (instances.size() == 0)
        {
            entry.alive = 0;
            entry.animation = 0;
            entry.origin = lastKnownLocation;
            entry.velocity = vec2s(0, 0);
        }
        else
        {
            if (instances.size() >= 2)
                cout << "Multiple instances of singletion found" << endl;
            entry.alive = 1;
            entry.animation = instances[0].segmentHash;
            entry.origin = instances[0].origin;

            const vector<Game::ObjectInst> &prevInstances = prevState.objects.find(objectName)->second;

            if (prevInstances.size() == 0)
                entry.velocity = vec2s(0, 0);
            else
                entry.velocity = entry.origin - prevInstances[0].origin;
        }
    }
    result.computeHashes();
    return result;
}

void RecallDatabase::predictAllTransitions(const ReplayDatabase &replays, const vector<Game::StateInst> &states, const string &objectName, const string &filename)
{
    cout << "Predicting all transitions for " << objectName << endl;

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
        metric.action = 1.0f;
        metric.animation = 1.0f;
        metric.position = 0.0001f;
        ObjectTransition predictedTransition = objectSamples[objectName]->predictTransitionSingleton(replays, states, baseFrameIndex, states[baseFrameIndex].variables.find("action")->second, objectName, metric);
        file << predictedTransition.velocity.x << ",";
        file << predictedTransition.velocity.y << ",";
        file << predictedTransition.nextAnimation << ",";
        file << predictedTransition.nextAlive << ",";

        file << endl;
    }
}
