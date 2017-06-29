float4 vs_main(float4 pos : POSITION) : SV_POSITION
{
	return pos;
}
float4 ps_main(float4 pos : SV_POSITION) : SV_TARGET
{
	return float4(0.0f, 1.0f, 0.0f, 1.0f);
}