#pragma once
#include <dsound.h>
namespace Sound
{
	class Buffer
	{
		friend class System;
		void* buffer_;
		unsigned int nBytes_ = 0;
		unsigned int nChannels_ = 0;
		unsigned int sampleRate_ = 0;
		short* mappedData_ = nullptr;
		unsigned int mappedSize_ = 0;
		IDirectSoundBuffer8* _DsBuff() { return reinterpret_cast<IDirectSoundBuffer8*>(buffer_); }
		//static IDirectSoundBuffer8* _GetDirectSoundBuffer(Buffer* buff);
	public:
		short* lock();
		void unlock();
		void release();
		void play();
		void playLooping();
		void fillPartBuffer(void* data, unsigned int nStart, unsigned int nBytes);
		unsigned int position();

	};
}