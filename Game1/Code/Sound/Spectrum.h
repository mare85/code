#ifndef SOUND_SPECTRUM_DEF
#define SOUND_SPECTRUM_DEF
/*
 * (C) Copytight 2017 Marek Bielawski
 * Sound::Spectrum
 */
#include <mutex>
#include <thread>
namespace Sound {

class Spectrum
{
public:
	static const unsigned int WindowSize = 4096;
private:
	short* feedBuffer_;
	short* windowBuffer_;
	float* inRe_;
	float* inIm_;
	float* outputRe_;
	float* outputIm_;
	float* outputNorm_;
	void* fft_;	
	size_t feedSize_ = 0;
	bool running_ = false;
	std::mutex windowBufferMutex_;
	std::mutex outputNormMutex_;
	std::mutex runningFlagMutex_;
	std::thread updateThread_;
	int nReferences_ = 0;
	void _updateFunc();
public:
	Spectrum();
	~Spectrum();
	void feed( short* data, size_t count );
	void getNorm( float* outBuffer );
	bool isReferenced() const { return nReferences_ > 0; }
	void incrementReferences() { ++nReferences_; }
	void decrementReferences() { --nReferences_; }
};

}
#endif
