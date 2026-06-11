#include "global.h"
#include "InputHandler_BasicKeyboard.h"
#include "RageUtil.h"
#include "PrefsManager.h"
#include <emscripten/html5.h>

REGISTER_INPUT_HANDLER( BasicKeyboard );

static std::vector<int32_t> EventQueueKey = std::vector<int32_t>();
static std::vector<int8_t> EventQueuePressed = std::vector<int8_t>();

static bool keyCallback(int eventType, const EmscriptenKeyboardEvent *event, void *USERDATA) {
	switch (eventType)
	{
	case EMSCRIPTEN_EVENT_KEYDOWN:
		EventQueuePressed.push_back(true);
		EventQueueKey.push_back(event->charCode);
		break;
	
	case EMSCRIPTEN_EVENT_KEYUP:
		EventQueuePressed.push_back(false);
		EventQueueKey.push_back(event->charCode);
		break;
	
	default:
		break;
	}
	return true;
}

InputHandler_BasicKeyboard::InputHandler_BasicKeyboard()
{
	emscripten_set_keydown_callback("#canvas", NULL, true, &keyCallback);
	emscripten_set_keyup_callback("#canvas", NULL, true, &keyCallback);
}

InputHandler_BasicKeyboard::~InputHandler_BasicKeyboard()
{
	emscripten_set_keydown_callback("#canvas", NULL, true, NULL);
	emscripten_set_keyup_callback("#canvas", NULL, true, NULL);
}

void InputHandler_BasicKeyboard::GetDevicesAndDescriptions(vector<InputDevice>& vDevicesOut, vector<CString>& vDescriptionsOut)
{
	vDevicesOut.push_back( InputDevice(DEVICE_KEYBOARD) );
	vDescriptionsOut.push_back( "BasicKeyboard" );
}

void InputHandler_BasicKeyboard::Update(float fDeltaTime)
{
	static DeviceInput input;

	while(EventQueueKey.size() > 0) {
		input = DeviceInput(DEVICE_KEYBOARD, EventQueueKey[0]);
		ButtonPressed(input, EventQueuePressed[0]);
		EventQueueKey.erase(EventQueueKey.begin());
		EventQueuePressed.erase(EventQueuePressed.begin());
	}
	InputHandler::UpdateTimer();
}