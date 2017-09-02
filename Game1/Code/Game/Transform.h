#pragma once

#include <list>
#include <map>
#include <Util/Utils.h>

namespace Game 
{
	class Transform;
	class Scene;
	class Object;
	typedef std::list<Game::Transform*> TransformList;
	typedef std::map<unsigned int, Game::Transform*> TransformMap;

	class Transform
	{
		char name_ [ 250 ];
		unsigned int hashName_;

		Transform* parent_ = nullptr;
		Game::Scene* scene_ = nullptr;
		TransformList children_;
		Game::Object* object_;
		vmath::Vector3 translation_ = vmath::Vector3( .0f );
		vmath::Quat rotation_ = vmath::Quat::identity();
		vmath::Vector3 scale_ = vmath::Vector3(1.0f);
	

		vmath::Matrix4 localPose_ = vmath::Matrix4::identity();
		vmath::Matrix4 worldPose_ = vmath::Matrix4::identity();

		bool visible_ = true;

	public:
		Transform(const char* name);
		~Transform() {}

		void computeLocalPose();
		void computeWorldPose();

		vmath::Matrix4 getWorldPose() const { return worldPose_; }
		vmath::Vector3 getTranslation() const { return translation_; }
		void setTranslation(const vmath::Vector3& val) { translation_ = val; }
		vmath::Vector3 getScale() const { return scale_; }
		void setScale(const vmath::Vector3& val) { scale_ = val; }
		vmath::Quat getRotation() const { return rotation_; }
		void setRotation(const vmath::Quat& val) { rotation_ = val; }
		void carryObject(Object* obj);
		Object* getObject() const { return object_; }
		const char* getName() const { return name_; }
		unsigned int getHashName() const { return hashName_; }
		void addChild(Transform* child) { children_.push_back(child); child->parent_ = this;  }
		Game::Scene* getScene() const { return scene_; }
		void removeChild( Transform* child );
		Transform* getParent( ) const { return parent_; }
		void setVisible(bool val) {visible_ = val;}
		bool isVisible() const {return visible_;}
	};
}


