
#include "main.h"

void ObjectAnalyzer::init(const SegmentDatabase &segments, const vec2i &screenDimensions)
{
    objects.resize(segments.objects.size());
    for (ObjectStatistics &o : objects)
    {
        o.heatmap.allocate(screenDimensions.x, screenDimensions.y);
        o.heatmap.setPixels(vec4uc(0, 0, 0, 255));
    }
}

UINT64 ObjectAnalyzer::frameObjectHash(FrameID frame, int objectIndex)
{
    return (UINT64)frame.frameIndex + (UINT64)frame.replayIndex * 0x10000ULL + objectIndex * 0x100000000ULL;
}

const ObjectTrack& ObjectAnalyzer::findObjectTrack(FrameID frame, int objectIndex) const
{
    const UINT64 hash = frameObjectHash(frame, objectIndex);

    auto it = objectTracks.find(hash);
    if (it == objectTracks.end())
    {
        cout << "Object track not found?" << endl;
        
    }
    return *it->second;
}

void ObjectAnalyzer::AnalyzeFrame(const SegmentDatabase &segments, const ReplayFrame &frame)
{
    map<int, int> objectOccurrences;
    for (auto &o : frame.objectAnnotations)
    {
        objectOccurrences[o.objectID]++;
        objects[o.objectID].heatmap(o.origin.x, o.origin.y) = vec4uc(255, 0, 0, 255);
        objects[o.objectID].segmentHashes.insert(frame.segmentAnnotations[o.segments[0]].segmentHash);
    }

    for (auto &p : objectOccurrences)
    {
        ObjectStatistics &stats = objects[p.first];
        while (stats.frameOccurrenceCount.size() <= p.second)
            stats.frameOccurrenceCount.push_back(0);
        stats.frameOccurrenceCount[p.second]++;
    }
}

void ObjectAnalyzer::outputViz(const string &dir) const
{
    util::makeDirectory(dir);
    ofstream file(dir + "stats.csv");
    file << "Object index,Object occurrences" << endl;
    for (int i = 0; i < objects.size(); i++)
    {
        file << i;
        for (int j = 0; j < objects[i].frameOccurrenceCount.size(); j++)
        {
            file << "," << objects[i].frameOccurrenceCount[j];
        }
        file << endl;
    }

    file << "Object index,Segment hashes" << endl;
    for (int i = 0; i < objects.size(); i++)
    {
        file << i;
        for (UINT64 s : objects[i].segmentHashes)
        {
            file << "," << s;
        }
        file << endl;
    }

    file << endl << "Object track,Start frame,length" << endl;
    for (int i = 0; i < objects.size(); i++)
    {
        for (const ObjectTrack *track : objects[i].tracks)
            file << "o" << i << " track," << track->startFrame.toString() << ',' << track->track.size() << endl;
    }

    for (int i = 0; i < objects.size(); i++)
    {
        LodePNG::save(objects[i].heatmap, dir + to_string(i) + ".png");
    }
}

void ObjectAnalyzer::startTracking(const ReplayFrame &firstFrame, int replayIndex)
{
    liveReplayIndex = replayIndex;
    liveTracks.clear();

    for (int objectIndex = 0; objectIndex < firstFrame.objectAnnotations.size(); objectIndex++)
    {
        const ObjectAnnotation &o = firstFrame.objectAnnotations[objectIndex];
        
        ObjectTrack *newTrack = new ObjectTrack;
        newTrack->startFrame = FrameID(replayIndex, firstFrame.index);
        newTrack->track.push_back(objectIndex);
        objects[o.objectID].tracks.push_back(newTrack);

        liveTracks[objectIndex] = newTrack;
    }
}

