#pragma once
#include <map>
namespace Graphics
{

	class Texture;
	class Shader;
	class GdiContext;

	typedef std::map<unsigned int, Texture*> TextureMap;
	typedef std::map<unsigned int, Shader*> ShaderMap;

	class Store
	{
		TextureMap textures_;
		ShaderMap shaders_;

		static Store* _instance;
		Store();
		~Store();
	public:

		bool hasTexture(unsigned int hashName) { return textures_.find(hashName) != textures_.end(); }
		bool hasShader(unsigned int hashName) { return shaders_.find(hashName) != shaders_.end(); }
		Texture* getTexture( unsigned int hashName );
		Shader* getShader( unsigned int hashName );
		void addTexture(unsigned int hashName, Texture* texture);
		void addShader(unsigned int hashName, Shader* shader);

		static void releaseInstance();
		static Store* getInstance();
		void unloadResources(GdiContext* gdiContext);
		
	};
	
}