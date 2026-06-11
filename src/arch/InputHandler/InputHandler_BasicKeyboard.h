#ifndef INPUT_HANDLER_MONKEY_SCRIPT
#define INPUT_HANDLER_MONKEY_SCRIPT

#include "InputHandler.h"
#include "RageTimer.h"
#include "RageInputDevice.h"

class InputHandler_BasicKeyboard: public InputHandler
{
public:
	InputHandler_BasicKeyboard();
	~InputHandler_BasicKeyboard();
	void GetDevicesAndDescriptions( vector<InputDevice>& vDevicesOut, vector<CString>& vDescriptionsOut );
	void Update( float fDeltaTime );
};

#endif