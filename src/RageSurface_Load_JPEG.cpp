#include "global.h"
#include "RageSurface_Load_JPEG.h"
#include "RageUtil.h"
#include "RageFile.h"
#include "RageSurface.h"

#include <setjmp.h>


// extern "C" {
// }

static RageSurface *RageSurface_Load_JPEG( RageFile *f, const char *fn )
{
	
	RageSurface *img = NULL; /* volatile to prevent possible problems with setjmp */

	int w, h, c;
	img->pixels = stbi_load(fn, &w, &h, &c, STBI_rgb);

	if( c == STBI_grey )
	{
		img = CreateSurface( w, h, 8, 0, 0, 0, 0 );

		for( int i = 0; i < 256; ++i )
		{
			RageSurfaceColor color;
			color.r = color.g = color.b = (int8_t) i;
			color.a = 0xFF;
			img->fmt.palette->colors[i] = color;
		}
	} else {
		img = CreateSurface( w, h, 24,
				Swap24BE( 0xFF0000 ),
				Swap24BE( 0x00FF00 ),
				Swap24BE( 0x0000FF ),
				Swap24BE( 0x000000 ) );
	}
	img->w = w;
	img->h = h;

	return img;
}

RageSurfaceUtils::OpenResult RageSurface_Load_JPEG( const CString &sPath, RageSurface *&ret, bool bHeaderOnly, CString &error )
{
	RageFile f;
	if( !f.Open( sPath ) )
	{
		error = f.GetError();
		return RageSurfaceUtils::OPEN_FATAL_ERROR;
	}

	ret = RageSurface_Load_JPEG( &f, sPath );
	if( ret == NULL )
	{
		error = "\n\nEither its the wrong format or RageFile was not open/didint exist. - Niko\n\n";
		return RageSurfaceUtils::OPEN_UNKNOWN_FILE_FORMAT; // XXX
	}

	return RageSurfaceUtils::OPEN_OK;
}


/*
 * (c) 2004 Glenn Maynard
 * All rights reserved.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, and/or sell copies of the Software, and to permit persons to
 * whom the Software is furnished to do so, provided that the above
 * copyright notice(s) and this permission notice appear in all copies of
 * the Software and that both the above copyright notice(s) and this
 * permission notice appear in supporting documentation.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF
 * THIRD PARTY RIGHTS. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR HOLDERS
 * INCLUDED IN THIS NOTICE BE LIABLE FOR ANY CLAIM, OR ANY SPECIAL INDIRECT
 * OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */
