#include "Transform.h"
#include <Game/Object.h>

namespace Game
{
	Transform::Transform(const char * name)
	{
		strcpy_s(name_, name);
		hashName_ = Util::createHash(name);
	}
	void Transform::computeLocalPose()
	{
		localPose_ = vmath::Matrix4(
			vmath::Matrix3( rotation_ ) * vmath::Matrix3::scale( scale_ ),
			translation_
			);
	}
	void Transform::computeWorldPose()
	{
		if (parent_ == nullptr)
			worldPose_ = localPose_;
		else
		{
			worldPose_ = parent_->worldPose_ * vmath::Matrix4(localPose_);
		}
		for (Transform* tr : children_)
		{
			tr->computeWorldPose();
		}
	}
	void Transform::carryObject(Object * obj)
	{
		object_ = obj;
		obj->setTransform(this);
	}

	void Transform::removeChild(Transform* child)
	{
		children_.remove( child );
		child->parent_ = nullptr;
	}

}
