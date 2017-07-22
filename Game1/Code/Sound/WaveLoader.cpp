#include "WaveLoader.h"
#include <stdio.h>
#include <assert.h>
#include <dsound.h>

void Sound::WaveData::load(const char * filename)
{
	FILE* f;
	fopen_s(&f, filename, "rb");
	assert(f != nullptr);
	WavHeader header;
	size_t count = fread_s(&header, sizeof(WavHeader), sizeof(WavHeader), 1, f);
	assert(count == 1);
	bool isRiff = (
		header.chunkId[0] == 'R' && 
		header.chunkId[1] == 'I' && 
		header.chunkId[2] == 'F' && 
		header.chunkId[3] == 'F' );
	bool isWave = (
		header.format[0] == 'W' &&
		header.format[1] == 'A' &&
		header.format[2] == 'V' &&
		header.format[3] == 'E' );
	bool isFmt = (
		header.subChunkId[0] == 'f' &&
		header.subChunkId[1] == 'm' &&
		header.subChunkId[2] == 't' &&
		header.subChunkId[3] == ' ');
	bool isData = (
		header.dataChunkId[0] == 'd' &&
		header.dataChunkId[1] == 'a' &&
		header.dataChunkId[2] == 't' &&
		header.dataChunkId[3] == 'a');
	assert(isRiff);
	assert(isWave);
	assert(isFmt);
	assert(isData);
	assert(header.audioFormat == WAVE_FORMAT_PCM);
	assert(header.bitsPerSample == 16);
	nChannels = header.numChannels;
	sampleRate = header.sampleRate;
	nBytes = header.dataSize;
	unsigned int nShorts = (nBytes >> 1);
	nBlocks = nShorts / nChannels;
	data = new short[nShorts];
	count = fread(data, 2, nShorts, f);
	assert(count == nShorts);
}

void Sound::WaveData::release()
{
	delete[] data;
}
