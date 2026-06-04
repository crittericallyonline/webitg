#ifndef ARCH_HOOKS_UNIX_H
#define ARCH_HOOKS_UNIX_H

#include "ArchHooks.h"
class ArchHooks_Emscripten: public ArchHooks
{
public:
	ArchHooks_Emscripten();

	// void MountInitialFilesystems( const CString &sDirOfExecutable );

	void DumpDebugInfo();
	// void SystemReboot( bool bForceSync = true );

	void SetTime( tm newtime );

	// void BoostThreadPriority();
	// void UnBoostThreadPriority();

	uint64_t GetDiskSpaceTotal( const CString &sDir );
	uint64_t GetDiskSpaceFree( const CString &sDir );

	// bool OpenMemoryRange( unsigned short start_port, unsigned short bytes );
	// void CloseMemoryRange( unsigned short start_port, unsigned short bytes );

	// bool GetNetworkAddress( CString &sIP, CString &sNetmask, CString &sError );

	static int64_t m_iStartTime;
};

#ifdef ARCH_HOOKS
#error "More than one ArchHooks selected!"
#endif
#define ARCH_HOOKS ArchHooks_Emscripten

#endif