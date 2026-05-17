#include "global.h"
#include "../RageLog.h"
#include "../RageUtil.h"

#include "EmscriptenIO.h"
#include "../arch/USB/USBDriver_Impl.h"

CString EmscriptenIO::m_sInputError;
int EmscriptenIO::m_iInputErrorCount = 0;

bool EmscriptenIO::Open() {
    return true;
}
void EmscriptenIO::Close() {}