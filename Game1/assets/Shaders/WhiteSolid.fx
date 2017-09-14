cbuffer cbChangesPerFrame : register(b0)
{
	matrix world_;
	matrix view_;
	matrix projection_;
};

struct VS_Input
{
	float3 pos : POSITION;
};
struct PS_Input
{
	float4 pos : SV_POSITION;
};

PS_Input vs_main(VS_Input vertex)
{
	
	PS_Input vsOut = (PS_Input)0;
	matrix vp = mul(projection_, view_);
	matrix wvp = mul(vp, world_ );
	vsOut.pos = mul(wvp, float4( vertex.pos, 1.0 ) );

	return vsOut;
}
float4 ps_main(PS_Input frag ) : SV_TARGET
{
	return float4( 1.0, 1.0, 1.0, 1.0 );
}