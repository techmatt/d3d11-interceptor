
struct ObjectCollection
{
    void save(const string &filename) const;
    void load(const string &filename);

    vector<LocalizedObject> objects;
};

struct FrameObjectData
{
    vector<LocalizedObjectData> objects;
};

struct FrameCollection
{
    void save(const string &filename) const;
    void load(const string &filename);

    vector<FrameObjectData*> frames;
};
