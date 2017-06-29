cbuffer cbChangesPerFrame : register(b0)
{
	matrix world_;
	matrix view_;
	matrix projection_;
	float4 user0_;
};

struct VS_Input
{
	float4 pos : POSITION;
};
struct PS_Input
{
	float4 pos : SV_POSITION;
};

PS_Input vs_main(VS_Input vertex)
{
	PS_Input vsOut = (PS_Input)0;
	matrix vp = mul(projection_, view_);
	matrix wvp = mul(vp, world_);
	float4 vpos = float4(vertex.pos.xy, .0, 1.0);
	vsOut.pos = mul(wvp, vpos);

	return vsOut;
}
float4 ps_main(PS_Input frag) : SV_TARGET
{
	return float4( .0, .0, .0, .7 );
}