
struct MyD3DAssets;
struct SignatureColorMap;

struct DrawParameters
{
    DrawParameters(UINT _IndexCount, UINT _StartIndexLocation, INT _BaseVertexLocation)
    {
        IndexCount = _IndexCount;
        StartIndexLocation = _StartIndexLocation;
        BaseVertexLocation = _BaseVertexLocation;
        signature = (UINT64)-1;
    }
    UINT IndexCount;
    UINT StartIndexLocation;
    INT BaseVertexLocation;

    UINT64 signature;
};

struct LocalizedObjectVertex : public BinaryDataSerialize< LocalizedObjectVertex >
{
    vec3f worldPos;
};

struct LocalizedObjectData : public BinaryDataSerialize< LocalizedObjectData >
{
    int drawIndex;
    UINT64 signature;
    bbox3f boundingBox;
};

struct LocalizedObject
{
    //
    // Interceptor functions
    //
    static UINT64 computeSignature(MyD3DAssets &assets, const DrawParameters &params);
    static bbox3f computeBoundingBox(MyD3DAssets &assets, const DrawParameters &params);
    void load(MyD3DAssets &assets, const DrawParameters &params, bool loadVertexData);
    void loadFromDrawIndexed(MyD3DAssets &assets, UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation);
    void loadFromDraw(MyD3DAssets &assets, UINT  VertexCount, UINT  StartVertexLocation);

    //
    // Learner functions
    //
    void toMesh(const SignatureColorMap &colorMap, TriMeshf &mesh) const;

    LocalizedObjectData data;
    vector<LocalizedObjectVertex> vertices;
    vector<unsigned short> indices;
};

template<class BinaryDataBuffer, class BinaryDataCompressor>
inline BinaryDataStream<BinaryDataBuffer, BinaryDataCompressor>& operator<<(BinaryDataStream<BinaryDataBuffer, BinaryDataCompressor>& s, const LocalizedObject& o) {
    s << o.data << o.vertices << o.indices;
    return s;
}

template<class BinaryDataBuffer, class BinaryDataCompressor>
inline BinaryDataStream<BinaryDataBuffer, BinaryDataCompressor>& operator>>(BinaryDataStream<BinaryDataBuffer, BinaryDataCompressor>& s, LocalizedObject& o) {
    s >> o.data >> o.vertices >> o.indices;
    return s;
}
