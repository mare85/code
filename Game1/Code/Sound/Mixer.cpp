#include "Mixer.h"
#include <Sound/System.h>
#include <assert.h>
#include <Sound/Bank.h>
#include <Sound/Buffer.h>
#include <Sound/BankManager.h>
#include <Sound/Spectrum.h>

void Sound::VoiceInput::load(Bank* bnk, unsigned int inBankSoundId, 
	unsigned int soundId, bool looping, bool softAttack,
	unsigned short volume, short panning, 
	unsigned short scriptVolume, short scriptPanning
	)
{
	assert( sampleNum_ == 0 && bank_ == nullptr);
	bank_ = bnk;
	progress_ = 0;
	bank_->getSoundBufferLock(inBankSoundId, &buffer_, &sampleNum_);
	looping_ = looping;
	soundId_ = soundId;
	panning_ = panning;
	volume_ = (softAttack ? 0 : volume);
	targetVolume_ = volume;
	targetPanning_= panning_;
	scriptPanning_ = scriptPanning;
	scriptVolume_ = scriptVolume;
	stopping_ = false;
}

void Sound::VoiceInput::release()
{
	bank_->reportUnlock();
	clear();
}


void Sound::VoiceInput::readSample(short* l, short* r)
{
	if (progress_ == sampleNum_)
	{
		*l = 0;
		*r = 0;
		return;
	}

	if( pausePlayFade_ == 0 && paused_)
	{
		*l = 0;
		*r = 0;
		return;
	}

	short wavVal = buffer_[progress_];
	++progress_;
	if (looping_ && progress_ == sampleNum_)
	{
		progress_ = 0;
	}
	int volume = (int)( ( (unsigned int)volume_ * (unsigned int)scriptVolume_ ) >>16 );
	int panning = max(min((int)panning_ + (int)scriptPanning_, 0x7fff), -0x7fff);
	unsigned int volLeft = ( unsigned int) ( (min(0x00007fff - panning, 0x00007fff) * volume) >> 15 );
	volLeft = ( ( volLeft * pausePlayFade_ )>> 16 );
	unsigned int volRight = (unsigned int) ((min(0x00007fff + panning, 0x00007fff) * volume) >> 15 );
	volRight = ( ( volRight * pausePlayFade_ )>> 16 );
	*l = (short)((wavVal * volLeft) >> 16);
	*r = (short)((wavVal * volRight) >> 16);

	if ((volume_ >> 6) > (targetVolume_ >> 6))
	{
		volume_ -= 0x40;
	}
	else if ((volume_ >> 6) < (targetVolume_ >> 6))
	{
		volume_ += 0x40;
	}
	else
	{
		volume_ = targetVolume_;
	}

	if ((panning_ >> 6) > (targetPanning_ >> 6))
	{
		panning_ -= 0x40;
	}
	else if ((panning_ >> 6) < (targetPanning_ >> 6))
	{
		panning_ += 0x40;
	}
	else
	{
		panning_ = targetPanning_;
	}

	if( paused_ && pausePlayFade_ )
	{
		pausePlayFade_ = ( ( ( pausePlayFade_>>6 ) - 1 )<<6 );
	}
	else if( !paused_ && pausePlayFade_ < 0xffff)
	{
		pausePlayFade_ = (unsigned short)min( (unsigned int ) (pausePlayFade_ )+ (1<<6), 0xffff);
	}

	if (stopping_ && volume_ == 0)
	{
		progress_ = sampleNum_;
		looping_ = false;
	}
}

Sound::Mixer* __instance = nullptr;

void Sound::Mixer::_StartUp()
{
	buffer_ = System::Instance()->createEmpty(4096U);
	thread_ = std::thread(__processCall, this);
	SetThreadPriority(thread_.native_handle(), THREAD_PRIORITY_HIGHEST);
}

void Sound::Mixer::_ShutDown()
{
	{
		std::lock_guard<std::mutex> lock(mutex_);
		terminate_ = true;
	}
	thread_.join();
}

unsigned int Sound::Mixer::_reserveVoice()
{
	unsigned int lastIndex = freeVoicesIndices_[nFreeVoiceIndices_ - 1];
	--nFreeVoiceIndices_;
	return lastIndex;
}

