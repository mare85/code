#pragma once
#include <map>
#include <string>
namespace Black
{
	class TextFileIndex
	{
		char* loadedFileBuffer_ = nullptr;
		unsigned int lineCount_ = 0;
		char** lines_ = nullptr;
	public:
		TextFileIndex(unsigned int levelId, const char* filename );
		~TextFileIndex();

		const char* line(unsigned int i) { return (const char*)lines_[i]; }
		const char* operator[](unsigned int index) { return (const char*)lines_[index]; }

	};

	class TextlevelIndex
	{
		typedef std::map<std::string, Black::TextFileIndex*> _FileMap;
		_FileMap files_;
	public:
		const char* get(const char* filenameandline);
		const char* operator[](const char* filenameandline) { return get(filenameandline); }
		TextlevelIndex(unsigned int levelId, const char* lang);
		~TextlevelIndex();
	};
}