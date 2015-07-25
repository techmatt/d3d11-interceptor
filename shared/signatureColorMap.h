
struct SignatureColorEntry : public BinaryDataSerialize< SignatureColorEntry >
{
    SignatureColorEntry() {}
    SignatureColorEntry(vec3f _color, int _pixelCount)
    {
        color = _color;
        pixelCount = _pixelCount;
    }
    vec3f color;
    int pixelCount;
};

struct SignatureColorMap
{
    void save(const string &filename) const;
    void load(const string &filename);

    void record(UINT64 signature, const vec3f &color, int pixelCount);

    vec3f getColor(UINT64 signature) const;

    unordered_map<UINT64, SignatureColorEntry> colors;
};
