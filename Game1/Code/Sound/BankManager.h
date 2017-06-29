#pragma once

#include <map>
#include <string>

namespace Sound
{

	class Bank;
	class BankManager
	{
		std::map<std::string, Bank*> banks_;

		BankManager();
		~BankManager();
	public:
		static void StartUp();
		static void ShutDown();
		void loadBank( const char* bankName );
		void unloadBank(const char* bankName);
		Bank* get(const char* bankName);
		Bank* operator[](const char* bankName) { return this->get(bankName); }
		static BankManager* Instance();
	};
}