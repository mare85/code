#include "EditorMenu.h"
#include <Black/Editor/Editor.h>
#include <Black/Editor/Add.h>


void Black::Editor::EditorMenu::init()
{

	for (unsigned int i = 0; i < eNTextLabels; ++i)
	{
		char textname[25];
		sprintf_s(textname, "editor Text %d", i);
		Game::Text* text = new Game::Text(textname);
		text->setColor(vmath::Vector4(.0f, .0f, .0f, .5f));
		text->setLayer(101);
		text->setText(" ");
		text->setMaxCharNumber(250);
		text->setBaseScale(vmath::Vector3(10.f, 10.f, 1.0f));
		text->prepareFont("assets/fonts/arial");
		Game::Transform* t = new Game::Transform(textname);
		t->carryObject(text);
		thisNode_->scene_->addTransform(t);

		label_[i] = text;
		transform_[i] = t;
	}
}

void Black::Editor::EditorMenu::update(const Game::UpdateContext * uctx)
{
	input.feed(uctx->input, uctx->deltaTime);
	switch (state_)
	{
	case eStateOff:
		thisNode_->inEditionMode_ = false;
		curMenuItem_ = 0;
		if (prevState_ != eStateOff)
		{
			for (unsigned int i = 0; i < eNTextLabels; ++i)
			{
				label_[i]->setText(" ");
			}
			nMenuItems_ = 0;
		}
		break;
	case eStateAdd:
		thisNode_->inEditionMode_ = true;

		if (prevState_ != eStateAdd)
		{
			for (unsigned int i = 4; i < eNTextLabels; ++i)
			{
				label_[i]->setText(" ");

			}
			curMenuItem_ = 0;
			label_[0]->setText("add tree");
			label_[1]->setText("add grass");
			label_[2]->setText("add gfx");
			label_[3]->setText("add event");
			nMenuItems_ = 4;
			chosen_ = 0xffffffff;
		}
		if (chosen_ == 0) { add::tree(); }
		if (chosen_ == 1) { add::grass(); }
		if (chosen_ == 2) {
			changeState_ = eStateAddSprite;
		}
		if (chosen_ == 3) { add::event(); }
		break;
	case eStateAddSprite:
		thisNode_->inEditionMode_ = true;

		if (prevState_ != eStateAddSprite)
		{
			thisNode_->_GetAllJpgs("assets/gfx/blackSprites/*");

			for (unsigned int i = 0; i < thisNode_->nPaths_; ++i)
			{
				label_[i]->setText(thisNode_->directoryPaths_[i].label_);
			}
			for (unsigned int i = thisNode_->nPaths_; i < eNTextLabels; ++i)
			{
				label_[i]->setText(" ");
			}
			curMenuItem_ = 0;
			chosen_ = 0xffffffff;
		}
		if (chosen_ != 0xffffffff)
		{
			add::sprite(thisNode_->directoryPaths_[chosen_].path_);
		}
		break;
	case eStateEdit:
		thisNode_->inEditionMode_ = true;
		Game::ParamSet* ps = Editor::GetSelected()->getPaarmSet();
		if (!ps)
		{
			changeState_ = eStateOff;
			break;
		}
		if (prevState_ != eStateEdit)
		{
			nMenuItems_ = 0;
			for (unsigned int i = 0; i < ps->getNParams(); ++i)
			{
				char content[250];
				if (ps->getType(i) == Game::eFloatParam)
					sprintf_s(content, "%s : %1.2f", ps->getParamName(i), ps->getFloat(i));
				else if (ps->getType(i) == Game::eIntParam)
					sprintf_s(content, "%s : %d", ps->getParamName(i), ps->getInt(i));
				else
					sprintf_s(content, "%s : %s", ps->getParamName(i), ps->getString(i));
				label_[i]->setText(content);
				++nMenuItems_;
			}
			if (nMenuItems_ == 0)
			{
				changeState_ = eStateOff;
			}
			for (unsigned int i = nMenuItems_; i < eNTextLabels; ++i)
			{
				label_[i]->setText(" ");
			}
			curMenuItem_ = 0;
			chosen_ = 0xffffffff;
		}
		if (input.left)
		{
			if (ps->getType(curMenuItem_) == Game::eFloatParam)
			{
				ps->setFloat(curMenuItem_, ps->getFloat(curMenuItem_) - .05f, Editor::GetSelected() );
				char content[250];
				sprintf_s(content, "%s : %1.2f", ps->getParamName(curMenuItem_), ps->getFloat(curMenuItem_));
				label_[curMenuItem_]->setText(content);
			}
			if (ps->getType(curMenuItem_) == Game::eIntParam)
			{
				ps->setInt(curMenuItem_, ps->getInt(curMenuItem_) - 1, Editor::GetSelected());
				char content[250];
				sprintf_s(content, "%s : %d", ps->getParamName(curMenuItem_), ps->getInt(curMenuItem_));
				label_[curMenuItem_]->setText(content);
			}
		}
		if (input.right)
		{
			if (ps->getType(curMenuItem_) == Game::eFloatParam)
			{
				ps->setFloat(curMenuItem_, ps->getFloat(curMenuItem_) + .05f, Editor::GetSelected());
				char content[250];
				sprintf_s(content, "%s : %1.2f", ps->getParamName(curMenuItem_), ps->getFloat(curMenuItem_));
				label_[curMenuItem_]->setText(content);
			}
			if (ps->getType(curMenuItem_) == Game::eIntParam)
			{
				ps->setInt(curMenuItem_, ps->getInt(curMenuItem_) + 1, Editor::GetSelected());
				char content[250];
				sprintf_s(content, "%s : %d", ps->getParamName(curMenuItem_), ps->getInt(curMenuItem_));
				label_[curMenuItem_]->setText(content);
			}
		}
		if (thisNode_->keyboardMode_)
		{

			thisNode_->updateKeyInput(uctx);
			if (thisNode_->keyboardMode_)
			{
				char content[250];
				sprintf_s(content, "%s> %s~", ps->getParamName(curMenuItem_),  thisNode_->keyInput_);
				label_[curMenuItem_]->setText(content);
			}
			else
			{
				ps->setString(curMenuItem_, thisNode_->keyInput_, Editor::GetSelected());
				char content[250];
				sprintf_s(content, "%s: %s", ps->getParamName(curMenuItem_), ps->getString(curMenuItem_));
				label_[curMenuItem_]->setText(content);
				chosen_ = 0xffffffff;
				return;
			}
		}
		else if (chosen_ != 0xffffffff && ps->getType(curMenuItem_) == Game::eStringParam)
		{
			thisNode_->keyboardMode_ = true;
			strcpy_s(thisNode_->keyInput_, ps->getString(curMenuItem_));
		}
		break;
	}


	prevState_ = state_;
	if (changeState_ != eStateInvalid)
	{
		state_ = changeState_;
		changeState_ = eStateInvalid;
		chosen_ = 0xffffffff;
	}
	if (chosen_ != 0xffffffff && prevState_ != eStateEdit)
	{
		state_ = eStateOff;
		chosen_ = 0xffffffff;
	}
	if (nMenuItems_)
	{
		for (unsigned int i = 0; i < nMenuItems_; ++i)
		{
			if (curMenuItem_ == i)
			{
				label_[i]->setColor(vmath::Vector4(.5f, .0f, .0f, 1.0f));
			}
			else
			{
				label_[i]->setColor(vmath::Vector4(.0f, .0f, .0f, .5f));
			}
			vmath::Vector3 pos =
				vmath::Vector3(-6.0f, 0.0f, .0f)
				+ (float)(nMenuItems_ - i) * vmath::Vector3(.0f, .3f, .0f);

			transform_[i]->setTranslation(pos);
		}
		if (input.down && !thisNode_->keyboardMode_)
		{
			curMenuItem_ = (curMenuItem_ + 1) % nMenuItems_;
		}
		else if (input.up && !thisNode_->keyboardMode_ )
		{
			--curMenuItem_;
			if (curMenuItem_ < nMenuItems_) curMenuItem_ = 0;
		}
		else if (input.enter && !thisNode_->keyboardMode_)
		{
			chosen_ = curMenuItem_;
		}
		if (input.start && !thisNode_->keyboardMode_)
		{
			state_ = eStateOff;
		}
	}
}
