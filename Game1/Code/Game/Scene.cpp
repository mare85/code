#include "Scene.h"
#include <Game/Camera.h>
#include <algorithm>
namespace Game
{
	void Scene::loadData(Graphics::GdiContext * gdiContext)
	{
		blendState_ = gdiContext->createBlendState(Graphics::eAlphaOneMinusAlpha);
		gdiContext->setBlendState(blendState_, vmath::Vector4(.0f));
		objects_.sort(Object::CompareLayers);
		for (Object * obj : objects_ )
			obj->loadData(gdiContext);
		loaded_ = true;
	}
	void Scene::unloadData(Graphics::GdiContext * gdiContext)
	{
		gdiContext->releaseBlendState( blendState_);
		for (Object * obj : objects_)
			obj->unloadData(gdiContext);
	}
	void Scene::render(Graphics::GdiContext * gdiContext, Graphics::RenderContext* renderContext)
	{
		if (!enabled_)
			return;
		gdiContext->setBlendState(blendState_, vmath::Vector4(.0f));
		for (Object* obj : objects_)
			obj->updateGfx(gdiContext);

		if (activeCamera_)
		{
			for (Object * obj : objects_)
			{
				if(obj->getTransform()->isVisible())
					obj->render(gdiContext, renderContext);
			}
		}
	}

	void Scene::renderLayer(Graphics::GdiContext * gdiContext, Graphics::RenderContext * renderContext, int layer)
	{
		if (!enabled_)
			return;

		gdiContext->setBlendState(blendState_, vmath::Vector4(.0f));
		for (Object* obj : objects_)
		{
			if( obj->getLayer() == layer )
				obj->updateGfx(gdiContext);
		}

		for (Object * obj : objects_)
			if (obj->getLayer() == layer)
				obj->render(gdiContext, renderContext);
	}

	void Scene::overwriteObject(unsigned int hash, Object * obj)
	{
		Object* toRemove = findObject(hash);
		objects_.remove(toRemove);
		Transform* t = toRemove->getTransform();
		t->carryObject(obj);
		delete toRemove;
		objects_.push_back(obj);
	}

	void Scene::start()
	{
		for (Updater* updater : updaters_)
		{
			updater->start();
		}
	}

	void Scene::update(const UpdateContext * uctx)
	{
		if (!enabled_)
			return;
		for (auto& kv : allTransforms_)
		{
			kv.second->computeLocalPose();
		}
		for (Transform* tr : rootTransforms_)
		{
			tr->computeWorldPose();
		}
		for (Updater* upd : updaters_)
		{
			upd->update(uctx);
		}
	}

	void Scene::stop()
	{
		for (Updater* upd : updaters_)
		{
			upd->stop();
		}
	}

	Object * Scene::findObject(unsigned int hash)
	{
		ObjectList::iterator result = std::find_if( 
			begin( objects_ ), end( objects_ ), 
			[hash]( Object* obj ) { return obj->getHashName() == hash; } 
		);
		return *result;
	}

	Scene::~Scene()
	{
		for (auto& kv : cameras_)
		{
			Camera* cam = kv.second;
			delete cam;
		}
		for (auto& kv : allTransforms_)
		{
			Transform* tr = kv.second;
			delete tr;
		}
		for (Updater* upd : updaters_)
		{
			if( upd->isUpdaterManagedByScene() )
				delete upd;
		}
		for (Object* obj: objects_)
		{
			delete obj;
		}

	}
	
	Camera* Scene::createPersp(unsigned int transformHash, float fov)
	{
		if (!hasTransform(transformHash))
			return nullptr;
		Transform* trans = allTransforms_[transformHash];

		Camera* cam = nullptr;

		if (cameras_.find(transformHash) != cameras_.end())
		{
			cam = cameras_[transformHash];
		}
		else
		{
			cam = new Camera();
		}
		cam->transform_ = trans;
		cam->isOrtographic_ = false;
		cam->fov_ = fov;
		cameras_[transformHash] = cam;
		if (!activeCamera_)
			activeCamera_ = cam;
		return cam;
	}

	Camera* Scene::createOrtographicCamera(unsigned int transformHash, float extend)
	{
		if (!hasTransform(transformHash) )
			return nullptr;
		Transform* trans = allTransforms_[transformHash];

		Camera* cam = nullptr;
		
		if (cameras_.find(transformHash)  != cameras_.end())
		{
			cam = cameras_[transformHash];
		}
		else
		{
			cam = new Camera();
		}
		cam->transform_ = trans;
		cam->isOrtographic_ = true;
		cam->extend_ = extend;
		cameras_[transformHash] = cam;
		if (!activeCamera_)
			activeCamera_ = cam;
		return cam;
	}

	bool Scene::addTransform(Game::Transform * transform, Game::Transform* parent)
	{
		unsigned int tHash = transform->getHashName();
		if (hasTransform(tHash))
		{
			return false;
		}
		unsigned int pHash = parent ? ( parent->getHashName() ) : 0;
		if (pHash && !hasTransform( pHash ) )
		{
			return false;
		}
		allTransforms_[tHash] = transform;
		if (parent)
		{
			parent->addChild(transform);
		}
		else
		{
			rootTransforms_.push_back(transform);
		}
		Object *obj = transform->getObject();
		if (obj)
		{
			objects_.push_back( obj );
		}

		
		return true;
	}

	void Scene::removeUpdater(Updater * upd)
	{
		updaters_.remove(upd);
	}

	void Scene::reloadGfx(Graphics::GdiContext* gdiContext, int layerId)
	{
		for (Object * obj : objects_)
			if(obj->getLayer() == layerId )
				obj->unloadData(gdiContext);
		for (Object * obj : objects_)
			if(obj->getLayer() == layerId )
				obj->loadData(gdiContext);
	}

	void Scene::removeObjectWithTransform(unsigned int hash)
	{
		Object* obj = findObject( hash );
		Transform* t = obj->getTransform();
		Transform* p =  t->getParent();
		if(p)
			p->removeChild( t );
		rootTransforms_.remove( t );
		allTransforms_.erase( t->getHashName() );
		objects_.remove( obj );
		delete obj;
		delete t;
	}
}