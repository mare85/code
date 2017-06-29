#pragma once
#include <windows.h>
#include <dsound.h>
#include <Util/Utils.h>

#ifndef _DEBUG
#define AK_OPTIMIZED 1
#endif


namespace Sound
{
	class Buffer;
	class Bank;
	
	class System
	{
		IDirectSound8* device_ = nullptr;
		IDirectSoundBuffer* primaryBuffer_ = nullptr;

		static System* __instance;

	public:
		static void createInstance();
		static void releaseInstance();
		void startUp(HWND hwnd);
		void shutDown();
		static System* Instance();
		Buffer* createEmpty(unsigned int nBlocks, unsigned int nChannels = 2, unsigned int sampleRate = 44100);
		Buffer* createEmpty(float time, unsigned int nChannels = 2, unsigned int sampleRate = 44100);
		Buffer* loadWav(const char* filename);

	};

	bool startUp( HWND hwnd, const char* soundsDir);
	void shutDown();
	Buffer* createEmpty(unsigned int nBlocks, unsigned int nChannels, unsigned int sampleRate);
	Buffer* loadWav(const char* filename);
	void Release(Buffer*&buff);
	void loadBank(const char* bankName);
	void unloadBank(const char* bankName);
	unsigned int playSound(const char* bankName, const char* soundName, float volume = 1.0f, float panning = .0f);
	void stopSound(unsigned int soundId, bool immediate = false);
	void stopAll();
	void stopBankImmediately(Bank* bnk);
	void setVolume(unsigned int soundId, float vol);
	void setPanning(unsigned int soundId, float pan);
	void stopBank(Bank* bnk);
	Bank* getBank(const char* bankName);
	const char* dir();
	const char* path( const char* inFile );
	void pause(unsigned int soundId, bool val = true);
	inline void unpause(unsigned int soundId) { pause(soundId, false); }
	void pauseBank(Bank* bank, bool val = true);
	inline void unpauseBank(Bank* bank) { pauseBank(bank, false); }
	void pauseAll(bool val = true);
	inline void unpauseAll() { pauseAll( false ); }


	
	
}


