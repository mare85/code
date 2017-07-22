#include "Editor.h"
#include <Black/Editor/EditorMenu.h>
#include <Black/Editor/Add.h>
#include <Black/GroundManager.h>
#include <Black/Ground.h>
#include <Black/TreeMesh.h>
#include <Black/GrassMesh.h>
#include <Black/BlackSprite.h>
#include <Black/Event.h>
#include <Input/Listener.h>
#include <Input/Constants.h>
#include <Game/App.h>
#include <Game/Scene.h>
#include <Game/Transform.h>
#include <windows.h>
#include <sal.h>

Black::Editor::Editor* __instance = nullptr;

Black::Editor::Editor* Black::Editor::Editor::GetInstance()
{
	return __instance;
}




void Black::Editor::Editor::_GetAllJpgs(const char * dirname)
{
	bool searchOn = true;
	WIN32_FIND_DATA fileData;
	HANDLE fileHandle;
	fileHandle = FindFirstFile(dirname, &fileData);
	unsigned int dirnamelen = (unsigned int)strlen(dirname);
	if (!fileHandle)
		searchOn = false;


	nPaths_ = 0;
	while (searchOn)
	{
		const char* fname = fileData.cFileName;
		unsigned int fnamelen = (unsigned int)strlen(fname);
		if (fnamelen > 4)
		{
			const char* ext = fname + (fnamelen - 4);
			char extLower[5];
			extLower[4] = 0;
			for (unsigned int i = 0; i < 4; ++i)
			{
				extLower[i] = static_cast<char>(tolower(ext[i]));
			}
			if (strcmp(extLower, ".jpg") == 0)
			{
				strcpy_s( directoryPaths_[nPaths_].path_, dirname);
				strcpy_s(directoryPaths_[nPaths_].path_ + dirnamelen - 1, 50, fname);
				strcpy_s(directoryPaths_[nPaths_].label_, fname);
				directoryPaths_[nPaths_].label_[fnamelen - 4] = 0;
				++nPaths_;
			}
		}
		searchOn = ( FindNextFile(fileHandle, &fileData) == TRUE );
	}
}

void Black::Editor::Editor::_RenderSector(Graphics::GdiContext* gdiContext, Graphics::RenderContext* renderContext)
{
	Editor::_Sector& sector = sectors_[currentSector_];
	Game::Scene* scene = scene_;
	for (unsigned int i = 0; i < sector.nHashes; ++i)
	{
		if (i != selectedIndex_)
		{
			Game::Object* obj = scene->findObject(sector.hashes_[i]);
			obj->render(gdiContext, renderContext);
		}
	}
}

void Black::Editor::Editor::_RenderSelection(Graphics::GdiContext* gdiContext, Graphics::RenderContext* renderContext)
{
	Editor::_Sector& sector = sectors_[currentSector_];
	Game::Scene* scene = scene_;
	if (sector.nHashes > selectedIndex_)
	{
		unsigned int hash = sector.hashes_[selectedIndex_];
		Game::Object* obj = scene->findObject( hash);
		obj->render(gdiContext, renderContext );
	}
}

Black::Editor::Editor::_Sector * Black::Editor::Editor::_GetCurrentSector()
{
	return &(sectors_[currentSector_]);
}

Black::Editor::Editor::Editor(Game::Scene* scene)
	: scene_( scene )
{
	scene_->addUpdater( this);
	__instance = this;
	menu_.thisNode_ = this;
	menu_.init();
	keyInput_[0] = 0;
}


Black::Editor::Editor::~Editor()
{
	__instance = nullptr;
}


void Black::Editor::Editor::start()
{
}

