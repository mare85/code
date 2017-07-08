#include "fft.h"
#include <cmath>

static const float TWO_PI = 6.28318f;

void Util::dft(float * in1, float * in2, float * out1, float * out2, unsigned int nSamples)
{
	for (unsigned int k = 0; k < nSamples; ++k)
	{
		float o1 = .0f;
		float o2 = .0f;
		for (unsigned int n = 0; n < nSamples; ++n)
		{
			o1 += in1[n] * cosf(TWO_PI * k * n / nSamples);
			o2 -= in1[n] * sinf(TWO_PI * k * n / nSamples);
			o1 += in2[n] * sinf(TWO_PI * k * n / nSamples);
			o2 += in2[n] * cosf(TWO_PI * k * n / nSamples);

		}
		out1[k] = o1;
		out2[k] = o2;
	}

}

void Util::FFT1024::init()
{
	float angle = .0f;
	float step = TWO_PI / 1024;
	for (unsigned int i = 0; i < 1024; ++i)
	{
		sins[i] = sinf(angle);
		coss[i] = sinf(angle);
		angle += step;
	}
}

void Util::FFT1024::forward(float * in1, float * in2, float * out1, float * out2)
{
}

void Util::FFT1024::dft(float * in1, float * in2, float * out1, float * out2)
{

	for (unsigned int k = 0; k < 1024; ++k)
	{
		float o1 = .0f;
		float o2 = .0f;
		for (unsigned int n = 0; n < 1024; ++n)
		{
			unsigned int index = (k*n) & 1023;
			o1 += in1[n] * coss[index];
			o2 -= in1[n] * sins[index];
			o1 += in2[n] * sins[index];
			o2 += in2[n] * coss[index];
		}
		out1[k] = o1;
		out2[k] = o2;
	}
}