void ObjectAnalyzer::trackFrames(const ReplayFrame &frameA, const ReplayFrame &frameB)
{
    map<int, ObjectTrack*> newTracks;

    vector<int> processedObjectsB(frameB.objectAnnotations.size());
    for (int &x : processedObjectsB)
        x = 0;

    for (int objectIndexA = 0; objectIndexA < frameA.objectAnnotations.size(); objectIndexA++)
    {
        const ObjectAnnotation &annotationA = frameA.objectAnnotations[objectIndexA];
        
        int objectBMatch = -1;
        int bestDist = learningParams().objectMotionRadius + 1;
        for (int objectIndexB = 0; objectIndexB < frameB.objectAnnotations.size(); objectIndexB++)
        {
            const ObjectAnnotation &annotationB = frameB.objectAnnotations[objectIndexB];
            if (annotationA.objectID == annotationB.objectID && processedObjectsB[objectIndexB] == 0)
            {
                const int xDiff = abs(annotationA.origin.x - annotationB.origin.x);
                const int yDiff = abs(annotationA.origin.y - annotationB.origin.y);
                const int dist = max(xDiff, yDiff);
                if (dist < bestDist)
                {
                    bestDist = dist;
                    objectBMatch = objectIndexB;
                }
            }
        }

        if (objectBMatch != -1)
        {
            processedObjectsB[objectBMatch] = 1;
            ObjectTrack *liveTrack = liveTracks[objectIndexA];

            liveTrack->track.push_back(objectBMatch);

            newTracks[objectBMatch] = liveTrack;
        }
    }

    for (int objectIndexB = 0; objectIndexB < frameB.objectAnnotations.size(); objectIndexB++)
    {
        const ObjectAnnotation &annotationB = frameB.objectAnnotations[objectIndexB];
        if (processedObjectsB[objectIndexB] == 0)
        {
            ObjectTrack *newTrack = new ObjectTrack;
            newTrack->startFrame = FrameID(liveReplayIndex, frameB.index);
            newTrack->track.push_back(objectIndexB);
            
            objects[annotationB.objectID].tracks.push_back(newTrack);

            newTracks[objectIndexB] = newTrack;
        }
    }

    liveTracks = newTracks;
}

void ObjectAnalyzer::finalizeTracks()
{
    for (const ObjectStatistics &o : objects)
    {
        for (ObjectTrack *track : o.tracks)
        {
            for (int frameOffset = 0; frameOffset < track->track.size(); frameOffset++)
            {
                const UINT64 hash = frameObjectHash(track->startFrame.delta(frameOffset), track->track[frameOffset]);
                objectTracks[hash] = track;
            }
        }
    }
}

void ObjectAnalyzer::outputSegmentBlacklist(AppState &state, const Replay &replay) const
{
    unordered_set<UINT64> blacklistedSegmentHashes;

    const ReplayFrame &firstFrame = *replay.frames[0];
    for (int i = 0; i < objects.size(); i++)
    {
        for (const ObjectTrack *track : objects[i].tracks)
        {
            if (track->startFrame.frameIndex == 0 && track->track.size() == replay.frames.size())
            {
                const UINT objectIndex = track->track[0];
                const ObjectAnnotation &objectAnnotation = firstFrame.objectAnnotations[objectIndex];
                
                bool trackHomogenous = true;
                const vec2s startLocation = objectAnnotation.origin;
                for (int frameIndex = 0; trackHomogenous && frameIndex < track->track.size(); frameIndex++)
                {
                    if (replay.frames[frameIndex]->objectAnnotations[track->track[frameIndex]].origin != startLocation)
                        trackHomogenous = false;
                }

                if (trackHomogenous)
                {
                    const UINT64 hash = firstFrame.segmentAnnotations[objectAnnotation.segments[0]].segmentHash;
                    blacklistedSegmentHashes.insert(hash);
                }
            }
        }
    }

    for (UINT64 hash : state.segmentDatabase.segmentBlacklist)
        blacklistedSegmentHashes.insert(hash);

    vector<UINT64> blacklistedSegmentHashesVec;
    for (UINT64 hash : blacklistedSegmentHashes)
        blacklistedSegmentHashesVec.push_back(hash);

    const string filename = learningParams().ROMDatasetDir + "blacklistedSegments.dat";
    BinaryDataStreamFile file(filename, true);
    file.writePrimitiveVector(blacklistedSegmentHashesVec);
    file.closeStream();
}

void ObjectAnalyzer::assignObjectNames()
{
    for (const string &line : util::getFileLinesCreate(learningParams().ROMDatasetDir + "modelSpec/objectNames.txt", 3))
    {
        const vector<string> words = util::split(line, '\t');
        if (words.size() == 2)
        {
            const UINT64 segmentHash = convert::toUInt64(words[0]);
            const string objectName = words[1];

            for (int i = 0; i < objects.size(); i++)
            {
                if (objects[i].segmentHashes.count(segmentHash) > 0)
                    objects[i].name = objectName;
            }
        }
    }
}