#include "BankScript.h"
#include <Sound/Bank.h>
#include <stdio.h>

void Sound::BankScriptContainer::init(Bank * bnk, Util::StringArray & tokenizedString)
{
	assert(bank_ == nullptr);
	bank_ = bnk;
	//count scripts
	nScripts_ = 0;
	unsigned int index = 0xffffffff;
	while ((index = tokenizedString.findNext("script", index)) != 0xffffffff)
	{
		++nScripts_;
	}
	// count commands
	nCommands_ = 0;
	while ((index = tokenizedString.findNext("play", index)) != 0xffffffff)
		++nCommands_;
	while ((index = tokenizedString.findNext("loop", index)) != 0xffffffff)
		++nCommands_;
	while ((index = tokenizedString.findNext("random", index)) != 0xffffffff)
		++nCommands_;
	assert(nCommands_ && nScripts_);
	// alloc
	scriptStarts_ = new unsigned char[nScripts_ * 2];
	scriptLengths_ = scriptStarts_ + nScripts_;
	commandIds_ = new unsigned char[nCommands_];
	delays_ = new float[nCommands_];
	nameBuffer_ = new char[nScripts_ * eNameLength];
	commandParamBuffer_ = new char[nCommands_ * eCommandParamBufferSize];
	names_ = new char*[nScripts_];

	commandParam_ = new void*[nCommands_];
	for (unsigned int i = 0;i < nCommands_; ++i)
	{
		commandParam_[i] = (void*)(commandParamBuffer_ + i * eCommandParamBufferSize);
	}
	for (unsigned int i = 0; i < nScripts_; ++i)
	{
		names_[i] = nameBuffer_ + i * eNameLength;
	}
	unsigned char commandIndex = 0;
	unsigned char scriptIndex = 0;
	index = tokenizedString.findNext("script", 0xffffffff);
	while (commandIndex != nCommands_)
	{
		++index;
		strcpy_s(names_[scriptIndex], eNameLength, tokenizedString[index] );
		++index;
		unsigned int nextIndex = tokenizedString.findNext("script", index);
		scriptStarts_[scriptIndex] = commandIndex;
		scriptLengths_[scriptIndex] = 0;
		while (index < nextIndex && commandIndex != nCommands_)
		{
			
			sscanf_s(tokenizedString[index], "@%f", delays_ + commandIndex);
			++index;
			float vol = 1.0f;
			float pan = 0.0f;
			if (strcmp( tokenizedString[index], "random") == 0 )
			{
				commandIds_[commandIndex] = eCommandRandom;
				RandomParams* params = reinterpret_cast<RandomParams*>(commandParam_[commandIndex]);
				++index;
				

				sscanf_s(tokenizedString[index], "%d", &(params->nEntries ) );
				for (unsigned int i = 0; i < params->nEntries; ++i)
				{
					++index;
					params->bankIds[ i ] = bank_->getNameIndex(tokenizedString[index]);
				}
				if (index + 1 < tokenizedString.size() && strcmp(tokenizedString[index + 1], "volpan") == 0)
				{
					sscanf_s(tokenizedString[index + 2], "%f", &(vol));
					sscanf_s(tokenizedString[index + 3], "%f", &(pan));
					
					index += 3;
				}
				params->volume = (unsigned  short)(vol * 0xffff);
				params->pan = (unsigned  short)(pan * 0x7fff);
			}
			else if (strcmp(tokenizedString[index], "play") == 0)
			{
				commandIds_[commandIndex] = eCommandPlay;
				PlayParams* params = reinterpret_cast<PlayParams*>(commandParam_[commandIndex]);
				++index;
				params->bankId = bank_->getNameIndex(tokenizedString[index]);
				if (index + 1 < tokenizedString.size() && strcmp(tokenizedString[index + 1], "volpan") == 0)
				{
					sscanf_s(tokenizedString[index + 2], "%f", &(vol));
					sscanf_s(tokenizedString[index + 3], "%f", &(pan));
					index += 3;
				}
				params->volume = (unsigned  short)(vol * 0xffff);
				params->pan = (unsigned  short)(pan * 0x7fff);
			}
			else if (strcmp(tokenizedString[index], "loop") == 0)
			{
				commandIds_[commandIndex] = eCommandLoop;
				LoopParams* params = reinterpret_cast<LoopParams*>(commandParam_[commandIndex]);
				++index;
				params->bankId = bank_->getNameIndex(tokenizedString[index]);
				if (index + 1 < tokenizedString.size() && strcmp(tokenizedString[index + 1], "volpan") == 0)
				{
					sscanf_s(tokenizedString[index + 2], "%f", &(vol));
					sscanf_s(tokenizedString[index + 3], "%f", &(pan));
					index += 3;
				}
				params->volume = (unsigned  short)(vol * 0xffff);
				params->pan = (unsigned  short)(pan * 0x7fff);
			}
			else
			{
				assert(false);
			}
			++commandIndex;
			++scriptLengths_[scriptIndex];
			++index;
		}
		++scriptIndex;
	}
}


unsigned int Sound::BankScriptContainer::getScriptId(const char* name)
{
	for (unsigned int i = 0; i < nScripts_; ++i)
	{
		if (strcmp(names_[i], name) == 0)
			return i;
	}
	assert(false);
	return 0;
}

void Sound::BankScriptContainer::getScript(unsigned int id, BankScript* out)
{
	assert(id < nScripts_);
	unsigned int len = scriptLengths_[id];
	out->nCommands_ = len;
	out->bank_ = bank_;
	unsigned int startInd = scriptStarts_[id];
	out->commandIds_ = commandIds_ + startInd;
	out->commandParams_ = commandParam_ + startInd;
	out->delays_ = delays_ + startInd;
}


Sound::BankScriptContainer::~BankScriptContainer()
{
	delete[] scriptStarts_;
	delete[] commandIds_;
	delete[] nameBuffer_;
	delete[] commandParamBuffer_;
	delete[] names_;
}