void Black::Editor::Editor::update(const Game::UpdateContext* uctx)
{
	if (!Game::App::EditorEnabled())
		return;
	Ground* ground = GroundManager::getInstance()->getCurrentGround();
	unsigned int posX = (ground->getColliderIndex(position_.getX() + GroundManager::HalfSegmentWidth) - 2) % GroundManager::eNSectors;
	unsigned int elevationIndex = layerId_ * GroundManager::eNSectors + posX;
	menu_.update( uctx );
	
	unsigned int layerId = GroundManager::getInstance()->getLayerIndex();
	float x = position_.getX();
	currentSector_ = GetSectorId(layerId, x);
	if (uctx->input->getKeyDown(Input::Key::Insert))
	{
		menu_.state_ = EditorMenu::eStateAdd;
	}
	else if (uctx->input->getKeyDown(Input::Key::LCtrl))
	{
		menu_.state_ = EditorMenu::eStateEdit;
	}
	else if (menu_.state_ == EditorMenu::eStateOff)
	{
		if (uctx->input->getKeyDown(Input::Key::PageDown))
		{
			GroundManager::getInstance()->elevate(elevationIndex, -.6f);
		}
		else if (uctx->input->getKeyDown(Input::Key::PageUp))
		{
			GroundManager::getInstance()->elevate(elevationIndex, .6f);
		}

		if (uctx->input->getKeyDown(Input::Key::S))
		{
			//GroundManager::getInstance()->saveLayer(layerId_);
			GroundManager::getInstance()->save();
			char filePath[250];
			sprintf_s(filePath, "assets/levels/level%d/level.txt", GroundManager::getInstance()->getLevelId());
			Script::saveLevel(scene_, filePath);
		}
		if (uctx->input->getKeyDown(Input::Key::Delete))
		{
			RequestRemoveSelected();
		}
		if (uctx->input->getKeyDown(Input::Key::Tab))
		{
			selectedIndex_++;
		}
		_Sector& sec = sectors_[currentSector_];
		if (selectedIndex_ >= sec.nHashes)
			selectedIndex_ = 0;

		if( uctx->input->getKeyDown(Input::Key::C) /*&& uctx->input->isKeyPressed(Input::Key::LCtrl)*/ )
		{
			_Sector& sec = sectors_[currentSector_];
			if( sec.nHashes )
			{
				unsigned int hash = sec.hashes_[ selectedIndex_ ];
				Game::Object* obj = scene_->findObject(hash);
				if( dynamic_cast<TreeMesh*>(obj) )
				{
					copyPaste_.ty_ = _CopyPaste::eTree;
					obj->getPaarmSet()->copyTo( copyPaste_.bufferValues_, 4096);
				}
				else if( dynamic_cast<GrassMesh*>(obj) )
				{
					copyPaste_.ty_ = _CopyPaste::eGrass;
					obj->getPaarmSet()->copyTo( copyPaste_.bufferValues_, 4096);
				}
				else if( dynamic_cast<BlackSprite*>(obj) )
				{
					copyPaste_.ty_ = _CopyPaste::eSprite;
					obj->getPaarmSet()->copyTo( copyPaste_.bufferValues_, 4096);
					strcpy_s( copyPaste_.stringParam_, ( static_cast<BlackSprite*>(obj) )->getFilename() );
				}
				else if( dynamic_cast<Event*>(obj) )
				{
					copyPaste_.ty_ = _CopyPaste::eEvent;
					obj->getPaarmSet()->copyTo( copyPaste_.bufferValues_, 4096);
				}

			}
		}
		
		if( uctx->input->getKeyDown(Input::Key::V) /*&& uctx->input->isKeyPressed(Input::Key::LCtrl)*/ )
		{
			switch( copyPaste_.ty_ )
			{
				case _CopyPaste::eTree :
				{
					Game::Object* tree = add::tree();
					tree->getPaarmSet()->copyValuesFrom(copyPaste_.bufferValues_, tree);
					
					break;
				}
				case _CopyPaste::eGrass :
				{
					Game::Object* grass = add::grass();
					grass->getPaarmSet()->copyValuesFrom(copyPaste_.bufferValues_, grass);
					break;
				}
				case _CopyPaste::eEvent :
				{
					Game::Object* event = add::event();
					event->getPaarmSet()->copyValuesFrom(copyPaste_.bufferValues_, event);
					break;
				}
				case _CopyPaste::eSprite :
				{
					Game::Object* sprite = add::sprite( copyPaste_.stringParam_ );
					sprite->getPaarmSet()->copyValuesFrom(copyPaste_.bufferValues_, sprite);
					break;
				}
			}
			

		}
	}
}

