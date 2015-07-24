
struct MyD3DAssets;

struct DrawParameters
{
    DrawParameters(UINT _IndexCount, UINT _StartIndexLocation, INT _BaseVertexLocation)
    {
        IndexCount = _IndexCount;
        StartIndexLocation = _StartIndexLocation;
        BaseVertexLocation = _BaseVertexLocation;
    }
    UINT IndexCount;
    UINT StartIndexLocation;
    INT BaseVertexLocation;
};

struct LocalizedObjectVertex : public BinaryDataSerialize< LocalizedObjectVertex >
{
    vec3f worldPos;
};

struct LocalizedObject
{
    void load(MyD3DAssets &assets, const DrawParameters &params);
    void loadFromDrawIndexed(MyD3DAssets &assets, UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation);
    void loadFromDraw(MyD3DAssets &assets, UINT  VertexCount, UINT  StartVertexLocation);
    void toMesh(TriMeshf &mesh);

    int drawIndex;
    vector<LocalizedObjectVertex> vertices;
    vector<unsigned short> indices;
};

template<class BinaryDataBuffer, class BinaryDataCompressor>
inline BinaryDataStream<BinaryDataBuffer, BinaryDataCompressor>& operator<<(BinaryDataStream<BinaryDataBuffer, BinaryDataCompressor>& s, const LocalizedObject& o) {
    s << o.drawIndex << o.vertices << o.indices;
    return s;
}

template<class BinaryDataBuffer, class BinaryDataCompressor>
inline BinaryDataStream<BinaryDataBuffer, BinaryDataCompressor>& operator>>(BinaryDataStream<BinaryDataBuffer, BinaryDataCompressor>& s, LocalizedObject& o) {
    s >> o.drawIndex >> o.vertices >> o.indices;
    return s;
}
