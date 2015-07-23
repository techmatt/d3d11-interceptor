
struct MyD3DAssets;

struct LocalizedObjectVertex : public BinaryDataSerialize< LocalizedObjectVertex >
{
    vec3f worldPos;
};

struct LocalizedObject
{
    void loadFromDrawIndexed(MyD3DAssets &assets, UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation);
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
