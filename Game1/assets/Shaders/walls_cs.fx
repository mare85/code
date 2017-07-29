ByteAddressBuffer Bufferin : register(t0);
RWByteAddressBuffer BufferOut : register(u0);

[numthreads(64, 1, 1)]
void cs_main( uint3 DTid : SV_DispatchThreadID )
{
	float l = asfloat( Bufferin.Load( DTid.x * 4 ) );
	float4 outPutL = float4(l,l,l,l);
	BufferOut.Store4( DTid.x*16, asuint(outPutL) );
}