#pragma once
#include <windows.h>
#include <dinput.h>
#include <xinput.h>
#include <Util/Utils.h>
#include <Input/Constants.h>

namespace Input
{
	class Listener
	{
		enum {
			nPads = 4
		};
		LPDIRECTINPUT8 diObject_;
		LPDIRECTINPUTDEVICE8 diKeyboard_;
		LPDIRECTINPUTDEVICE8 diMouse_;
		bool keyboardReady_ = false;
		char keyboardKeys_[256];
		char prevKeyboardKeys_[256];
		int mouseDeltaX;
		int mouseDeltaY;
		char mouseButtons_[4];
		char prevMouseButtons_[4];

		XINPUT_STATE padState_[nPads];
		XINPUT_STATE prevPadState_[nPads];
		bool padReady_[nPads];

	public:

		void startUp(HINSTANCE appInstance, HWND windowHandle);
		void shutDown();
		void update();
		Listener();
		~Listener();

		bool isKeyPressed(unsigned char id)  const { return (keyboardKeys_[id] & 0x80) != 0; }
		bool isKeyPressed(Key c)  const { return isKeyPressed(static_cast<unsigned char>(c)); }

		bool getKeyDown(unsigned char id)    const { return (keyboardKeys_[id] & 0x80) != 0 && (prevKeyboardKeys_[id] & 0x80) == 0; }
		bool getKeyDown(Key c)  const { return getKeyDown(static_cast<unsigned char>(c)); }
		bool getKeyUp(unsigned char id)      const { return (keyboardKeys_[id] & 0x80) == 0 && (prevKeyboardKeys_[id] & 0x80) != 0; }
		bool getKeyUp(Key c)  const { return getKeyUp(static_cast<unsigned char>(c)); }
		
		bool isMousePressed(unsigned char id)  const { return mouseButtons_[id] != 0; }
		bool getMouseDown(unsigned char id)    const { return mouseButtons_[id] != 0 && prevMouseButtons_[id] == 0; }
		bool getMouseUp(unsigned char id)      const { return mouseButtons_[id] == 0 && prevMouseButtons_[id] != 0; }

		int getMouseDeltaX() {return mouseDeltaX; }
		int getMouseDeltaY() {return mouseDeltaY; }
		vmath::Vector3 getMouseDelta() { return vmath::Vector3(mouseDeltaX * 1.0f, mouseDeltaY*  1.0f, .0f); }



		bool isPadActive(int padId)          const { return padReady_[padId]; }
		float getPadLeftTrigger(int padId)   const { return padState_[padId].Gamepad.bLeftTrigger / 255.0f; }
		float getPadRightTrigger(int padId)  const { return padState_[padId].Gamepad.bRightTrigger / 255.0f; }

		vmath::Vector3 getPadLeftStick(int padId) const { 
			float x = padState_[padId].Gamepad.sThumbLX * 1.0f / SHRT_MAX;
			float y = padState_[padId].Gamepad.sThumbLY * 1.0f / SHRT_MAX;
			return vmath::Vector3(x, y, .0f);
		}
		vmath::Vector3 getPadRightStick(int padId) const {
			float x = padState_[padId].Gamepad.sThumbRX * 1.0f / SHRT_MAX;
			float y = padState_[padId].Gamepad.sThumbRY * 1.0f / SHRT_MAX;
			return vmath::Vector3(x, y, .0f);
		}

		bool getPadButton( int padId, Pad::ButtonMask mask) const;
		bool getPadButtonOnce(int padId, Pad::ButtonMask mask) const;
		

	};

}
