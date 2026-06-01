#ifndef INPUT_HANDLER_EMSCRIPTEN_H
#define INPUT_HANDLER_EMSCRIPTEN_H

#include "InputHandler.h"
#include "RageThreads.h"

class InputHandler_Emscripten: public InputHandler {
public:
	InputHandler_Emscripten();
	~InputHandler_Emscripten();
	void Update(float fDeltaTime);
	void GetDevicesAndDescriptions( vector<InputDevice>& vDevicesOut, vector<CString>& vDescriptionsOut );

private:
	RageTimer m_LastUpdate;
	int m_iInputsSinceUpdate;
	RageThread InputThread;
	bool m_bFoundDevice;
	bool m_bShutdown;
	uint32_t m_iInputData, m_iLastInputData;
	uint32_t m_iWriteData;
};

#endif // INPUT_HANDLER_EMSCRIPTEN
