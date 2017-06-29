#include "fft.h"
#include <cmath>

void Util::dft(float * in1, float * in2, float * out1, float * out2, unsigned int nSamples)
{
	static const float TWO_PI = 6.28318f;
	for (unsigned int k = 0; k < nSamples; ++k)
	{
		
		float o1 = .0f;
		float o2 = .0f;
		for (unsigned int n = 0; n < nSamples; ++n)
		{
			o1 += in1[n] * cosf(TWO_PI * k * n / nSamples);
			o2 -= in2[n] * sinf(TWO_PI * k * n / nSamples);
		}
		out1[k] = o1;
		out2[k] = o2;
	}

}
