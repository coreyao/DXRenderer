float4x4 mViewProj; 
float fIntensity;

struct VS_OUTPUT
{
	float4 Position   : POSITION;
};

VS_OUTPUT VSMain(in float3 vPosition : POSITION)
{
	VS_OUTPUT Output;

	Output.Position = mul(float4(vPosition.x, vPosition.y, vPosition.z, 1.0), mViewProj);

	return Output;
}

struct PS_OUTPUT
{
	float4 RGBColor : COLOR0;
};

PS_OUTPUT PSMain(VS_OUTPUT In)
{
	PS_OUTPUT Output;
	Output.RGBColor = float4(1.0, 1.0, 1.0, 1.0);

	return Output;
}


