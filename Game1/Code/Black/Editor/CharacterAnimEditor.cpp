#include "CharacterAnimEditor.h"
#include <Game/Text.h>
#include <Game/Transform.h>
#include <Game/Scene.h>
#include <Game/Updater.h>

#include <Util/Utils.h>
#include <Input/Constants.h>
#include <Input/Listener.h>

void Black::Editor::CharacterAnimEditor::_Evaluate()
{
	float t = curValues_.time_;
	_FindKeyIndex( );
	
	assert( curKeyIndex_< numKeys_ );
	_Key& key = keys_[curKeyIndex_];

	if( t == keys_[curKeyIndex_].time_ )
	{
		for( unsigned int i = 0; i < CharacterObject::eBoneCount; ++i )
		{
			curValues_.angles_[ i ] = key.angles_[ i ];
		}
		curValues_.basePosX_ = key.basePosX_;
		curValues_.basePosY_ = key.basePosY_;
	}
	else
	{
		assert(curKeyIndex_ < numKeys_ - 1);
		_Key& key2 = keys_[curKeyIndex_ +1];
		float progress = (t - key.time_ ) / ( key2.time_ - key.time_ );
		//linear for now
		float progress0 = 1.0f - progress;
		for( unsigned int i = 0; i < CharacterObject::eBoneCount; ++i )
		{
			curValues_.angles_[ i ] = key.angles_[ i ] * progress0 + key2.angles_[ i ] * progress;
		}
		curValues_.basePosX_ = key.basePosX_* progress0 + key2.basePosX_ * progress;
		curValues_.basePosY_ = key.basePosY_* progress0 + key2.basePosY_ * progress;
	}
}

void Black::Editor::CharacterAnimEditor::_AddKey()
{
	for( unsigned int i = numKeys_ - 1; i > curKeyIndex_; --i)
	{
		_CopyKey(keys_[i + 1], keys_[i]);
	}
	curKeyIndex_++;
	_CopyKey(keys_[curKeyIndex_], curValues_);
	numKeys_++;
}

void Black::Editor::CharacterAnimEditor::_CopyKey(_Key & dst, _Key & src)
{
	for (unsigned int i = 0; i < CharacterObject::eBoneCount; ++i)
		dst.angles_[i] = src.angles_[i];
	dst.basePosX_ = src.basePosX_;
	dst.basePosY_ = src.basePosY_;
	dst.time_ = src.time_;
}


void Black::Editor::CharacterAnimEditor::_DeleteKey(unsigned int index)
{
	for (unsigned int i = curKeyIndex_; i < numKeys_ - 1; ++i)
	{
		_CopyKey(keys_[i], keys_[i+1]);
	}
	--curKeyIndex_;
	--numKeys_;
}

void Black::Editor::CharacterAnimEditor::_SaveAnim()
{
	char fullFilename[500];
	sprintf_s(fullFilename, "assets/levels/level1/%s.anim", filename_);
	FILE* f = nullptr;
	fopen_s(&f, fullFilename, "w");
	fwrite(&numKeys_, 4, 1, f);
	fwrite(&tottalTime_, 4, 1, f);
	fwrite(keys_, sizeof(_Key), numKeys_, f);
	fclose(f);
}

void Black::Editor::CharacterAnimEditor::_LoadAnim()
{
	FILE* f = nullptr;
	fopen_s(&f, directoryPaths_[curMenu_].path_, "r");
	fread_s(&numKeys_, 4, 4, 1, f);
	fread_s(&tottalTime_, 4, 4, 1, f);
	fread_s(&keys_, sizeof(_Key) * nMaxKeys, sizeof(_Key), numKeys_, f);
	fclose(f);
	strcpy_s(filename_, directoryPaths_[curMenu_].label_);
	filename_[strlen(filename_) - 5] = 0;
	curValues_.time_ = .0f;
	_FindKeyIndex();
}

