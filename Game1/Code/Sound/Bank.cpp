#include "Bank.h"
#include <Util/Utils.h>
#include <assert.h>
#include <windows.h>
#include <string.h>
#include <Sound/WaveLoader.h>
#include <Sound/System.h>
#include <Util/Tokenizer.h>

Sound::Bank::Bank(const char * scriptFileName, bool isScript)
{
	assert(isScript);
	char* fileBuffer = nullptr;
	unsigned int fileSize;
	Util::readFile( path( scriptFileName ), &fileBuffer, &fileSize );

	Util::Tokenizer tokens( fileBuffer );

	unsigned int nVoiceTokens = 0;

	for (unsigned int i = 0; i < tokens.size(); ++i)
	{
		if (strcmp(tokens[i], "script") == 0)
			break;
		++nVoiceTokens;
	}
	assert(!(nVoiceTokens & 1)); //parity
	assert(nVoiceTokens <= 2 * eNMaxSounds);
	nSounds_ = static_cast<unsigned char>( nVoiceTokens >> 1);
	
	nameBuffer_ = new char[nSounds_ * eNameLength];
	names_ = new char*[nSounds_];
	startOffsets_ = new unsigned int[nSounds_ * 2];
	lengths_ = startOffsets_ + nSounds_;
	WaveData* wavedatas = new WaveData[nSounds_];
	nBlocks_ = 0;
	for (unsigned int i = 0; i < nSounds_; ++i)
	{
		names_[i] = nameBuffer_ + i * eNameLength;
		const char* name = tokens[i * 2];
		const char* filename = tokens[i * 2 + 1];
		strcpy_s(names_[i], (rsize_t)eNameLength, name);
		wavedatas[i].load( path( filename ) );
		assert(wavedatas[i].nChannels == 1);
		startOffsets_[i] = nBlocks_;
		lengths_[i] = wavedatas[i].nBlocks;
		nBlocks_ += wavedatas[i].nBlocks;
	}
	data_ = new short[nBlocks_];
	for (unsigned int i = 0; i < nSounds_; ++i)
	{
		memcpy(data_ + startOffsets_[i], wavedatas[i].data, lengths_[i] * sizeof(short));
	}
	delete[] wavedatas;
	scriptContainer_.init(this, tokens);
	Util::releaseFileBuffer( fileBuffer );
}

Sound::Bank::~Bank()
{
	Sound::stopBankImmediately( this);
	assert(refCounter_ == 0);

	if(nameBuffer_)
		delete [] nameBuffer_;
	if (names_)
		delete [] names_;
	if (startOffsets_)
		delete[] startOffsets_;
}

unsigned char Sound::Bank::getNameIndex(const char * name)
{
	for (unsigned char i= 0; i < nSounds_; ++i)
	{
		if (strcmp(name, names_[i]) == 0)
		{
			return i;
		}
	}
	return 0xff;
}

bool Sound::Bank::has(const char * name)
{
	for (unsigned int i = 0; i < nSounds_; ++i )
	{
		if (strcmp(name, names_[i]) == 0)
		{
			return true;
		}
	}
	return false;
}
