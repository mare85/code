#include "CharacterFunctions.h"

#include <Script/ScriptFileLoader.h>
#include <Black/Character.h>

void __Teleport(const Script::Params& p, Game::Scene* scene)
{
	(void)scene;
	Black::Character* c = Black::Character::GetInstance();
	unsigned int layerId = p.intParam[0];
	float x = p.floatParam[0];
	c->teleport(layerId, x);
}

void __Visible(const Script::Params& p, Game::Scene* scene)
{
	(void)scene;
	Black::Character* c = Black::Character::GetInstance();
	int val = p.intParam[0];
	c->setVisible(val != 0);
}

void __Rundown(const Script::Params& p, Game::Scene* scene)
{
	(void)scene;
	Black::Character* c = Black::Character::GetInstance();
	int val = p.intParam[0];
	c->setRundown(val != 0);
}


void __Speed(const Script::Params& p, Game::Scene* scene)
{
	(void)scene;
	Black::Character* c = Black::Character::GetInstance();
	float val = p.floatParam[0];
	c->setTargetMaxSpeed( val );
}

void __SwitchPlaneAllow(const Script::Params& p, Game::Scene* scene)
{
	(void)scene;
	Black::Character* c = Black::Character::GetInstance();
	int val = p.intParam[0];
	c->setswitchPlaneAllowed(val != 0);
}


void Script::registerCharacterFunctions()
{
	registerCommand("character", "teleport", "fd", &__Teleport);
	registerCommand("character", "vis", "d", &__Visible);
	registerCommand("character", "rundown", "d", &__Rundown);
	registerCommand("character", "speed", "f", &__Speed);
	registerCommand("character", "switchplane", "d", &__SwitchPlaneAllow);
}
