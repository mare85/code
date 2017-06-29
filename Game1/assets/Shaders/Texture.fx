Texture2D texture_ : register(t0);
SamplerState textureSampler_ : register(s0);
struct VS_Input
{
	float4 pos : POSITION;
	float4 tex0 : TEXCOORD0;
};
struct PS_Input
{
	float4 pos : SV_POSITION;
	float2 tex0 : TEXCOORD0;
};


PS_Input vs_main(VS_Input vertex)
{
	
	PS_Input vsOut = (PS_Input)0;
	vsOut.pos = vertex.pos;
	vsOut.tex0 = vertex.tex0.xy;
	return vsOut;
}
float4 ps_main(PS_Input frag ) : SV_TARGET
{
	return texture_.Sample(textureSampler_, frag.tex0);
}