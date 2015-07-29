
void setupDeviceDebugLayer(ID3D11Device *device);

struct GPUDrawBuffers
{
    GPUDrawBuffers(MyD3DAssets &assets) :
        VSConstants(assets.getVSConstantBuffer()),
        index(assets.getActiveIndexBuffer()),
        vertex(assets.getActiveVertexBuffer())
    {

    }

    const BufferCPU *VSConstants;
    const IndexBufferState index;
    const VertexBufferState vertex;
};