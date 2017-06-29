#include "Utils.h"
#include <string.h>
#include <Util/RandomGenerator.h>
#include <cstdio>


static unsigned int __primes[ 1024 ];

void Util::readFile(const char * filename, char ** buffer, unsigned int * size)
{
	FILE* f = nullptr;
	
	fopen_s(&f, filename, "r");
	if (!f)
	{
		*size = 0;
		*buffer = 0;
		return;
	}
	fseek(f, 0, SEEK_END);
	*size = ftell(f);
	fseek(f, 0, SEEK_SET);
	char* fileBuffer = new char[*size +1 ];
	fread_s(fileBuffer, *size, 1, *size, f);
	fileBuffer[*size] = 0;
	fclose(f);
	*buffer = fileBuffer;
}

void Util::releaseFileBuffer( char * buffer )
{
	delete buffer;
}

void Util::initHash()
{
	RandomGenerator gen(0xfeedbadd);

	for( unsigned int i = 0; i < 1024; ++i )
	{
		__primes[ i ] = gen.getUint( 0xffffffff);
	}
}

unsigned int Util::createHash(const char* name)
{
	unsigned int nameLen = ( unsigned int )strlen(name);
	unsigned int hash = 0;
	for (unsigned int i = 0; i < nameLen; ++i)
	{
		unsigned int prime = __primes[i & 1023];
		hash += prime * name[i];
	}
	return hash;
}
	
vmath::Vector3 Util::bezier3p(const vmath::Vector3 & x1, const vmath::Vector3 & x2, const vmath::Vector3 & x3, float t)
{
	float a1 = 1.0f - t;
	float a3 = t*t;
	float a2 = a1*t*2.0f;
	a1 *= a1;
	return x1*a1 + x2*a2 + x3*a3;
}

vmath::Vector3 Util::bezier4p(const vmath::Vector3 & x1, const vmath::Vector3 & x2, const vmath::Vector3 & x3, const vmath::Vector3 & x4, float t)
{
	float t2 = t*t;
	float t_ = 1.0f - t;
	float t_2 = t_ * t_;

	float a1 = t_2 * t_;
	float a2 = 3.0f * t_2 * t;
	float a3 = 3.0f * t_ * t2;
	float a4 = t * t2;
		
	return x1*a1 + x2*a2 + x3*a3 + x4 * a4;
}

void Util::fillBezier(vmath::Vector4 * out, const vmath::Vector4 * in, unsigned int steps)
{
	vmath::Vector3 x1 = in[0].getXYZ();
	vmath::Vector3 x2 = in[1].getXYZ();
	vmath::Vector3 x3 = in[2].getXYZ();
	vmath::Vector3 x4 = in[3].getXYZ();

	vmath::Vector3 dir21 = vmath::normalize(x2 - x1);
	vmath::Vector3 dir32 = vmath::normalize(x3 - x2);
	vmath::Vector3 dir43 = vmath::normalize(x4 - x3);
		
	vmath::Vector3 side1 = vmath::rotate( vmath::Quat::rotationZ(1.57f), dir21 ) * in[0].getW();
	vmath::Vector3 side2 = vmath::rotate( vmath::Quat::rotationZ(1.57f), normalize( dir21 + dir32 ) ) * in[1].getW();
	vmath::Vector3 side3 = vmath::rotate( vmath::Quat::rotationZ(1.57f), normalize(dir43 + dir32)) * in[2].getW();
	vmath::Vector3 side4 = vmath::rotate( vmath::Quat::rotationZ(1.57f), dir43 ) * in[3].getW();

	vmath::Vector3 a1 = x1 - side1;
	vmath::Vector3 a2 = x2 - side2;
	vmath::Vector3 a3 = x3 - side3;
	vmath::Vector3 a4 = x4 - side4;

	vmath::Vector3 b1 = x1 + side1;
	vmath::Vector3 b2 = x2 + side2;
	vmath::Vector3 b3 = x3 + side3;
	vmath::Vector3 b4 = x4 + side4;

	float tstep = 1.0f / (steps - 1.0f);
	vmath::Vector3 aPrev = a1;
	vmath::Vector3 bPrev = b1;
	vmath::Vector4* ptr = out;

	for (unsigned int i = 0; i < steps; ++i)
	{
		float t = i * tstep;
		vmath::Vector3 aNext = bezier4p(a1, a2, a3, a4, t);
		vmath::Vector3 bNext = bezier4p(b1, b2, b3, b4, t);
		ptr[0] = vmath::Vector4( aPrev, 1.0f );
		ptr[1] = vmath::Vector4( bNext, 1.0f );
		ptr[2] = vmath::Vector4( aNext, 1.0f );
		ptr[3] = vmath::Vector4( aPrev, 1.0f );
		ptr[4] = vmath::Vector4( bPrev, 1.0f );
		ptr[5] = vmath::Vector4( bNext, 1.0f );

		ptr += 6;
		aPrev = aNext;
		bPrev = bNext;

	}

}

