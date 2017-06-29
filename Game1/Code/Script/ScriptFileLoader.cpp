#include "ScriptFileLoader.h"
#include <Game/Scene.h>
#include <Game/App.h>
#include <Game/Object.h>
#include <Game/ParamSet.h>
#include <Script/MenuScript.h>

#include <Script/LevelFunctions.h>
#include <Script/EnvFunctions.h>
#include <Script/CharacterFunctions.h>
#include <Script/SoundFunctions.h>




class _Commands
{
private:
	enum {
		eNMaxCommands = 200,
		eCommandNameLength = 16,
		eCommandParamsLength = 8
	};
	Script::Command commands[eNMaxCommands];
	
	unsigned int nRegisteredCommands = 0;
	char nameBuffer[eNMaxCommands * eCommandNameLength];
	char paramDescBuffer[eNMaxCommands * eCommandParamsLength];
	char* commandNames[eNMaxCommands];
	char* commandParamDescs[eNMaxCommands];
public:
	void init() {
		for (unsigned int i = 0; i < eNMaxCommands; ++i)
		{
			commandNames[i] = nameBuffer + i * eCommandNameLength;
		}
		for (unsigned int i = 0; i < eNMaxCommands; ++i)
		{
			commandParamDescs[i] = paramDescBuffer + i * eCommandNameLength;
		}
		nRegisteredCommands = 0;
	}
	void registerCommand(const char* commandName, const char* paramDesc, Script::Command com) {
		commands[nRegisteredCommands] = com;
		strcpy_s(commandNames[nRegisteredCommands], eCommandNameLength, commandName);
		strcpy_s(commandParamDescs[nRegisteredCommands], eCommandParamsLength, paramDesc);
		++nRegisteredCommands;
	}
	unsigned int num() { return nRegisteredCommands; }

	int findCommand(const char* name)
	{
		for (unsigned int i = 0; i < nRegisteredCommands; ++i)
		{
			if (strcmp(name, commandNames[i]) == 0)
			{
				return i;
			}
		}
		return -1;
	}
	void run(Game::Scene * scene, const char * commandLine)
	{
		char commandName[eCommandNameLength];
		unsigned int scannedNo = sscanf_s(commandLine, "%s", commandName, eCommandNameLength);
		char* paramsString = (char*)commandLine + strlen(commandName) + 1;

		int id = findCommand(commandName);
		assert(id != -1);

		const char *paramDesc = commandParamDescs[id];
		unsigned int nParams = (unsigned int)strlen(paramDesc);
		Script::Params p;
		unsigned int nInts = 0, nStrings = 0, nFloats = 0;

		for (unsigned int j = 0; j < nParams; ++j)
		{
			char paramString[255];
			sscanf_s(paramsString, "%s", paramString, 255);
			paramsString += strlen(paramString) + 1;
			char pDesc = paramDesc[j];
			if (pDesc == 's')
			{
				sscanf_s(paramString, "%s", p.stringBuff + nStrings * 250, 250);
				++nStrings;
			}
			if (pDesc == 'd')
			{
				sscanf_s(paramString, "%d", p.intParam + nInts);
				++nInts;
			}
			if (pDesc == 'f')
			{
				sscanf_s(paramString, "%f", p.floatParam + nFloats);
				++nFloats;
			}

		}
		(commands[id])(p, scene);
	}

};

static std::map<std::string, _Commands> __CommandsMap__;



static _Commands commands;





void Script::runScript(Game::Scene * scene, unsigned int levelId, const char * localFilename)
{
	char filename[250];
	sprintf_s(filename, "assets/levels/level%d/%s", levelId, localFilename);
	char *fileBuffer = nullptr;
	unsigned int fileSize = 0;
	Util::readFile(filename, &fileBuffer, &fileSize);
	if (!fileBuffer)
	{
		return;
	}
	float delay = .0f;
	unsigned int filePtr = 0;

	while (filePtr < fileSize)
	{
		char commandLine[600];
		unsigned int len = (unsigned int )( strstr(fileBuffer + filePtr, "\n") - fileBuffer - filePtr );
		
		len = min(fileSize - filePtr, len);
		memcpy_s(commandLine, 600, fileBuffer + filePtr, len);
		commandLine[len] = 0;
		filePtr += len + 1;
		if (filePtr > fileSize)
			break;
		float del = -200.0f;
		sscanf_s(commandLine, "@%f", &del);
		if (del >= .0f)
		{
			delay = del;
		}
		else
		{
			if (delay == .0f)
			{
				Script::runCommand(scene, commandLine);
			}
			else
			{
				addTimedFunction(delay, scene, commandLine);
			}
		}
	}
	
	Util::releaseFileBuffer(fileBuffer);
	//test me


}


void Script::saveLevel(Game::Scene* scene, const char* filename)
{
	unsigned int nSerializable = 0;
	for( Game::Object* obj : scene->objects_ )
	{
		if( obj->hasSerialize() )
		{
			++nSerializable;
			if( obj->hasPaarmSet() )
			{ 
				nSerializable += obj->getPaarmSet()->getNParams();
			}
		}
	}
	FILE* f = nullptr;
	fopen_s( &f, filename, "w" );
	for (Game::Object* obj : scene->objects_)
	{
		if (obj->hasSerialize())
		{
			unsigned char buffer[ 200 ];
			unsigned int objStrlen = 0;
			obj->serialize( buffer, &objStrlen, 200 );
			fprintf_s( f, "%s", buffer, 200);
		}
	}
	fclose(f);
}


void Script::initFileLoader()
{
	commands.init();
	registerCharacterFunctions();
	registerLevelFunctions();
	registerEnvFunctions();
	registerSoundFunctions();
}

void Script::runCommand(Game::Scene * scene, const char * commandLine)
{
	const char* strippedCommandLine = strstr(commandLine, ".") + 1;
	unsigned int namespcLen = strippedCommandLine - commandLine - 1;
	char namespc[20];
	memcpy(namespc, commandLine, namespcLen);
	namespc[namespcLen] = 0;
	__CommandsMap__[std::string(namespc)].run(scene, strippedCommandLine);
}

void Script::registerCommand(const char* namespaceName, const char* commandName, const char* paramDesc, Command com)
{
	std::string key(namespaceName);
	if (__CommandsMap__.find(key) == __CommandsMap__.end())
	{
		__CommandsMap__[key].init();
	}
	__CommandsMap__[key].registerCommand(commandName, paramDesc, com);
}