#pragma once
#include <map>
#include <list>
namespace Game {
	class Scene;
}
namespace Script
{

	typedef void (*MainMenuFunction)(void);
	typedef std::map<unsigned int, unsigned int> MainMenuFunctionMap;


	void registerMainMenuCall( char* name, MainMenuFunction ptr );
	void callMainMenuFunction( char* name );
	
	class _TimedFunction
	{
		float tMinus_ = .0f;
		Game::Scene* scene_ = nullptr;
		MainMenuFunction function_;
		char command_[500];
		bool active_ = true;
		

		bool isCommand = false;
	public:
		_TimedFunction( float time, MainMenuFunction func ) : tMinus_(time), function_( func ){  }
		_TimedFunction(float time, const char* command, Game::Scene* scene) 
			: tMinus_(time) 
			, scene_(scene)
			, isCommand(true)
		{ strcpy_s(command_, command); }
		bool isActive() const { return active_;}
		void run();
		void update( float dt )
		{
			if( !active_ ) return;
			tMinus_ -= dt;
			if( tMinus_ < .0f)
			{
				run();
				active_ = false;
			}
		}
	};

	typedef std::list<_TimedFunction> TimedFunctions;
	
	

	void updateTimedFunctions( float dt);
	void addTimedFunction( float time, MainMenuFunction func);
	void addTimedFunction(float time, Game::Scene* scene, const char* command);
	
	class MainmenuFunctionContainer
	{
		enum {
			eMaxScripts = 200,
		};


		friend void registerMainMenuCall( char* name, MainMenuFunction ptr );
		friend void callMainMenuFunction( char* name );
		MainMenuFunction scriptTable_[eMaxScripts];
		MainMenuFunctionMap map_;
		unsigned int nScripts_ = 0;
		MainmenuFunctionContainer();
		~MainmenuFunctionContainer();
	public:
		static MainmenuFunctionContainer* getInstance();
		static void releaseInstance();
	};
	
}


