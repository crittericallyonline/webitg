#include "global.h"
#include "LowLevelWindow_GLFW.h"
#include <GLFW/glfw3.h>

#ifdef __EMSCRIPTEN__ // --use-port=contrib.glfw3
#include <GLFW/emscripten_glfw3.h>
#include <emscripten/html5_webgl.h>
#endif

#include "RageLog.h"
#include "RageDisplay.h" // for REFRESH_DEFAULT
#include "StepMania.h"

static void *ResizeEvent(GLFWwindow* glfwDisplay, int width, int height) {
    LowLevelWindow_GLFW istg = *(LowLevelWindow_GLFW *)glfwGetWindowUserPointer(glfwDisplay);
    istg.SetSize(width, height);
    return NULL;

}
static void *FocusEvent(GLFWwindow* glfwDisplay, int focused) {
    LowLevelWindow_GLFW istg = *(LowLevelWindow_GLFW *)glfwGetWindowUserPointer(glfwDisplay);
    istg.SetClosed();
    return NULL;

}
static void *CloseEvent(GLFWwindow* glfwDisplay) {
    // i swear to god
    LowLevelWindow_GLFW istg = *(LowLevelWindow_GLFW *)glfwGetWindowUserPointer(glfwDisplay);
    istg.SetClosed();
    return NULL;
}

LowLevelWindow_GLFW::LowLevelWindow_GLFW()
{
    if(!glfwInit()) {
        LOG->Trace("glfwInit() Failed, exiting program.");
        throw "UNABLE TO INITIALIZE GLFW";
    }

#ifdef __EMSCRIPTEN__
    emscripten::glfw3::SetNextWindowCanvasSelector("#canvas");
#endif

    glfwDisplay = glfwCreateWindow(640, 480, "GLFW Window", 0, 0);
    glfwMakeContextCurrent(glfwDisplay);
    glfwSetWindowUserPointer(glfwDisplay, reinterpret_cast<void *>(this));
    
    // setting the callbacks early
    glfwSetWindowSizeCallback(glfwDisplay, (GLFWwindowsizefun) &ResizeEvent);
    glfwSetWindowFocusCallback(glfwDisplay, (GLFWwindowfocusfun) &FocusEvent);
    glfwSetWindowCloseCallback(glfwDisplay, (GLFWwindowclosefun) &CloseEvent);
}

LowLevelWindow_GLFW::~LowLevelWindow_GLFW()
{
#ifndef __EMSCRIPTEN__
    emscripten::glfw3::UnmakeCanvasResizable(glfwDisplay);
    glfwDestroyWindow(glfwDisplay);
    glfwMakeContextCurrent(0);
    glfwTerminate();
#else
    emscripten_webgl_destroy_context(WebGL_contextHandle);
    WebGL_contextHandle = 0;
#endif
}

void *LowLevelWindow_GLFW::GetProcAddress(CString s)
{
#ifndef __EMSCRIPTEN__
	return (void *) glfwGetProcAddress(s.c_str());
#else
    return emscripten_webgl2_get_proc_address("#canvas");
#endif
}

// we never actually delete and make a new context so yea
CString LowLevelWindow_GLFW::TryVideoMode( RageDisplay::VideoModeParams p, bool &bNewDeviceOut )
{
    LOG->Info("TEST CAN THIS WINDOW BE SEEN?");
	CurrentParams = p;
#ifndef __EMSCRIPTEN__
    glfwSetWindowTitle(glfwDisplay, p.sWindowTitle);

    // use (p.sIconFile)
    // magic here too 

	ASSERT( p.bpp == 16 || p.bpp == 32 );

#else
    // do some magic with javascript to change the page icon and title
#endif

	switch( p.bpp )
	{
	case 16:
        glfwSetWindowAttrib(glfwDisplay, GLFW_RED_BITS, 5);
        glfwSetWindowAttrib(glfwDisplay, GLFW_GREEN_BITS, 6);
        glfwSetWindowAttrib(glfwDisplay, GLFW_BLUE_BITS, 5);
		break;
	case 32:
        glfwSetWindowAttrib(glfwDisplay, GLFW_RED_BITS, 8);
        glfwSetWindowAttrib(glfwDisplay, GLFW_GREEN_BITS, 8);
        glfwSetWindowAttrib(glfwDisplay, GLFW_BLUE_BITS, 8);
    default: break;
	}

    glfwSetWindowAttrib(glfwDisplay, GLFW_DEPTH_BITS, 16);
    glfwSetWindowAttrib(glfwDisplay, GLFW_DOUBLEBUFFER, GLFW_TRUE);
    glfwSetWindowAttrib(glfwDisplay, GLFW_CURSOR_HIDDEN, !p.windowed);

    glfwSwapInterval(!p.vsync);

	static bool bLogged = false;
	if( !bLogged )
	{
		bLogged = true;
		LOG->Info( "GLFW version: %i.%i.%i", GLFW_VERSION_MAJOR, GLFW_VERSION_MINOR, GLFW_VERSION_REVISION );
	}

	{
		int r = glfwGetWindowAttrib(glfwDisplay, GLFW_RED_BITS);
        int g = glfwGetWindowAttrib(glfwDisplay, GLFW_GREEN_BITS);
        int b = glfwGetWindowAttrib(glfwDisplay, GLFW_BLUE_BITS);
        int a = glfwGetWindowAttrib(glfwDisplay, GLFW_ALPHA_BITS);
        int colorbits = r + g + b;
        int depth = glfwGetWindowAttrib(glfwDisplay, GLFW_DEPTH_BITS);
        int stencil = glfwGetWindowAttrib(glfwDisplay, GLFW_STENCIL_BITS);
		LOG->Info("Got %i bpp (%i%i%i%i), %i depth, %i stencil",
			colorbits, r, g, b, a, depth, stencil);
	}
	return "";	// we set the video mode successfully
}

void LowLevelWindow_GLFW::SwapBuffers()
{
#ifndef __EMSCRIPTEN__
	glfwSwapBuffers(glfwDisplay);
#else
    emscripten_webgl_commit_frame();
#endif
}
void LowLevelWindow_GLFW::Update(float fDeltaTime)
{
#ifndef __EMSCRIPTEN__ // browser handles events
    glfwPollEvents();
#endif
}



void LowLevelWindow_GLFW::SetSize(int width, int height) {
    CurrentParams.width = width;
    CurrentParams.height = height;
    DISPLAY->ResolutionChanged();
}

void LowLevelWindow_GLFW::SetFocused(int focused) {
    // if(!DISPLAY->GetVideoModeParams().windowed) {
    //     DISPLAY->SetVideoMode( DISPLAY->GetVideoModeParams() );
    // }
    FocusChanged(focused);
}

void LowLevelWindow_GLFW::SetClosed() {
    LOG->Trace("SDL_QUIT: shutting down");
    ExitGame();
}
