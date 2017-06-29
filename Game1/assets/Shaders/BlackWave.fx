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
	//matrix wvp = mul(vp, world_ );

	float4 worldPos = mul( world_, float4(vertex.pos.xy, .0, 1.0) );
	float2 offset = vertex.pos.z * float2( sin( user0_.x + vertex.pos.w * 6.283 ), cos( user0_.x * 0.8 + vertex.pos.w * 12.566 ) );
	float2 waveDiff = worldPos.xy - user0_.yz;
	float waveDiffLenSqr = dot( waveDiff, waveDiff );
	if( waveDiffLenSqr > 1.0 )
	{
		waveDiff /= waveDiffLenSqr;
	}
	offset += vertex.pos.z * user0_.w *  waveDiff;

	
	float4 vpos = float4(worldPos + offset, .0, 1.0);
	vsOut.pos = mul(vp, vpos);
	
	return vsOut;
}
float4 ps_main(PS_Input frag ) : SV_TARGET
{
	return 1.0;
}