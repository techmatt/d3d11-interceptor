
#include "main.h"

void LocalizedObject::toMesh(const SignatureColorMap &colorMap, TriMeshf &mesh)
{
    vector<TriMeshf::Vertexf> meshVertices;
    vector<unsigned int> meshIndices;

    auto vertexValid = [](const vec3f &v)
    {
        return (v.x != 0.0f);
    };
    
    vec3f color(1.0f, 0.0f, 1.0f);

    if (colorMap.colors.count(signature) > 0)
    {
        auto &entry = colorMap.colors.find(signature)->second;
        if (entry.pixelCount == 0)
            color = vec3f(0.0f, 1.0f, 1.0f);
        else
            color = entry.color;
    }

    for (int triIndex = 0; triIndex < vertices.size() - 2; triIndex++)
    {
        const vec3f &v0 = vertices[triIndex + 0].worldPos;
        const vec3f &v1 = vertices[triIndex + 1].worldPos;
        const vec3f &v2 = vertices[triIndex + 2].worldPos;

        int i0 = indices[triIndex + 0];
        int i1 = indices[triIndex + 1];
        int i2 = indices[triIndex + 2];

        if (i1 == i0 + 1 && i2 == i0 + 2 && vertexValid(v0) && vertexValid(v1) && vertexValid(v2))
        {
            vec3f normal = math::triangleNormal(v0, v1, v2);

            normal.x = abs(normal.x);
            normal.y = abs(normal.y);
            normal.z = abs(normal.z);

            float shading = max(max(normal.x, normal.y), normal.z);
            //vec3f triColor = normal.getNormalized() * 0.5f + vec3f(0.5f);
            vec3f triColor = color * shading;

            for (int vIndex = 0; vIndex < 3; vIndex++)
            {
                TriMeshf::Vertexf v;
                v.position = vertices[triIndex + vIndex].worldPos;
                v.color = vec4f(triColor, 1.0f);
                meshVertices.push_back(v);
                meshIndices.push_back((UINT)meshIndices.size());
            }
        }
    }

    if (meshVertices.size() > 0)
        mesh = TriMeshf(meshVertices, meshIndices);
}
