#ifndef UTIL_FFTTEST_DEF
#define UTIL_FFTTEST_DEF
/*
 * (C) Copytight 2017 Marek Bielawski
 * Util::FFTTest
 */
#include <Util/FFT.h>
namespace Util {

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
	float tolerance = .003f;
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

bool testFFT8();

}
#endif
