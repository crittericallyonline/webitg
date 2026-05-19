#include "../../global.h"
#include "LowLevelWindow_Emscripten.h"
#include "../../RageLog.h"
#include "../../RageDisplay.h" // for REFRESH_DEFAULT
#include "../../StepMania.h"

#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <GLES2/gl2.h>

// EM_BOOL LowLevelWindow_Emscripten::KeyboardEvent(int eventType, const EmscriptenKeyboardEvent *event, void *userData) {
// 	LowLevelWindow_Emscripten myWin = *((LowLevelWindow_Emscripten *) userData);
// 	return EM_TRUE;
// }

EM_BOOL LowLevelWindow_Emscripten::ResizeEvent(int eventType, const EmscriptenUiEvent *event, void *userData) {
	if(eventType != EMSCRIPTEN_EVENT_RESIZE) return EM_FALSE;
	LowLevelWindow_Emscripten myWin = *((LowLevelWindow_Emscripten *) userData);
	myWin.CurrentParams.width = event->documentBodyClientWidth;
	myWin.CurrentParams.height = event->documentBodyClientHeight;
	DISPLAY->ResolutionChanged();
	return EM_TRUE;
}

LowLevelWindow_Emscripten::LowLevelWindow_Emscripten()
{
	if(!glfwInit()) {
		this->~LowLevelWindow_Emscripten();
		return;
	}
	
	emscripten_webgl_init_context_attributes(&Attrs);
    Attrs.majorVersion = 2;
    Attrs.minorVersion = 2;
	Context = emscripten_webgl_create_context("#canvas", &Attrs);

	// callbacks
	// emscripten_set_keydown_callback_on_thread(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, this, true, this->KeyboardEvent, EM_CALLBACK_THREAD_CONTEXT_MAIN_RUNTIME_THREAD);
	// emscripten_set_keypress_callback_on_thread(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, this, true, this->KeyboardEvent, EM_CALLBACK_THREAD_CONTEXT_MAIN_RUNTIME_THREAD);
	// emscripten_set_keyup_callback_on_thread(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, this, true, this->KeyboardEvent, EM_CALLBACK_THREAD_CONTEXT_MAIN_RUNTIME_THREAD);
	emscripten_set_resize_callback_on_thread(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, this, true, this->ResizeEvent, EM_CALLBACK_THREAD_CONTEXT_MAIN_RUNTIME_THREAD);
}

LowLevelWindow_Emscripten::~LowLevelWindow_Emscripten()
{
	// emscripten_set_keydown_callback_on_thread(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, nullptr, true, nullptr, EM_CALLBACK_THREAD_CONTEXT_MAIN_RUNTIME_THREAD);
	// emscripten_set_keypress_callback_on_thread(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, nullptr, true, nullptr, EM_CALLBACK_THREAD_CONTEXT_MAIN_RUNTIME_THREAD);
	// emscripten_set_keyup_callback_on_thread(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, nullptr, true, nullptr, EM_CALLBACK_THREAD_CONTEXT_MAIN_RUNTIME_THREAD);
	emscripten_set_resize_callback_on_thread(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, nullptr, true, nullptr, EM_CALLBACK_THREAD_CONTEXT_MAIN_RUNTIME_THREAD);
	emscripten_webgl_destroy_context(Context);
}

void *LowLevelWindow_Emscripten::GetProcAddress(CString s)
{
    return nullptr;
}

CString LowLevelWindow_Emscripten::TryVideoMode( RageDisplay::VideoModeParams p, bool &bNewDeviceOut )
{
	//check if we even need to update the icon before updating it every TryVideoMode call.
	if(CurrentParams.sIconFile != p.sIconFile) {
		EM_ASM({
			console.log("Windowed: " + $0);
			let link = document.querySelector("link[rel~='icon']");
			if (!link) {
				link = document.createElement('link');
				link.rel = 'icon';
				document.head.appendChild(link);
			}
			link.href = stringToUTF8($0);
		}, p.sIconFile.c_str()); // pass in the path to change the favicon.(x) to the new string
	}
	CurrentParams = p;
	
	EM_ASM({
		console.log("Windowed: " + $0);
		document.body.style.cursor = $0 ? "" : "none";
	}, p.windowed);

	bNewDeviceOut = true;
	return "";	// we set the video mode successfully
}

void LowLevelWindow_Emscripten::SwapBuffers()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	emscripten_webgl_commit_frame();
	glFlush();
}

void LowLevelWindow_Emscripten::Update(float fDeltaTime)
{
	glfwPollEvents();
}