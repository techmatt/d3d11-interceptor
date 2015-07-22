
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