#include "ParamSet.h"
#include <cstring>
#include <cstdio>
#include <assert.h>
#include <Game/Object.h>

Game::ParamSet::ParamSet()
	: nParams_(0) 
{
	initNames();
}

void Game::ParamSet::initNames()
{
	for (unsigned int i = 0; i < nMaxParams; ++i)
	{
		names_[i] = nameBuffer + nNameMaxLen * i;
	}
}

void Game::ParamSet::setFloat(unsigned int id, float val, Game::Object* obj)
{
	assert(type_[id] == ParamType::Float && id < nParams_);
	float *target = reinterpret_cast<float*>(paramPointer_[id]);
	*target = val;
	if (changeCallbacks_[id] )
	{
		changeCallbacks_[ id]( obj );
	}
}

void Game::ParamSet::setString(unsigned int id, const char * val, Game::Object* obj)
{
	assert(type_[id] == ParamType::String && id < nParams_);
	char *target = reinterpret_cast<char*>(paramPointer_[id]);
	strcpy_s(target, nNameMaxLen, val);
	if (changeCallbacks_[id])
	{
		changeCallbacks_[id](obj);
	}
}

void Game::ParamSet::setInt(unsigned int id, int val, Game::Object * obj)
{
	assert(type_[id] == ParamType::Int && id < nParams_);
	int *target = reinterpret_cast<int*>(paramPointer_[id]);
	*target = val;
	if (changeCallbacks_[id])
	{
		changeCallbacks_[id](obj);
	}
}

float Game::ParamSet::getFloat(unsigned int id)
{
	assert(type_[id] == ParamType::Float && id < nParams_);
	float *source = reinterpret_cast<float*>(paramPointer_[id]);
	return *source;
}

const char * Game::ParamSet::getString(unsigned int id)
{
	assert(type_[id] == ParamType::String && id < nParams_);
	const char *source= reinterpret_cast<const char*>(paramPointer_[id]);
	return source;
}

int Game::ParamSet::getInt(unsigned int id)
{
	assert(type_[id] == ParamType::Int && id < nParams_);
	int *source = reinterpret_cast<int*>(paramPointer_[id]);
	return *source;
}

unsigned int Game::ParamSet::getIdByname(const char * name)
{
	for (unsigned int i = 0; i < nParams_; ++i)
	{
		if (strcmp(name, names_[i]) == 0)
			return i;
	}
	assert(false);// , "givven name not found");
	return 0xffffffff;
}

unsigned int Game::ParamSet::registerParam(ParamType type, const char * name, void*paramPtr, ParamChangeCallback changeCallback)
{
	assert(nParams_ < nMaxParams);
	strcpy_s(names_[nParams_], nNameMaxLen, name);
	type_[nParams_] = type;
	paramPointer_[nParams_] = paramPtr;
	changeCallbacks_[nParams_] = changeCallback;
	++nParams_;
	return nParams_ - 1;
}

void Game::ParamSet::serialize(char * outString, unsigned int stringMaxLen, Game::Object* obj)
{
	char* bufferOut = outString + strlen(outString);
	for (unsigned int i = 0; i < getNParams(); ++i)
	{
		Game::ParamType pt = getType(i);
		if (pt == ParamType::Float)
		{
			sprintf_s(bufferOut, stringMaxLen - strlen(outString), "level.setfloat %d %s %3.2f\n",
				obj->getHashName(), getParamName(i), getFloat(i));
		}
		else if (pt == ParamType::Int)
		{
			sprintf_s(bufferOut, stringMaxLen - strlen(outString), "level.setint %d %s %d\n",
				obj->getHashName(), getParamName(i), getInt(i));
		}
		else
		{
			sprintf_s(bufferOut, stringMaxLen - strlen(outString), "level.setstring %d %s %s\n",
				obj->getHashName(), getParamName(i), getString(i));
		}
		bufferOut = outString + strlen(outString);
	}
}

void Game::ParamSet::copyTo(ParamSet * dst)
{
	memcpy(dst, this, sizeof(this));
	for (unsigned int i = 0; i < nMaxParams; ++i)
	{
		dst->names_[i] = dst->nameBuffer + nNameMaxLen * i;
	}
}

void Game::ParamSet::copyValuesFrom(unsigned char* valueBuffer, Game::Object* callbackObject)
{
	unsigned char* valuePtr = valueBuffer;
	for( unsigned int i = 0; i < nParams_; ++i )
	{
		switch( type_[i] )
		{
		case ParamType::Float:
			{
				float* fVal = reinterpret_cast<float* >( valuePtr );
				setFloat( i, *fVal,callbackObject );
				valuePtr += sizeof(float);
				break;
				
			}
		case ParamType::Int:
			{
				int* iVal = reinterpret_cast<int* >( valuePtr );
				setInt( i, *iVal,callbackObject );
				valuePtr += sizeof(int);
				break;
			}
			case ParamType::String:
				const char* sVal = reinterpret_cast<const char* >( valuePtr );
				setString( i, sVal, nullptr );
				valuePtr += strlen( sVal ) + 1;
				break;
		}
	}

}

void Game::ParamSet::copyTo(unsigned char* valueBuffer, unsigned int outSize)
{
	unsigned char* valuePtr = valueBuffer;
	unsigned int writtenSize = 0;
	for( unsigned int i = 0; i < nParams_; ++i )
	{
		switch( type_[i] )
		{
		case ParamType::Float:
			{
				writtenSize += sizeof( float );
				assert( writtenSize <= outSize );
				float* floatPtr = reinterpret_cast< float* >(valuePtr);
				*floatPtr  = getFloat( i );
				valuePtr += sizeof( float );
				break;
			}
			case ParamType::Int:
			{
				writtenSize += sizeof( int );
				assert( writtenSize <= outSize );
				int* intPtr = reinterpret_cast< int* >(valuePtr);
				*intPtr = getInt( i );
				valuePtr += sizeof( int );
				break;
			}	
			case ParamType::String:
			{
				const char* str = getString(i);
				writtenSize += static_cast<unsigned int>( strlen( str ) + 1 );
				assert( writtenSize <= outSize );
				char* strParam = reinterpret_cast<char*>(valuePtr);
				strcpy_s( strParam, 4096 - writtenSize, str );
				valuePtr += strlen( str ) + 1;
				break;
			}
		}
	}


}
