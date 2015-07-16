
namespace ml {

struct FASTFrame
{
    Bitmap bmp;
    std::vector<vec2i> corners;
};

struct FASTCorrespondence
{
    FASTCorrespondence() {}
    FASTCorrespondence(const vec2i &start, const vec2i &end)
    {
        startCoord = start;
        endCoord = end;
    }
    vec2i startCoord, endCoord;
};

class FASTDetector
{
public:
    FASTDetector()
    {
        _fastStorage.cornersFinal = new vec2i[maxCornerCount];
        _fastStorage.cornersFinalLength = maxCornerCount;

        _fastStorage.cornersDetect = new vec2i[maxCornerCount];
        _fastStorage.cornersDetectLength = maxCornerCount;

        _fastStorage.scores = new int[maxCornerCount];
        _fastStorage.scoresLength = maxCornerCount;

        _fastStorage.rowStorage = new int[2048];
        _fastStorage.rowStorageLength = 2048;

        _downsampleStorage.resize(imageWidth * imageHeight / 4);
    }

    void ProcessFrame(FASTFrame &frame, int threshold);
    std::vector<FASTCorrespondence> ComputeCorrespondences(const FASTFrame &startFrame, const FASTFrame &endFrame);

    void TestFrames(const Bitmap &bmpA, const Bitmap &bmpB, int threshold, const Bitmap &mask, UINT frameIndex);

private:
    static const UINT maxCornerCount = 32768;
    static const UINT imageWidth = 1024;
    static const UINT imageHeight = 768;
    static const UINT downsampleWidth = imageWidth / 2;
    static const UINT downsampleHeight = imageHeight / 2;

    static UINT64 Block(vec2i v)
    {
        return (UINT64)v.x + ((UINT64)v.y << 32);
    }

    std::vector<vec2i> DetectCorners(const Bitmap &bmp, UINT threshold);
    std::vector<vec2i> DetectCornersDownsample(const Bitmap &bmp, UINT threshold);

    float ComputeDistance(UINT cornerA, UINT cornerB, const std::vector<const float*> &featuresA, const std::vector<const float*> &featuresB);
    static Bitmap VisualizeCorners(const Bitmap &bmp, const std::vector<vec2i> &corners, const std::vector<RGBColor> &cornerColors);

    std::vector<float> _featureStorageA;
    std::vector<float> _featureStorageB;
    FastStorage _fastStorage;
    std::vector<BYTE> _downsampleStorage;
};

class FeatureGenerator3Channel
{
public:
    static const int radius = 5;
    static const UINT featureCount = (radius * 2 + 1) * (radius * 2 + 1) * 3;
    static const int skip = 1;
    static const int skipRadius = radius * skip;

    static float Diff(const float *a, const float *b)
    {
        float result = 0.0f;
        for (int i = 0; i < featureCount; i++)
        {
            float diff = b[i] - a[i];
            result += diff * diff;
        }
        return result;
    }

    static void Generate(const Bitmap &bmp, const vec2i &pos, float *result)
    {
        int featureIndex = 0;
        for (int y = pos.y - skipRadius; y <= pos.y + skipRadius; y += skip)
            for (int x = pos.x - skipRadius; x <= pos.x + skipRadius; x += skip)
            {
                if (bmp.isValidCoordinate(x, y))
                {
                    result[featureIndex++] = bmp(y, x).r;
                    result[featureIndex++] = bmp(y, x).g;
                    result[featureIndex++] = bmp(y, x).b;
                }
                else
                {
                    result[featureIndex++] = 0.0f;
                    result[featureIndex++] = 0.0f;
                    result[featureIndex++] = 0.0f;
                }
            }
    }
};

class FeatureGenerator1Channel
{
public:
    static const int radius = 5;
    static const UINT featureCount = (radius * 2 + 1) * (radius * 2 + 1);
    static const int skip = 1;
    static const int skipRadius = radius * skip;

    static float Diff(const float *a, const float *b)
    {
        float result = 0.0f;
        for (int i = 0; i < featureCount; i++)
        {
            float diff = b[i] - a[i];
            result += diff * diff;
        }
        return result;
    }

    static void Generate(const Bitmap &bmp, const vec2i &pos, float *result)
    {
        int featureIndex = 0;
        for (int y = pos.y - skipRadius; y <= pos.y + skipRadius; y += skip)
            for (int x = pos.x - skipRadius; x <= pos.x + skipRadius; x += skip)
            {
                if (bmp.isValidCoordinate(x, y))
                {
                    result[featureIndex++] = bmp(y, x).r;
                }
                else
                {
                    result[featureIndex++] = 0.0f;
                }
            }
    }
};

}