void Black::Editor::Editor::updateKeyInput(const Game::UpdateContext* uctx)
{
	char c = 0;
	bool isShift =
		uctx->input->isKeyPressed(Input::Key::LShift) ||
		uctx->input->isKeyPressed(Input::Key::RShift);

	for (unsigned int i = 0; !c && i < 256; ++i)
	{
		if (uctx->input->getKeyDown(static_cast<unsigned char>(i)))
		{
			c = Input::Key::getChar((Input::Key::Keycode)i, isShift);
		}
	}
	unsigned int len = (unsigned int) strlen(keyInput_);
	if (c && len < 249)
	{
		keyInput_[len] = c;
		keyInput_[len + 1] = 0;
	}

	if (uctx->input->getKeyDown(Input::Key::Backspace) && len )
	{
		keyInput_[len - 1] = 0;
	}
	if (uctx->input->getKeyDown(Input::Key::Enter) && len)
	{
		keyboardMode_ = false;
	}
}

void Black::Editor::Editor::stop()
{

}

void Black::Editor::Editor::SetPosition(vmath::Vector3 pos)
{
	if( !__instance ) return;
	__instance->position_ = pos;
}

void Black::Editor::Editor::SetLayerID(int val)
{
	if( !__instance ) return;
	__instance->layerId_= val;
}

void Black::Editor::Editor::Add(Game::Object* obj)
{
	if( !__instance ) return;
	unsigned int layerId = obj->getLayer();
	float x = obj->getTransform()->getTranslation().getX();
 	unsigned int sectorId = GetSectorId( layerId, x );
	_Sector& sec = __instance->sectors_[sectorId];
	assert( sec.nHashes < eSectorSize );
	sec.hashes_[ sec.nHashes ] = obj->getHashName();
	++sec.nHashes;
}

unsigned int Black::Editor::Editor::GetSectorId(unsigned int layer, float x)
{
	if( !__instance ) 0;
	Black::Ground* ground = Black::GroundManager::getInstance()->getGround(layer);
	unsigned int ix = ground->getColliderIndex(x);
	return GroundManager::eNSectors * layer + ix;
}

void Black::Editor::Editor::RequestRemoveSelected()
{
	if( !__instance ) return;
	_Sector& sec = __instance->sectors_[ __instance->currentSector_];
	unsigned int selectedId = __instance->selectedIndex_;
	if( selectedId >= sec.nHashes )
		return;
	unsigned int hash = sec.hashes_[ selectedId ];
	--sec.nHashes;
	sec.hashes_[ selectedId ] = sec.hashes_[ sec.nHashes ];
	__instance->removeHashList[ __instance->nToRemove ] = hash;
	++__instance->nToRemove;
	__instance->selectedIndex_ = 0;
}

void Black::Editor::Editor::RemoveRequested(Graphics::GdiContext* gdi)
{
	if( !__instance ) return;
	for( unsigned int i = 0; i < __instance->nToRemove; ++i )
	{
		unsigned int hash = __instance->removeHashList[ i ];

		Game::Object* obj = __instance->scene_->findObject( hash );
		obj->unloadData( gdi);
		__instance->scene_->removeObjectWithTransform( hash );
	}
	__instance->nToRemove = 0;
	
}

void Black::Editor::Editor::RenderSector(Graphics::GdiContext* gdiContext, Graphics::RenderContext* renderContext)
{
	if (!__instance) return;
	__instance->_RenderSector(gdiContext, renderContext);
}

void Black::Editor::Editor::RenderSelection(Graphics::GdiContext* gdiContext, Graphics::RenderContext* renderContext)
{
	if (!__instance) return;
	__instance->_RenderSelection(gdiContext, renderContext);
}

Game::Object * Black::Editor::Editor::GetSelected() 
{
	if (!__instance) return nullptr;
	Editor::_Sector& sector = __instance->sectors_[__instance->currentSector_];
	Game::Scene* scene = __instance->scene_;
	if (sector.nHashes > __instance->selectedIndex_)
	{
		unsigned int hash = sector.hashes_[__instance->selectedIndex_];
		Game::Object* obj = scene->findObject(hash);
		return obj;
	}
	return nullptr;
}

bool Black::Editor::Editor::IsInEditionMode()
{
	if( !__instance )
		return false;
	return __instance->inEditionMode_;
}
