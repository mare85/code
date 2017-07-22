#include "EventSystem.h"
#include <Black/Event.h>
#include <Black/Ground.h>
#include <Black/TextSubsystem.h>
#include <Script/ScriptFileLoader.h>
#include <Game/App.h>
#include <Game/Scene.h>

Black::EventSystem* __instance;

Black::EventSystem::EventSystem()
{
	memset(sectorCounts_, 0, sizeof(unsigned int) * nSectorsPerLayer * nLayers);
}

void Black::EventSystem::StartUp(Game::Scene* scene)
{
	assert( __instance == nullptr);
	__instance = new EventSystem();
	__instance->scene_ = scene;
	__instance->scene_->addUpdater(__instance);

}

void Black::EventSystem::ShutDown()
{
	if (!__instance)
		return;
	if (__instance && __instance->scene_)
	{
		__instance->scene_->removeUpdater(__instance);
	}
	
	delete __instance;
	__instance = nullptr;
}

Black::EventSystem* Black::EventSystem::GetInstance()
{
	assert(__instance);
	return __instance;
}

void Black::EventSystem::addEvent(Event * evt)
{
	float fmin, fmax;
	unsigned int layerId = evt->getLayer();
	Ground* ground = GroundManager::getInstance()->getGround(layerId);
	evt->getBounds(&fmin, &fmax);
	unsigned int imin = ground->getColliderIndex(fmin) + nSectorsPerLayer * layerId;
	unsigned int imax = ground->getColliderIndex(fmax) + nSectorsPerLayer * layerId;
	for (unsigned int i = imin; i <= imax; ++i)
	{
		unsigned int count = sectorCounts_[i];
		assert(count < nEventsPerSector);
		sectors_[i * nEventsPerSector + count] = evt;
		++(sectorCounts_[i]);
	}
}

void Black::EventSystem::removeEvent(Event * evt)
{
	unsigned int layerId = evt->getLayer();
	float fmin, fmax;
	Ground* ground = GroundManager::getInstance()->getGround(layerId);
	evt->getBounds(&fmin, &fmax);
	unsigned int imin = ground->getColliderIndex(fmin) + nSectorsPerLayer * layerId;
	unsigned int imax = ground->getColliderIndex(fmax) + nSectorsPerLayer * layerId;
	for (unsigned int i = imin; i <= imax; ++i)
	{
		unsigned int count = sectorCounts_[i];
		unsigned int sectorId = i * nEventsPerSector;
		Event** sectorEvts = sectors_ + sectorId;
		for (unsigned int j = 0; j < count; ++j)
		{
			if (sectorEvts[j] == evt)
			{
				--count;
				sectorEvts[j] = sectorEvts[count];
				j = count;
				sectorCounts_[i] = count;
			}
		}
	}
}

void Black::EventSystem::feedPosition(const vmath::Vector3 & pos, unsigned int layerId, unsigned int levelId)
{
	position_ = pos;
	layerId_ = layerId;
	levelId_ = levelId;
	x_ = pos.getX();
	unsigned int ix = GroundManager::getInstance()->getGround(layerId)->getColliderIndex(x_);
	currentSector_ = layerId * nSectorsPerLayer + ix;
}

void Black::EventSystem::update(const Game::UpdateContext * uctx)
{
	(void)uctx;
	Event** sector = sectors_ + currentSector_ * nEventsPerSector;
	unsigned int count = sectorCounts_[currentSector_];



	Event* newEvents[nSectorsPerLayer];
	unsigned int nNewEvents = 0;

	for (unsigned int i = 0; i < count; ++i)
	{
		Event* evt = sector[i];
		
		float xMin, xMax;
		evt->getBounds(&xMin, &xMax);
		if (xMin < x_ && xMax > x_ && !Game::App::EditorEnabled())
		{
			newEvents[nNewEvents] = evt;
			++nNewEvents;
			if( ! isInEvent(evt) )
			{
				if (evt->isText())
				{
					TextSubsystem::Instance()->addText(evt->getScript());
				}
				else
				{
					Script::runScript(Game::App::Instance()->getScene(), levelId_, evt->getScript());
				}
				
				i = count; // one event at a time
				if (evt->isDisabledOnEnter())
				{
					evt->disable();
					removeEvent(evt);
				}
			}
		}
	}
	nCurrentEvents = 0;
	for (unsigned int i = 0; i < nNewEvents; ++i)
	{
		currentEvents[nCurrentEvents] = newEvents[ i ];
		++nCurrentEvents;
	}
}

bool Black::EventSystem::isInEvent(Event * evt)
{
	for (unsigned int i = 0; i < nCurrentEvents; ++i)
	{
		if (currentEvents[i] == evt)
		{
			return true;
		}
	}
	return false;
}
