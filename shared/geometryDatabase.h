
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
        baseDir = R"(C:\Code\d3d11-interceptor\Dolphin-x64\geometryDatabase\)";
        geometryDir = baseDir + "geometry/";
        imagesDir = baseDir + "images/";
        util::makeDirectory(baseDir);
        util::makeDirectory(geometryDir);
        util::makeDirectory(imagesDir);
    }
    bool hasSignature(UINT64 signature)
    {
        if (geometry.count(signature) == 0)
        {
            const bool fileExists = util::fileExists(geometryDir + to_string(signature));
            geometry[signature] = GeometryDatabaseEntry(fileExists);
        }
        return geometry.find(signature)->second.fileExists;
    }

    void record(const LocalizedObject &object, const Bitmap &image)
    {
        const string geometryFilename = geometryDir + to_string(object.data.signature);
        const string imageFilename = imagesDir + to_string(object.data.signature) + ".png";

        BinaryDataStreamFile file(geometryFilename, true);
        file << object;
        file.closeStream();

        LodePNG::save(image, imageFilename);

        geometry[object.data.signature] = GeometryDatabaseEntry(true);
    }

    const LocalizedObject* loadGeometry(UINT64 signature)
    {
        if (geometry.count(signature) == 0)
        {
            const string filename = baseDir + to_string(signature);
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

    string baseDir, geometryDir, imagesDir;
    map<UINT64, GeometryDatabaseEntry> geometry;
};