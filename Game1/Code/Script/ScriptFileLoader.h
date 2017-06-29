#pragma once

namespace Game
{
	class Scene;
}



namespace Script
{


	struct Params
	{
		char* stringParams[3];

		char stringBuff[750];
		float floatParam[3];
		int intParam[3];
		Params() {
			stringParams[0] = stringBuff;
			stringParams[1] = stringBuff + 250;
			stringParams[2] = stringBuff + 500;
		}
	};
	typedef void(*Command)(const Params& p, Game::Scene* scene);

	void initFileLoader();
	void runCommand(Game::Scene* scene, const char* commandLine);
	void runScript(Game::Scene* scene, unsigned int levelId, const char* localFilename );
	void saveLevel(Game::Scene* scene, const char* filename);
	void registerCommand(const char* namespaceName, const char* commandName, const char* paramDesc, Command com);
}
