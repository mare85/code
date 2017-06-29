cbuffer cbChangesPerFrame : register(b0)
{
	float4 pixelSize_;
};

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
	float pixelSize : COLOR0;

};


PS_Input vs_main(VS_Input vertex)
{

	PS_Input vsOut = (PS_Input)0;
	vsOut.pos = vertex.pos;
	vsOut.tex0 = vertex.tex0.xy;
	vsOut.pixelSize = pixelSize_.x;
	return vsOut;
}
float4 ps_main(PS_Input frag) : SV_TARGET
{
	//3 samples
	float sampSum =
	texture_.Sample(textureSampler_, frag.tex0.xy).x* .5 +
	texture_.Sample(textureSampler_, frag.tex0.xy + float2(0, frag.pixelSize)).x * .25 +
	texture_.Sample(textureSampler_, frag.tex0.xy + float2(0, -frag.pixelSize)).x * .25
	;

	return float4(sampSum, sampSum, sampSum, 1.0);
}