#pragma once


namespace Util
{

	static void dft(float* in1, float* in2, float *out1, float *out2, unsigned int nSamples);

	class FFT1024
	{
		float sins[1024];
		float coss[1024];
		void _Forward(float* in1, float*in2, float*out1, float*out2, unsigned int step);
	public:
		void init();
		void forward(float* in1, float*in2, float*out1, float*out2);
		void dft(float* in1, float* in2, float *out1, float *out2);

		

	};


}