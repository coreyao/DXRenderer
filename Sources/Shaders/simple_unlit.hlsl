float4x4 mViewProj; 
float fIntensity;

struct VS_OUTPUT
{
	float4 Position   : POSITION;
	float3 Normal  : Normal;
	float2 TexCoord  : TEXCOORD;
};

VS_OUTPUT VSMain(in float3 vPosition : POSITION,
				in float3 vNormal : NORMAL,
				in float2 vTexCoord : TEXCOORD)
{
	VS_OUTPUT Output;

	Output.Position = mul(float4(vPosition.x, vPosition.y, vPosition.z, 1.0), mViewProj);
	Output.Normal = vNormal;
	Output.TexCoord = vTexCoord;

	return Output;
}

struct PS_OUTPUT
{
	float4 RGBColor : COLOR0;
};

texture g_MeshTexture : Texture;
sampler MeshTextureSampler =
sampler_state
{
	Texture = <g_MeshTexture>;
	MipFilter = LINEAR;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
};


PS_OUTPUT PSMain(VS_OUTPUT In)
{
	PS_OUTPUT Output;
	//Output.RGBColor = float4(tex2D(MeshTextureSampler, In.TexCoord).rgb, 1.0);
	Output.RGBColor = float4(1.0, 1.0, 1.0, 1.0);
	return Output;
}


