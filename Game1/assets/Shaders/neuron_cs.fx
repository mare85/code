//srvMatrix_,srvRadiuses_,srvPulse_,srvBeziers_ -> uavVertexPos_
ByteAddressBuffer BufferMatrix: register(t0);
ByteAddressBuffer BufferRadius: register(t1);
ByteAddressBuffer BufferPulse: register(t2);
ByteAddressBuffer BufferBeziers: register(t3);
RWByteAddressBuffer vpos: register(u0);

[numthreads(64, 1, 1)]
void cs_getVPos( uint3 DTid : SV_DispatchThreadID )
{
	float4 matx = asfloat( BufferMatrix.Load4( DTid.x * 64 ) );
	float4 maty = asfloat( BufferMatrix.Load4( DTid.x * 64 + 16 ) );
	float4 matz = asfloat( BufferMatrix.Load4( DTid.x * 64 + 32 ) );
	float4 matt = asfloat( BufferMatrix.Load4( DTid.x * 64 + 48 ) );
	uint radiusIndex = ( DTid.x % (50) ) * 4;
	float radius = asfloat( BufferRadius.Load( radiusIndex ) );
	float pulse = asfloat( BufferPulse.Load( DTid.x * 4 ) );
	float thickness = .1;
	float3 thicknessDir = thickness * matz.xyz;
	float3 halfThicknesDir = thicknessDir * -.5;
	radius *= pulse;
	float sins[] = {.0, .951, .5878, -.5878, -.951};
	float coss[] = {1.0, .309, -.809, -.809, .309 };
	uint outIndex = 120 * DTid.x;
	[unroll]
	for( unsigned int i = 0; i < 5; ++i )
	{
		float3 pos1 = halfThicknesDir + 
			radius * ( matx.xyz * sins[i] + maty.xyz * coss[i] ) + 
			matt.xyz;
		float3 pos2 = pos1 + thicknessDir;
		vpos.Store3( outIndex, asuint(pos1) );
		vpos.Store3( outIndex + 12, asuint(pos2) );
		outIndex += 24;
	}
}
