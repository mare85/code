#pragma once

namespace Sound
{
	struct WavHeader
	{
		char chunkId[4];
		unsigned long chunkSize;
		char format[4];
		char subChunkId[4];
		unsigned long subChunkSize;
		unsigned short audioFormat;
		unsigned short numChannels;
		unsigned long sampleRate;
		unsigned long bytesPerSecond;
		unsigned short blockAlign;
		unsigned short bitsPerSample;
		char dataChunkId[4];
		unsigned long dataSize;
	};


	struct WaveData
	{
		unsigned int sampleRate = 0;
		unsigned int nChannels = 0;
		unsigned int nBlocks = 0;
		unsigned int nBytes = 0;
		short* data = nullptr;
		void load(const char* filename);
		void release();
	};

}