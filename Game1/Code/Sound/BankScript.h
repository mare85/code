#pragma once

#include <windows.h>
#include <Util/StringArray.h>
#include <assert.h>

namespace Sound
{
	class Bank;

	class BankScript
	{
		friend class BankScriptContainer;
		unsigned int nCommands_=  0;
		void** commandParams_ = nullptr;
		unsigned char* commandIds_ = nullptr; // x nCommands
		float* delays_ = nullptr; // x nCommands
		Bank* bank_ = nullptr;
	public:
		unsigned int size() const { return nCommands_; }
		float delay(unsigned int i) const { assert(i < nCommands_);return delays_[i]; }
		unsigned char commandId(unsigned int i) const { assert(i < nCommands_);return commandIds_[i]; }
		Bank* bank() const { return bank_; }
		void* params(unsigned int i) const { assert(i < nCommands_); return commandParams_[i]; }
	};

	enum CommandId {
		eCommandPlay = 0,
		eCommandLoop = 1,
		eCommandRandom = 2

	};
	struct RandomParams
	{
		unsigned char nEntries;
		unsigned char bankIds[10];
		unsigned short volume;
		short pan;
	};
	struct PlayParams
	{
		unsigned char bankId;
		unsigned short volume;
		short pan;
	};
	struct LoopParams
	{
		unsigned char bankId;
		unsigned short volume;
		short pan;
	};


	class BankScriptContainer
	{
	public:

	private:
		friend class Bank;
		Bank* bank_ = nullptr;
		enum
		{
			eNameLength = 40,
			eCommandParamBufferSize = 20
		};

		char* nameBuffer_ = nullptr;
		char* commandParamBuffer_ = nullptr;
		char** names_ = nullptr; // x nScripts
		unsigned int nScripts_ = 0;
		unsigned char* scriptStarts_ = nullptr; // x nScripts
		unsigned char* scriptLengths_ = nullptr; // x nScripts
		unsigned int nCommands_ = 0;
		unsigned char* commandIds_ = nullptr; // x nCommands
		float* delays_ = nullptr; // x nCommands
		void** commandParam_ = nullptr; // x nCommands

		BankScriptContainer() {}
		void init(Bank* bnk, Util::StringArray& tokenizedString );
		
	public:
		~BankScriptContainer();
		unsigned int getScriptId(const char* name);
		void getScript(unsigned int id, BankScript* out);
		void getScript(const char* name, BankScript* out) { getScript(getScriptId(name), out); }

	};
}