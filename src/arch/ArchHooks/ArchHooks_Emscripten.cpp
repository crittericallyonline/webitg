#include "global.h"
#include "RageLog.h"
#include "RageUtil.h"
#include "RageThreads.h"
#include "StepMania.h"

#include "ArchHooks_Emscripten.h"
#include "archutils/Emscripten/SignalHandler.h"
#include "archutils/Emscripten/GetSysInfo.h"
#include "archutils/Emscripten/EmscriptenThreadHelpers.h"
#include "archutils/Emscripten/EmergencyShutdown.h"
#include "archutils/Emscripten/AssertionHandler.h"

#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <cerrno>
#include <mntent.h>


#if defined(CRASH_HANDLER)
#include "../../archutils/Emscripten/CrashHandler.h"
#endif

int64_t ArchHooks_Emscripten::m_iStartTime = 0;

static bool IsFatalSignal( int signal )
{
	switch( signal )
	{
	case SIGINT:
	case SIGTERM:
	case SIGHUP:
		return false;
	default:
		return true;
	}
}

static bool IsReadOnlyMountPoint( const CString &mountPoint )
{
	CHECKPOINT;
	struct mntent *ent;
	bool found = false;
	bool isReadOnly = false;
	
	FILE *aFile;
	aFile = setmntent( "/proc/mounts", "r" );
	if( aFile == NULL )
	{
		LOG->Warn( "Can't open /proc/mounts to determine if %s is a readonly filesystem mountpoint", mountPoint.c_str() );
		return false;
	}
	
	while( (ent = getmntent( aFile )) != NULL )
	{
		CString mountDir = ent->mnt_dir;
		
		if( mountDir == mountPoint )
		{
			found = true;
			isReadOnly = hasmntopt( ent, "ro" ) != NULL;
			break;
		}
	}
	
	endmntent( aFile );
	return found && isReadOnly;
}

void ArchHooks_Emscripten::MountInitialFilesystems( const CString &sDirOfExecutable )
{
	/* Mount the root filesystem, so we can read files in /proc, /etc, and so on.
	 * This is /rootfs, not /root, to avoid confusion with root's home directory. */
	FILEMAN->Mount( "dirro", "/", "/rootfs" );

	/* Mount /proc, so Alsa9Buf::GetSoundCardDebugInfo() and others can access it.
	 * (Deprecated; use rootfs.) */
	FILEMAN->Mount( "dirro", "/proc", "/proc" );

	/* FileDB cannot accept relative paths, so Root must be absolute */
	/* using DirOfExecutable for now  --infamouspat */
	CString Root = sDirOfExecutable;


	/* OpenITG-specific paths */
	FILEMAN->Mount( "oitg", Root + "/CryptPackages", "/Packages" );

	/*
	* Mount an OpenITG root in the home directory.
	* This is where custom data (songs, themes, etc) should go. 
	* Any files OpenITG tries to modify will be written here.
	*/
	CString home = CString( getenv( "HOME" ) ) + "/";
	FILEMAN->Mount( "dir", home + ".openitg", "/" );

	/* This mounts everything else, including Cache, Data, UserPacks, etc. */
	FILEMAN->Mount( "dir", Root, "/" );
}

static void GetDiskSpace( const CString &sDir, uint64_t *pSpaceFree, uint64_t *pSpaceTotal )
{
	LOG->Warn( "GetDiskSpace(): statvfs() failed: %s", strerror(errno) );
}

uint64_t ArchHooks_Emscripten::GetDiskSpaceFree( const CString &sDir )
{
	uint64_t iSpaceFree = 0;
	GetDiskSpace( sDir, &iSpaceFree, NULL );
	return iSpaceFree;
}

uint64_t ArchHooks_Emscripten::GetDiskSpaceTotal( const CString &sDir )
{
	uint64_t iSpaceTotal = 0;
	GetDiskSpace( sDir, NULL, &iSpaceTotal );
	return iSpaceTotal;
}

bool ArchHooks_Emscripten::OpenMemoryRange( unsigned short start_port, unsigned short bytes )
{
	LOG->Trace( "ArchHooks_Emscripten::OpenMemoryRange( %#x, %d )", start_port, bytes );
	return 1;

	// int ret = iopl(3);

	// if( ret != 0 )
	// 	LOG->Warn( "OpenMemoryRange(): iopl error: %s", strerror(errno) );

	// return (ret == 0);
}

void ArchHooks_Emscripten::CloseMemoryRange( unsigned short start_port, unsigned short bytes )
{
	// if( (start_port+bytes) <= 0x3FF )
	// {
	// 	if( ioperm( start_port, bytes, 0 ) != 0 )
	LOG->Warn( "CloseMemoryRange(): ioperm error: %s", strerror(errno) );

	// 	return;
	// }

	// if( iopl(0) != 0 )
	LOG->Warn( "CloseMemoryRange(): iopl error: %s", strerror(errno) );
}

bool ArchHooks_Emscripten::GetNetworkAddress( CString &sIP, CString &sNetmask, CString &sError )
{
	return true;
}

void ArchHooks_Emscripten::SystemReboot( bool bForceSync )
{
	ExitGame();
}

