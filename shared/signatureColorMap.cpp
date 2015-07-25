
#include "Main.h"

void SignatureColorMap::save(const string &filename) const
{
    BinaryDataStreamFile file(filename, true);
    file << colors;
    file.closeStream();
}

void SignatureColorMap::load(const string &filename)
{
    BinaryDataStreamFile file(filename, false);
    file >> colors;
    file.closeStream();
}

void SignatureColorMap::record(UINT64 signature, const vec3f &color, int pixelCount)
{
    if (colors.count(signature) == 0)
    {
        colors[signature] = SignatureColorEntry(color, pixelCount);
        return;
    }

    if (colors[signature].pixelCount < pixelCount)
    {
        colors[signature] = SignatureColorEntry(color, pixelCount);
    }
}
