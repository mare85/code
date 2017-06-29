#include "MenuScript.h"
#include "ScriptFileLoader.h"
#include <Util/Utils.h>

static Script::MainmenuFunctionContainer* __instance = nullptr;
Script::MainmenuFunctionContainer::MainmenuFunctionContainer()
{
}


Script::MainmenuFunctionContainer::~MainmenuFunctionContainer()
{
}

Script::MainmenuFunctionContainer* Script::MainmenuFunctionContainer::getInstance()
{
	if( !__instance )
	{
		__instance = new MainmenuFunctionContainer();
	}
	return __instance;
}

void Script::MainmenuFunctionContainer::releaseInstance()
{
	if( __instance )
	{
		delete __instance;
	}
}

void Script::registerMainMenuCall(char* name, MainMenuFunction ptr)
{
	MainmenuFunctionContainer* sc = MainmenuFunctionContainer::getInstance();
	unsigned int hashName = Util::createHash(name);
	sc->scriptTable_[sc->nScripts_] = ptr;
	sc->map_[hashName] = sc->nScripts_;
	++(sc->nScripts_);
}

void Script::callMainMenuFunction(char* name)
{
	MainmenuFunctionContainer* sc = MainmenuFunctionContainer::getInstance();
	unsigned int hashName = Util::createHash(name);
	MainMenuFunctionMap::const_iterator iter = sc->map_.find(hashName);
	if (iter == sc->map_.end())
	{
		return;
	}
	(sc->scriptTable_[ iter->second ] )();
}

static Script::TimedFunctions _timedFunctions;


void Script::updateTimedFunctions(float dt)
{
	for( TimedFunctions::iterator it = _timedFunctions.begin(); it != _timedFunctions.end();  )
	{
		it->update( dt);
		if( it->isActive() )
		{
			++it;
		}
		else
		{
			it = _timedFunctions.erase( it );
		}
	}
}

void Script::addTimedFunction(float time, MainMenuFunction func)
{
	_timedFunctions.push_back( _TimedFunction( time, func ) );
}

void Script::addTimedFunction(float time, Game::Scene * scene, const char * command)
{
	_timedFunctions.push_back(_TimedFunction(time, command, scene));
}

void Script::_TimedFunction::run()
{
	if (isCommand == false)
	{
		function_();
	}
	else
	{
		Script::runCommand(scene_, command_);
	}
}
