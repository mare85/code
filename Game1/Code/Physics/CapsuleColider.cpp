#include "CapsuleColider.h"

bool Physics::CapsuleColider::isPointInBounds(const vmath::Vector3 & pos) const
{
	vmath::Vector3 diff = pos - center_;
	float dotUp = vmath::dot(upVector_, diff);
	vmath::Vector3 flatDiff = diff - upVector_ * dotUp;
	float flatDist = vmath::length(flatDiff);
	float upDist = vmath::length(pos - _upCenter_);
	float downDist = vmath::length(pos - _upCenter_);
	return flatDist < radius_ && (
		fabsf(dotUp) < halfHeight_ || upDist < radius_ || downDist < radius_
		);
}
