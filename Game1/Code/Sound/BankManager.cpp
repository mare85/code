#include "BankManager.h"
#include <Sound/Bank.h>
#include <assert.h>

static Sound::BankManager* __instance = nullptr;


Sound::BankManager::BankManager()
{
}


Sound::BankManager::~BankManager()
{
	std::map<std::string, Bank*>::iterator it = banks_.begin();
	while( it != banks_.end() )
	{
		std::map<std::string, Bank*>::iterator it2 = it;
		++it;
		delete it2->second;
		banks_.erase(it2);
	}
}

void Sound::BankManager::StartUp()
{
	__instance = new BankManager();
}

void Sound::BankManager::ShutDown()
{
	

	delete __instance;
	__instance = nullptr;
}

void Sound::BankManager::loadBank(const char* bankName)
{
	std::string name(bankName);
	if (banks_.find(name) == banks_.end())
	{
		banks_[name] = new Bank(bankName, true);
	}
}

void Sound::BankManager::unloadBank(const char* bankName)
{
	std::string name(bankName);
	std::map<std::string, Bank*>::iterator it = banks_.find(name);
	assert(it != banks_.end());
	delete it->second;
	banks_.erase(it);
}

Sound::Bank* Sound::BankManager::get(const char* bankName)
{
	std::string name(bankName);
	std::map<std::string, Bank*>::iterator it = banks_.find(name);
	assert(it != banks_.end());
	return it->second;
}

Sound::BankManager* Sound::BankManager::Instance()
{
	assert(__instance);
	return __instance;
}
