#include "EnvFunctions.h"

#include <Script/ScriptFileLoader.h>
#include <Game/App.h>
#include <Black/BirdsManager.h>


static void __FadeOut(const Script::Params& p, Game::Scene* scene)
{
	(void)p;
	float time = p.floatParam[0];
	Game::App::Instance()->fade(1.0f, time);
}
static void __FadeIn(const Script::Params& p, Game::Scene* scene)
{
	(void)p;
	float time = p.floatParam[0];
	Game::App::Instance()->fade(0.0f, time);
}

static void __Color0(const Script::Params& p, Game::Scene* scene)
{
	(void)p;
	float r = p.floatParam[0];
	float g = p.floatParam[1];
	float b = p.floatParam[2];
	Game::App::Instance()->setPPColor0(vmath::Vector3(r, g, b));
}
static void __Color1(const Script::Params& p, Game::Scene* scene)
{
	(void)p;
	float r = p.floatParam[0];
	float g = p.floatParam[1];
	float b = p.floatParam[2];
	Game::App::Instance()->setPPColor1(vmath::Vector3(r, g, b));
}

static void __BirdsBurst(const Script::Params& p, Game::Scene* scene)
{
	(void)scene;
	float rate = p.floatParam[0];
	float time = p.floatParam[1];
	Black::BirdsManager::Instance()->burst( rate, time );
}

void Script::registerEnvFunctions()
{
	registerCommand("env", "fadeout", "f", &__FadeOut);
	registerCommand("env", "fadein", "f", &__FadeIn);
	registerCommand("env", "color0", "fff", &__Color0);
	registerCommand("env", "color1", "fff", &__Color1);
	registerCommand("env", "birds", "ff", &__BirdsBurst);
}
