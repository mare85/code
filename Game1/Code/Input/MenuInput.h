#pragma once


namespace Input
{

	class Listener;

	class MenuInput
	{
		
		bool leftRightImmediate_ = false;
	public:
		MenuInput();
		~MenuInput();
		void feed(Input::Listener* listener, float dt);
		void setLeftRightImmediate(bool val) { leftRightImmediate_ = val; }

		float upTimer_ = .0f;
		float downTimer_ = .0f;
		float leftTimer_ = .0f;
		float rightTimer_ = .0f;
		bool up = false;
		bool down = false;
		bool left = false;
		bool right = false;
		bool enter = false;
		bool start = false;
	};


}