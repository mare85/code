cbuffer cbChangesPerFrame : register(b0)
{
	matrix world_;
	matrix view_;
	matrix projection_;
};

struct VS_Input
{
	float4 pos : POSITION;
	float4 tex0 : TEXCOORD0;
	float col : COLOR0;
};
struct PS_Input
{
	float4 pos : SV_POSITION;
	float2 tex0 : TEXCOORD0;
	float col : COLOR0;
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
	vsOut.col = vertex.col;

	return vsOut;
}
float4 ps_main(PS_Input frag ) : SV_TARGET
{
	float x = frac( frag.tex0.x );
	float y = frac( frag.tex0.y );
	
	float col = pow( x * (1-x) * 16 * y * (1 - y), .2 ) * frag.col;
	float4 texCol = float4(col, col, col,1.0 );
		
	float fogClarity = max( min( 16.0 / frag.dist - 0.3, 1.0 ) , .0 );
	texCol.r = texCol.r * fogClarity;
	texCol.g = texCol.g * fogClarity;
	texCol.b = texCol.b * fogClarity;
	return texCol;
}