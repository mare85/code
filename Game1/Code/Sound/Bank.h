#pragma once

#include <Sound/BankScript.h>

namespace Sound
{

	class Bank
	{
		BankScriptContainer scriptContainer_;
		enum
		{
			eNameLength = 80,
			eNMaxSounds = 250
		};
		short* data_ = nullptr;
		unsigned int nBlocks_ = 0;
		unsigned char nSounds_ = 0;
		unsigned int* startOffsets_ = nullptr;
		unsigned int* lengths_ = nullptr;
		unsigned int refCounter_ = 0;
		char* nameBuffer_ = nullptr;
		char** names_ = nullptr;
	public:
		Bank( const char * scriptFileName, bool isScript);
		~Bank();
		unsigned char getNameIndex(const char* name);
		bool has(const char* name);
		bool locked() { return refCounter_ != 0; }
		void getSoundBufferLock(unsigned int index, short** buffer, unsigned int* length)
		{
			*buffer = data_ + startOffsets_[index];
			*length = lengths_[index];
			++refCounter_;
		}
		void getSoundBufferLock(const char* name, short** buffer, unsigned int* length)
		{
			unsigned int index = getNameIndex(name);
			getSoundBufferLock(index, buffer, length);
		}
		void reportUnlock()
		{
			--refCounter_;
		}

		unsigned int getScriptId(const char* name) { return scriptContainer_.getScriptId(name); }

		void getScript(unsigned int id, BankScript* out) { scriptContainer_.getScript(id, out); }
		void getScript(const char* name, BankScript* out) { scriptContainer_.getScript(name, out); };
	};


}