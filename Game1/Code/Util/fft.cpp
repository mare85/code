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

void Util::FFT1024::_Forward(float* in1, float*in2, float*out1, float*out2, unsigned int step, unsigned int n)
{
// 	if (n == 1)
// 	{
// 		*out1 = *in1;
// 		*out2 = *in2;
// 	}
// 	else
// 	{
// 		unsigned int step2 = step << 1;
// 		unsigned int n2 = n>>1;
// 		_Forward(in1, in2, out1, out2, step2, n2);
// 		_Forward(in1 + step, in2 + step, out1 + n2, out2 + n2, step2, n2);
// 		for (unsigned int k1 = 0; k1 < n2; ++k1)
// 		{
// 			unsigned int k2 = k1+ n2;
// 			float even1 = out1[k1];
// 			float even2 = out2[k1];
// 			float odd1 = out1[k2];
// 			float odd2 = out2[k2];
// 			out1[k1] = even1 + odd1 * coss[k2] + odd2 * sins[k2];
// 			out2[k1] = even2 + odd2 * coss[k2] - odd1 * sins[k2];
// 			out1[k2] = even1 - odd1 * coss[k2] + odd2 * sins[k2];
// 			out2[k2] = even1 - odd2 * coss[k2] - odd1 * sins[k2];
//			still wrong
// 		}
//
// 	}
// 	
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
	//float temp1[1024];
	//float temp2[1024];
	//float* tempIn1 = temp1;
	//float* tempIn2 = temp2;
	//for (unsigned int i = 0; i < 1024; ++i)
	//{
	//	tempIn1[i] = in1[i];
	//	tempIn2[i] = in2[i];
	//}

	//float* tempOut1 = out1;
	//float* tempOut2 = out2;
	//unsigned int iterationCount = 512;
	//for (unsigned int it = 2; it < 1024; it <<= 1)
	//{
	//	for (unsigned int i = 0; i < iterationCount; ++i)
	//	{
	//		float* o1 = tempOut1 + i;
	//		float* o2 = tempOut2 + i;
	//		for (unsigned int k = 0; k < 1024; k += iterationCount)
	//		{
	//			// 		out1[k] = o1 * coss[1] + o2 * sins[1] + e1;
	//			// 		out2[k] = o2 * coss[1] - o1 * sins[1] + e2;
	//			o1 += in1[n] * coss[index];
	//			o2 -= in1[n] * sins[index];
	//			o1 += in2[n] * sins[index];
	//			o2 += in2[n] * coss[index];
	//			float tRe = coss[1]
	//			tempOut[i + ] = 



	//		}
	//	}


	//	float* tmpPtr = tempIn1;
	//	tempIn1 = tempOut1;
	//	tempOut1 = tmpPtr;
	//	tmpPtr = tempIn2;
	//	tempIn2 = tempOut2;
	//	tempOut2 = tmpPtr;
	//	iterationCount >>= 1;
	//}
// 	for (unsigned int k = 0; k < 512; ++k)
// 	{
// 
// 	}
// 	for (unsigned int k = 512; k < 1024; ++k)
// 	{
// 
// 	}
// 
// 
// 	for (unsigned int k = 0; k < 1024; ++k)
// 	{
// 		float o1 = .0f;
// 		float o2 = .0f;
// 		float e1 = .0f;
// 		float e2 = .0f;
// 
// 		for (unsigned int n = 0; n < 512; ++n)
// 		{
// 			unsigned int indexE = ((k*n) & 1023) << 1;
// 			unsigned int indexO = indexE + 1;
// 			e1 += in1[indexE] * coss[indexE];
// 			e2 -= in1[indexE] * sins[indexE];
// 			e1 += in2[indexE] * sins[indexE];
// 			e2 += in2[indexE] * coss[indexE];
// 			o1 += in1[indexO] * coss[indexE];
// 			o2 -= in1[indexO] * sins[indexE];
// 			o1 += in2[indexO] * sins[indexE];
// 			o2 += in2[indexO] * coss[indexE];
// 		}
// 		out1[k] = o1 * coss[1] + o2 * sins[1] + e1;
// 		out2[k] = o2 * coss[1] - o1 * sins[1] + e2;
// 	}
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