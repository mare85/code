#include "System.h"

#include <assert.h>
#include <Sound/WaveLoader.h>
#include <Sound/Buffer.h>
#include <Sound/BankManager.h>
#include <Sound/Mixer.h>


static char __soundDir[512];

namespace Sound
{
///////////////////////////////////////////////////////////////////////////////

	System* System::__instance = nullptr;

	void System::createInstance()
	{
		__instance = new System();
	}
	void System::releaseInstance()
	{
		delete __instance;
	}

	System * System::Instance()
	{
		return __instance;
	}


	void System::startUp(HWND hwnd)
	{
		HRESULT res;
		res = DirectSoundCreate8(NULL, &device_, NULL);
		assert(SUCCEEDED(res));
		res = device_->SetCooperativeLevel(hwnd, DSSCL_PRIORITY);
		assert(SUCCEEDED(res));
		DSBUFFERDESC bufferDesc;
		bufferDesc.dwSize = sizeof(DSBUFFERDESC);
		bufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME;
		bufferDesc.dwBufferBytes = 0;
		bufferDesc.dwReserved = 0;
		bufferDesc.lpwfxFormat = NULL;
		bufferDesc.guid3DAlgorithm = GUID_NULL;
		res = device_->CreateSoundBuffer(&bufferDesc, &primaryBuffer_, NULL);
		assert(SUCCEEDED(res));
		WAVEFORMATEX waveFormat;
		waveFormat.wFormatTag = WAVE_FORMAT_PCM;
		waveFormat.cbSize = 0;
		waveFormat.nChannels = 2;
		waveFormat.nSamplesPerSec = 44100;
		waveFormat.nBlockAlign = waveFormat.nChannels * 2;
		waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
		waveFormat.wBitsPerSample = 16;
		res = primaryBuffer_->SetFormat(&waveFormat);
		assert(SUCCEEDED(res));
	}

	void System::shutDown()
	{
		if (primaryBuffer_)
		{
			primaryBuffer_->Release();
			primaryBuffer_ = nullptr;
		}
		if (device_)
		{
			device_->Release();
			device_ = nullptr;
		}
	}

	Buffer* System::createEmpty(unsigned int nBlocks, unsigned int nChannels, unsigned int sampleRate)
	{
		Buffer* out = new Buffer();
		out->nChannels_ = nChannels;
		out->sampleRate_ = sampleRate;
		out->nBytes_ = 2 * nChannels * nBlocks;
		
		HRESULT res;
		WAVEFORMATEX waveFormat;
		waveFormat.wFormatTag = WAVE_FORMAT_PCM;
		waveFormat.nSamplesPerSec = sampleRate;
		waveFormat.wBitsPerSample = 16;
		waveFormat.nChannels = static_cast<WORD>( nChannels );
		waveFormat.nBlockAlign = static_cast<WORD>(2 * nChannels);
		waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
		waveFormat.cbSize = 0;

		DSBUFFERDESC bufferDesc;
		bufferDesc.dwSize = sizeof(DSBUFFERDESC);
		bufferDesc.dwFlags = DSBCAPS_CTRLVOLUME;
		bufferDesc.dwBufferBytes = nBlocks * waveFormat.nBlockAlign;
		bufferDesc.dwReserved = 0;
		bufferDesc.lpwfxFormat = &waveFormat;
		bufferDesc.guid3DAlgorithm = GUID_NULL;

		IDirectSoundBuffer* tempBuf = nullptr;
		res = device_->CreateSoundBuffer(&bufferDesc, &(tempBuf), NULL);
		assert(SUCCEEDED(res));
		res = tempBuf->QueryInterface(IID_IDirectSoundBuffer8, (void**)&(out->buffer_));
		assert(SUCCEEDED(res));
		tempBuf->Release();
		return out;
	}

	Buffer * System::createEmpty(float time, unsigned int nChannels, unsigned int sampleRate)
	{
		unsigned int nBlocks = (unsigned int)(time * sampleRate);
		return createEmpty(nBlocks, nChannels, sampleRate);
	}

	Buffer* System::loadWav(const char* filename)
	{
		WaveData wav;
		wav.load(filename);
		Buffer* out = createEmpty(wav.nBlocks, wav.nChannels, wav.sampleRate);
		out->lock();
		memcpy(out->mappedData_, wav.data, wav.nBytes);
		out->unlock();
		wav.release();
		return out;
	}

	///////////////////////////////////////////////////////////////////////////////
	bool startUp(HWND hwnd, const char* soundsDir)
	{
		strcpy_s(__soundDir, soundsDir);
		System::createInstance();
		System::Instance()->startUp( hwnd);
		Mixer::StartUp();
		BankManager::StartUp();
		return true;
	}

	void shutDown()
	{
		BankManager::ShutDown();
		Mixer::ShutDown();
		System::Instance()->shutDown();
		System::releaseInstance();
	}

	Buffer* createEmpty(unsigned int nBlocks, unsigned int nChannels, unsigned int sampleRate)
	{
		return System::Instance()->createEmpty(nBlocks, nChannels, sampleRate);
	}
	
	Buffer* loadWav(const char* filename)
	{
		return System::Instance()->loadWav(filename);
	}
	void Release(Buffer*&buff)
	{
		buff->release();
		delete buff;
		buff = nullptr;
	}

	void loadBank(const char * bankName)
	{
		BankManager::Instance()->loadBank(bankName);
	}

	void unloadBank(const char * bankName)
	{
		BankManager::Instance()->unloadBank(bankName);
	}

	unsigned int playSound(const char * bankName, const char * soundName, float volume, float panning)
	{
		return Mixer::Instance()->play(bankName, soundName, (unsigned short)(volume * 0xffff), (short)(panning* 0x7fff) );
	}


	void stopSound(unsigned int soundId, bool immediate)
	{
		if (immediate)
			Mixer::Instance()->stop(soundId);
		else
			Mixer::Instance()->softStop(soundId);
	}

	void stopAll()
	{
		Mixer::Instance()->softStopAll();
	}

	void stopBankImmediately(Bank * bnk)
	{
		Mixer::Instance()->stopBankImmediately(bnk);
	}

	void setVolume(unsigned int soundId, float vol)
	{
		Mixer::Instance()->setVolume(soundId, (unsigned short)( vol * (0xffff) ) );
	}

	void setPanning(unsigned int soundId, float pan)
	{
		Mixer::Instance()->setPan(soundId, (short)(pan * (0x7fff)));
	}

	void stopBank(Bank * bnk)
	{
		Mixer::Instance()->stopBank(bnk);
	}

	Bank * getBank(const char * bankName)
	{
		return BankManager::Instance()->get(bankName);
	}

	const char * dir()
	{
		return (const char * )__soundDir;
	}

	const char * path(const char * inFile)
	{
		static char out[500];
		sprintf_s( out, "%s/%s", dir(), inFile );
		return out;
	}

	void pause(unsigned int soundId, bool val)
	{
		Mixer::Instance()->pause(soundId, val);
	}

	void pauseBank(Bank * bank, bool val)
	{
		Mixer::Instance()->pauseBank( bank, val );
	}

	void pauseAll(bool val)
	{
		Mixer::Instance()->pauseAll(val);
	}

}