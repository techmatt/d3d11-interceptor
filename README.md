# d3d11-interceptor
A wrapper and interceptor for the D3D11 API, replacing d3d11.dll

To use the interceptor:

You will need to disable Safe DLL Search Mode:
https://msdn.microsoft.com/en-us/library/windows/desktop/ms682586(v=vs.85).aspx

Specifically create the
HKEY_LOCAL_MACHINE\System\CurrentControlSet\Control\Session Manager\SafeDllSearchMode
registry value and set it to 0 (DWORD).

https://dolphin-emu.org/download/dev/f5b0468179f1c3c15cfb5957218ee9f27cb47d77/

struct VertexShaderConstants
{
       float4 posnormalmatrix[6];
       float4 projection[4];
       int4 materials[4];
       struct Light
       {
               int4 color;
               float4 cosatt;
               float4 distatt;
               float4 pos;
               float4 dir;
       } lights [8];
       float4 texmatrices[24];
       float4 transformmatrices[64];
       float4 normalmatrices[32];
       float4 posttransformmatrices[64];
       float4 pixelcentercorrection;
};

