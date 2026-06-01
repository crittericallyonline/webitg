#ifndef LOW_LEVEL_WINDOW_NULL_H
#define LOW_LEVEL_WINDOW_NULL_H

#include "LowLevelWindow.h"

class LowLevelWindow_Null: public LowLevelWindow
{
public:
	LowLevelWindow_Null();
	~LowLevelWindow_Null();
	void *GetProcAddress(CString s);
	CString TryVideoMode( RageDisplay::VideoModeParams p, bool &bNewDeviceOut );
	void SwapBuffers();
	void Update(float fDeltaTime);

	RageDisplay::VideoModeParams GetVideoModeParams() const { return CurrentParams; }

private:
	RageDisplay::VideoModeParams CurrentParams;
};

#ifdef ARCH_LOW_LEVEL_WINDOW
#error "More than one LowLevelWindow selected!"
#endif
#define ARCH_LOW_LEVEL_WINDOW LowLevelWindow_Null

#endif