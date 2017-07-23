#pragma once
#include <windows.h>
#include <Util/RandomGenerator.h>
#include <thread>
#include <mutex>

namespace Sound
{
	class Buffer;
	class Bank;
	class BankScript;
	class VoiceInput
	{
		short * buffer_ = nullptr;
		unsigned int sampleNum_ = 0;
		unsigned int progress_ = 0;
		bool looping_ = false;
		bool stopping_ = false;
		bool paused_ = false;
		Bank* bank_ = nullptr;
		unsigned short volume_ = 0xffff; // 0 to ushort max
		unsigned short scriptVolume_ = 0xffff; // 0 to ushort max
		short panning_ = 0; // -short max to short max
		short scriptPanning_ = 0; // -short max to short max
		unsigned short targetVolume_ = 0xffff; // 0 to ushort max
		short targetPanning_ = 0; // -short max to short max
		unsigned int soundId_ = 0;
		unsigned short pausePlayFade_ = 0xffff;

	public:
		
		void clear()
		{
			buffer_ = nullptr;
			sampleNum_ = 0;
			progress_ = 0;
			volume_ = 0xffff;
			panning_ = 0;
			looping_ = false;
			paused_ = false;
			pausePlayFade_ = 0xffff;
			bank_ = nullptr;
			soundId_ = 0;
		}
		void load(Bank* bnk, unsigned int inBankSoundId, unsigned int soundId, bool looping = false, bool softAttack = false,
			unsigned short volume = 0xffff, short panning = 0,
			unsigned short scriptVolume = 0xffff, short scriptPanning = 0);
		void release();
		bool isFinished() { return progress_ == sampleNum_ && !looping_; }
		bool isLoaded() { return bank_ && sampleNum_; }
		bool isPlaying() { return bank_ && ( sampleNum_ > progress_ || looping_ ); }
		bool isInBank(Bank* bnk) { return bnk == bank_; }
		void readSample(short* l, short* r);
		//short panning() const { return panning_; }
		//unsigned short volume() const { return volume_; }
		unsigned int soundId() const { return soundId_; }
		void volume(unsigned short val) { if(!stopping_) targetVolume_ = val; }
		void panning(short val) { targetPanning_ = val; }
		void softStop() { stopping_ = true; targetVolume_ = 0; }
		void pause( bool val = true) {paused_ = val;}
		void unpause() {paused_ = false;}
	};

	class Mixer
	{
		Util::RandomGenerator gen;
		std::mutex mutex_;
		enum {
			nMaxVoices  = 64,
			nQueueLength = 32,
		};
		Buffer* buffer_;
		unsigned int capturePosition_ = 0;
		VoiceInput voice_[nMaxVoices];
		unsigned int freeVoicesIndices_[nMaxVoices];
		unsigned int nFreeVoiceIndices_ = nMaxVoices;

		float playQueueDelays_[nQueueLength];
		Bank* playQueueBanks_[nQueueLength];
		bool playQueueLooping_[nQueueLength];
		bool playQueueSoftAttack_[nQueueLength];
		bool playQueueIsPaused_[nQueueLength];
		unsigned int playQueueBankIds_[nQueueLength];
		unsigned int playQueueSoundIds_[nQueueLength];
		unsigned short playQueueVolumes_[nQueueLength];
		short playQueuePanning_[nQueueLength];
		unsigned short playQueueScriptVolumes_[nQueueLength];
		short playQueueScriptPanning_[nQueueLength];

		unsigned int queueLength_ = 0;
		unsigned int lastUpdatePlayPos_ = 0;
		unsigned int soundCyclicCounter_ = 20; // for id generation goes through all uint range

		void _StartUp();
		void _ShutDown();
		unsigned int _reserveVoice();
		void _freeVoice( unsigned int i);
		Mixer();
		~Mixer() {}
		void __process();
		bool terminate_ = false;
		std::thread thread_;
		static void __processCall(void * ptr);
		void updateQueue(float deltaTime);
	public:
		static void StartUp();
		static void ShutDown();
		//unsigned int playVoice(const char* bankName, const char* soundName);
		//unsigned int playVoice(Bank * bnk, unsigned int index);
		unsigned int play(Bank * bnk, unsigned int index, unsigned short volume, short panning);
		unsigned int play(const char* bankName, const char* soundName, unsigned short volume, short panning);
		void addBankScript(BankScript* script, unsigned int soundId, unsigned short volume, short panning );
		void stopVoice(unsigned int voiceIndex);
		void stop(unsigned int soundId);
		void softStop(unsigned int soundId);
		void pause(unsigned int soundId, bool val = true);
		void unpause(unsigned int soundId) { pause(soundId, false); }
		void pauseBank(Bank* bank, bool val = true);
		void unpauseBank(Bank* bank) { pauseBank(bank, false); }
		void softStopAll();
		void stopBankImmediately(Bank* bank);

		void pauseAll(bool val = true);
		void unpauseAll() { pauseAll( false ); }

		void setVolume(unsigned int soundId, unsigned short volume);
		void setPan(unsigned int soundId, short pan);
		void stopBank(Bank * bnk);
		static Mixer* Instance();

		
	};


}