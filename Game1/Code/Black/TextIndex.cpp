#include "TextIndex.h"
#include <Util/Utils.h>
#include <stdio.h>
#include <windows.h>
#include <sal.h>

Black::TextFileIndex::TextFileIndex(unsigned int levelId, const char * filename)
{
	char globalFilename[300];
	sprintf_s(globalFilename, "assets/levels/level%d/%s", levelId, filename);
	unsigned int fileSize = 0;
	Util::readFile(globalFilename, &loadedFileBuffer_, &fileSize);


	// counting line ends
	lineCount_ = 1;
	for (unsigned int i = 0; i < fileSize; ++i)
	{
		if (loadedFileBuffer_[i] == '\n') ++lineCount_;
	}

	//lines_ = (char**)(malloc(lineCount_ * sizeof(void*)));

	lines_ = (char**)new void*[lineCount_];
	unsigned int lineNo = 1;
	lines_[0] = loadedFileBuffer_;

	for (unsigned int i = 0; i < fileSize; ++i)
	{
		if (loadedFileBuffer_[i] == '\r')
		{
			loadedFileBuffer_[i] = 0;
		}
		if (loadedFileBuffer_[i] == '\n'&& lineNo < lineCount_)
		{
			loadedFileBuffer_[i] = 0;
			lines_[lineNo] = loadedFileBuffer_ + i + 1;
			++lineNo;
		}
	}
}

Black::TextFileIndex::~TextFileIndex()
{
	Util::releaseFileBuffer(loadedFileBuffer_);
	//if (lines_)
	//	free(lines_);
	if (lines_)
		delete [] lines_;
}

const char * Black::TextlevelIndex::get(const char * filenameandline)
{
	char filename[300];
	unsigned int lineNo = 0;
	unsigned int len = (unsigned int)strlen(filenameandline);
	unsigned int colonIndex = 0;
	for (unsigned int i = 0; i < len; ++i)
	{
		filename[i] = filenameandline[i];
		if (filenameandline[i] == ':')
		{
			colonIndex = i;
			filename[i] = 0;
			break;
		}
	}



	sscanf_s(filenameandline + colonIndex + 1, "%d", &lineNo);
	return files_[filename]->line(lineNo);

	
}

Black::TextlevelIndex::TextlevelIndex(unsigned int levelId, const char * lang)
{
	bool searchOn = true;
	WIN32_FIND_DATA fileData;
	HANDLE fileHandle;

	char dirname[300];
	
	unsigned int langLen = (unsigned int )strlen(lang);


	sprintf_s(dirname, "assets/levels/level%d/%s/*", levelId, lang);

	

	fileHandle = FindFirstFile(dirname, &fileData);
	unsigned int dirnamelen = (unsigned int)strlen(dirname);
	if (!fileHandle)
		searchOn = false;


	while (searchOn)
	{
		const char* fname = fileData.cFileName;
		unsigned int fnamelen = (unsigned int)strlen(fname);
		if (fnamelen > 4)
		{
			const char* ext = fname + (fnamelen - 4);
			char extLower[5];
			extLower[4] = 0;
			for (unsigned int i = 0; i < 4; ++i)
			{
				extLower[i] = tolower(ext[i]);
			}
			if (strcmp(extLower, ".txt") == 0)
			{
				char fullFileName[300];
				strcpy_s(fullFileName, lang);
				fullFileName[langLen] = '/';
				strcpy_s(fullFileName + langLen + 1, 300 - langLen - 1, fname);
				files_[fname] = new TextFileIndex(levelId, fullFileName);
			}
		}
		searchOn = ( TRUE == FindNextFile(fileHandle, &fileData) );
	}
}

Black::TextlevelIndex::~TextlevelIndex()
{
	for (auto& kv : files_)
	{
		delete kv.second;
	}
}
