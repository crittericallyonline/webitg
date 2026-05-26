#include "global.h"
#include "RageSurface_Load_PNG.h"
#include "RageUtil.h"
#include "RageLog.h"
#include "RageFile.h"
#include "RageSurface.h"

/*
 * I prefer static over anonymous namespaces for functions; it's clearer and seems
 * to have no language drawbacks over anonymous namespaces, which are only really
 * needed when declaring things more complicated than functions.
 *
 * There's an implementation advantage, though: functions declared in anonymous
 * namespaces are still exported by gcc -rdynamic, which means the crash handler can
 * still resolve them.  static functions aren't, so we end up getting a wrong match.
 */

namespace
{
static RageSurface *RageSurface_Load_PNG( RageFile *f, const char *fn, bool bHeaderOnly )
{
	RageSurface *img = NULL;


	int width, height;
	int bit_depth, color_type;
	img->pixels = stbi_load(fn, &width, &height, &bit_depth, STBI_rgb_alpha);

	if( bHeaderOnly )
	{
		img = CreateSurfaceFrom( width, height, 32, 0, 0, 0, 0, NULL, width*4 );
		free(img->pixels);
		return img;
	}

	/* These are set for type == PALETTE. */
	RageSurfaceColor colors[256];
	int iColorKey = -1;

	/* We import three types of files: paletted, RGBX and RGBA.  The only difference
	 * between RGBX and RGBA is that RGBX won't set the alpha mask, so it's easier
	 * to tell later on that there's no alpha (without actually having to do a pixel scan). */
	enum { PALETTE, RGBX, RGBA } type;
	switch( color_type )
	{
	case STBI_grey:
		for( int i = 0; i < 256; ++i )
		{
			colors[i].r = colors[i].g = colors[i].b = (int8_t) i;
			colors[i].a = 0xFF;
		}

		type = PALETTE;
		break;

	case STBI_grey_alpha: 
		type = RGBA;
		// png_set_gray_to_rgb( png );
		break;
	// case PNG_COLOR_TYPE_PALETTE:
	// 	type = PALETTE;
	// 	break;
	case STBI_rgb:
		type = RGBX;
		break;
	case STBI_rgb_alpha:
		type = RGBA;
		break;
	default:
		FAIL_M(ssprintf( "%i", color_type) );
	}

	switch( type )
	{
	case PALETTE:
		img = CreateSurface( width, height, 8, 0, 0, 0, 0 );
		memcpy( img->fmt.palette->colors, colors, 256*sizeof(RageSurfaceColor) );

		if( iColorKey != -1 )
			img->format->palette->colors[ iColorKey ].a = 0;

		break;
	case RGBX:
	case RGBA:
		img = CreateSurface( width, height, 32,
				Swap32BE( 0xFF000000 ),
				Swap32BE( 0x00FF0000 ),
				Swap32BE( 0x0000FF00 ),
				Swap32BE( type == RGBA? 0x000000FF:0x00000000 ) );
		break;
	default:
		FAIL_M(ssprintf( "%i", type) );
	}

	return img;
}

};

RageSurfaceUtils::OpenResult RageSurface_Load_PNG( const CString &sPath, RageSurface *&ret, bool bHeaderOnly, CString &error )
{
	RageFile f;
	if( !f.Open( sPath ) )
	{
		error = f.GetError();
		return RageSurfaceUtils::OPEN_FATAL_ERROR;
	}

	ret = RageSurface_Load_PNG( &f, sPath, bHeaderOnly );
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
