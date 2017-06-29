#pragma once

#include <Util/Utils.h>
namespace Game
{

	struct SceneDescriptor
	{
		enum ElemType{
			eNone,
			eSprite,
			eOrthoCamera,
			ePerspCamera,
		} ;


		struct Elem
		{
			vmath::Vector3 pos_;
			vmath::Vector3 eulers_;
			vmath::Vector3 scale_;
			Elem* children_;
			unsigned int nChildren_;
			ElemType type_;
			char name_[255];
			char strParam1_[255];
			float param1_;
			
		};

		Elem* elems_;
		unsigned int nElems_;
		SceneDescriptor();
		~SceneDescriptor();
	};


}