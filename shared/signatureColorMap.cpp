
#include "Main.h"

vec3f SignatureColorMap::getColor(UINT64 signature) const
{
    vec3f color(1.0f, 0.0f, 1.0f);

    if (colors.count(signature) > 0)
    {
        auto &entry = colors.find(signature)->second;
        if (entry.pixelCount == 0)
            color = vec3f(0.0f, 1.0f, 1.0f);
        else
            color = entry.color;
    }

    return color;
}

void SignatureColorMap::save(const string &filename) const
{
    BinaryDataStreamFile file(filename, true);
    file << colors;
    file.closeStream();
}

void SignatureColorMap::load(const string &filename)
{
    if (!util::fileExists(filename)) return;
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
