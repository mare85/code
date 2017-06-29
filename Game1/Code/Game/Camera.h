#pragma once
#include <Util/Utils.h>

namespace Game
{

	class Transform; 
	class Scene;
	class Camera
	{
		friend class Scene;
		Transform* transform_ = nullptr;
		bool isOrtographic_ = false;
		float fov_ = 1.0f;
		float extend_ = 1.0f;
		float zExtend_ = 100.0f;
		float zNear_ = 1.0f;
		float zFar_ = 1000.0f;
		float xWrap_ = .0f;
		bool xWrapEnabled_ = false;

		Camera() {}
		~Camera() {}
	public:
		
		vmath::Matrix4 getView()const ;
		vmath::Matrix4 getProjection( float aspect )const;
		void setFov(float val) { fov_ = val; }
		void setExtend(float val) { extend_ = val; }
		void setZExtend(float val) { zExtend_ = val; }
		void setNearFar(float n, float f) { zNear_ = n; zFar_ = f; }
		void setXWrap(float val) { xWrap_ = val;  xWrapEnabled_ = true; }
		bool isXWrap() const { return xWrapEnabled_; }
		float getXWrap() const { return xWrap_; }
	};

}