void Sound::Mixer::_freeVoice(unsigned int i)
{
	freeVoicesIndices_[nFreeVoiceIndices_] = i;
	nFreeVoiceIndices_++;
}

Sound::Mixer::Mixer()
{
	for (unsigned int i = 0; i < nMaxVoices; ++i)
	{
		voice_[i].clear();
		freeVoicesIndices_[i] = i;
	}
	nFreeVoiceIndices_ = nMaxVoices;
}

Sound::Mixer::~Mixer()
{
	delete spectrum_;
}

void Sound::Mixer::__process()
{
	std::lock_guard<std::mutex> lock(mutex_);
	unsigned int playPos = (buffer_->position() )>>2; // by four (1- strereo 2 short)
	
	unsigned int updateQueueDiff = ( ( playPos - lastUpdatePlayPos_ ) & 4095 );
	
	if (updateQueueDiff > 200) // update about 100 fps -> more frequent gets stuck
	{
		float dtime = 200 / 44100.0f;
		updateQueue(dtime);
		lastUpdatePlayPos_ = playPos;
	}

	unsigned int playSector = playPos / 1024;
	if (playSector < capturePosition_)
	{
		playSector += 4;
	}
	playSector = playSector % 4;
		if (playSector - capturePosition_ < 2)
	{
		return;
	}

	
	short outData[2048];
	short *outPtr = outData;
	for (unsigned int i = 0; i < 1024; ++i)
	{
		short outSampleL = 0;
		short outSampleR = 0;
		// voices here
		for (unsigned int j = 0; j < nMaxVoices; ++j)
		{
			VoiceInput& v = voice_[j];
			if (v.isPlaying())
			{

				short l, r;
				voice_[j].readSample(&l, &r);
				
				outSampleL += l;
				outSampleR += r;
			}
			if (voice_[j].isLoaded() && voice_[j].isFinished())
			{
				voice_[j].release();
				_freeVoice(j);
			}
			
			
		}

		// voices here
		*outPtr = outSampleL;++outPtr;
		*outPtr = outSampleR;++outPtr;

		outData[(i << 1)] = outSampleL;
		outData[(i << 1) + 1] = outSampleR;
	}
	if( spectrum_ )
	{
		short spectrumFeed[1024];
		short* outPtr = outData;
		for( unsigned int i = 0 ; i < 1024; ++i )
		{
			spectrumFeed[ i ] = (short)( ( (int)*outPtr + (int)*(outPtr + 1) ) / 2 );
			outPtr += 2;
		}
		spectrum_->feed( spectrumFeed, 1024 );
	}
	buffer_->fillPartBuffer(outData, 4096 * capturePosition_, 4096);
	capturePosition_ = (capturePosition_ + 1 ) & 3;
}

void Sound::Mixer::__processCall(void * ptr)
{
	Mixer* voices = reinterpret_cast<Mixer*>(ptr);
	voices->buffer_->playLooping();
	while(!voices->terminate_)
	{ 
		voices->__process();
	}
	//return (DWORD)0;
}

void Sound::Mixer::StartUp()
{
	assert(!__instance);
	__instance = new Sound::Mixer();
	__instance->_StartUp();
}

void Sound::Mixer::ShutDown()
{
	assert(__instance);
	__instance->_ShutDown();
	delete __instance;
	__instance = nullptr;
}
//unsigned int Sound::Mixer::playVoice(const char * bankName, const char * soundName)
//{
//	std::lock_guard<std::mutex> lock(mutex_);
//	unsigned int voiceIndex = _reserveVoice();
//	VoiceInput& voice = voice_[ voiceIndex ];
//	Bank* bnk = BankManager::Instance()->get(bankName);
//	unsigned int nameIndex = bnk->getNameIndex(soundName);
//	++soundCyclicCounter_;
//	voice.load(bnk, nameIndex, soundCyclicCounter_);
//	return soundCyclicCounter_;
//}

unsigned int Sound::Mixer::play(Bank * bnk, unsigned int index, unsigned short volume, short panning)
{
	BankScript script;
	bnk->getScript(index, &script);
	++soundCyclicCounter_;
	addBankScript(&script, soundCyclicCounter_, volume, panning);
	return soundCyclicCounter_;
}

