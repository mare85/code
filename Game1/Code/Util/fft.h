#pragma once

#include <Util/RandomGenerator.h>
#include <memory>
#include <cmath>
namespace Util
{
template<unsigned int N> inline unsigned int bitReshuffle(unsigned int val)
{
	unsigned int outMask = 1;
	unsigned int out = 0;
	for(unsigned int i = N>>1; i > 0; i>>=1)
	{
		out |= (i&val)?outMask:0;
		outMask<<=1;
	}
	return out;
}
template<> inline unsigned int bitReshuffle<1024>(unsigned int val)
{
	unsigned int 
	out = ( val & 0x01F )<<5 | ( val & 0x3E0 )>>5;
	unsigned int valcenter = ( out & 0x084 );
	out = ( out & 0x063 )<<3 | ( out & 0x318 )>>3;
	out = ( out & 0x129 )<<1 | ( out & 0x252 )>>1;
	return out | valcenter;
}
static const float TWO_PI = 6.28318f;
void dft(float* in1, float* in2, float *out1, float *out2, unsigned int nSamples);
template <unsigned int N>
class FFT
{
	float sins[N];
	float coss[N];
	unsigned int bitSwaps[N>>1];
	void _ForwardInplace( float* re, float* im ) 
	{
		unsigned int nBitswaps = N>>1;
		for( unsigned int i = 0; i < nBitswaps; ++i )
		{
			unsigned int j = bitSwaps[i];
			std::swap( re[i], re[j] );
			std::swap( im[i], im[j] );
		}
		unsigned int sinStep = N;
		for( unsigned int j = 1; j < N; j <<= 1 )
		{
			unsigned int sinIndex = 0;
			unsigned int j2 = j<<1;
			for( unsigned int m = 0; m < j; ++m ) 
			{
				//auto t = pi * sign * m / j;
				float& co = coss[ sinIndex ];
				float& si = sins[ sinIndex ];
				//auto w = complex<double>( cos( t ), sin( t ) );
				for(unsigned int i = m; i < N; i += j2) 
				{
					float reE = re[i], imE = im[i];
					float reO = re[i + j], imO = im[i + j];
					//co,si * z = ( co - i * si ) * (z.r + i * z.i ) = co * (z.r + i*z.i) - i * si( z.r + i*z.i ) =
					// = (co * z.r + si * z.i ) + i * ( -si * z.r + co * z.i ) 
					float reOddIngredient = co * reO + si * imO;
					float imOddIngredient = - si * reO + co * imO;

					re[ i ] = reE + reOddIngredient;
					im[ i ] = imE + imOddIngredient;
					re[ i + j ] = reE - reOddIngredient;
					im[ i + j ] = imE - imOddIngredient;
					//complex<double> zi = zs.at(i), t = w * zs.at(i + j);
					//zs.at(i) = zi + t;
					//zs.at(i + j) = zi - t;
				}
				sinIndex += sinStep;
			}
			sinStep >>= 1;
		}
		//if(n == 1)
		//{
		//	*out1 = *in1;
		//	*out2 = *in2;
		//	return;
		//}
		//const unsigned int nOver2 = n>>1;
		//// scrambling
		//for( unsigned int i = 0; i < nOver2; ++i)
		//{
		//	unsigned int iE = i<<1;
		//	unsigned int iO = iE + 1;
		//	out1[ i ] = in1[ iE ];
		//	out2[ i ] = in2[ iE ];
		//	out1[ i + nOver2 ] = in1[ iO ];
		//	out2[ i + nOver2 ] = in2[ iO ];
		//}
		//_Forward( out1, out2, in1, in2, nOver2 );
		//_Forward( out1 + nOver2, out2 + nOver2, in1 + nOver2, in2 + nOver2, nOver2 );
		//const unsigned int sinIndex = N / n;
		//const float& co = coss[ sinIndex ];
		//const float& si = sins[ sinIndex ];
		//for( unsigned int i = 0; i < nOver2; ++i )
		//{
		//	unsigned int iE = i;
		//	unsigned int iO = i + nOver2;
		//	float odd1 = co * in1[ iO ] + si * in2[iO];
		//	float odd2 = co * in2[ iO ] - si * in1[iO];
		//	out1[ iE ] = in1[ iE ] + odd1;
		//	out2[ iE ] = in2[ iE ] + odd2;
		//	out1[ iO ] = in1[ iE ] - odd1;
		//	out2[ iO ] = in2[ iE ] - odd2;
		//}
	}
public:
	void init() {
		
		float angle = .0f;
		float step = TWO_PI / N;
		for (unsigned int i = 0; i < N; ++i)
		{
			sins[i] = sinf(angle);
			coss[i] = cosf(angle);
			angle += step;
		}
		unsigned int nBitswaps = N>>1;
		for( unsigned int i = 0; i < nBitswaps; ++i )
		{
			bitSwaps[i] = bitReshuffle<N>(i);
		}
	}
	void forward(float* in1, float*in2, float*out1, float*out2)
	{
		//float tmp1[N];
		//float tmp2[N];
		static const unsigned int nBytes = sizeof(float) * N;
		memcpy( out1, in1, nBytes);
		memcpy( out2, in2, nBytes);
		_ForwardInplace( out1, out2);
	}
	void dft(float* in1, float* in2, float *out1, float *out2)
	{
		for (unsigned int k = 0; k < N; ++k)
		{
			float o1 = .0f;
			float o2 = .0f;
			for (unsigned int n = 0; n < N; ++n)
			{
				unsigned int index = (k*n) & (N-1);
				o1 += in1[n] * coss[index];
				o1 += in1[n] * sins[index];
				o2 -= in2[n] * sins[index];
				o2 += in2[n] * coss[index];
			}
			out1[k] = o1;
			out2[k] = o2;
		}
	}
};

class FFT8
{
	static const float sin45;
	float sins[8];
	float coss[8];
public:
	void init();
	void dft(float* in1, float* in2, float *out1, float *out2);
	void forward(float* in1, float*in2, float*out1, float*out2);
};
bool testFFT8();
bool testFFT1024();
template <unsigned int N>
bool testFFT()
{	
	FFT<N> fft;
	fft.init();
	float reIn[N];
	float imIn[N];
		
	float reOutDft[N];
	float imOutDft[N];
	float reOutFft[N];
	float imOutFft[N];
	Util::RandomGenerator gen;
	float tolerance = .001f;
	for (unsigned int ti = 0; ti < 100; ++ti)
	{
		bool failed = false;
		for (unsigned int i = 0; i < N; ++i)
		{
			reIn[i] = gen.getFloat(-.5f, .5f);
			imIn[i] = gen.getFloat(-.5f, .5f);
		}
		fft.dft(reIn, imIn, reOutDft, imOutDft);
		fft.forward(reIn, imIn, reOutFft, imOutFft);
		for (unsigned int i = 0; i < N; ++i)
		{
			float diff1 = reOutDft[i] - reOutFft[i];
			float diff2 = imOutDft[i] - imOutFft[i];
			if (abs(diff1) > tolerance || abs(diff2) > tolerance)
				failed = true;
		}
		if (failed)
		{
			return false;
		}
	}
	return true;
}
}