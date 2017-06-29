#pragma once
#include <Game/Object.h>
#include <Game/Updater.h>
#include <Game/Transform.h>
#include <list>
#include <Util/Utils.h>
#include <Script/ScriptFileLoader.h>

namespace Game
{
	class Camera;

	typedef std::list<Game::Object*> ObjectList;
	typedef std::list<Game::Updater*> UpdaterList;
	typedef std::map<unsigned int, Game::Camera*> CameraMap;
	class BlendState;

	class Scene :
		public Game::Object,
		public Game::Updater
	{
		friend void Script::saveLevel(Game::Scene* scene, const char* filename);
	private:
		TransformList rootTransforms_;
		TransformMap allTransforms_;
		ObjectList objects_;
		CameraMap cameras_;
		UpdaterList updaters_;
		Camera* activeCamera_ = nullptr;
		Graphics::BlendState* blendState_;
		bool enabled_ = true;		
		bool loaded_ = false;

		
	public:
		//Object
		void loadData(Graphics::GdiContext* gdiContext);
		void unloadData(Graphics::GdiContext* gdiContext);
		void reloadGfx(Graphics::GdiContext* gdiContext, int layerId);
		void render(Graphics::GdiContext* gdiContext, Graphics::RenderContext* renderContext);
		void renderLayer(Graphics::GdiContext* gdiContext, Graphics::RenderContext* renderContext, int layer);
		void overwriteObject(unsigned int hash, Object* obj);
		void removeObjectWithTransform( unsigned int hash );

		//Updater
		void start();
		void update(const UpdateContext* uctx);
		void stop();
		void setEnabled(bool val) { enabled_ = val; }
		Object* findObject(unsigned int hash);

		Scene(const char* name) : Game::Object(name) {};
		~Scene();

		bool hasTransform(const char* name) { hasTransform(Util::createHash(name)); }
		bool hasTransform(unsigned int hashName) { return allTransforms_.find(hashName) != allTransforms_.end(); }

		void addUpdater(Updater* upd) { updaters_.push_back(upd); }
		void setActiveCamera(unsigned int transformHash) { activeCamera_ = cameras_[transformHash]; }

		Camera* createPersp(unsigned int transformHash, float fov);
		Camera* createOrtographicCamera(unsigned int transformHash, float extend);
		const Camera* getActiveCamera() const { return activeCamera_; }

		bool addTransform(Game::Transform* transform, Game::Transform* parent = nullptr );
		bool isLoaded() const { return loaded_; }
		void removeUpdater(Updater* upd);


		
	};

}