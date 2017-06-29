#pragma once

#include <Util/Utils.h>

namespace Physics
{

	
	struct TopDescriptor
	{
		vmath::Vector3 normalLeft;
		vmath::Vector3 normalRight;
		vmath::Vector3 posLeft;
		vmath::Vector3 posRight;
	};

	class CubicFunctionColider
	{
		float xMin_ = -1.0f;
		float xMax_ = -1.0f;
		vmath::Vector4 functionUp_;
		vmath::Vector4 functionBottom_;
		ggfgvnfg
		
		

		

		bool hasBottom_;

	public:

		
		

		CubicFunctionColider() {}
		~CubicFunctionColider() {}
	};


}
