#include "Store.h"

#include <Graphics/GdiContext.h>

namespace Graphics
{
	Store* Store::_instance = nullptr;

	Store::Store()
	{
	}


	Store::~Store()
	{
	}
	Texture * Store::getTexture(unsigned int hashName)
	{
		if( textures_.find( hashName ) == textures_.end() )
			return nullptr;
		return textures_[hashName];
	}
	Shader * Store::getShader(unsigned int hashName)
	{
		if (shaders_.find(hashName) == shaders_.end())
			return nullptr;
		return shaders_[hashName];
	}
	ComputeShader * Store::getComputeShader(unsigned int hashName)
	{
		if (computeShaders_.find(hashName) == computeShaders_.end())
			return nullptr;
		return computeShaders_[hashName];
	}
	void Store::addTexture(unsigned int hashName, Texture * texture)
	{
		if (textures_.find(hashName) != textures_.end())
			return;
		textures_[hashName] = texture;
	}
	void Store::addShader(unsigned int hashName, Shader * shader)
	{
		if (shaders_.find(hashName) != shaders_.end())
			return;
		shaders_[hashName] = shader;
	}
	void Store::addComputeShader(unsigned int hashName, ComputeShader * shader)
	{
		if (computeShaders_.find(hashName) != computeShaders_.end())
			return;
		computeShaders_[hashName] = shader;
	}
	void Store::releaseInstance()
	{
		if(_instance)
			delete _instance;	
		_instance = nullptr;
	}
	Store * Store::getInstance()
	{
		if (_instance == nullptr)
		{
			_instance = new Store();
		}
		return _instance;
	}
	void Store::unloadResources(GdiContext * gdiContext)
	{
		for (auto& kv : textures_)
		{
			gdiContext->releaseTexture(kv.second);
		}
		textures_.clear();
		for (auto& kv : shaders_)
		{
			gdiContext->releaseShader(kv.second);
		}
	}
}