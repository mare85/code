#include <Sound/Spectrum.h>
/*
 * (C) Copytight 2017 Marek Bielawski
 * Sound::Spectrum
 */
#include <Util/FFT.h>
#include <assert.h>
namespace Sound {

void Spectrum::_updateFunc()
{
	running_ = true;
	bool running = true;
	float shortToFloatScale = 1.0f / 0x8000;
	Util::FFT<WindowSize>* fft = reinterpret_cast< Util::FFT<WindowSize>* >( fft_ );

	while( running )
	{
		windowBufferMutex_.lock();

		for( unsigned int i = 0; i < WindowSize; ++i )
		{
			inRe_[ i ] = shortToFloatScale * windowBuffer_[ i ];
			inIm_[ i ] = .0f;
		}

		windowBufferMutex_.unlock();
		
		fft->reverse( inRe_, inIm_, outputRe_, outputIm_ );

		outputNormMutex_.lock();

		for( unsigned int i = 0; i < WindowSize; ++i )
		{
			float re = outputRe_[ i ];
			float im = outputIm_[ i ];
			outputNorm_[ i ] = sqrt( re*re + im*im );
		}

		outputNormMutex_.unlock();

		std::lock_guard<std::mutex> lck( runningFlagMutex_ );
		running = running_;
	}
}

Spectrum::Spectrum()
{
	feedBuffer_ = new short[ WindowSize * 2 ];
	windowBuffer_ = feedBuffer_ + WindowSize;
	inRe_ = new float[ WindowSize * 5 ];
	inIm_ = inRe_ + WindowSize;
	outputRe_ = inIm_ + WindowSize;
	outputIm_ = outputRe_ + WindowSize;
	outputNorm_ = outputIm_ + WindowSize;
	memset( inRe_, 0, sizeof(float) * WindowSize * 5 );
	memset( feedBuffer_, 0, sizeof( short ) * WindowSize * 2 );
	fft_ = new Util::FFT<WindowSize>();
	Util::FFT<WindowSize>* fft = reinterpret_cast< Util::FFT<WindowSize>* >( fft_ );
	fft->init();
	updateThread_ = std::thread( &Spectrum::_updateFunc, this );
	nReferences_ = 1;
}

Spectrum::~Spectrum()
{
	assert( ! nReferences_ );
	running_ = false;
	updateThread_.join();
	delete [] fft_;
	delete [] feedBuffer_;
	delete [] inRe_;
}

void Spectrum::feed( short* data, size_t count )
{
	unsigned int inIndex = 0;

	while( inIndex < count )
	{
		feedBuffer_[ feedSize_ ] = data[ inIndex ];
		++feedSize_;

		if( feedSize_ == WindowSize )
		{
			std::lock_guard<std::mutex> lck (windowBufferMutex_);

			for( unsigned int i = 0; i < WindowSize; ++i )
			{
				windowBuffer_[ i ] = feedBuffer_[ i ];
			}
			feedSize_ = 0;
		}
		++inIndex;
	}
}

void Spectrum::getNorm( float* outBuffer )
{
	std::lock_guard<std::mutex> lock( outputNormMutex_ );
	for( unsigned int i = 0; i < WindowSize; ++i )
	{
		outBuffer[ i ] = outputNorm_[ i ];
	}
}

}
