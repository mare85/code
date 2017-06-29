#pragma once


namespace Input
{
	class Listener;
}

namespace Game
{
	struct UpdateContext
	{
		float deltaTime;
		Input::Listener* input;
	};

	class Updater
	{
	public:
		
		Updater() {}
		virtual ~Updater() {}

		virtual void start() = 0;
		virtual void update(const UpdateContext* uctx) = 0;
		virtual void stop() = 0;
	};

}