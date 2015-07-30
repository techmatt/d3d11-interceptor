

struct FrameObjectData
{
    map<UINT64, const LocalizedObjectData*> makeUniqueSignatureMap() const
    {
        map<UINT64, const LocalizedObjectData*> result;
        for (const LocalizedObjectData &o : objectData)
        {
            if (result.count(o.signature) == 1)
                result[o.signature] = nullptr;
            else
                result[o.signature] = &o;
        }
        return result;
    }

    map<UINT64, vector<const LocalizedObjectData*> > makeCompleteSignatureMap() const
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

    vector<LocalizedObjectData> objectData;
    vector<LocalizedObject> objectMeshes;
};

struct FrameCollection
{
    void save(const string &filename) const;
    void load(const string &filename);

    vector<FrameObjectData*> frames;
};
