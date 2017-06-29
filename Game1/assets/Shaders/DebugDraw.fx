cbuffer cbChangesPerFrame : register(b0)
{
	matrix view_;
	matrix projection_;
};

struct VS_Input
{
	float4 pos : POSITION;
	float4 col : COLOR0;
};
struct PS_Input
{
	float4 pos : SV_POSITION;
	float4 col : COLOR0;
};

PS_Input vs_main(VS_Input vertex)
{
	PS_Input vsOut = (PS_Input)0;
	matrix vp = mul(projection_, view_);
	vsOut.pos = mul(vp, float4( vertex.pos.xyz, 1.0));
	vsOut.col = vertex.col;
	return vsOut;
}
float4 ps_main(PS_Input frag) : SV_TARGET
{
	return frag.col;
}