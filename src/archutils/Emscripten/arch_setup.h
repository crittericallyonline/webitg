#ifndef ARCH_SETUP_EMSCRIPTEN_H
#define ARCH_SETUP_EMSCRIPTEN_H

#if !defined(MISSING_STDINT_H) /* need to define int64_t if so */
#include <stdint.h>
#endif


inline uint32_t ArchSwap32( uint32_t n )
{
	return  (( n & 0x000000FF ) << 24) |
			(( n & 0x0000FF00 ) << 8) |
			(( n & 0x00FF0000 ) >> 8) |
			(( n & 0xFF000000 ) >> 24);
}

inline uint32_t ArchSwap24( uint32_t n )
{
	return ArchSwap32(n) >> 8;
}

inline uint16_t ArchSwap16( uint16_t n )
{
	return ((n & 0xFF00) >> 8) | ((n & 0x00FF) << 8);
}

#define HAVE_BYTE_SWAPS
#define ENDIAN_LITTLE

#endif