unsigned int Sound::Mixer::play(const char* bankName, const char* soundName, unsigned short volume, short panning)
{
	BankScript script;
	Bank* bnk = BankManager::Instance()->get(bankName);
	unsigned int index = bnk->getScriptId(soundName);
	bnk->getScript(index, &script);
	++soundCyclicCounter_;
	addBankScript(&script, soundCyclicCounter_, volume, panning);
	return soundCyclicCounter_;
}

void Sound::Mixer::addBankScript(BankScript* script, unsigned int soundId, unsigned short volume, short panning)
{
	
	std::lock_guard<std::mutex> lock(mutex_);
	Bank* bank = script->bank();
	for (unsigned int i = 0; i < script->size(); ++i)
	{
		float delay = script->delay(i);
		unsigned char commandId = script->commandId(i);
		unsigned char bankId = 0;
		bool looping = false;
		bool softAttack = false;
		bool isPaused = false;
		unsigned int queueIndex = 0;
		unsigned short vol = 0xffff;
		short pan = 0;
		
		while (playQueueDelays_[queueIndex] > delay && queueIndex < queueLength_)
		{
			++queueIndex;
		}
		for (unsigned int j = queueLength_; j > queueIndex; --j)
		{
			playQueueDelays_[j ] = playQueueDelays_[j - 1];
			playQueueBanks_[j] = playQueueBanks_[j - 1];
			playQueueLooping_[j] = playQueueLooping_[j - 1];
			playQueueSoftAttack_[j] = playQueueSoftAttack_[j - 1];
			playQueueIsPaused_[j] = playQueueIsPaused_[j - 1];
			playQueueBankIds_[j] = playQueueBankIds_[j - 1];
			playQueueSoundIds_[ j ] = playQueueSoundIds_[j - 1];
			playQueueVolumes_[j] = playQueueVolumes_[j - 1];
			playQueuePanning_[j] = playQueuePanning_[j - 1];
			playQueueScriptVolumes_[j] = playQueueScriptVolumes_[j - 1];
			playQueueScriptPanning_[j] = playQueueScriptPanning_[j - 1];
		}
		
		switch (commandId)
		{
		case eCommandPlay:
		{
			PlayParams* params = reinterpret_cast<PlayParams*>(script->params(i));
			bankId = params->bankId;
			vol = params->volume;
			pan = params->pan;
			break;
		}
		case eCommandLoop:
		{
			LoopParams* params = reinterpret_cast<LoopParams*>(script->params(i));
			bankId = params->bankId;
			vol = params->volume;
			pan = params->pan;
			looping = true;
			softAttack = true;
			break;
		}
		case eCommandRandom:
		{
			RandomParams* params = reinterpret_cast<RandomParams*>(script->params(i));
			bankId = params->bankIds[gen.getUint(params->nEntries)];
			vol = params->volume;
			pan = params->pan;
			break;
		}
		};
		playQueueDelays_[queueIndex] = delay;
		playQueueBanks_[queueIndex] = bank;
		playQueueBankIds_[queueIndex] = bankId;
		playQueueLooping_[queueIndex] = looping;
		playQueueSoftAttack_[queueIndex] = softAttack;
		playQueueIsPaused_[queueIndex] = isPaused;
		playQueueSoundIds_[queueIndex] = soundId;
		playQueueVolumes_[queueIndex] = volume;
		playQueuePanning_[queueIndex] = panning;
		playQueueScriptVolumes_[queueIndex] = vol;
		playQueueScriptPanning_[queueIndex] = pan;
		++queueLength_;
	}
}

void Sound::Mixer::stopVoice(unsigned int voiceIndex)
{
	std::lock_guard<std::mutex> lock(mutex_);
	if (voice_[voiceIndex].isLoaded() )
	{
		voice_[voiceIndex].release();
		_freeVoice(voiceIndex);
	}

}
void Sound::Mixer::stop(unsigned int soundId)
{
	for (unsigned int i = 0; i < nMaxVoices; ++i)
	{
		if (voice_[i].soundId() == soundId)
		{
			stopVoice(i);
		}
	}
}

void Sound::Mixer::softStop(unsigned int soundId)
{
	for (unsigned int i = 0; i < nMaxVoices; ++i)
	{
		if (voice_[i].soundId() == soundId)
		{
			voice_[i].softStop();
		}
	}
}

