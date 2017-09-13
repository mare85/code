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
	unsigned int bitSwaps[N];
	void _ForwardInplace( float* re, float* im ) 
	{
		for( unsigned int i = 0; i < N; ++i )
		{
			unsigned int j = bitSwaps[i];
			if( i < j )
			{
				std::swap( re[i], re[j] );
				std::swap( im[i], im[j] );
			}
		}
		unsigned int sinStep = N>>1;
		for( unsigned int spread = 1; spread < N; spread <<= 1 )
		{
			unsigned int sinIndex = 0;
			unsigned int step = spread<<1;
			for( unsigned int k = 0; k < spread; ++k ) 
			{
				float& co = coss[ sinIndex ];
				float& si = sins[ sinIndex ];
				for(unsigned int i = k; i < N; i += step) 
				{
					float reE = re[i], imE = im[i];
					float reO = re[i + spread], imO = im[i + spread];
					float reOddIngredient = co * reO + si * imO;
					float imOddIngredient = -si * reO + co * imO;
					re[ i ] = reE + reOddIngredient;
					im[ i ] = imE + imOddIngredient;
					re[ i + spread ] = reE - reOddIngredient;
					im[ i + spread ] = imE - imOddIngredient;
				}
				sinIndex += sinStep;
			}
			sinStep >>= 1;
		}
	}
	void _ReverseInplace( float* re, float* im )
	{
		for( unsigned int i = 0; i < N; ++i )
		{
			unsigned int j = bitSwaps[i];
			if( i < j )
			{
				std::swap( re[i], re[j] );
				std::swap( im[i], im[j] );
			}
		}
		unsigned int sinStep = N>>1;
		for( unsigned int spread = 1; spread < N; spread <<= 1 )
		{
			unsigned int sinIndex = 0;
			unsigned int step = spread<<1;
			for( unsigned int k = 0; k < spread; ++k ) 
			{
				float& co = coss[ sinIndex ];
				float& si = sins[ sinIndex ];
				for(unsigned int i = k; i < N; i += step) 
				{
					float reE = re[i], imE = im[i];
					float reO = re[i + spread], imO = im[i + spread];
					float reOddIngredient = co * reO - si * imO;
					float imOddIngredient = si * reO + co * imO;
					re[ i ] = reE + reOddIngredient;
					im[ i ] = imE + imOddIngredient;
					re[ i + spread ] = reE - reOddIngredient;
					im[ i + spread ] = imE - imOddIngredient;
				}
				sinIndex += sinStep;
			}
			sinStep >>= 1;
		}
		float scale = 1.0f / N;
		for( unsigned int i =0; i < N; ++i )
		{
			re[ i ] *= scale;
			im[ i ] *= scale;
		}
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
		for( unsigned int i = 0; i < N; ++i )
		{
			bitSwaps[i] = bitReshuffle<N>(i);
		}
	}
	void forward( float* in1, float* in2, float* out1, float* out2 )
	{
		static const unsigned int nBytes = sizeof(float) * N;
		memcpy( out1, in1, nBytes);
		memcpy( out2, in2, nBytes);
		_ForwardInplace( out1, out2);
	}
	void reverse( float* in1, float* in2, float* out1, float* out2 )
	{
		static const unsigned int nBytes = sizeof(float) * N;
		memcpy( out1, in1, nBytes);
		memcpy( out2, in2, nBytes);
		_ReverseInplace( out1, out2);
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
				o1 += in2[n] * sins[index];
				o2 -= in1[n] * sins[index];
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
	float re2[N];
	float im2[N];
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
		fft.reverse( reOutFft, imOutFft, re2, im2 );
		for (unsigned int i = 0; i < N; ++i)
		{
			float diff1 = reIn[i] - re2[i];
			float diff2 = imIn[i] - im2[i];
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