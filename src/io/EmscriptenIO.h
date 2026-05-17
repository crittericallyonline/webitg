#ifndef IO_EMSCRIPTEN_H
#define IO_EMSCRIPTEN_H

#include "USBDriver.h"

class EmscriptenIO : public USBDriver
{
public:
/*
	virtual bool Open();
	virtual void Close();
*/
	bool Open();
    void Close();
	/* Globally accessible for diagnostics purposes. */
	static int m_iInputErrorCount;
	static CString m_sInputError;
}

#endif