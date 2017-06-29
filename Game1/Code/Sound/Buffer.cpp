#include "Buffer.h"

#include <windows.h>
#include <assert.h>

///////////////////////////////////////////////////////////////////////////////
//
//IDirectSoundBuffer8* Sound::Buffer::_GetDirectSoundBuffer(Sound::Buffer* buff)
//{
//	return reinterpret_cast<IDirectSoundBuffer8*>(buff->buffer_);
//
//}
short* Sound::Buffer::lock()
{
	HRESULT res = _DsBuff()->Lock(0, nBytes_, (void**)&mappedData_, (DWORD*)&mappedSize_, NULL, 0, 0);
	assert(SUCCEEDED(res));
	return mappedData_;
}

void Sound::Buffer::unlock()
{
	HRESULT res = _DsBuff()->Unlock((void*)mappedData_, mappedSize_, NULL, 0);
	assert(SUCCEEDED(res));
}

void Sound::Buffer::fillPartBuffer(void* data, unsigned int nStart, unsigned int nBytes)
{
	unsigned char* bufferPtr = nullptr;
	unsigned int buffersize;
	HRESULT res;
	res = _DsBuff()->Lock(nStart, nBytes, (void**)&bufferPtr, (DWORD*)&buffersize, NULL, 0, 0);
	assert(SUCCEEDED(res));
	memcpy(bufferPtr, data, nBytes);
	res = _DsBuff()->Unlock((void*)bufferPtr, buffersize, NULL, 0);
	assert(SUCCEEDED(res));

}
void Sound::Buffer::release()
{
	if (buffer_)
	{
		_DsBuff()->Release();
		buffer_ = nullptr;
	}
}
void Sound::Buffer::play()
{
	IDirectSoundBuffer8* buf = _DsBuff();
	HRESULT res;
	res = buf->SetCurrentPosition(0);
	assert(SUCCEEDED(res));
	res = buf->SetVolume(DSBVOLUME_MAX);
	assert(SUCCEEDED(res));
	res = buf->Play(0, 0, 0);
	assert(SUCCEEDED(res));
}

void Sound::Buffer::playLooping()
{
	IDirectSoundBuffer8* buf = _DsBuff();
	HRESULT res;
	res = buf->SetCurrentPosition(0);
	assert(SUCCEEDED(res));
	res = buf->SetVolume(DSBVOLUME_MAX);
	assert(SUCCEEDED(res));
	res = buf->Play(0, 0, DSBPLAY_LOOPING);
	assert(SUCCEEDED(res));
}

unsigned int Sound::Buffer::position()
{
	DWORD playPos = 0;
	_DsBuff()->GetCurrentPosition(&playPos, nullptr);
	return (unsigned int)playPos;
}
