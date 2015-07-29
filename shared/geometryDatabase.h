
struct GeometryDatabaseEntry
{
    GeometryDatabaseEntry() {}
    GeometryDatabaseEntry(bool _fileExists)
    {
        geometry = nullptr;
        fileExists = _fileExists;
    }
    const LocalizedObject *geometry;
    bool fileExists;
};

struct GeometryDatabase
{
    GeometryDatabase()
    {
        dir = R"(C:\Code\d3d11-interceptor\Dolphin-x64\geometryDatabase\)";
    }
    bool hasSignature(UINT64 signature)
    {
        if (geometry.count(signature) == 0)
        {
            const bool fileExists = util::fileExists(dir + to_string(signature));
            geometry[signature] = GeometryDatabaseEntry(fileExists);
        }
        return geometry.find(signature)->second.fileExists;
    }

    void record(const LocalizedObject &object)
    {
        const string filename = dir + to_string(object.data.signature);
        BinaryDataStreamFile file(filename, true);
        file << object;
        file.closeStream();

        geometry[object.data.signature] = GeometryDatabaseEntry(true);
    }

    const LocalizedObject* loadGeometry(UINT64 signature)
    {
        if (geometry.count(signature) == 0)
        {
            const string filename = dir + to_string(signature);
            const bool fileExists = util::fileExists(filename);
            geometry[signature] = GeometryDatabaseEntry(fileExists);
            if (fileExists)
            {
                BinaryDataStreamFile file(filename, false);
                LocalizedObject *object = new LocalizedObject();
                file >> *object;
                file.closeStream();
            }
        }
        return geometry[signature].geometry;
    }

    string dir;
    map<UINT64, GeometryDatabaseEntry> geometry;
};