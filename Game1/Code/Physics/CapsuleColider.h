#pragma once

#include <Util/Utils.h>
namespace Physics
{
	class CapsuleColider
	{
		vmath::Vector3 center_;
		vmath::Vector3 upVector_ = vmath::Vector3(.0f, 1.0f, .0f);

		vmath::Vector3 _upCenter_;
		vmath::Vector3 _downCenter_;
		float halfHeight_;
		float radius_;
		void _updateCenters() {
			vmath::Vector3 half = halfHeight_ * upVector_;
			_upCenter_ = center_ + half;
			_downCenter_ = center_ - half;
		}
	public:
		CapsuleColider(vmath::Vector3& center, float halfHeight, float radius)
			: center_( center )
			, halfHeight_( halfHeight )
			, radius_( radius )
		{}
		~CapsuleColider() {}

		void setUpVector(const vmath::Vector3& vec) { upVector_ = vec; }
		void setCenter(const vmath::Vector3 &center) { center_ = center; _updateCenters(); }
		void setHalfHeight(float val) { halfHeight_ = val; _updateCenters();  }
		void setRadius(float val) { radius_ = val; }
		bool isPointInBounds(const vmath::Vector3& pos) const;
	};

}