#include "TextSubsystem.h"
#include <Game/Scene.h>
#include <Game/Transform.h>
#include <Game/Text.h>
#include <Black/Character.h>
#include <assert.h>
#include <stdio.h>


static Black::TextSubsystem* __instance = nullptr;

void Black::TextSubsystem::_removeText(unsigned int i)
{
	--num_;
	if (num_ != i)
	{
		progress_[i] = progress_[num_];
		positions_[i] = positions_[num_];
		velocities_[i] = velocities_[num_];
		time_[i] = time_[num_];
		Game::Text* tmpText = textObjs_[i];
		textObjs_[i] = textObjs_[num_];
		textObjs_[num_] = tmpText;
		Game::Transform* tmpTransform = transforms_[i];
		transforms_[i] = transforms_[num_];
		transforms_[num_] = tmpTransform;
		strcpy_s(texts_[i], nTextLength, texts_[num_]);
	}
}

Black::TextSubsystem::TextSubsystem(Game::Scene* scene, unsigned int levelId, const char* lang, const char* fontName )
	: textIndex_(levelId, lang)
{
	assert(__instance == nullptr);
	__instance = this;
	for (unsigned int i = 0; i < nMaxTexts; ++i)
	{
		texts_[i] = textBuffer + nTextLength * i;
		char name[30];
		sprintf_s(name, "textSubSystem%d", i);
		textObjs_[i] = new Game::Text(name);
		Game::Text* text = textObjs_[i];
		text->setLayer(101);
		
		text->setText(" ");
		text->setColor( vmath::Vector4( 1.0f, 1.0f, 1.0f, .0f) );
		text->setMaxCharNumber(nTextLength);
		text->setBaseScale(vmath::Vector3(12.f, 12.f, 1.0f));
		textObjs_[i]->prepareFont(fontName);
		transforms_[i] = new Game::Transform(name);
		transforms_[i]->carryObject(textObjs_[i]);
		scene->addTransform(transforms_[i]);
	}
}

Black::TextSubsystem::~TextSubsystem()
{
	__instance = nullptr;
}

void Black::TextSubsystem::update(const Game::UpdateContext * uctx)
{
	float dt = uctx->deltaTime;
	vmath::Vector3 camPos = Character::GetInstance()->cameraPosition();


	// removing
	for (unsigned int i = num_ - 1; i < num_; --i)
	{
		if (progress_[i] > time_[i])
		{
			_removeText(i);
		}
	}

	for ( unsigned int i = 0; i < num_; ++i )
	{
		progress_[ i ] += dt;
		float col = min(progress_[i] / fadeTime_, 0.9f);
		col = min((time_[i] - progress_[i]) / fadeTime_, col);
		positions_[i] += velocities_[i] * dt;
		vmath::Vector3 camSpacePos = positions_[i] - camPos;
		transforms_[i]->setTranslation(camSpacePos);
		textObjs_[i]->setColor(vmath::Vector4(1.0f, 1.0f, 1.0f, col));
		transforms_[i]->setScale(vmath::Vector3(1.0f, 1.0f, 1.0f));
	}
	for (unsigned int i = num_; i < nMaxTexts; ++i)
	{
		transforms_[i]->setScale(vmath::Vector3(0.0f, 0.0f, 1.0f));
	}

	/*const char* debugText = ("debug_text");
	static float debugTimer = .0f;
	debugTimer += dt;
	if (debugTimer > 0.8f)
	{
		debugTimer = .0f;
		addText(debugText, 4.0f);
	}*/
}

void Black::TextSubsystem::addText(const char * text, float time)
{
	assert(num_ < 10);
	vmath::Vector3 camPos = Character::GetInstance()->cameraPosition();
	time_[num_] = time;
	progress_[num_] = .0f;
	strcpy_s(texts_[num_], nTextLength, text);
	textObjs_[num_]->setText(text);
	positions_[num_] = camPos 
		+ vmath::Vector3(-0.8f, -3.0f, .0f) 
		+ vmath::rotate( 
			vmath::Quat::rotationZ( gen.getFloat( -1.0f, 1.0f) ), 
				vmath::Vector3(0.0f, 3.0f, .0f) );
	velocities_[num_] = vmath::Vector3(gen.getFloat(-.1f, .1f), gen.getFloat(-.1f, .1f), .0f);
	++num_;
}

void Black::TextSubsystem::addText(const char * filenameAndLine)
{
	unsigned char buffer[nTextLength];
	Util::utf8RawToLatin2Pl((unsigned char*)textIndex_[filenameAndLine], buffer, nTextLength);
	addText((const char*)buffer, 7.0f); // 7.0f 

}

Black::TextSubsystem* Black::TextSubsystem::Instance()
{
	assert(__instance);
	return __instance;
}

