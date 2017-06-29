#pragma once
#include <assert.h>

namespace Util
{
	class StringArray
	{
		char** pointers_ = nullptr;
		char* buffer_ = nullptr;
		unsigned int nPointers_ = 0;
	public:
		StringArray(const char* fiename, const char* separators= " \n\t\r");
		~StringArray();
		const char* get(unsigned int i) { assert(i < nPointers_); return pointers_[i]; }
		const char* operator[](unsigned int i) { return get(i); }
		unsigned int size() { return nPointers_; }
		unsigned int findNext(const char* value, unsigned int start);
	

	};

}
