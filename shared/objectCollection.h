
struct ObjectCollection
{
    void save(const string &filename) const;
    void load(const string &filename);

    vector<LocalizedObject> objects;
};