void Black::Editor::CharacterAnimEditor::updateKeyInput(const Game::UpdateContext * uctx)
{
	char c = 0;
	bool isShift =
		uctx->input->isKeyPressed(Input::Key::LShift) ||
		uctx->input->isKeyPressed(Input::Key::RShift);

	for (unsigned int i = 0; !c && i < 256; ++i)
	{
		if (uctx->input->getKeyDown(i))
		{
			c = Input::Key::getChar((Input::Key::Keycode)i, isShift);
		}
	}
	unsigned int len = (unsigned int)strlen(keyInput_);
	if (c && len < 249)
	{
		keyInput_[len] = c;
		keyInput_[len + 1] = 0;
	}

	if (uctx->input->getKeyDown(Input::Key::Backspace) && len)
	{
		keyInput_[len - 1] = 0;
	}
	if (uctx->input->getKeyDown(Input::Key::Enter) && len)
	{
		keyboardMode_ = false;
	}
}
void Black::Editor::CharacterAnimEditor::_GetAllAnimFilenames()
{
	bool searchOn = true;
	WIN32_FIND_DATA fileData;
	HANDLE fileHandle;
	char dirname[1024];
	sprintf_s(dirname, "assets/levels/level%d/*", levelId_);
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
			const char* ext = fname + (fnamelen - 5);
			char extLower[6];
			extLower[5] = 0;
			for (unsigned int i = 0; i < 5; ++i)
			{
				extLower[i] = tolower(ext[i]);
			}
			if (strcmp(extLower, ".anim") == 0)
			{
				strcpy_s(directoryPaths_[nPaths_].path_, dirname);
				strcpy_s(directoryPaths_[nPaths_].path_ + dirnamelen - 1, 50, fname);
				strcpy_s(directoryPaths_[nPaths_].label_, fname);
				//directoryPaths_[nPaths_].label_[fnamelen - 5] = 0;
				++nPaths_;
			}
		}
		searchOn = (FindNextFile(fileHandle, &fileData) == TRUE);
	}
}
void Black::Editor::CharacterAnimEditor::_UpdateLoadMode(const Game::UpdateContext * uctx)
{
	unsigned int managedMenuentry = 0;
	char textBuffer[250];
	vmath::Vector4 defaultColor(.0f, .0f, .0f, .4f);
	vmath::Vector4 chosenColor(0.5f, .0f, .0f, 1.0f);
	for( unsigned int i = 0 ; i < nPaths_;++i )
	{
		if (curMenu_ == managedMenuentry)
		{
			menuText_[managedMenuentry]->setColor(chosenColor);
		}
		else
		{
			menuText_[managedMenuentry]->setColor(defaultColor);
		}
		menuText_[managedMenuentry]->setText(directoryPaths_[i].label_);
		++managedMenuentry;
	}
	{
		if (curMenu_ == managedMenuentry)
		{
			menuText_[managedMenuentry]->setColor(chosenColor);
		}
		else
		{
			menuText_[managedMenuentry]->setColor(defaultColor);
		}
		menuText_[managedMenuentry]->setText("back");
		++managedMenuentry;
	}



	nMenuItems_ = managedMenuentry;

	for (unsigned int i = nMenuItems_; i < nMaxMenuItems; ++i)
	{
		menuText_[i]->setText(" ");
	}
	characterObject_->setAngles(curValues_.angles_);
	characterObject_->setHeightOffset(curValues_.basePosY_);
	characterObject_->setSideOffset(curValues_.basePosX_);

	if (menuInput_.up)
	{
		--curMenu_;
		if (curMenu_ > nMenuItems_)
			curMenu_ = nMenuItems_ - 1;
	}
	if (menuInput_.down)
	{
		++curMenu_;
		if (curMenu_ > nMenuItems_)
			curMenu_ = 0;
	}
	if (menuInput_.enter)
	{
		if (curMenu_ == nMenuItems_ - 1)
		{
			loadMode_ = false;
			curMenu_ = 0;
		}
		else
		{
			_LoadAnim();
			loadMode_ = false;
			curMenu_ = 0;
		}
	}
	if (menuInput_.start)
	{
		loadMode_ = false;
		curMenu_ = 0;
	}


}
Black::Editor::CharacterAnimEditor::CharacterAnimEditor(Game::Scene * scene)
	: Game::Updater()
	, scene_( scene )
{
	float basePosX_ = -3.2f;
	float basePosY_ = 1.5f;
	float yStep = 3.0f / nMaxMenuItems;

	for (unsigned int i = 0; i < nMaxMenuItems; ++i)
	{
		char textname[25];
		sprintf_s(textname, "animEditorText%d", i);
		Game::Text* text = new Game::Text(textname);
		text->setColor(vmath::Vector4(.0f, .0f, .0f, .5f));
		text->setLayer(101);
		text->setText("");
		text->setMaxCharNumber(250);
		text->setBaseScale(vmath::Vector3(5.f, 5.f, 1.0f));
		text->prepareFont("assets/fonts/arial");
		Game::Transform* t = new Game::Transform(textname);
		t->setTranslation( vmath::Vector3( basePosX_, basePosY_ - i * yStep, .0f ) );
		t->carryObject(text);
		scene->addTransform(t);

		menuText_[i] = text;
		menuTransforms_[i] = t;
	}
	{
		Game::Transform* t = new Game::Transform("AnimEditorcharacter");
		characterObject_ = new Black::CharacterObject("AnimEditorcharacterObj", "assets/gfx/woman.jpg", 100);
		t->carryObject( characterObject_);
		scene->addTransform(t);
		t->setTranslation( vmath::Vector3( .2f, -.7f, .0f ));
	}
	filename_[0] = 0;
	scene->addUpdater( this);
	keys_[0].time_ = .0f;
	keys_[1].time_ = 1.0f;
	keys_[0].zeroValues();
	keys_[1].zeroValues();
	numKeys_ = 2;
	tottalTime_ = 1.0f;
	curValues_.time_ = .0f;
	curValues_.zeroValues();
}

