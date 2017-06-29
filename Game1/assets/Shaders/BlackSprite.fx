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
};
struct PS_Input
{
	float4 pos : SV_POSITION;
	float2 tex0 : TEXCOORD0;
};

PS_Input vs_main(VS_Input vertex)
{
	
	PS_Input vsOut = (PS_Input)0;
	matrix vp = mul(projection_, view_);
	matrix wvp = mul(vp, world_ );
	float4 pos = float4(vertex.pos.x, vertex.pos.y, .0, 1.0);
	vsOut.pos = mul(wvp, pos);
	vsOut.tex0 = float2( vertex.pos.z, vertex.pos.w );
	return vsOut;
}
float4 ps_main(PS_Input frag ) : SV_TARGET
{
	float texCol = texture_.Sample(textureSampler_, frag.tex0).x;
	texCol = 1.0;
	if (texCol < .05)
		discard;
	return texture_.Sample(textureSampler_, frag.tex0).x;
}