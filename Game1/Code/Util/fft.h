#pragma once


namespace Util
{

	static void dft(float* in1, float* in2, float *out1, float *out2, unsigned int nSamples);

	class FFT1024
	{
		float sins[1024];
		float coss[1024];
		void _Forward(float* in1, float*in2, float*out1, float*out2, unsigned int step, unsigned int n);
	public:
		void init();
		void forward(float* in1, float*in2, float*out1, float*out2);
		void dft(float* in1, float* in2, float *out1, float *out2);

	};

	class FFT8
	{
		const float sin45 = .707106f;
		float sins[8];
		float coss[8];
	public:
		void init();
		void dft(float* in1, float* in2, float *out1, float *out2);
		void forward(float* in1, float*in2, float*out1, float*out2);

	};


}