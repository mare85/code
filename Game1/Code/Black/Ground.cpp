#include "Ground.h"
#include <Util/RandomGenerator.h>

void Black::Ground::generateRandom( unsigned int seed)
{
	Util::RandomGenerator gen(seed);
	vmath::Vector3 prevnrm ( .0f, 1.0f, .0f);
	width_ = xMax_ - xMin_;
	vmath::Vector3 prevpos (xMin_, .0f, .0f);
	float xStep = (width_) / (eNColliders);
	vmath::Vector3 pos (xMin_ + xStep, .0f, .0f);
	

	for( unsigned int i = 0; i < eNColliders; ++i )
	{
		vmath::Vector3 nextPos = pos + vmath::Vector3( xStep, gen.getFloat( -.5f, .5f ), .0f );
		
		nextPos.setY(gen.getFloat(-1.5f, 1.5f));
		vmath::Vector3 diff = nextPos - prevpos;
		vmath::Vector3 nrm = normalize( diff );
		nrm = vmath::Vector3( -nrm.getY(), nrm.getX(), .0f );
		CubicFunctionColider::TopDescriptor desc;
		
		desc.normalLeft = prevnrm;

		desc.normalRight = nrm;
		desc.posLeft = prevpos;
		if (i == eNColliders - 1)
		{
			pos.setY(.0f);
			desc.normalRight = vmath::Vector3(.0f, 1.0f, .0f);

		}
		desc.posRight = pos;
		
		colliders_[i].setup( desc );
		prevpos = pos;
		pos = nextPos;
		prevnrm = nrm;
	}
	if (xMax_ != pos.getX())
	{
		unsigned int i = 12312;
		++i;
		--i;
	}
}

void Black::Ground::generate(float * height)
{
	vmath::Vector3 prevnrm(.0f, 1.0f, .0f);
	width_ = xMax_ - xMin_;
	vmath::Vector3 prevpos(xMin_, .0f, .0f);
	float xStep = (width_) / (eNColliders);
	vmath::Vector3 pos(xMin_ + xStep, .0f, .0f);

	for (unsigned int i = 0; i < eNColliders; ++i)
	{
		vmath::Vector3 nextPos = pos + vmath::Vector3(xStep, .0f, .0f);

		//nextPos.setY(height[ i ] + (step?1.0f: .0f));
		nextPos.setY(height[i]);
		vmath::Vector3 diff = nextPos - prevpos;
		vmath::Vector3 nrm = normalize(diff);
		nrm = vmath::Vector3(-nrm.getY(), nrm.getX(), .0f);
		CubicFunctionColider::TopDescriptor desc;

		desc.normalLeft = prevnrm;

		desc.normalRight = nrm;
		desc.posLeft = prevpos;
		if (i == eNColliders - 1)
		{
			pos.setY(.0f);
			desc.normalRight = vmath::Vector3(.0f, 1.0f, .0f);

		}
		desc.posRight = pos;

		colliders_[i].setup(desc);
		prevpos = pos;
		pos = nextPos;
		prevnrm = nrm;
	}
}

//float __correction = .42f;
float __correction = .0f;

float Black::Ground::getTopHeight(float x)
{
	float xCorrect = x +__correction;
	if (xCorrect > Black::GroundManager::WorldWrap)
	{
		xCorrect -= Black::GroundManager::WorldWidth;
	}
	else if (xCorrect < -Black::GroundManager::WorldWrap)
	{
		xCorrect += Black::GroundManager::WorldWidth;
	}
	unsigned int ix = (unsigned int)floorf((eNColliders ) * (xCorrect - xMin_) / width_);
	ix = min(ix, eNColliders - 1U);
	return colliders_[ix].getTopHeight(xCorrect);

}

unsigned int Black::Ground::getColliderIndex(float x)
{
	float xCorrect = x + __correction;
	if (xCorrect > Black::GroundManager::WorldWrap)
	{
		xCorrect -= Black::GroundManager::WorldWidth;
	}
	else if (xCorrect < -Black::GroundManager::WorldWrap)
	{
		xCorrect += Black::GroundManager::WorldWidth;
	}
	int ix = (int)floorf((eNColliders)* (xCorrect - xMin_) / width_);
	ix = max( min(ix, eNColliders - 1), 0 );
	return (unsigned int)ix;
}
