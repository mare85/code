#include "Camera.h"
#include <Game/Transform.h>
namespace Game
{
	vmath::Matrix4 Camera::getView() const 
	{
		return vmath::affineInverse( transform_->getWorldPose() );
	}
	vmath::Matrix4 Camera::getProjection(float aspect) const
	{
		if (isOrtographic_)
		{
			float extendX = extend_ * aspect;
			float extendY = extend_;
			return vmath::Matrix4::orthographic(
				-extendX, extendX,
				-extendY, extendY,
				zExtend_, -zExtend_);
		}
		else
		{
			return vmath::Matrix4::perspective(fov_, aspect, zNear_, zFar_);
		}
	}
}