void Sound::Mixer::pause(unsigned int soundId, bool val)
{
	for (unsigned int i = 0; i < nMaxVoices; ++i)
	{
		if (voice_[i].soundId() == soundId)
		{
			voice_[i].pause(val);
		}
	}
	for( unsigned int i = 0; i < queueLength_; ++i )
	{
		if( playQueueSoundIds_[i] == soundId )
		{
			playQueueIsPaused_[ i ] = val;
		}
	}
}

void Sound::Mixer::pauseBank(Bank* bank, bool val)
{
	for (unsigned int i = 0; i < nMaxVoices; ++i)
	{
		if (voice_[i].isInBank(bank) )
		{
			voice_[i].pause(val);
		}
	}
	for( unsigned int i = 0; i < queueLength_; ++i )
	{
		if( playQueueBanks_[i] == bank )
		{
			playQueueIsPaused_[ i ] = val;
		}
	}
}

void Sound::Mixer::softStopAll()
{
	for (unsigned int i = 0; i < nMaxVoices; ++i)
	{
			voice_[i].softStop();
	}
	queueLength_ = 0;
}

void Sound::Mixer::stopBankImmediately(Bank * bank)
{
	for (unsigned int i = 0; i < nMaxVoices; ++i)
	{
		if (voice_[i].isInBank( bank) )
		{
			stopVoice(i);
		}
	}
}

void Sound::Mixer::pauseAll(bool val)
{
	for (unsigned int i = 0; i < nMaxVoices; ++i)
	{
		voice_[i].pause(val);
	}
	for( unsigned int i = 0; i < queueLength_; ++i )
	{
		playQueueIsPaused_[ i ] = val;
	}
}

void Sound::Mixer::setVolume(unsigned int soundId, unsigned short volume)
{
	for (unsigned int i = 0; i < nMaxVoices; ++i)
	{
		if (voice_[i].soundId() == soundId)
		{
			voice_[i].volume(volume);
		}
	}
}

void Sound::Mixer::setPan(unsigned int soundId, short pan)
{
	for (unsigned int i = 0; i < nMaxVoices; ++i)
	{
		if (voice_[i].soundId() == soundId)
		{
			voice_[i].panning(pan);
		}
	}

}

void Sound::Mixer::stopBank(Bank * bnk)
{
	std::lock_guard<std::mutex>lock(mutex_);
	for (unsigned int i = 0; i < nMaxVoices; ++i)
	{
		VoiceInput &voice = voice_[i];
		if (voice.isLoaded() && voice.isInBank( bnk ) )
		{
			voice.release();
			_freeVoice(i);
		}
	}
}

void Sound::Mixer::initSpectrum()
{
	if( spectrum_ == nullptr )
	{
		spectrum_ = new Spectrum();
	}
	else
	{
		spectrum_->incrementReferences();
	}
}

void Sound::Mixer::freeSpectrum()
{
	if( spectrum_ )
	{
		spectrum_->decrementReferences();
		if( ! spectrum_->isReferenced() )
		{
			delete spectrum_;
			spectrum_ = nullptr;
		}
	}
}

void Sound::Mixer::getSpectrum( float* outNorm )
{
	if( spectrum_ )
		spectrum_->getNorm( outNorm );
}

Sound::Mixer* Sound::Mixer::Instance()
{
	assert(__instance);
	return __instance;
}

void Sound::Mixer::updateQueue(float deltaTime)
{
	unsigned int playIndex = queueLength_;
	for (unsigned int i = 0; i < queueLength_; ++i)
	{
		playQueueDelays_[i] -= deltaTime;
		if (playQueueDelays_[i] < .0f && playIndex == queueLength_ )
		{
			playIndex = i;
		}
	}
	
	for (unsigned int i = playIndex; i < queueLength_; ++i)
	{
		unsigned int voiceIndex = _reserveVoice();
		VoiceInput& voice = voice_[voiceIndex];
		voice.load(playQueueBanks_[i], playQueueBankIds_[i], playQueueSoundIds_[i], playQueueLooping_[i], playQueueSoftAttack_[i],
			playQueueVolumes_[i], playQueuePanning_[i], playQueueScriptVolumes_[i], playQueueScriptPanning_[i]);

		if( playQueueIsPaused_[i] )
		{
			voice.pause();
		}
	}
	queueLength_ = playIndex;
}