Black::Editor::CharacterAnimEditor::~CharacterAnimEditor()
{
}

void Black::Editor::CharacterAnimEditor::update(const Game::UpdateContext * uctx)
{
	float dt = uctx->deltaTime;
	menuInput_.feed( uctx->input, dt);
	menuInput_.setLeftRightImmediate( true );
	_Evaluate();

	if (loadMode_)
	{
		_UpdateLoadMode(uctx);
		return;
	}
	unsigned int managedMenuentry = 0;
	char textBuffer[250];
	vmath::Vector4 defaultColor(.0f, .0f, .0f, .4f);
	vmath::Vector4 chosenColor(0.5f, .0f, .0f, 1.0f);
	{
		if(curMenu_ == managedMenuentry)
		{
			if (!keyboardMode_)
			{
				sprintf_s(textBuffer, "filename: assets/levels/level%d/[%s].anim", levelId_, filename_);
				menuText_[managedMenuentry]->setText(textBuffer);
				if (menuInput_.enter)
				{
					keyboardMode_ = true;
					strcpy_s(keyInput_, filename_);
				}
			}
			else
			{
				updateKeyInput(uctx);
				sprintf_s(textBuffer, "filename: assets/levels/level%d/[%s~].anim", levelId_, keyInput_);
				menuText_[managedMenuentry]->setText(textBuffer);
				if (!keyboardMode_)
				{
					strcpy_s(filename_, keyInput_);
				}
			}
			menuText_[managedMenuentry]->setColor(chosenColor);
		}
		else
		{
			sprintf_s(textBuffer, "filename: assets/levels/level%d/%s.anim", levelId_, filename_);
			menuText_[managedMenuentry]->setText(textBuffer);
			menuText_[managedMenuentry]->setColor(defaultColor);
		}
		++managedMenuentry;
	}
	{

		sprintf_s(textBuffer, "<< %d >> level", levelId_);
		menuText_[managedMenuentry]->setText(textBuffer);
		
		if (curMenu_ == managedMenuentry)
		{
			menuInput_.setLeftRightImmediate(false);
			if (levelId_ > 1 && menuInput_.left)
			{
				levelId_ -= 1;
			}
			else if (menuInput_.right)
			{
				levelId_ += 1;
			}
			menuText_[managedMenuentry]->setColor(chosenColor);
		}
		else
		{
			menuText_[managedMenuentry]->setColor(defaultColor);
		}
		++managedMenuentry;

	}
	{
		if (curMenu_ == managedMenuentry )
		{
			if(menuInput_.enter)
				_SaveAnim();
			menuText_[managedMenuentry]->setColor(chosenColor);
		}
		else
		{
			menuText_[managedMenuentry]->setColor(defaultColor);
		}
		menuText_[managedMenuentry]->setText("save");
		++managedMenuentry;
	}
	{
		if (curMenu_ == managedMenuentry )
		{
			if (menuInput_.enter)
			{
				loadMode_ = true;
				curMenu_ = 0;
				_GetAllAnimFilenames();
			}
			menuText_[managedMenuentry]->setColor(chosenColor);
		}
		else
		{
			menuText_[managedMenuentry]->setColor(defaultColor);
		}
		menuText_[managedMenuentry]->setText("load");
		++managedMenuentry;
	}

	{
		//tottal time
		sprintf_s(textBuffer, "<< %010.2f >> tottal time", tottalTime_);
		menuText_[managedMenuentry]->setText(textBuffer);
		if (curMenu_ == managedMenuentry)
		{
			menuText_[managedMenuentry]->setColor(chosenColor);
			if (menuInput_.left && tottalTime_ > .1f)
			{
				tottalTime_ = max( .1f, tottalTime_ - dt);
				curValues_.time_ = min(curValues_.time_, tottalTime_);
				assert(numKeys_ >= 2);
				keys_[numKeys_ - 1].time_ = tottalTime_;
				while(tottalTime_ <= keys_[numKeys_ - 2].time_ )
				{
					_DeleteKey(numKeys_ - 2);
					curKeyIndex_ = numKeys_ - 1;
				}
				
			}
			else if( menuInput_.right )
			{
				tottalTime_ = max(.1f, tottalTime_ + dt);
				keys_[numKeys_ - 1].time_ = tottalTime_;
			}
		}
		else
		{
			menuText_[managedMenuentry]->setColor(defaultColor);
		}
		++managedMenuentry;
	}
	{
		//
		char playstop[5];
		strcpy_s(playstop, (isPlaying_ ? "stop": "play"));
		sprintf_s(textBuffer, "<< %010.2f >> currentTime [%s] ", curValues_.time_, playstop);
		menuText_[managedMenuentry]->setText(textBuffer);
		if (curMenu_ == managedMenuentry)
		{
			menuText_[managedMenuentry]->setColor(chosenColor);
			if (menuInput_.enter)
				isPlaying_ = !isPlaying_;
			else if (menuInput_.left && tottalTime_ > .1f)
			{
				curValues_.time_ -= dt;
				if (curValues_.time_ < .0f)
					curValues_.time_ += tottalTime_;
			}
			else if (menuInput_.right)
			{
				curValues_.time_ += dt;
				if (curValues_.time_ > tottalTime_)
					curValues_.time_ -= tottalTime_;
			}
		}
		else
		{
			menuText_[managedMenuentry]->setColor(defaultColor);
		}
		++managedMenuentry;
	}
	if (!isPlaying_)
	{
		{
			// next key prev kexy
			sprintf_s(textBuffer, "<< %d /%d >> key", curKeyIndex_ + 1, numKeys_);
			menuText_[managedMenuentry]->setText(textBuffer);
			if (curMenu_ == managedMenuentry)
			{
				menuInput_.setLeftRightImmediate( false) ;
				menuText_[managedMenuentry]->setColor(chosenColor);
				if (menuInput_.left)
				{
					--curKeyIndex_;
					if (curKeyIndex_ > numKeys_)
					{
						curKeyIndex_ = numKeys_ - 1;
					}
					curValues_.time_ = keys_[curKeyIndex_].time_;
				}
				else if (menuInput_.right)
				{
					++curKeyIndex_;
					if (curKeyIndex_ >= numKeys_)
					{
						curKeyIndex_ = 0;
					}
					curValues_.time_ = keys_[curKeyIndex_].time_;
				}
			}
			else
			{
				menuText_[managedMenuentry]->setColor(defaultColor);
			}
			++managedMenuentry;
		}

		{
			// add key delete key
			if (inKey_)
			{
				if (curKeyIndex_ == 0 || curKeyIndex_ == numKeys_ - 1)
				{
					sprintf_s(textBuffer, "---");
				}
				else
				{
					sprintf_s(textBuffer, "delete key");
				}
				if (menuInput_.enter && curMenu_ == managedMenuentry)
				{
					_DeleteKey();
				}
			}
			else
			{
				sprintf_s(textBuffer, "add key");
				if (menuInput_.enter && curMenu_ == managedMenuentry)
				{
					_AddKey();
				}
			}
			menuText_[managedMenuentry]->setText(textBuffer);
			menuText_[managedMenuentry]->setColor((curMenu_ == managedMenuentry) ? chosenColor : defaultColor);
			++managedMenuentry;
		}
		if (inKey_)
		{
			for (unsigned int i = 0; i < characterObject_->getNBones(); ++i)
			{
				const char* boneName = characterObject_->getBoneName(i);
				char textBuffer[160];
				sprintf_s(textBuffer, "<< %010.2f >> %s", keys_[curKeyIndex_].angles_[i], characterObject_->getBoneName(i));
				menuText_[managedMenuentry]->setText(textBuffer);
				menuText_[managedMenuentry]->setColor((curMenu_ == managedMenuentry) ? chosenColor : defaultColor);
				if (curMenu_ == managedMenuentry)
				{
					if (menuInput_.left)
					{
						keys_[curKeyIndex_].angles_[i] -= dt;
					}
					if (menuInput_.right)
					{
						keys_[curKeyIndex_].angles_[i] += dt;
					}
				}
				++managedMenuentry;
			}
			// position y
			{
				char textBuffer[160];
				sprintf_s(textBuffer, "<< %010.2f >> position Y", keys_[curKeyIndex_].basePosY_);
				menuText_[managedMenuentry]->setText(textBuffer);
				menuText_[managedMenuentry]->setColor((curMenu_ == managedMenuentry) ? chosenColor : defaultColor);
				if (curMenu_ == managedMenuentry)
				{
					if (menuInput_.left)
					{
						keys_[curKeyIndex_].basePosY_ -= dt;
					}
					if (menuInput_.right)
					{
						keys_[curKeyIndex_].basePosY_ += dt;
					}

				}
				++managedMenuentry;
			}
			// position x
			{
				char textBuffer[160];
				sprintf_s(textBuffer, "<< %010.2f >> position X", keys_[curKeyIndex_].basePosX_);
				menuText_[managedMenuentry]->setText(textBuffer);
				menuText_[managedMenuentry]->setColor((curMenu_ == managedMenuentry) ? chosenColor : defaultColor);
				if (curMenu_ == managedMenuentry)
				{
					if (menuInput_.left)
					{
						keys_[curKeyIndex_].basePosX_ -= dt;
					}
					if (menuInput_.right)
					{
						keys_[curKeyIndex_].basePosX_ += dt;
					}
				}
				++managedMenuentry;
			}

		}
	}
	nMenuItems_ = managedMenuentry;

	for (unsigned int i = nMenuItems_; i < nMaxMenuItems; ++i)
	{
		menuText_[i]->setText(" ");
	}
	characterObject_->setAngles(curValues_.angles_);
	characterObject_->setHeightOffset(curValues_.basePosY_);
	characterObject_->setSideOffset(curValues_.basePosX_);


	if (menuInput_.up)
	{
		--curMenu_;
		if (curMenu_ > nMenuItems_)
			curMenu_ = nMenuItems_ - 1;
	}
	if (menuInput_.down)
	{
		++curMenu_;
		if (curMenu_ > nMenuItems_)
			curMenu_ = 0;
	}
	if (isPlaying_)
	{
		curValues_.time_ += dt;
		if (curValues_.time_ > tottalTime_)
		{
			curValues_.time_ -= tottalTime_;
		}
	}
}
