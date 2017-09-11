#include "BankScript.h"
#include <Sound/Bank.h>
#include <stdio.h>

void Sound::BankScriptContainer::init(Bank * bnk, Util::Tokenizer & tokenizedString)
{
	assert(bank_ == nullptr);
	bank_ = bnk;
	nScripts_ = tokenizedString.numberOf( "script" );
	// count commands
	nCommands_ = 
		tokenizedString.numberOf( "play" ) + 
		tokenizedString.numberOf( "loop" ) + 
		tokenizedString.numberOf( "random" );
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
	typedef Util::Tokenizer::Iterator Iter;
	Iter scriptIt = tokenizedString.find( "script" );
	while ( scriptIt != end( tokenizedString ) )
	{
		Iter scriptEnd = tokenizedString.findNext( "script" , scriptIt );
		assert ( scriptEnd - scriptIt > 2 );
		strcpy_s( names_[ scriptIndex ], eNameLength, *( scriptIt + 1 ) );
		scriptStarts_[scriptIndex] = commandIndex;
		scriptLengths_[scriptIndex] = 0;
		for( Iter it = scriptIt + 2; it != scriptEnd; )
		{
			sscanf_s( *it, "@%f", delays_ + commandIndex );
			++it;
			float vol = 1.0f;
			float pan = 0.0f;
			if (strcmp( *it, "random") == 0 )
			{
				commandIds_[commandIndex] = eCommandRandom;
				RandomParams* params = reinterpret_cast<RandomParams*>(commandParam_[commandIndex]);
				++it;

				sscanf_s( *it, "%d", &(params->nEntries ) );
				++it;
				for (unsigned int i = 0; i < params->nEntries; ++i)
				{
					params->bankIds[ i ] = bank_->getNameIndex( *it );
					++it;
				}

				if ( it != scriptEnd && strcmp( *it, "volpan") == 0)
				{
					++it;
					sscanf_s( *it, "%f", &(vol)); ++it;
					sscanf_s( *it, "%f", &(pan)); ++it;
				}
				params->volume = (unsigned  short)(vol * 0xffff);
				params->pan = (unsigned  short)(pan * 0x7fff);
			}
			else if (strcmp( *it ,"play") == 0)
			{
				commandIds_[commandIndex] = eCommandPlay;
				PlayParams* params = reinterpret_cast<PlayParams*>(commandParam_[commandIndex]);
				++it;

				params->bankId = bank_->getNameIndex( *it );
				++it;
				if ( it != scriptEnd && strcmp( *it, "volpan") == 0)
				{
					++it;
					sscanf_s( *it, "%f", &(vol)); ++it;
					sscanf_s( *it, "%f", &(pan)); ++it;
				}
				params->volume = (unsigned  short)(vol * 0xffff);
				params->pan = (unsigned  short)(pan * 0x7fff);
			}
			else if( strcmp( *it, "loop") == 0 )
			{
				commandIds_[commandIndex] = eCommandLoop;
				LoopParams* params = reinterpret_cast<LoopParams*>(commandParam_[commandIndex]);
				++it;
				params->bankId = bank_->getNameIndex( *it );
				++it;
				if ( it != scriptEnd && strcmp( *it, "volpan") == 0)
				{
					++it;
					sscanf_s( *it, "%f", &(vol)); ++it;
					sscanf_s( *it, "%f", &(pan)); ++it;
				}
				params->volume = (unsigned  short)(vol * 0xffff);
				params->pan = (unsigned  short)(pan * 0x7fff);
			}
			++commandIndex;
			++scriptLengths_[ scriptIndex ];
		}
		++scriptIndex;
		scriptIt = scriptEnd;
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