void Util::fillBezierStrip(vmath::Vector4 * out, const vmath::Vector4 * in, unsigned int steps)
{
	vmath::Vector3 x1 = in[0].getXYZ();
	vmath::Vector3 x2 = in[1].getXYZ();
	vmath::Vector3 x3 = in[2].getXYZ();
	vmath::Vector3 x4 = in[3].getXYZ();

	vmath::Vector3 dir21 = vmath::normalize(x2 - x1);
	vmath::Vector3 dir32 = vmath::normalize(x3 - x2);
	vmath::Vector3 dir43 = vmath::normalize(x4 - x3);

	vmath::Vector3 side1 = vmath::rotate(vmath::Quat::rotationZ(1.57f), dir21) * in[0].getW();
	vmath::Vector3 side2 = vmath::rotate(vmath::Quat::rotationZ(1.57f), normalize(dir21 + dir32)) * in[1].getW();
	vmath::Vector3 side3 = vmath::rotate(vmath::Quat::rotationZ(1.57f), normalize(dir43 + dir32)) * in[2].getW();
	vmath::Vector3 side4 = vmath::rotate(vmath::Quat::rotationZ(1.57f), dir43) * in[3].getW();

	vmath::Vector3 a1 = x1 - side1;
	vmath::Vector3 a2 = x2 - side2;
	vmath::Vector3 a3 = x3 - side3;
	vmath::Vector3 a4 = x4 - side4;

	vmath::Vector3 b1 = x1 + side1;
	vmath::Vector3 b2 = x2 + side2;
	vmath::Vector3 b3 = x3 + side3;
	vmath::Vector3 b4 = x4 + side4;

	float tstep = 1.0f / (steps - 3.0f);

	vmath::Vector4* ptr = out;
	ptr[0] = vmath::Vector4(a1, 1.0f);
	ptr[1] = vmath::Vector4(a1, 1.0f);
	ptr += 2;
	for (unsigned int i = 0; i < steps - 2; ++i)
	{
		float t = i * tstep;
		vmath::Vector3 aNext = bezier4p(a1, a2, a3, a4, t);
		vmath::Vector3 bNext = bezier4p(b1, b2, b3, b4, t);
		ptr[0] = vmath::Vector4(aNext, 1.0f);
		ptr[1] = vmath::Vector4(bNext, 1.0f);
		ptr += 2;
	}
	ptr[0] = vmath::Vector4(a4, 1.0f);;
	ptr[1] = vmath::Vector4(a4, 1.0f);;
}

