#include "Listener.h"

#include <assert.h>

void Input::Listener::startUp(HINSTANCE appInstance, HWND windowHandle)
{
	HRESULT hr = DirectInput8Create(appInstance, DIRECTINPUT_VERSION,
		IID_IDirectInput8, (void**)&diObject_, 0);
	assert(SUCCEEDED(hr));
	hr = diObject_->CreateDevice(GUID_SysKeyboard,
		&diKeyboard_, 0);
	assert(SUCCEEDED(hr));

	hr = diKeyboard_->SetDataFormat(&c_dfDIKeyboard);
	assert(SUCCEEDED(hr));
	hr = diKeyboard_->SetCooperativeLevel(windowHandle, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
	assert(SUCCEEDED(hr));
	hr = diKeyboard_->Acquire();
	assert(SUCCEEDED(hr));


	ZeroMemory( keyboardKeys_, sizeof( keyboardKeys_ ) );
	ZeroMemory( prevKeyboardKeys_, sizeof( prevKeyboardKeys_ ) );

	hr = diObject_->CreateDevice( GUID_SysMouse, &diMouse_, 0 );
	assert(SUCCEEDED(hr));
	diMouse_->SetDataFormat( &c_dfDIMouse );
	assert(SUCCEEDED(hr));

	hr = diMouse_->SetCooperativeLevel(windowHandle, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
	assert(SUCCEEDED( hr ) );
	hr = diMouse_->Acquire();
	assert(SUCCEEDED(hr));

}

void Input::Listener::shutDown()
{
	if (diKeyboard_)
	{
		diKeyboard_->Unacquire();
		diKeyboard_->Release();
	}
	if (diObject_)
	{
		diObject_->Release();
	}
}

void Input::Listener::update()
{
	memcpy(prevKeyboardKeys_, keyboardKeys_, sizeof(keyboardKeys_));
	diKeyboard_->GetDeviceState(sizeof(keyboardKeys_),
		(LPVOID)&keyboardKeys_);

	DIMOUSESTATE mouseState;

	memcpy( prevMouseButtons_, mouseButtons_, sizeof( mouseButtons_ ) );
	diMouse_->GetDeviceState( sizeof( DIMOUSESTATE), (LPVOID)&mouseState );
	mouseDeltaX = mouseState.lX;
	mouseDeltaY = mouseState.lY;
	if( abs( mouseDeltaX ) > 1000 ) mouseDeltaX = 0;
	if( abs( mouseDeltaY ) > 1000 ) mouseDeltaY = 0;
	
	mouseButtons_[ 0 ] = ( mouseState.rgbButtons[0] & 0x80 );
	mouseButtons_[ 1 ] = ( mouseState.rgbButtons[1] & 0x80 );
	mouseButtons_[ 2 ] = ( mouseState.rgbButtons[2] & 0x80 );
	mouseButtons_[ 3 ] = ( mouseState.rgbButtons[3] & 0x80 );
	
	for (unsigned int i = 0; i < nPads; ++i)
	{
		unsigned long result = XInputGetState(i, padState_ + i );
		padReady_[i] = (result == ERROR_SUCCESS);
	}
}

Input::Listener::Listener()
{
}


Input::Listener::~Listener()
{
}

bool Input::Listener::getPadButton(int padId, Pad::ButtonMask mask) const
{
	return (padState_[padId].Gamepad.wButtons & mask )!= 0;
}

bool Input::Listener::getPadButtonOnce(int padId, Pad::ButtonMask mask) const
{
	return  (prevPadState_[padId].Gamepad.wButtons & mask )== 0  && (padState_[padId].Gamepad.wButtons & mask )!= 0;
}
