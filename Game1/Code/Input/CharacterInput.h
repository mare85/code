#pragma once


namespace Input
{

	class Listener;

	class CharacterInput
	{
	public:
		float leftX_ = .0f;
		float leftY_ = .0f;
		bool jump = false;
		void feed(Input::Listener* listener, float dt);
		CharacterInput();
		~CharacterInput();
	};

}

