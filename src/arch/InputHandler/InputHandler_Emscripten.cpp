#include "global.h"
#include "RageLog.h"
#include "RageDisplay.h"

#include "arch/ArchHooks/ArchHooks.h"


// required I/O routines
#include "InputHandler_Emscripten.h"

// debug stuff
#include "RageUtil.h"
#include "ScreenManager.h"
#include "DiagnosticsUtil.h"

#include <GLFW/emscripten_glfw3.h>

// REGISTER_INPUT_HANDLER( Emscripten );

InputHandler_Emscripten::InputHandler_Emscripten()
{
	LOG->Trace( "Opened EM_IO board." );
	DiagnosticsUtil::SetInputType("EM_IO");

	// report every 5000 updates
	m_DebugTimer.SetName( "EM_IO" );
	m_DebugTimer.AutoReport( false );
	m_DebugTimer.SetInterval( 5000 );
}

InputHandler_Emscripten::~InputHandler_Emscripten()
{
	if( !InputThread.IsCreated() )
		return;

	m_bShutdown = true;
	m_DebugTimer.Report();

	LOG->Trace( "Shutting down EM_IO thread..." );
	InputThread.Wait();
	LOG->Trace( "EM_IO thread shut down." );

}

void InputHandler_Emscripten::GetDevicesAndDescriptions( vector<InputDevice>& vDevicesOut, vector<CString>& vDescriptionsOut )
{
	if( m_bFoundDevice )
	{
		vDevicesOut.push_back( InputDevice(DEVICE_KEYBOARD) );
		vDescriptionsOut.push_back( "EM_IO" );
	}
}

void InputHandler_Emscripten::Update( float fDeltaTime ) {

}