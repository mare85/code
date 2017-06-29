#pragma once

#include <Util/Utils.h>
#include <Black/GroundManager.h>

namespace Black
{


	class CubicFunctionColider
	{
	public:
		struct TopDescriptor
		{
			vmath::Vector3 normalLeft;
			vmath::Vector3 normalRight;
			vmath::Vector3 posLeft;
			vmath::Vector3 posRight;
		};

	private:
		float xMin_ = -1.0f;
		float xMax_ = 1.0f;
		float width_ = 2.0f;
		float widthInv_ = .5f;
		float functionTop_[4];
		float functionBottom_[4];
		
		bool hasBottom_;

	public:
		CubicFunctionColider(){};
		CubicFunctionColider(TopDescriptor& desc) {
			setup( desc);
		}

		void setup( TopDescriptor& desc)
		{
			xMin_ = desc.posLeft.getX();
			xMax_ = desc.posRight.getX();
			width_ = xMax_ - xMin_;
			widthInv_ = 1.0f / width_;
			float startA = widthInv_ * -desc.normalLeft.getX() / desc.normalLeft.getY() * 4.0f;
			float endA = widthInv_ * -desc.normalRight.getX() / desc.normalRight.getY() * 4.0f;
			
			
			float heightDiff = desc.posRight.getY() - desc.posLeft.getY();
			float heightLeft = desc.posLeft.getY();
			functionTop_[3] = startA + endA - 2.0f * heightDiff;
			functionTop_[2] = -2.0f * startA - endA + 3.0f * heightDiff;
			functionTop_[1] = startA;
			functionTop_[0] = heightLeft;

		}

		~CubicFunctionColider() {}

		float getTopHeight( float x ) { 
			float t = (x - xMin_) * widthInv_; 
			float t2 = t*t;
			return t2*t*functionTop_[ 3 ] + t2 * functionTop_[ 2 ]  + t*functionTop_[ 1 ] + functionTop_[ 0 ];
		}
		float getXMin() const { return xMin_; }
		float getXMax() const { return xMax_; }
		float getWidth() const { return width_; }
	};


	struct Ground
	{
		float xMin_ = -GroundManager::WorldWrap;
		float xMax_ =  GroundManager::WorldWrap;
		float width_ = GroundManager::WorldWidth;
		enum {
			eNColliders = GroundManager::eNSectors
		};

		CubicFunctionColider colliders_[ eNColliders ];

		void generateRandom( unsigned int seed = 0xb3dc107d );
		void generate(float* height);
		float getTopHeight(float x);
		unsigned int getColliderIndex(float x);

		

	};

}

