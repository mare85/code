//srvMatrix_,srvRadiuses_,srvPulse_,srvBeziers_ -> uavVertexPos_
cbuffer cbChangesPerFrame : register(b0)
{
	uint nSegmentsPerEdge_;
	float offsetRange_;
};

ByteAddressBuffer BufferMatrix: register(t0);
ByteAddressBuffer BufferRadius: register(t1);
ByteAddressBuffer BufferProgAmp: register(t2);
ByteAddressBuffer BufferBeziers: register(t3);
ByteAddressBuffer BufferOffsets: register(t4);
RWByteAddressBuffer vpos: register(u0);
RWByteAddressBuffer vcol: register(u1);

[numthreads(64, 1, 1)]
void cs_getVPos( uint3 DTid : SV_DispatchThreadID )
{
	const int edgeIndex = DTid.x / nSegmentsPerEdge_;
	const int tIndex = DTid.x - edgeIndex * nSegmentsPerEdge_;
	float t = ( .5 + tIndex ) / nSegmentsPerEdge_;
	const int progAmpIndex = edgeIndex * 8;
	const int offsetIndex1 = edgeIndex * 32;
	const int offsetIndex2 = offsetIndex1 + 16;
	const int beziersIndex = tIndex * 16;

	float prog = asfloat( BufferProgAmp.Load( progAmpIndex ) );
	float amp = asfloat( BufferProgAmp.Load( progAmpIndex + 4 ) );
	float x = t - prog;
	float pulse0 = max(.0f, 1.0 - 10.0 * x*x);
	pulse0 *= 16.0 * prog * (1.0 - prog) * t * (1.0 - t);
	float col = pulse0;
	pulse0 *= amp;
	float pulse = pulse0 + 1.0;

	float4 matx = asfloat( BufferMatrix.Load4( DTid.x * 64 ) );
	float4 maty = asfloat( BufferMatrix.Load4( DTid.x * 64 + 16 ) );
	float4 matz = asfloat( BufferMatrix.Load4( DTid.x * 64 + 32 ) );
	float4 matt = asfloat( BufferMatrix.Load4( DTid.x * 64 + 48 ) );

	float3 offs1 = asfloat( BufferOffsets.Load3( offsetIndex1 ) );
	float3 offs2 = asfloat( BufferOffsets.Load3( offsetIndex2 ) );
	float4 b = asfloat( BufferBeziers.Load4( beziersIndex ) );
	offs1 = offsetRange_ * sin( offs1 * 6.283 );
	offs2 = offsetRange_ * sin( offs2 * 6.283 );
	float3 offset = offs1 * (b.x + b.y ) + offs2 * (b.z + b.w) + pulse0 * matx * .3;
	matt += float4( offset, .0 );

	uint radiusIndex = ( DTid.x % (nSegmentsPerEdge_) ) * 4;
	float radius = asfloat( BufferRadius.Load( radiusIndex ) );
	float thickness = .1;
	float3 thicknessDir = thickness * matz.xyz;
	float3 halfThicknesDir = thicknessDir * -.5;
	radius *= pulse;
	float sins[] = {.0, .951, .5878, -.5878, -.951};
	float coss[] = {1.0, .309, -.809, -.809, .309 };
	uint outIndex = 120 * DTid.x;
	uint outIndexCol = 40 * DTid.x;
	[unroll]
	for( unsigned int i = 0; i < 5; ++i )
	{
		float3 pos1 = halfThicknesDir + 
			radius * ( matx.xyz * sins[i] + maty.xyz * coss[i] ) + 
			matt.xyz;
		float3 pos2 = pos1 + thicknessDir;
		vpos.Store3( outIndex, asuint(pos1) );
		vpos.Store3( outIndex + 12, asuint(pos2) );
		vcol.Store( outIndexCol, asuint(col) );
		vcol.Store( outIndexCol + 4, asuint(col) );
		outIndex += 24;
	}
}