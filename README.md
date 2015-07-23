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

struct PixelShaderConstants
{
	int4 colors[4];
	int4 kcolors[4];
	int4 alpha;
	float4 texdims[8];
	int4 zbias[2];
	int4 indtexscale[2];
	int4 indtexmtx[6];
	int4 fogcolor;
	int4 fogi;
	float4 fogf[2];
	float4 zslope;
	float4 efbscale;
};

----

struct Light
{
	int4 color;
	float4 cosatt;
	float4 distatt;
	float4 pos;
	float4 dir;
};

cbuffer VSBlock
{
	float4 cpnmtx[6];
	float4 cproj[4];
	int4 cmtrl[4];
	Light clights[8];
	float4 ctexmtx[24];
	float4 ctrmtx[64];
	float4 cnmtx[32];
	float4 cpostmtx[64];
	float4 cpixelcenter;
};

struct VS_OUTPUT
{
	float4 pos : POSITION;
	float4 colors_0 : COLOR0;
	float4 colors_1 : COLOR1;
	float3 tex0 : TEXCOORD0;
	float4 clipPos : TEXCOORD1;
};

VS_OUTPUT main(
  float3 rawnorm0 : NORMAL0,
  float4 color0 : COLOR0,
  float4 rawpos : POSITION)
{
	VS_OUTPUT o;
	float4 pos = float4(dot(cpnmtx[0], rawpos), dot(cpnmtx[1], rawpos), dot(cpnmtx[2], rawpos), 1.0);
	float3 _norm0 = normalize(float3(dot(cpnmtx[3].xyz, rawnorm0), dot(cpnmtx[4].xyz, rawnorm0), dot(cpnmtx[5].xyz, rawnorm0)));
	o.pos = float4(dot(cproj[0], pos), dot(cproj[1], pos), dot(cproj[2], pos), dot(cproj[3], pos));
	int4 lacc;
	float3 ldir, h, cosAttn, distAttn;
	float dist, dist2, attn;
	{
		int4 mat = int4(round(color0 * 255.0));
		lacc = int4(255, 255, 255, 255);
		lacc.w = 255;
		lacc = clamp(lacc, 0, 255);
		o.colors_0 = float4((mat * (lacc + (lacc >> 7))) >> 8) / 255.0;
	}
	o.colors_1 = o.colors_0;
	float4 coord = float4(0.0, 0.0, 1.0, 1.0);
	{
		coord = float4(0.0, 0.0, 1.0, 1.0);
		coord = float4(rawnorm0.xyz, 1.0);
		o.tex0.xyz = float3(dot(coord, ctexmtx[0]), dot(coord, ctexmtx[1]), dot(coord, ctexmtx[2]));
		float4 P0 = cpostmtx[0];
		float4 P1 = cpostmtx[1];
		float4 P2 = cpostmtx[2];
		o.tex0.xyz = normalize(o.tex0.xyz);
		o.tex0.xyz = float3(dot(P0.xyz, o.tex0.xyz) + P0.w, dot(P1.xyz, o.tex0.xyz) + P1.w, dot(P2.xyz, o.tex0.xyz) + P2.w);
	}
	o.clipPos = o.pos;
	o.pos.z = -o.pos.z;
	o.pos.xy = o.pos.xy - o.pos.w * cpixelcenter.xy;
	return o;
}

----

struct VS_OUTPUT {
	float4 pos : POSITION;
	float4 colors_0 : COLOR0;
	float4 colors_1 : COLOR1;
	float3 tex0 : TEXCOORD0;
	float3 tex1 : TEXCOORD1;
	float4 clipPos : TEXCOORD2;
};
VS_OUTPUT main(
  float3 rawnorm0 : NORMAL0,
  float2 tex0 : TEXCOORD0,
  float4 rawpos : POSITION) {
	VS_OUTPUT o;
	float4 pos = float4(dot(cpnmtx[0], rawpos), dot(cpnmtx[1], rawpos), dot(cpnmtx[2], rawpos), 1.0);
	float3 _norm0 = normalize(float3(dot(cpnmtx[3].xyz, rawnorm0), dot(cpnmtx[4].xyz, rawnorm0), dot(cpnmtx[5].xyz, rawnorm0)));
	o.pos = float4(dot(cproj[0], pos), dot(cproj[1], pos), dot(cproj[2], pos), dot(cproj[3], pos));
	int4 lacc;
	float3 ldir, h, cosAttn, distAttn;
	float dist, dist2, attn;
	{
		int4 mat = cmtrl[2];
		lacc = int4(255, 255, 255, 255);
		lacc.w = 255;
		lacc = clamp(lacc, 0, 255);
		o.colors_0 = float4((mat * (lacc + (lacc >> 7))) >> 8) / 255.0;
	}
	o.colors_1 = o.colors_0;
	float4 coord = float4(0.0, 0.0, 1.0, 1.0);
	{
		coord = float4(0.0, 0.0, 1.0, 1.0);
		coord = float4(rawnorm0.xyz, 1.0);
		o.tex0.xyz = float3(dot(coord, ctexmtx[0]), dot(coord, ctexmtx[1]), dot(coord, ctexmtx[2]));
		float4 P0 = cpostmtx[3];
		float4 P1 = cpostmtx[4];
		float4 P2 = cpostmtx[5];
		o.tex0.xyz = normalize(o.tex0.xyz);
		o.tex0.xyz = float3(dot(P0.xyz, o.tex0.xyz) + P0.w, dot(P1.xyz, o.tex0.xyz) + P1.w, dot(P2.xyz, o.tex0.xyz) + P2.w);
	}
	{
		coord = float4(0.0, 0.0, 1.0, 1.0);
		coord = float4(tex0.x, tex0.y, 1.0, 1.0);
		o.tex1.xyz = float3(dot(coord, ctexmtx[3]), dot(coord, ctexmtx[4]), 1);
		float4 P0 = cpostmtx[0];
		float4 P1 = cpostmtx[1];
		float4 P2 = cpostmtx[2];
		o.tex1.xyz = float3(dot(P0.xyz, o.tex1.xyz) + P0.w, dot(P1.xyz, o.tex1.xyz) + P1.w, dot(P2.xyz, o.tex1.xyz) + P2.w);
	}
	o.clipPos = o.pos;
	o.pos.z = -o.pos.z;
	o.pos.xy = o.pos.xy - o.pos.w * cpixelcenter.xy;
	return o;
}

