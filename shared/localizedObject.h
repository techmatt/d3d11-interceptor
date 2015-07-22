
struct MyD3DAssets;

struct LocalizedObjectVertex
{
    vec3f worldPos;
    vec4uc color;
};

struct LocalizedObject
{
    void loadFromDrawIndexed(MyD3DAssets &assets, UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation);

    int drawIndex;
    vector<LocalizedObjectVertex> vertices;
};

template<class T>
ostream& operator << (ostream &os, const vector<T> &objects)
{
    os << objects.size() << endl;
    for (const auto &o : objects)
    {
        os << o << ' ';
    }
    os << endl;
    return os;
}

template<class T>
istream& operator >> (istream &is, vector<T> &objects)
{
    size_t length;
    is >> length;
    objects.resize(length);
    for (int i = 0; i < length; i++)
    {
        is >> objects[i];
    }
    return is;
}

inline ostream& operator << (ostream &os, const LocalizedObjectVertex &v)
{
    os << v.worldPos << ' ' << vec4f(v.color) << ' ';
    return os;
}

inline ostream& operator << (ostream &os, const LocalizedObject &o)
{
    os << o.drawIndex << ' ' << o.vertices << endl;
    return os;
}

inline istream& operator >> (istream &is, LocalizedObjectVertex &v)
{
    vec4f color;
    is >> v.worldPos >> color;
    v.color = color;
    return is;
}

inline istream& operator >> (istream &is, LocalizedObject &o)
{
    is >> o.drawIndex >> o.vertices;
    return is;
}