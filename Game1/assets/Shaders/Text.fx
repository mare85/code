cbuffer cbuff : register(b0)
{
	matrix world_;
	matrix projection_;
	float4 color_;
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
	float4 col :COLOR;
};




PS_Input vs_main(VS_Input vertex)
{

	PS_Input vsOut = (PS_Input)0;
	matrix wp = mul(projection_, world_);

	vsOut.pos = mul(wp, vertex.pos);
	vsOut.tex0 = vertex.tex0.xy;
	vsOut.col= color_;
	return vsOut;
}
float4 ps_main(PS_Input frag) : SV_TARGET
{
	float4 texCol = texture_.Sample(textureSampler_, frag.tex0) * frag.col;
	if (texCol.a < .05)
		discard;	
	return texCol;
}