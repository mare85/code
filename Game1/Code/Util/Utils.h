#pragma once
#include <vminclude.h>
namespace vmath = Vectormath::Aos;
namespace Util
{
	void readFile(const char* filename, char** buffer, unsigned int* size);
	void releaseFileBuffer(char* buffer );
	void initHash();
	unsigned int createHash(const char*);

	inline float clamp(float val, float min, float max) { return fmaxf(fminf(val, max), min); }
	inline vmath::Vector3 limitVector3(const vmath::Vector3& vec, float maxLen)
	{
		float len = fmaxf( length(vec), 1.e-8f );
		return vmath::select(vec, vec * maxLen / len, len > maxLen);
	}

	vmath::Vector3 bezier3p( 
		const vmath::Vector3& x1,
		const vmath::Vector3& x2,
		const vmath::Vector3& x3,
		float t
		);

	vmath::Vector3 bezier4p(
		const vmath::Vector3& x1,
		const vmath::Vector3& x2,
		const vmath::Vector3& x3,
		const vmath::Vector3& x4,
		float t
		);


	void fillBezier(vmath::Vector4* out,
		const vmath::Vector4* in, 
		unsigned int steps
		);

	void fillBezierStrip(vmath::Vector4* out,
		const vmath::Vector4* in,
		unsigned int steps
		);

	void utf8RawToLatin2Pl(unsigned char * utf8, unsigned char * latin2, unsigned int maxOutSize);
	void fillHair(
		vmath::Vector4* out,
		unsigned int steps,
		const vmath::Vector3& x1,
		const vmath::Vector3& x2,
		const vmath::Vector3& x3,
		float startWidth,
		float rnd = .0f

		);
}