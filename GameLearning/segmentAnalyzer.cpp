
#include "main.h"

void SegmentAnalyzer::analyze(const FrameCollection &frames)
{
    for (int frameIndex = 1; frameIndex < frames.frames.size(); frameIndex++)
    {
        const auto &prevSigMap = frames.frames[frameIndex - 1]->makeCompleteSignatureMap();
        const auto &sigMap = frames.frames[frameIndex]->makeCompleteSignatureMap();

        for (const auto &sig : sigMap)
        {
            auto &segment = segments[sig.first];
            segment.frameCount++;
            segment.instancesPerFrame.record((double)sig.second.size());

            for (const auto &instance : sig.second)
            {
                segment.dimensions.record(instance->bbox.getExtent().length());

                if (sig.second.size() == 1 &&
                    prevSigMap.count(sig.first) > 0 &&
                    prevSigMap.find(sig.first)->second.size() == 1)
                {
                    const vec3f delta = instance->centroid - prevSigMap.find(sig.first)->second[0]->centroid;
                    segment.distMoved.record(delta.length());
                }
            }
        }
    }
}

void SegmentAnalyzer::dump(const string &filename)
{
    ofstream file(filename);
    file << "Signature\tFrames\tInstances per frame\tDimensions\tDist Moved" << endl;
    for (const auto &segment : segments)
    {
        file << segment.first << '\t';
        file << segment.second.frameCount << '\t';
        file << segment.second.instancesPerFrame.toString() << '\t';
        file << segment.second.dimensions.toString() << '\t';
        file << segment.second.distMoved.toString() << endl;
    }
}