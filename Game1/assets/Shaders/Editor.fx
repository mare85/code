
Texture2D texture0_ : register(t0);
SamplerState texture0Sampler_: register(s0);
Texture2D texture1_ : register(t1);
SamplerState texture1Sampler_: register(s1);

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
	float2 sampCoord = frag.tex0.xy;
	float edgeVal = ( frag.tex0.x - .5 ) * 2.0;
	edgeVal *= edgeVal * -.2;
	sampCoord.y = pow(sampCoord.y, 1.2 + edgeVal);
	float2 sampCoordX = sampCoord + float2(.004, .0);
	float2 sampCoordY = sampCoord + float2(.0, .006);

	float samp0 = texture0_.Sample(texture0Sampler_, sampCoord).x;
	float samp0_X = texture0_.Sample(texture0Sampler_, sampCoordX).x;
	float samp0_Y = texture0_.Sample(texture0Sampler_, sampCoordY).x;
	float samp1 = texture1_.Sample(texture1Sampler_, sampCoord).x;
	float samp1_X = texture1_.Sample(texture1Sampler_, sampCoordX).x;
	float samp1_Y = texture1_.Sample(texture1Sampler_, sampCoordY).x;
	float col1 = min( abs( samp1 - samp1_X ) + abs( samp1 - samp1_Y ), 1.0 );
	float col0 = min( abs( samp0 - samp0_X ) + abs( samp0 - samp0_Y ), 1.0 - samp1);
	col0 = min(col0, 1.0 - col1);
	
	if( col0 < .1 && col1 < .1 )
		discard;
	return col0 * float4( 1.0, 1.0, 1.0, .1 ) + col1 * float4( 0.5, .0, .0, 1.0);
}
