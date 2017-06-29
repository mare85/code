cbuffer cbChangesPerFrame : register(b0)
{
	matrix world_;
	matrix view_;
	matrix projection_;
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
	float dist : TEXCOORD1;
};




PS_Input vs_main(VS_Input vertex)
{
	
	PS_Input vsOut = (PS_Input)0;
	matrix vp = mul(projection_, view_);
	matrix wvp = mul(vp, world_ );
	
	vsOut.pos = mul(wvp, vertex.pos);
	vsOut.tex0 = vertex.tex0.xy;
	vsOut.dist = -mul( mul(view_, world_), vertex.pos ).z;

	return vsOut;
}
float4 ps_main(PS_Input frag ) : SV_TARGET
{
	float4 texCol = texture_.Sample(textureSampler_, frag.tex0);
	float fogClarity = max( min( 16.0 / frag.dist - 0.3, 1.0 ) , .0 ) * .3;
	texCol.r = texCol.r * fogClarity;
	texCol.g = texCol.g * fogClarity;
	texCol.b = texCol.b * fogClarity;
	if (texCol.a < .05)
		discard;
	return texCol;
}