void Util::utf8RawToLatin2Pl(unsigned char * utf8, unsigned char * latin2, unsigned int maxOutSize)
{
	bool contin = true;
	unsigned int inIndex = 0;
	unsigned int outIndex = 0;
	while (contin)
	{
		unsigned char first = utf8[inIndex];
		unsigned char second = utf8[inIndex + 1];
		if (first == 0xC4 && second == 0x84) { latin2[outIndex] = 0xA1; outIndex++; inIndex += 2; }//A
		else if (first == 0xC4 && second == 0x85) { latin2[outIndex] = 0xB1; outIndex++; inIndex += 2; }//a
		else if (first == 0xC4 && second == 0x86) { latin2[outIndex] = 0xC6; outIndex++; inIndex += 2; }//C
		else if (first == 0xC4 && second == 0x87) { latin2[outIndex] = 0xE6; outIndex++; inIndex += 2; }//c
		else if (first == 0xC4 && second == 0x98) { latin2[outIndex] = 0xC9; outIndex++; inIndex += 2; }//E
		else if (first == 0xC4 && second == 0x99) { latin2[outIndex] = 0xE9; outIndex++; inIndex += 2; }//e
		else if (first == 0xC5 && second == 0x83) { latin2[outIndex] = 0xD1; outIndex++; inIndex += 2; }//N
		else if (first == 0xC5 && second == 0x84) { latin2[outIndex] = 0xF1; outIndex++; inIndex += 2; }//n
		else if (first == 0xC5 && second == 0x9A) { latin2[outIndex] = 0xA6; outIndex++; inIndex += 2; }//S
		else if (first == 0xC5 && second == 0x9B) { latin2[outIndex] = 0xB6; outIndex++; inIndex += 2; }//s
		else if (first == 0xC5 && second == 0x81) { latin2[outIndex] = 0xA3; outIndex++; inIndex += 2; }//£
		else if (first == 0xC5 && second == 0x82) { latin2[outIndex] = 0xB3; outIndex++; inIndex += 2; }//³
		else if (first == 0xC3 && second == 0x93) { latin2[outIndex] = 0xD3; outIndex++; inIndex += 2; }//Ó
		else if (first == 0xC3 && second == 0xB3) { latin2[outIndex] = 0xF3; outIndex++; inIndex += 2; }//ó
		else if (first == 0xC5 && second == 0xBB) { latin2[outIndex] = 0xAF; outIndex++; inIndex += 2; }//Z
		else if (first == 0xC5 && second == 0xBC) { latin2[outIndex] = 0xBF; outIndex++; inIndex += 2; }//z
		else if (first == 0xC5 && second == 0xB9) { latin2[outIndex] = 0xAC; outIndex++; inIndex += 2; }//X
		else if (first == 0xC5 && second == 0xBA) { latin2[outIndex] = 0xBC; outIndex++; inIndex += 2; }//x
		else
		{
			latin2[outIndex] = first;
			inIndex += 1;
			outIndex++;
		}
		if (first == 0  || outIndex == maxOutSize - 1)
		{
			contin = false;
		}
	}
	latin2[outIndex] = 0;
	
}

void Util::fillHair(
	vmath::Vector4 * out, 
	unsigned int steps, 
	const vmath::Vector3 & x1, 
	const vmath::Vector3 & x2, 
	const vmath::Vector3 & x3, 
	float startWidth,
	float rnd

	)
{
		
	vmath::Vector3 side1 = vmath::Matrix3::rotationZ( 1.57f) * vmath::normalize(x2 - x1);
	vmath::Vector3 side2 = vmath::Matrix3::rotationZ(1.57f) * vmath::normalize(x3 - x2);
	side2 = normalize(side1 + side2);
	side1 *= startWidth * .5f;
	side2 *= startWidth * .25f;
	vmath::Vector3 a1 = x1 - side1;
	vmath::Vector3 a2 = x2 - side2;
	vmath::Vector3 b1 = x1 + side1;
	vmath::Vector3 b2 = x2 + side2;
	float tstep = 1.0f / (steps - 1.0f);
	vmath::Vector4* ptr = out;
	ptr[0] = vmath::Vector4( a1, rnd );
	ptr[1] = vmath::Vector4( a1, rnd );
	float len = length( x3 - x1 );
	ptr += 2;
		
	for (unsigned int i = 0; i < steps -2; ++i)
	{
		float t = i * tstep;
		vmath::Vector3 aNext = bezier3p(a1, a2, x3, t);
		vmath::Vector3 bNext = bezier3p(b1, b2, x3, t);
		aNext.setZ( t * len * .2f );
		bNext.setZ( t * len * .2f );
			
			
		ptr[0] = vmath::Vector4( aNext, rnd );
		ptr[1] = vmath::Vector4( bNext, rnd );
		ptr += 2;
	}
	vmath::Vector3 aNext = x3;
	aNext.setZ( len * .2f );
	ptr[0] = vmath::Vector4( aNext, rnd );
	ptr[1] = vmath::Vector4( aNext, rnd );
}
