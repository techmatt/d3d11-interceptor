
struct ObjectCollection
{
    void save(const string &filename) const;
    void load(const string &filename);

    vector<LocalizedObject> objects;
};

struct FrameObjectData
{
    map<UINT64, const LocalizedObjectData*> makeSignatureMap() const
    {
        map<UINT64, const LocalizedObjectData*> result;
        for (const LocalizedObjectData &o : objects)
            result[o.signature] = &o;
        return result;
    }
    vector<LocalizedObjectData> objects;
    vector<LocalizedObject> objectMeshes;
};

struct FrameCollection
{
    void save(const string &filename) const;
    void load(const string &filename);

    vector<FrameObjectData*> frames;
};
