#include "SoundFunctions.h"
#include <Script/ScriptFileLoader.h>
#include <Sound/System.h>

void __PlaySound(const Script::Params& p, Game::Scene* scene)
{
	(void)scene;
	Sound::playSound( p.stringParams[0], p.stringParams[1] );
}
void __LoadBank(const Script::Params& p, Game::Scene* scene)
{
	(void)scene;
	Sound::loadBank( p.stringParams[0] );
}

void __UnloadBank(const Script::Params& p, Game::Scene* scene)
{
	(void)scene;
	Sound::unloadBank( p.stringParams[0] );
}

void Script::registerSoundFunctions()
{
	registerCommand("sound", "play", "ss", &__PlaySound);
	registerCommand("sound", "load", "s", &__LoadBank);
	registerCommand("sound", "unload", "s", &__UnloadBank);
}
