
struct MyD3DAssets;
struct SignatureColorMap;
struct GPUDrawBuffers;

struct DrawParameters
{
    DrawParameters(UINT _IndexCount, UINT _StartIndexLocation, INT _BaseVertexLocation)
    {
        IndexCount = _IndexCount;
        StartIndexLocation = _StartIndexLocation;
        BaseVertexLocation = _BaseVertexLocation;
        signature = 0;
    }
    UINT IndexCount;
    UINT StartIndexLocation;
    INT BaseVertexLocation;

    UINT64 signature;
};

struct LocalizedObjectVertex : public BinaryDataSerialize< LocalizedObjectVertex >
{
    vec3f worldPos;
    vec2f tex0;
};

struct LocalizedObjectData : public BinaryDataSerialize< LocalizedObjectData >
{
    static const int vertexStoreCount = 6;

    int drawIndex;
    UINT64 signature;
    vec3f centroid;
    bbox3f bbox;
    vec3f vertices[vertexStoreCount];
};

struct LocalizedObject
{
    //
    // Interceptor functions
    //
    static int computeIndexSkip(MyD3DAssets &assets, const DrawParameters &params, const GPUDrawBuffers &buffers);
    static UINT64 computeSignature(MyD3DAssets &assets, const DrawParameters &params, const GPUDrawBuffers &buffers);
    static void computeBoundingInfo(MyD3DAssets &assets, const DrawParameters &params, const GPUDrawBuffers &buffers, LocalizedObjectData &result);

    void load(MyD3DAssets &assets, const DrawParameters &params, const GPUDrawBuffers &buffers, bool loadVertexData);
    void loadFromDrawIndexed(MyD3DAssets &assets, const GPUDrawBuffers &buffers, UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation);
    void loadFromDraw(MyD3DAssets &assets, const GPUDrawBuffers &buffers, UINT  VertexCount, UINT  StartVertexLocation);

    //
    // Learner functions
    //
    void saveDescription(const string &filename) const;
    UINT64 computeSignatureFromVertices() const;
    void toMesh(const SignatureColorMap &colorMap, TriMeshf &mesh) const;

    LocalizedObjectData data;
    //string signatureDebug;
    vector<LocalizedObjectVertex> vertices;
    vector<int> indices;
};

template<class BinaryDataBuffer, class BinaryDataCompressor>
inline BinaryDataStream<BinaryDataBuffer, BinaryDataCompressor>& operator<<(BinaryDataStream<BinaryDataBuffer, BinaryDataCompressor>& s, const LocalizedObject& o) {
    s << o.data;
    s.writePrimitiveVector(o.vertices);
    s.writePrimitiveVector(o.indices);
    return s;
}

template<class BinaryDataBuffer, class BinaryDataCompressor>
inline BinaryDataStream<BinaryDataBuffer, BinaryDataCompressor>& operator>>(BinaryDataStream<BinaryDataBuffer, BinaryDataCompressor>& s, LocalizedObject& o) {
    s >> o.data;
    s.readPrimitiveVector(o.vertices);
    s.readPrimitiveVector(o.indices);
    return s;
}
