#ifndef LOW_LEVEL_WINDOW_GLFW_H
#define LOW_LEVEL_WINDOW_GLFW_H

#include "LowLevelWindow.h"

typedef struct GLFWwindow GLFWwindow;

class LowLevelWindow_GLFW: public LowLevelWindow
{
public:
	LowLevelWindow_GLFW();
	~LowLevelWindow_GLFW();
	void *GetProcAddress(CString s);
	CString TryVideoMode( RageDisplay::VideoModeParams p, bool &bNewDeviceOut );
	void SwapBuffers();
	void Update(float fDeltaTime);

	void SetSize(int width, int height);
	void SetFocused(int focused);
	void SetClosed();

	RageDisplay::VideoModeParams GetVideoModeParams() const { return CurrentParams; }

private:
	RageDisplay::VideoModeParams CurrentParams;
	GLFWwindow *glfwDisplay;
	unsigned long WebGL_contextHandle;
};

#ifdef ARCH_LOW_LEVEL_WINDOW
#error "More than one LowLevelWindow selected!"
#endif
#define ARCH_LOW_LEVEL_WINDOW LowLevelWindow_GLFW
#endif // LOW_LEVEL_WINDOW_GLFW_H
