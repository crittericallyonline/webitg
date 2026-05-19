#include "global.h"

/* This handler is used for odd cases where we don't use SDL for input. */

#include "InputHandler_Linux_tty.h"
#include "InputHandler_Linux_tty_keys.h"

#include "../../RageUtil.h"
#include "../../RageLog.h"
#include "../../RageException.h"

#include <emscripten/html5.h>
#include <GLFW/glfw3.h>

#include "../../archutils/Unix/SignalHandler.h"


#include <errno.h>

#include <sys/ioctl.h>
#include <fcntl.h>

#include <termios.h>


static int keys[GLFW_KEY_LAST];

/* This is normally a singleton.  Keep track of it, so we can access it
 * from our signal handler. */
static InputHandler_Linux_tty *handler = NULL;

void InputHandler_Linux_tty::OnCrash(int signo)
{
	/* Make sure we delete the input handler if we crash, so we don't leave
	 * the terminal in raw mode. */
	delete handler;
	handler = NULL;
}

static EM_BOOL keyboardCallback(int eventType, const EmscriptenKeyboardEvent *event, void *userData) {
	switch (eventType)
	{
	case EMSCRIPTEN_EVENT_KEYUP:
		keys[event->charCode] = 1;
		break;

	case EMSCRIPTEN_EVENT_KEYDOWN:
		keys[event->charCode] = 0;
		break;

	default:
		return EM_FALSE;
	}
	return EM_TRUE;
}

InputHandler_Linux_tty::InputHandler_Linux_tty()
{
	memset(keys, 0, sizeof(keys));

	emscripten_set_keydown_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, NULL, true, keyboardCallback);
	emscripten_set_keyup_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, NULL, true, keyboardCallback);

	handler = this;
	SignalHandler::OnClose(OnCrash);
}


InputHandler_Linux_tty::~InputHandler_Linux_tty()
{
	emscripten_set_keydown_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, NULL, true, NULL);
	emscripten_set_keyup_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, NULL, true, NULL);
	handler = NULL;
}

void InputHandler_Linux_tty::Update(float fDeltaTime)
{
	glfwPollEvents();
	InputHandler::UpdateTimer();
}

void InputHandler_Linux_tty::GetDevicesAndDescriptions(vector<InputDevice>& vDevicesOut, vector<CString>& vDescriptionsOut)
{
	vDevicesOut.push_back( InputDevice(DEVICE_KEYBOARD) );
	vDescriptionsOut.push_back( "Keyboard" );
}

/*
 * (c) 2003-2004 Glenn Maynard
 * All rights reserved.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, and/or sell copies of the Software, and to permit persons to
 * whom the Software is furnished to do so, provided that the above
 * copyright notice(s) and this permission notice appear in all copies of
 * the Software and that both the above copyright notice(s) and this
 * permission notice appear in supporting documentation.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF
 * THIRD PARTY RIGHTS. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR HOLDERS
 * INCLUDED IN THIS NOTICE BE LIABLE FOR ANY CLAIM, OR ANY SPECIAL INDIRECT
 * OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */
