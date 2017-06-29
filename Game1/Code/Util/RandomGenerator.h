#pragma once

#include <Util/Utils.h>

namespace Util
{
	class RandomGenerator
	{
		unsigned int a = 0x2389ebc1;
		unsigned int b = 0xfb379523;
		unsigned int c = 0x891a5ac7;

		unsigned int _next() {
			a += b * ( a % c );
			b *= c + a;
			b += c;
			return a;
		};
	public:
		
		RandomGenerator(unsigned int seed) : a(seed) {}
		RandomGenerator() : a(123458923) {}
		~RandomGenerator() {}
		unsigned int getUint(unsigned int max) { return _next() % max; }
		float getFloat(float min, float max) { 
			float float01 = _next() * 1.0f / (unsigned int)-1;
			float diff = max - min;
			return float01 * diff + min;
		}
		float getFloat(float max) {
			float float01 = _next() * 1.0f / (unsigned int)-1;
			return float01 * max;
		}
		float getFloat() {
			return _next() * 1.0f / (unsigned int)-1;
		}
		vmath::Vector3 getVector3(float reach)
		{
			return vmath::Vector3(
				getFloat( -reach, reach ),
				getFloat(-reach, reach),
				getFloat(-reach, reach)
				);
		}
	};

}