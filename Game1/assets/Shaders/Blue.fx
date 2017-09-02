cbuffer cbChangesPerFrame : register(b0)
{
	matrix world_;
	matrix view_;
	matrix projection_;
};

struct VS_Input
{
	float3 pos : POSITION;
	float3 nrm : NORMAL;
};
struct PS_Input
{
	float4 pos : SV_POSITION;
	float3 nrm : NORMAL;
};

PS_Input vs_main(VS_Input vertex)
{
	PS_Input vsOut = (PS_Input)0;
	float4 inPos = float4(vertex.pos, 1.0);
	matrix vp = mul(projection_, view_);
	matrix wvp = mul(vp, world_ );
	vsOut.pos = mul(wvp, inPos);
	vsOut.nrm = normalize( mul((float3x3)world_, vertex.nrm) );
	return vsOut;
}
float4 ps_main(PS_Input frag ) : SV_TARGET
{
	float diff = max( dot( frag.nrm, float3(.629, .629, -.629) ),.0 );
	diff *= diff * diff;
	float3 col = diff * float3(1.0, .8, .6) + float3(.0, .2, .4);
	return float4(col, 1.0); 
}