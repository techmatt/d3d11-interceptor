

struct FrameObjectData
{
    map<UINT64, vector<const LocalizedObjectData*> > makeSignatureMap() const
    {
        map<UINT64, vector<const LocalizedObjectData*> > result;
        for (const LocalizedObjectData &o : objectData)
        {
            result[o.signature].push_back(&o);
        }
        return result;
    }

    void transform(const mat4f &m)
    {
        for (LocalizedObjectData &o : objectData)
        {
            o.centroid = m * o.centroid;
            for (auto &v : o.vertices)
                if (!isnan(v.x))
                    v = m * v;
        }
        for (LocalizedObject &o : objectMeshes)
        {
            o.data.centroid = m * o.data.centroid;
            for (auto &v : o.vertices)
            {
                v.worldPos = m * v.worldPos;
            }
        }
    }

    int index;
    vector<LocalizedObjectData> objectData;
    vector<LocalizedObject> objectMeshes;

    static const int ControllerCount = 2;
    GCPadStatus padState[ControllerCount];
};

struct GameReplay
{
    void save(const string &filename) const;
    void load(const string &filename);

    string sourceFilename;
    vector<FrameObjectData*> frames;
};
