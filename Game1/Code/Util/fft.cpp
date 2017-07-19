#include "fft.h"
#include <cmath>
#include "RandomGenerator.h"

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

void Util::FFT8::init()
{
	float angle = .0f;
	float step = TWO_PI / 8;
	for (unsigned int i = 0; i < 8; ++i)
	{
		sins[i] = sinf(angle);
		coss[i] = cosf(angle);
		angle += step;
	}
}

void Util::FFT8::dft(float* in1, float* in2, float *out1, float *out2)
{
	for (unsigned int k = 0; k < 8; ++k)
	{
		float o1 = .0f;
		float o2 = .0f;
		for (unsigned int n = 0; n < 8; ++n)
		{
			unsigned int index = (k*n) & 7;
			o1 += in1[n] * coss[index];
			o2 -= in1[n] * sins[index];
			o1 += in2[n] * sins[index];
			o2 += in2[n] * coss[index];
		}
		out1[k] = o1;
		out2[k] = o2;
	}
}

void Util::FFT8::forward(float* r, float*i, float*out1, float*out2)
{
	float stack0 = r[0] + r[4];
	float stack1 = i[0] + i[4];
	float stack2 = r[2] + r[6];
	float stack3 = i[2] + i[6];
	float stack4 = stack0 + stack2;
	float stack5 = stack1 + stack3;
	float stack6 = stack0 - stack2;
	float stack7 = stack1 - stack3;
	stack0 = r[1] + r[5];
	stack1 = i[1] + i[5];
	stack2 = r[3] + r[7];
	stack3 = i[3] + i[7];
	float stack8 = stack0 + stack2;
	float stack9 = stack1 + stack3;
	out1[0] = stack4 + stack8;
	out1[4] = stack4 - stack8;
	out2[0] = stack5 + stack9;
	out2[4] = stack5 - stack9;
	stack4 = stack1 - stack3;
	stack5 = stack0 - stack2;
	out1[2] = stack6 + stack4;
	out1[6] = stack6 - stack4;
	out2[2] = stack7 - stack5;
	out2[6] = stack7 + stack5;
	stack0 = r[0] - r[4];
	stack1 = i[0] - i[4];
	stack2 = r[2] - r[6];
	stack3 = i[2] - i[6];
	stack4 = stack0 + stack3;
	stack5 = stack0 - stack3;
	stack6 = stack1 - stack2;
	stack7 = stack1 + stack2;
	out1[1] = stack4;
	out1[3] = stack5;
	out1[5] = stack4;
	out1[7] = stack5;
	out2[1] = stack6;
	out2[3] = stack7;
	out2[5] = stack6;
	out2[7] = stack7;


	stack0 = r[1] - r[5];
	stack1 = i[1] - i[5];
	stack2 = r[3] - r[7];
	stack3 = i[3] - i[7];

	

	stack4 = stack0 + stack1;
	stack5 = stack2 - stack3;
	
	
	stack6 = stack1 - stack0;
	stack7 = stack2 + stack3;
	stack8 = stack1 + stack0;
	

 



	out1[1] += sin45 * (stack4 - stack5);
	out1[3] += sin45 * (stack6 + stack7);
	out1[5] += sin45 * (stack5 - stack8);
	out1[7] += sin45 * (-stack6 - stack7);
	out2[1] += sin45 * (stack6 - stack7);
	out2[3] += sin45 * (-stack8 - stack5);
	out2[5] += sin45 * (-stack6 + stack7);
	out2[7] += sin45 * (stack4 + stack5);
}



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