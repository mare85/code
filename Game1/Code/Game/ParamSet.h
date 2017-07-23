#pragma once

namespace Game
{
	class Object;
}
namespace Game
{
	typedef void(*ParamChangeCallback)(Game::Object* obj);
	enum class ParamType
	{
		Float,
		String,
		Int,
	};

	class ParamSet
	{
		enum
		{
			nMaxParams = 10,
			nNameMaxLen = 80
		};


		void* paramPointer_[nMaxParams];
		char nameBuffer[nMaxParams * nNameMaxLen];
		char* names_[nMaxParams];
		ParamChangeCallback changeCallbacks_[nMaxParams];



		ParamType type_[nMaxParams];

		unsigned int nParams_ = 0;
	public:
		ParamSet();
		~ParamSet() {}
		void initNames();

		void setFloat(unsigned int id, float val, Game::Object* obj);
		void setString(unsigned int id, const char* val, Game::Object* obj);
		void setInt(unsigned int id, int val, Game::Object* obj);
		float getFloat(unsigned int id);
		const char* getString(unsigned int id);
		int getInt(unsigned int id);
		unsigned int getIdByname(const char* name);
		unsigned int registerParam(ParamType type, const char* name, void*paramPtr, ParamChangeCallback changeCallback);
		unsigned int registerFloat(const char* name, void*paramPtr, ParamChangeCallback changeCallback) {
			return registerParam(ParamType::Float, name, paramPtr, changeCallback);
		}
		unsigned int registerString(const char* name, void*paramPtr, ParamChangeCallback changeCallback) {
			return registerParam(ParamType::String, name, paramPtr, changeCallback);
		}
		unsigned int registerInt(const char* name, void*paramPtr, ParamChangeCallback changeCallback) {
			return registerParam(ParamType::Int, name, paramPtr, changeCallback);
		}
		void serialize(char* outString, unsigned int stringMaxLen, Game::Object* obj);

		ParamType getType(unsigned int id) const { return type_[id]; }
		const char* getParamName(unsigned int id) const { return (const char*)names_[id]; }
		unsigned int getNParams() const { return nParams_; }
		void copyTo(ParamSet* dst);
		void copyValuesFrom(unsigned char* valueBuffer, Game::Object* callbackObject);
		void copyTo( unsigned char* valueBuffer, unsigned int outSize);
		void clear() { nParams_ = 0; }
	};

}