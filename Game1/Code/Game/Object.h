#pragma once
#include "Graphics/GdiContext.h"
#include "Util/Utils.h"
#include <assert.h>
#include <Graphics/RenderContext.h>


namespace Graphics
{
	class Shader;
}

namespace Game
{
	class Transform;
	class ParamSet;
	
	class Object
	{
	
		Transform* transform_;
	protected :
		char name_[250];
		unsigned int hashName_ = 0;
		ParamSet* paramSet_ = nullptr;
		int layer_ = 0;
	public:
		Object() {}
		Object(const char* name) { strcpy_s(name_, name); hashName_ = Util::createHash(name_); }
		virtual ~Object() {};
		virtual void loadData(Graphics::GdiContext* gdiContext) = 0;
		virtual void unloadData(Graphics::GdiContext* gdiContext) = 0;
		virtual void render(Graphics::GdiContext* gdiContext, Graphics::RenderContext* renderContext ) = 0;
		virtual void updateGfx(Graphics::GdiContext* gdiContext) {}
		virtual void serialize( unsigned char* buffer, unsigned int* outBytes, unsigned int maxBytes ) { (void)buffer; (void)outBytes; (void)maxBytes;}
		virtual bool hasSerialize() { return false;}
		virtual bool isSelectable() { return false; }
		void setLayer(int val) { layer_ = val; }
		int getLayer() const { return layer_; }
		void setTransform(Transform* val) { transform_ = val; }
		Transform* getTransform() const { return transform_; }
		const char* getName() const { return name_; }
		unsigned int getHashName() const { return hashName_; }
		void setName( const char* name ) { strcpy_s(name_, name); hashName_ = Util::createHash(name_); }
		bool hasPaarmSet() const { return paramSet_ != nullptr; }
		ParamSet* getPaarmSet() const { return paramSet_; }
		void allocParamSet();


		static bool CompareLayers(Object* obj1, Object* obj2) { return obj1->layer_ < obj2->layer_; }

	};
}