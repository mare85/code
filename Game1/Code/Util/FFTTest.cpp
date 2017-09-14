#include <Util/FFTTest.h>
/*
 * (C) Copytight 2017 Marek Bielawski
 * Util::FFTTest
 */
namespace Util {

bool Util::testFFT8()
{
	FFT8 fft8;
	fft8.init();
	float reIn[8];
	float imIn[8];

	float reOutDft[8];
	float imOutDft[8];
	float reOutFft[8];
	float imOutFft[8];
	Util::RandomGenerator gen;
	float tolerance = .001f;
	for (unsigned int ti = 0; ti < 100; ++ti)
	{
		bool failed = false;
		for (unsigned int i = 0; i < 8; ++i)
		{
			reIn[i] = gen.getFloat(-.5f, .5f);
			imIn[i] = gen.getFloat(-.5f, .5f);
		}
		fft8.dft(reIn, imIn, reOutDft, imOutDft);
		fft8.forward(reIn, imIn, reOutFft, imOutFft);
		for (unsigned int i = 0; i < 8; ++i)
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
