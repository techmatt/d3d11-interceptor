
#include "main.h"

UINT64 LocalizedObject::computeSignatureFromVertices() const
{
    UINT64 sum = 0;
    const int signatureIndexStart = 16;

    const unsigned short firstIndex = indices[0];
    
    int prevIndex = -999;
    for (int indexIndex = signatureIndexStart; indexIndex < min(signatureIndexStart + 16, (int)indices.size()); indexIndex++)
    {
        const int curIndex = indices[indexIndex];
        
        bool valid = (curIndex == prevIndex + 1);
        if (valid)
        {
            const vec2f tex = vertices[indexIndex].tex0;
            const UINT32 *tStartI = (const UINT32 *)&tex;

            sum += UINT64(tStartI[0]);
            sum += UINT64(tStartI[1]);
        }

        prevIndex = curIndex;
    }

    return sum;
}

void LocalizedObject::saveDescription(const string &filename) const
{
    ofstream file(filename);

    file << "signature in data = " << data.signature << endl;
    file << "signature from vertices = " << computeSignatureFromVertices() << endl;
    //file << "signature debug:" << endl << signatureDebug << endl;
    file << "vertices:" << endl;
    for (auto &v : vertices)
    {
        file << "pos = " << v.worldPos << ", tex = " << v.tex0 << endl;
    }

    file << endl << endl;
    file << "indices:" << endl;
    for (auto &i : indices)
    {
        file << i << endl;
    }
}

void LocalizedObject::toMesh(const SignatureColorMap &colorMap, TriMeshf &mesh) const
{
    vector<TriMeshf::Vertexf> meshVertices;
    vector<unsigned int> meshIndices;

    auto vertexValid = [](const vec3f &v)
    {
        return (v.x == v.x && v.x != 0.0f);
    };
    
    vec3f color(1.0f, 1.0f, 1.0f);
    
    color = colorMap.getColor(data.signature);
    color += vec3f(0.2f, 0.2f, 0.2f);
    
    for (int triIndex = 0; triIndex < vertices.size() - 2; triIndex++)
    {
        const vec3f &v0 = vertices[triIndex + 0].worldPos;
        const vec3f &v1 = vertices[triIndex + 1].worldPos;
        const vec3f &v2 = vertices[triIndex + 2].worldPos;

        int i0 = indices[triIndex + 0];
        int i1 = indices[triIndex + 1];
        int i2 = indices[triIndex + 2];

        //i1 == i0 + 1 && i2 == i0 + 2 && 
        if (vertexValid(v0) && vertexValid(v1) && vertexValid(v2) && math::abs(i0 - i1) <= 3 && math::abs(i0 - i2) <= 3)
        {
            vec3f normal = math::triangleNormal(v0, v1, v2);

            normal.x = abs(normal.x);
            normal.y = abs(normal.y);
            normal.z = abs(normal.z);

            normal = normal;

            float shading = max(max(normal.x, normal.y), normal.z) * 1.25f;
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
