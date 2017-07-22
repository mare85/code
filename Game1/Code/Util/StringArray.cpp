#include "StringArray.h"
#include <Util/Utils.h>
#include <windows.h>
Util::StringArray::StringArray(const char * fiename, const char * separators)
{
	unsigned int fileLength = 0;
	readFile(fiename, &buffer_, &fileLength);
	bool isBreakChar = true;
	unsigned int nSeparators = (unsigned int)strlen(separators);
	nPointers_ = 0;
	for (unsigned int i = 0; i != fileLength; ++i)
	{
		bool newBreakChar = false;
		char c = buffer_[i];
		for (unsigned int s = 0; s != nSeparators; ++s)
		{
			if (separators[s] == c)
			{
				newBreakChar = true;
				buffer_[i] = 0;
			}
		}
		if (newBreakChar == false && isBreakChar == true)
			++nPointers_;
		isBreakChar = newBreakChar;
	}
	unsigned int nPointers = 0;
	isBreakChar = true;
	pointers_ = new char*[nPointers_];
	for (unsigned int i = 0; i != fileLength; ++i)
	{
		char c = buffer_[i];
		bool newBreakChar = (c == 0);
		if (newBreakChar == false && isBreakChar == true)
		{
			pointers_[nPointers] = buffer_ + i;
			++nPointers;
		}
		isBreakChar = newBreakChar;
		if (nPointers == nPointers_)
			return;
	}
}

Util::StringArray::~StringArray()
{
	releaseFileBuffer(buffer_);
	delete pointers_;
}

unsigned int Util::StringArray::findNext(const char * value, unsigned int start)
{
	//char otput[250];
	//wsprintfA(otput, "start %d, nPointers %d\n", start, nPointers_);
	//OutputDebugString(otput);
	for (unsigned int i = start + 1; i < nPointers_; ++i)
	{
		if (strcmp(pointers_[i], value) == 0)
		{
			return i;
		}
	}
	return 0xffffffff;
}