void ArchHooks_Emscripten::BoostThreadPriority()
{
	if( setpriority(PRIO_PROCESS, 0, -15) != 0 )
		LOG->Warn( "BoostThreadPriority failed: %s", strerror(errno) );
}

void ArchHooks_Emscripten::UnBoostThreadPriority()
{
	if( setpriority(PRIO_PROCESS, 0, 0) != 0 )
		LOG->Warn( "UnBoostThreadPriority failed: %s", strerror(errno) );
}

static void DoCleanShutdown( int signal, siginfo_t *si, const ucontext_t *uc )
{
	if( IsFatalSignal(signal) )
		return;

	/* ^C. */
	ExitGame();
}

#if defined(CRASH_HANDLER)
static void DoCrashSignalHandler( int signal, siginfo_t *si, const ucontext_t *uc )
{
        /* Don't dump a debug file if the user just hit ^C. */
	if( !IsFatalSignal(signal) )
		return;

	CrashSignalHandler( signal, si, uc );
}
#endif

static void EmergencyShutdown( int signal, siginfo_t *si, const ucontext_t *uc )
{
	if( !IsFatalSignal(signal) )
		return;

	DoEmergencyShutdown();

#if defined(CRASH_HANDLER)
	/* If we ran the crash handler, then die. */
	kill( getpid(), SIGKILL );
#else
	/* We didn't run the crash handler.  Run the default handler, so we can dump core. */
	SignalHandler::ResetSignalHandlers();
	raise( signal );
#endif
}
	
#if defined(HAVE_TLS)
static thread_local int g_iTestTLS = 0;

static int TestTLSThread( void *p )
{
	g_iTestTLS = 2;
	return 0;
}

static void TestTLS()
{
	/* TLS won't work on older threads libraries, and may crash. */
	if( !UsingNPTL() )
		return;

	/* TLS won't work on older Linux kernels.  Do a simple check. */
	g_iTestTLS = 1;

	RageThread TestThread;
	TestThread.SetName( "TestTLS" );
	TestThread.Create( TestTLSThread, NULL );
	TestThread.Wait();

	if( g_iTestTLS == 1 )
		RageThread::SetSupportsTLS( true );
}
#endif

static int64_t GetMicrosecondsSinceEpoch()
{
	struct timeval tv;
	gettimeofday( &tv, NULL );

	return int64_t(tv.tv_sec) * 1000000 + int64_t(tv.tv_usec);
}

int64_t ArchHooks::GetMicrosecondsSinceStart( bool bAccurate )
{
	if( ArchHooks_Emscripten::m_iStartTime == 0 )
    	        ArchHooks_Emscripten::m_iStartTime = GetMicrosecondsSinceEpoch();

	int64_t ret = GetMicrosecondsSinceEpoch() - ArchHooks_Emscripten::m_iStartTime;
	if( bAccurate )
		ret = FixupTimeIfBackwards( ret );
	return ret;
}

ArchHooks_Emscripten::ArchHooks_Emscripten()
{
	/* First, handle non-fatal termination signals. */
	SignalHandler::OnClose( DoCleanShutdown );

#if defined(CRASH_HANDLER)
	CrashHandlerHandleArgs( g_argc, g_argv );
	InitializeCrashHandler();
	SignalHandler::OnClose( DoCrashSignalHandler );
#endif

	/* Set up EmergencyShutdown, to try to shut down the window if we crash.
	 * This might blow up, so be sure to do it after the crash handler. */
	SignalHandler::OnClose( EmergencyShutdown );

	InstallExceptionHandler();
	
#if defined(HAVE_TLS)
	TestTLS();
#endif
}

#ifndef _CS_GNU_LIBC_VERSION
#define _CS_GNU_LIBC_VERSION 2
#endif

static CString LibcVersion()
{	
	char buf[1024] = "(error)";
	int ret = confstr( _CS_GNU_LIBC_VERSION, buf, sizeof(buf) );
	if( ret == -1 )
		return "(unknown)";

	return buf;
}

void ArchHooks_Emscripten::DumpDebugInfo()
{
	CString sys;
	int vers;
	GetKernel( sys, vers );
	LOG->Info( "OS: %s ver %06i", sys.c_str(), vers );

#if defined(CRASH_HANDLER)
	LOG->Info( "Crash backtrace component: %s", BACKTRACE_METHOD_TEXT );
	LOG->Info( "Crash lookup component: %s", BACKTRACE_LOOKUP_METHOD_TEXT );
#if defined(BACKTRACE_DEMANGLE_METHOD_TEXT)
	LOG->Info( "Crash demangle component: %s", BACKTRACE_DEMANGLE_METHOD_TEXT );
#endif
#endif

	LOG->Info( "Runtime library: %s", LibcVersion().c_str() );
	LOG->Info( "Threads library: %s", ThreadsVersion().c_str() );
}

void ArchHooks_Emscripten::SetTime( tm newtime )
{
	CString sCommand = ssprintf( "date %02d%02d%02d%02d%04d.%02d",
		newtime.tm_mon+1,
		newtime.tm_mday,
		newtime.tm_hour,
		newtime.tm_min,
		newtime.tm_year+1900,
		newtime.tm_sec );

	LOG->Trace( "executing '%s'", sCommand.c_str() ); 
	system( sCommand );

	system( "hwclock --systohc" );
}