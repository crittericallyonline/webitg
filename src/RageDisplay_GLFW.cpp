#include "global.h"

#ifdef __EMSCRIPTEN__
#define GL_GLEXT_PROTOTYPES
#include <GLFW/emscripten_glfw3.h>
// #ifdef HAS_GLES3
#include <GLES3/gl3.h>
#include <GLES3/gl2ext.h>
// #endif
#else
#include <GLFW/glfw3.h>
#endif

#include "RageSurface.h"
#include "RageSurfaceUtils.h"
#include "ScreenDimensions.h" // XXX

#include <set>
#include <sstream>

#if defined(DARWIN)
#include "archutils/Darwin/Vsync.h"
#endif


#include "RageDisplay.h"
#include "RageDisplay_GLFW.h"
#include "RageLog.h"
#include "RageTextureManager.h"
#include "RageMath.h"

#include "arch/LowLevelWindow/LowLevelWindow.h"

//
// Globals
//

static bool g_bReversePackedPixelsWorks = true;
static bool g_bColorIndexTableWorks = true;

/* OpenGL system information that generally doesn't change at runtime. */

/* Range and granularity of points and lines: */
float g_line_range[2];
float g_point_range[2];

/* OpenGL version * 10: */
int g_glVersion;

/* GLFW version * 10: */
int g_glfwVersion;

static int g_iMaxTextureUnits = 0;

/* We don't actually use normals (we don't turn on lighting), there's just
 * no GL_T2F_C4F_V3F. */
const GLenum RageSpriteVertexFormat = GL_T2F_C4F_N3F_V3F;

/* If we support texture matrix scaling, a handle to the vertex program: */
static GLuint g_bTextureMatrixShader = 0;

LowLevelWindow *wind;

static void InvalidateAllGeometry();

static RageDisplay::PixelFormatDesc PIXEL_FORMAT_DESC[RageDisplay::NUM_PIX_FORMATS] = {
	{
		/* R8G8B8A8 */
		32,
		{ 0xFF000000,
		  0x00FF0000,
		  0x0000FF00,
		  0x000000FF }
	}, {
		/* R4G4B4A4 */
		16,
		{ 0xF000,
		  0x0F00,
		  0x00F0,
		  0x000F },
	}, {
		/* R5G5B5A1 */
		16,
		{ 0xF800,
		  0x07C0,
		  0x003E,
		  0x0001 },
	}, {
		/* R5G5B5 */
		16,
		{ 0xF800,
		  0x07C0,
		  0x003E,
		  0x0000 },
	}, {
		/* R8G8B8 */
		24,
		{ 0xFF0000,
		  0x00FF00,
		  0x0000FF,
		  0x000000 }
	}, {
		/* B8G8R8A8 */
		24,
		{ 0x0000FF,
		  0x00FF00,
		  0xFF0000,
		  0x000000 }
	}, {
		/* A1B5G5R5 */
		16,
		{ 0x7C00,
		  0x03E0,
		  0x001F,
		  0x8000 },
	}
};

static map<GLenum, CString> g_Strings;
static void InitStringMap()
{
	static bool Initialized = false;
	if(Initialized) return;
	Initialized = true;
	#define X(a) g_Strings[a] = #a;
	X(GL_RGBA8);	X(GL_RGBA4);	X(GL_RGB5_A1);	X(GL_RGB5);	X(GL_RGBA);	X(GL_RGB);
	X(GL_BGR);	X(GL_BGRA);
	X(GL_COLOR_INDEX);
	X(GL_UNSIGNED_BYTE);	X(GL_UNSIGNED_SHORT_4_4_4_4); X(GL_UNSIGNED_SHORT_5_5_5_1);
	X(GL_UNSIGNED_SHORT_1_5_5_5_REV);
	X(GL_INVALID_ENUM); X(GL_INVALID_VALUE); X(GL_INVALID_OPERATION);
	X(GL_STACK_OVERFLOW); X(GL_STACK_UNDERFLOW); X(GL_OUT_OF_MEMORY);
}

static CString GLToString( GLenum e )
{
	if( g_Strings.find(e) != g_Strings.end() )
		return g_Strings[e];

	return ssprintf( "%i", int(e) );
}

/* GL_PIXFMT_INFO is used for both texture formats and surface formats.  For example,
 * it's fine to ask for a FMT_RGB5 texture, but to supply a surface matching
 * FMT_RGB8.  OpenGL will simply discard the extra bits.
 *
 * It's possible for a format to be supported as a texture format but not as a
 * surface format.  For example, if packed pixels aren't supported, we can still
 * use GL_RGB5_A1, but we'll have to convert to a supported surface pixel format
 * first.  It's not ideal, since we'll convert to RGBA8 and OGL will convert back,
 * but it works fine.
 */
struct GLPixFmtInfo_t {
	GLenum internalfmt; /* target format */
	GLenum format; /* target format */
	GLenum type; /* data format */
} GL_PIXFMT_INFO[RageDisplay::NUM_PIX_FORMATS] = {
	{
		/* R8G8B8A8 */
		GL_RGBA8,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
	}, {
		/* B4G4R4A4 */
		GL_RGBA4,
		GL_RGBA,
		GL_UNSIGNED_SHORT_4_4_4_4,
	}, {
		/* B5G5R5A1 */
		GL_RGB5_A1,
		GL_RGBA,
		GL_UNSIGNED_SHORT_5_5_5_1,
	}, {
		/* B5G5R5 */
		GL_RGB5,
		GL_RGBA,
		GL_UNSIGNED_SHORT_5_5_5_1,
	}, {
		/* B8G8R8 */
		GL_RGB8,
		GL_RGB,
		GL_UNSIGNED_BYTE,
	},
	{
		/* B8G8R8 */
		GL_RGB8,
		GL_BGR,
		GL_UNSIGNED_BYTE,
	}, {
		/* A1R5G5B5 (matches D3DFMT_A1R5G5B5) */
		GL_RGB5_A1,
		GL_BGRA,
		GL_UNSIGNED_SHORT_1_5_5_5_REV,
	}
};


static void FixLittleEndian()
{
#if defined(ENDIAN_LITTLE)
	static bool Initialized = false;
	if( Initialized )
		return;
	Initialized = true;

	for( int i = 0; i < RageDisplay::NUM_PIX_FORMATS; ++i )
	{
		RageDisplay::PixelFormatDesc &pf = PIXEL_FORMAT_DESC[i];

		/* OpenGL and RageSurface handle byte formats differently; we need
		 * to flip non-paletted masks to make them line up. */
		if( GL_PIXFMT_INFO[i].type != GL_UNSIGNED_BYTE || pf.bpp == 8 )
			continue;

		for( int mask = 0; mask < 4; ++mask)
		{
			int m = pf.masks[mask];
			switch( pf.bpp )
			{
			case 24: m = Swap24(m); break;
			case 32: m = Swap32(m); break;
			default: ASSERT(0);
			}
			pf.masks[mask] = m;
		}
	}
#endif
}


static void FlushGLErrors()
{
	/* Making an OpenGL call doesn't also flush the error state; if we happen
	 * to have an error from a previous call, then the assert below will fail. 
	 * Flush it. */
	while( glGetError() != GL_NO_ERROR )
		;
}

#define AssertNoGLError() \
{ \
	GLenum error = glGetError(); \
	ASSERT_M( error == GL_NO_ERROR, GLToString(error) ) \
}

static void TurnOffHardwareVBO()
{
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
}

RageDisplay_GLFW::RageDisplay_GLFW()
{
	LOG->Trace( "RageDisplay_GLFW::RageDisplay_GLFW()" );
	LOG->MapLog("renderer", "Current renderer: OpenGL");

	FixLittleEndian();
	InitStringMap();

	wind = LowLevelWindow::Create();
	g_bTextureMatrixShader = 0;
}

CString GetInfoLog( GLuint h )
{
	GLint iLength;
	GLchar *pInfoLog = new GLchar[512];
	glGetShaderInfoLog(h, 512, &iLength, pInfoLog);
	CString sRet = pInfoLog;
	delete [] pInfoLog;
	return sRet;
}

GLuint CompileShader( GLenum ShaderType, CString sBuffer )
{
	LOG->Info( "Attempting Shader Compilation: %s", sBuffer.c_str() );
	GLuint VertexShader = glCreateShader( GL_VERTEX_SHADER );
	const GLchar *pData = sBuffer.data();
	int iLength = sBuffer.size();
	glShaderSource(VertexShader, 1, &pData, (GLint*)&iLength);
	glCompileShader(VertexShader);

	GLint bCompileStatus  = GL_FALSE;
	glGetShaderiv( VertexShader, GL_COMPILE_STATUS, &bCompileStatus );

	if( !bCompileStatus )
	{
		LOG->Warn( "Compile failure (if you're using the ITG noteskins, prepare for doom): %s", GetInfoLog( VertexShader ).c_str() );
		return 0;
	}

	return VertexShader;
}

enum
{
	ATTRIB_TEXTURE_MATRIX_SCALE = 1
};

/* XXX: How should we include these?  Doing them like this is ugly.  Linking them in
 * from another file as a text symbol would be ideal, but that's completely different
 * on each platform, so it'd be a maintenance nightmare.  Reading them from a file would
 * be annoying, too. */

/*
const GLcharARB *g_TextureMatrixScaleShader = 
" \
attribute vec4 TextureMatrixScale; \
void main( void ) \
{ \
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex; \
	vec4 multiplied_tex_coord = gl_TextureMatrix[0] * gl_MultiTexCoord0; \
	gl_TexCoord[0] = (multiplied_tex_coord * TextureMatrixScale) + \
					(gl_MultiTexCoord0 * (vec4(1)-TextureMatrixScale)); \
	gl_FrontColor = gl_Color; \
} \
";
*/

const GLchar *g_TextureMatrixScaleShader = 
" \
attribute vec4 TextureMatrixScale; \
void main() { \
	gl_Position = (gl_ModelViewProjectionMatrix * gl_Vertex); \
	gl_TexCoord[0] = (gl_TextureMatrix[0] * gl_MultiTexCoord0 * TextureMatrixScale) + (gl_MultiTexCoord0 * (vec4(1)-TextureMatrixScale)); \
	gl_FrontColor = gl_Color; \
} \
";

void InitScalingScript()
{
	g_bTextureMatrixShader = 0;

	GLuint VertexShader = CompileShader( GL_VERTEX_SHADER, g_TextureMatrixScaleShader );
	if( VertexShader == 0 )
		return;

	g_bTextureMatrixShader = glCreateProgram();
	glAttachShader( g_bTextureMatrixShader, VertexShader );
	glDeleteShader( VertexShader );

	glBindAttribLocation( g_bTextureMatrixShader, ATTRIB_TEXTURE_MATRIX_SCALE, "TextureMatrixScale" );

	// Link the program.
	glLinkProgram( g_bTextureMatrixShader );
	GLint bLinkStatus = false;
	glGetShaderiv(g_bTextureMatrixShader, GL_LINK_STATUS, &bLinkStatus);

	if( !bLinkStatus )
	{
		LOG->Trace( "Scaling shader link failed: %s", GetInfoLog(g_bTextureMatrixShader).c_str() );
		glDeleteShader( g_bTextureMatrixShader );
		return;
	}

	glVertexAttrib2f(ATTRIB_TEXTURE_MATRIX_SCALE, 1, 1);
}

CString RageDisplay_GLFW::Init( VideoModeParams p, bool bAllowUnacceleratedRenderer )
{

	bool bIgnore = false;
	CString sError = SetVideoMode( p, bIgnore );
	if( sError != "" )
		return sError;

	// Log driver details
	LOG->Info( "GLFW Vendor: %s", glGetString(GL_VENDOR) );
	LOG->Info( "GLFW Renderer: %s", glGetString(GL_RENDERER) );
	LOG->Info( "GLFW Version: %s", glGetString(GL_VERSION) );
	LOG->Info( "GLFW Max texture size: %i", GetMaxTextureSize() );
	LOG->Info( "GLFW Texture units: %i", g_iMaxTextureUnits );
	LOG->Info( "GLFW Extensions: %s", glGetString(GL_EXTENSIONS) );
	LOG->Info( "GLFW Version: %s", glfwGetVersionString() );

	if( IsSoftwareRenderer() )
	{
		if( !bAllowUnacceleratedRenderer )
			return
				"Your system is reporting that OpenGL hardware acceleration is not available.  "
				"Please obtain an updated driver from your video card manufacturer.\n\n";
		LOG->Warn("This is a software renderer!");
	}

	/* Log this, so if people complain that the radar looks bad on their
	 * system we can compare them: */
	glGetFloatv(GL_ALIASED_LINE_WIDTH_RANGE, g_line_range);
	glGetFloatv(GL_ALIASED_POINT_SIZE_RANGE, g_point_range);
	InitScalingScript();

	return "";
}

void RageDisplay_GLFW::Update(float fDeltaTime)
{
	wind->Update(fDeltaTime);
}

bool RageDisplay_GLFW::IsSoftwareRenderer()
{
	return false;
}

RageDisplay_GLFW::~RageDisplay_GLFW()
{
	delete wind;
}

static void CheckReversePackedPixels()
{
	/* Try to create a texture. */
	FlushGLErrors();
	glTexImage2D(GL_PROXY_TEXTURE_2D,
				0, GL_RGBA, 
				16, 16, 0,
				GL_BGRA, GL_UNSIGNED_SHORT_1_5_5_5_REV, NULL);

	const GLenum glError = glGetError();
	if( glError == GL_NO_ERROR )
		g_bReversePackedPixelsWorks = true;
	else
	{
		g_bReversePackedPixelsWorks = false;
		LOG->Info("GL_UNSIGNED_SHORT_1_5_5_5_REV failed (%s), disabled",
			GLToString(glError).c_str() );
	}
}

void RageDisplay_GLFW::ResolutionChanged()
{
	// re-init the vertex shader
	InitScalingScript();

 	SetViewport(0,0);

	/* Clear any junk that's in the framebuffer. */
	if( BeginFrame() )
		EndFrame();
}

// Return true if mode change was successful.
// bNewDeviceOut is set true if a new device was created and textures
// need to be reloaded.
CString RageDisplay_GLFW::TryVideoMode( VideoModeParams p, bool &bNewDeviceOut )
{
	LOG->Trace( "RageDisplay_GLFW::TryVideoMode( { .windowed=%d, .width=%d, .height=%d, .bpp=%d, .rate=%d, .vsync=%d )", p.windowed, p.width, p.height, p.bpp, p.rate, p.vsync );
	CString err;
	err = wind->TryVideoMode( p, bNewDeviceOut );
	if( err != "" )
		return err;	// failed to set video mode

	if( bNewDeviceOut )
	{
		/* We have a new OpenGL context, so we have to tell our textures that
		 * their OpenGL texture number is invalid. */
		if(TEXTUREMAN)
			TEXTUREMAN->InvalidateTextures();

		/* Recreate all vertex buffers. */
		InvalidateAllGeometry();
	}

	this->SetDefaultRenderStates();

	/* Now that we've initialized, we can search for extensions (some of which
	 * we may need to set up the video mode). */
	// SetupExtensions();

	/* Set vsync the Windows way, if we can.  (What other extensions are there
	 * to do this, for other archs?) */
	// theres glfw now :3 - Niko
	glfwSwapInterval(!p.vsync);
	
	ResolutionChanged();

	return "";	// successfully set mode
}

void RageDisplay_GLFW::SetViewport(int shift_left, int shift_down)
{
	/* left and down are on a 0..SCREEN_WIDTH, 0..SCREEN_HEIGHT scale.
	 * Scale them to the actual viewport range. */
	shift_left = int( shift_left * float(wind->GetVideoModeParams().width) / SCREEN_WIDTH );
	shift_down = int( shift_down * float(wind->GetVideoModeParams().height) / SCREEN_HEIGHT );

	glViewport(shift_left, -shift_down, wind->GetVideoModeParams().width, wind->GetVideoModeParams().height);
}

int RageDisplay_GLFW::GetMaxTextureSize() const
{
	GLint size;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &size);
	return size;
}

bool RageDisplay_GLFW::BeginFrame()
{
	glClearColor( 0,0,0,1 );
	SetZWrite( true );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	return true;
}

void RageDisplay_GLFW::EndFrame()
{
	// glFlush(), not glFinish(); NVIDIA_GLX's glFinish()'s behavior is
	// nowhere near performance-friendly and uses unholy amounts of CPU for
	// Gog-knows-what.
	glFlush();

	wind->SwapBuffers();
	ProcessStatsOnFlip();
}

RageSurface* RageDisplay_GLFW::CreateScreenshot()
{
	int width = wind->GetVideoModeParams().width;
	int height = wind->GetVideoModeParams().height;

	const PixelFormatDesc &desc = PIXEL_FORMAT_DESC[FMT_RGBA8];
	RageSurface *image = CreateSurface( width, height, desc.bpp,
		desc.masks[0], desc.masks[1], desc.masks[2], 0 );

	FlushGLErrors();

	glReadBuffer( GL_FRONT );
	AssertNoGLError();
	
	glReadPixels(0, 0, wind->GetVideoModeParams().width, wind->GetVideoModeParams().height, GL_RGBA,
	             GL_UNSIGNED_BYTE, image->pixels);
	AssertNoGLError();

	RageSurfaceUtils::FlipVertically( image );

	return image;
}

RageDisplay::VideoModeParams RageDisplay_GLFW::GetVideoModeParams() const { return wind->GetVideoModeParams(); }

static void SetupVertices( const RageSpriteVertex v[], int iNumVerts )
{
	static float *Vertex, *Texture, *Normal;	
	static GLubyte *Color;
	static int Size = 0;
	if(iNumVerts > Size)
	{
		Size = iNumVerts;
		delete [] Vertex;
		delete [] Color;
		delete [] Texture;
		delete [] Normal;
		Vertex = new float[Size*3];
		Color = new GLubyte[Size*4];
		Texture = new float[Size*2];
		Normal = new float[Size*3];
	}

	for(unsigned i = 0; i < unsigned(iNumVerts); ++i)
	{
		Vertex[i*3+0]  = v[i].p[0];
		Vertex[i*3+1]  = v[i].p[1];
		Vertex[i*3+2]  = v[i].p[2];
		Color[i*4+0]   = v[i].c.r;
		Color[i*4+1]   = v[i].c.g;
		Color[i*4+2]   = v[i].c.b;
		Color[i*4+3]   = v[i].c.a;
		Texture[i*2+0] = v[i].t[0];
		Texture[i*2+1] = v[i].t[1];
		Normal[i*3+0] = v[i].n[0];
		Normal[i*3+1] = v[i].n[1];
		Normal[i*3+2] = v[i].n[2];
	}

	// glEnableClientState(GL_VERTEX_ARRAY);
	// glVertexPointer(3, GL_FLOAT, 0, Vertex);

	// glEnableClientState(GL_COLOR_ARRAY);
	// glColorPointer(4, GL_UNSIGNED_BYTE, 0, Color);

	// glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	// glTexCoordPointer(2, GL_FLOAT, 0, Texture);

	// glEnableClientState(GL_NORMAL_ARRAY);
	// glNormalPointer(GL_FLOAT, 0, Normal);
}

void RageDisplay_GLFW::SendCurrentMatrices()
{
	RageMatrix projection;
	RageMatrixMultiply( &projection, GetCentering(), GetProjectionTop() );
	// glMatrixMode( GL_PROJECTION );
	// glLoadMatrixf( (const float*)&projection );

	// OpenGL has just "modelView", whereas D3D has "world" and "view"
	RageMatrix modelView;
	RageMatrixMultiply( &modelView, GetViewTop(), GetWorldTop() );
	// glMatrixMode( GL_MODELVIEW );
	// glLoadMatrixf( (const float*)&modelView );

	// glMatrixMode( GL_TEXTURE );
	// glLoadMatrixf( (const float*)GetTextureTop() );
}


RageCompiledGeometry* RageDisplay_GLFW::CreateCompiledGeometry()
{
}

void RageDisplay_GLFW::DeleteCompiledGeometry( RageCompiledGeometry* p )
{

	delete p;
}

void RageDisplay_GLFW::DrawQuadsInternal( const RageSpriteVertex v[], int iNumVerts )
{
}

void RageDisplay_GLFW::DrawQuadStripInternal( const RageSpriteVertex v[], int iNumVerts )
{
}

void RageDisplay_GLFW::DrawFanInternal( const RageSpriteVertex v[], int iNumVerts )
{
}

void RageDisplay_GLFW::DrawStripInternal( const RageSpriteVertex v[], int iNumVerts )
{
}

void RageDisplay_GLFW::DrawTrianglesInternal( const RageSpriteVertex v[], int iNumVerts )
{
}

void RageDisplay_GLFW::DrawCompiledGeometryInternal( const RageCompiledGeometry *p, int iMeshIndex )
{
}

void RageDisplay_GLFW::DrawLineStripInternal( const RageSpriteVertex v[], int iNumVerts, float LineWidth )
{
}

void RageDisplay_GLFW::ClearAllTextures()
{
}

int RageDisplay_GLFW::GetNumTextureUnits()
{
}

void RageDisplay_GLFW::SetTexture( int iTextureUnitIndex, RageTexture* pTexture )
{
}
void RageDisplay_GLFW::SetTextureModeModulate()
{
}

void RageDisplay_GLFW::SetTextureModeGlow()
{
	
	
	if( !glfwExtensionSupported("GL_EXT_texture_env_combine") )
	{
		/* This is changing blend state, instead of texture state, which isn't
		 * great, but it's better than doing nothing. */
		glBlendFunc( GL_SRC_ALPHA, GL_ONE );
		return;
	}

	// /* Source color is the diffuse color only: */
	// glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);
	// glTexEnvi(GL_TEXTURE_ENV, GLenum(GL_COMBINE_RGB_EXT), GL_REPLACE);
	// glTexEnvi(GL_TEXTURE_ENV, GLenum(GL_SOURCE0_RGB_EXT), GL_PRIMARY_COLOR_EXT);

	// /* Source alpha is texture alpha * diffuse alpha: */
	// glTexEnvi(GL_TEXTURE_ENV, GLenum(GL_COMBINE_ALPHA_EXT), GL_MODULATE);
	// glTexEnvi(GL_TEXTURE_ENV, GLenum(GL_OPERAND0_ALPHA_EXT), GL_SRC_ALPHA);
	// glTexEnvi(GL_TEXTURE_ENV, GLenum(GL_SOURCE0_ALPHA_EXT), GL_PRIMARY_COLOR_EXT);
	// glTexEnvi(GL_TEXTURE_ENV, GLenum(GL_OPERAND1_ALPHA_EXT), GL_SRC_ALPHA);
	// glTexEnvi(GL_TEXTURE_ENV, GLenum(GL_SOURCE1_ALPHA_EXT), GL_TEXTURE);
}

void RageDisplay_GLFW::SetTextureModeAdd()
{
	// glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_ADD);
}

void RageDisplay_GLFW::SetTextureFiltering( bool b )
{

}

void RageDisplay_GLFW::SetBlendMode( BlendMode mode )
{
	glEnable(GL_BLEND);

	switch( mode )
	{
	case BLEND_NORMAL:
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
		break;
	case BLEND_ADD:
		glBlendFunc( GL_SRC_ALPHA, GL_ONE );
		break;
	case BLEND_NO_EFFECT:
		/* XXX: Would it be faster and have the same effect to say glDisable(GL_COLOR_WRITEMASK)? */
		glBlendFunc( GL_ZERO, GL_ONE );
		break;
	default:
		ASSERT(0);
	}
}

bool RageDisplay_GLFW::IsZWriteEnabled() const
{
	bool a;
	glGetBooleanv( GL_DEPTH_WRITEMASK, (unsigned char*)&a );
	return a;
}

bool RageDisplay_GLFW::IsZTestEnabled() const
{
	GLenum a;
	glGetIntegerv( GL_DEPTH_FUNC, (GLint*)&a );
	return a != GL_ALWAYS;
}

void RageDisplay_GLFW::ClearZBuffer()
{
	bool write = IsZWriteEnabled();
	SetZWrite( true );
    glClear( GL_DEPTH_BUFFER_BIT );
	SetZWrite( write );
}

void RageDisplay_GLFW::SetZWrite( bool b )
{
	glDepthMask( b );
}

void RageDisplay_GLFW::SetZBias( float f )
{
	float fNear = SCALE( f, 0.0f, 1.0f, 0.05f, 0.0f );
	float fFar = SCALE( f, 0.0f, 1.0f, 1.0f, 0.95f );

	glDepthRange( fNear, fFar );
}

void RageDisplay_GLFW::SetZTestMode( ZTestMode mode )
{
	glEnable( GL_DEPTH_TEST );
	switch( mode )
	{
	case ZTEST_OFF:				glDepthFunc( GL_ALWAYS );	break;
	case ZTEST_WRITE_ON_PASS:	glDepthFunc( GL_LEQUAL );	break;
	case ZTEST_WRITE_ON_FAIL:	glDepthFunc( GL_GREATER );	break;
	default:	ASSERT( 0 );
	}
}

void RageDisplay_GLFW::SetTextureWrapping( bool b )
{
	GLenum mode = b ? GL_REPEAT : GL_CLAMP_TO_EDGE;
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, mode );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, mode );
}

void RageDisplay_GLFW::SetMaterial( 
	const RageColor &emissive,
	const RageColor &ambient,
	const RageColor &diffuse,
	const RageColor &specular,
	float shininess
	)
{
	// TRICKY:  If lighting is off, then setting the material 
	// will have no effect.  Even if lighting is off, we still
	// want Models to have basic color and transparency.
	// We can do this fake lighting by setting the vertex color.
	// XXX: unintended: SetLighting must be called before SetMaterial
	// GLboolean bLighting;
	//  ( GL_LIGHTING, &bLighting );

	// if( bLighting )
	// {
	// 	// glMaterialfv( GL_FRONT, GL_EMISSION, emissive );
	// 	// glMaterialfv( GL_FRONT, GL_AMBIENT, ambient );
	// 	// glMaterialfv( GL_FRONT, GL_DIFFUSE, diffuse );
	// 	// glMaterialfv( GL_FRONT, GL_SPECULAR, specular );
	// 	// glMaterialf( GL_FRONT, GL_SHININESS, shininess );
	// }
	// else
	// {
	// 	// glColor4fv( emissive + ambient + diffuse );
	// }
}

void RageDisplay_GLFW::SetLighting( bool b )
{
	// if( b )	glEnable( GL_LIGHTING );
	// else	glDisable( GL_LIGHTING );
}

void RageDisplay_GLFW::SetLightOff( int index )
{
#ifndef __EMSCRIPTEN__
	glDisable( GL_LIGHT0+index );
#endif
}
void RageDisplay_GLFW::SetLightDirectional( 
	int index, 
	const RageColor &ambient, 
	const RageColor &diffuse, 
	const RageColor &specular, 
	const RageVector3 &dir )
{
#ifndef __EMSCRIPTEN__
	// Light coordinates are transformed by the modelview matrix, but
	// we are being passed in world-space coords.
	// glPushMatrix();
	glLoadIdentity();

	glEnable( GL_LIGHT0+index );
	// glLightfv(GL_LIGHT0+index, GL_AMBIENT, ambient);
	// glLightfv(GL_LIGHT0+index, GL_DIFFUSE, diffuse);
	// glLightfv(GL_LIGHT0+index, GL_SPECULAR, specular);
	float position[4] = {dir.x, dir.y, dir.z, 0};
	// glLightfv(GL_LIGHT0+index, GL_POSITION, position);

	// glPopMatrix();
#endif
}

void RageDisplay_GLFW::SetCullMode( CullMode mode )
{
	switch( mode )
	{
	case CULL_BACK:
		glEnable( GL_CULL_FACE );
		glCullFace( GL_BACK );
		break;
	case CULL_FRONT:
		glEnable( GL_CULL_FACE );
		glCullFace( GL_FRONT );
		break;
	case CULL_NONE:
        glDisable( GL_CULL_FACE );
		break;
	default:
		ASSERT(0);
	}
}

const RageDisplay::PixelFormatDesc *RageDisplay_GLFW::GetPixelFormatDesc(RagePixelFormat pf) const
{
	ASSERT( pf < NUM_PIX_FORMATS );
	return &PIXEL_FORMAT_DESC[pf];
}

void RageDisplay_GLFW::DeleteTexture( unsigned uTexHandle )
{
	unsigned int uTexID = uTexHandle;

	FlushGLErrors();
	glDeleteTextures(1,reinterpret_cast<GLuint*>(&uTexID));

	AssertNoGLError();
}


RageDisplay::RagePixelFormat RageDisplay_GLFW::GetImgPixelFormat( RageSurface* &img, bool &FreeImg, int width, int height, bool bPalettedTexture )
{
	RagePixelFormat pixfmt = FindPixelFormat( img->format->BitsPerPixel, img->format->Rmask, img->format->Gmask, img->format->Bmask, img->format->Amask );
	
	/* If img is paletted, we're setting up a non-paletted texture, and color indexes
	 * are too small, depalettize. */
	bool bSupported = true;
	if( !bPalettedTexture && img->fmt.BytesPerPixel == 1 && !g_bColorIndexTableWorks )
		bSupported = false;

	if( pixfmt == NUM_PIX_FORMATS || !SupportsSurfaceFormat(pixfmt) )
		bSupported = false;

	if( !bSupported )
	{
		/* The source isn't in a supported, known pixel format.  We need to convert
		 * it ourself.  Just convert it to RGBA8, and let OpenGL convert it back
		 * down to whatever the actual pixel format is.  This is a very slow code
		 * path, which should almost never be used. */
		pixfmt = FMT_RGBA8;
		ASSERT( SupportsSurfaceFormat(pixfmt) );

		const PixelFormatDesc *pfd = DISPLAY->GetPixelFormatDesc(pixfmt);

		RageSurface *imgconv = CreateSurface( width, height,
			pfd->bpp, pfd->masks[0], pfd->masks[1], pfd->masks[2], pfd->masks[3] );
		RageSurfaceUtils::Blit( img, imgconv, width, height );
		img = imgconv;
		FreeImg = true;
	}
	else
		FreeImg = false;

	return pixfmt;
}

/* If we're sending a paletted surface to a non-paletted texture, set the palette. */
void SetPixelMapForSurface( int glImageFormat, int glTexFormat, const RageSurfacePalette *palette )
{
	GLushort buf[4][256];
	memset( buf, 0, sizeof(buf) );

	for( int i = 0; i < palette->ncolors; ++i )
	{
		buf[0][i] = SCALE( palette->colors[i].r, 0, 255, 0, 65535 );
		buf[1][i] = SCALE( palette->colors[i].g, 0, 255, 0, 65535 );
		buf[2][i] = SCALE( palette->colors[i].b, 0, 255, 0, 65535 );
		buf[3][i] = SCALE( palette->colors[i].a, 0, 255, 0, 65535 );
	}

	FlushGLErrors();
	// glPixelMapusv( GL_PIXEL_MAP_I_TO_R, 256, buf[0] );
	// glPixelMapusv( GL_PIXEL_MAP_I_TO_G, 256, buf[1] );
	// glPixelMapusv( GL_PIXEL_MAP_I_TO_B, 256, buf[2] );
	// glPixelMapusv( GL_PIXEL_MAP_I_TO_A, 256, buf[3] );
	//  no support GLES3
	// glPixelStorei( GL_MAP_COLOR, true );
	GLenum error = glGetError();
	ASSERT_M( error == GL_NO_ERROR, GLToString(error) );
}

unsigned RageDisplay_GLFW::CreateTexture( 
	RagePixelFormat pixfmt,
	RageSurface* img,
	bool bGenerateMipMaps )
{
	ASSERT( pixfmt < NUM_PIX_FORMATS );
	ASSERT( img->w == power_of_two(img->w) && img->h == power_of_two(img->h) );


	/* Find the pixel format of the image we've been given. */
	bool FreeImg;
	RagePixelFormat imgpixfmt = GetImgPixelFormat( img, FreeImg, img->w, img->h, pixfmt == FMT_RGBA8 );

	GLenum glTexFormat = GL_PIXFMT_INFO[pixfmt].internalfmt;
	GLenum glImageFormat = GL_PIXFMT_INFO[imgpixfmt].format;
	GLenum glImageType = GL_PIXFMT_INFO[imgpixfmt].type;

	/* If the image is paletted, but we're not sending it to a paletted image,
	 * set up glPixelMap. */
	SetPixelMapForSurface( glImageFormat, glTexFormat, img->format->palette );

	// allocate OpenGL texture resource
	unsigned int uTexHandle;
	glGenTextures(1, reinterpret_cast<GLuint*>(&uTexHandle));
	ASSERT(uTexHandle);
	
	glBindTexture( GL_TEXTURE_2D, uTexHandle );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	GLint minFilter;
	if( bGenerateMipMaps )
	{
		if( wind->GetVideoModeParams().bTrilinearFiltering )
			minFilter = GL_LINEAR_MIPMAP_LINEAR;
		else
			minFilter = GL_LINEAR_MIPMAP_NEAREST;
	}
	else
	{
		minFilter = GL_LINEAR;
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);

	if( wind->GetVideoModeParams().bAnisotropicFiltering &&
		glfwExtensionSupported("GL_EXT_texture_filter_anisotropic") )
	{
		GLfloat largest_supported_anisotropy;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &largest_supported_anisotropy);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, largest_supported_anisotropy);
	}

	SetTextureWrapping( false );

	glPixelStorei(GL_UNPACK_ROW_LENGTH, img->pitch / img->format->BytesPerPixel);


	// if( pixfmt == FMT_PAL )
	// {
	// 	/* The texture is paletted; set the texture palette. */
	// 	GLubyte palette[256*4];
	// 	memset(palette, 0, sizeof(palette));
	// 	int p = 0;
	// 	/* Copy the palette to the format OpenGL expects. */
	// 	for(int i = 0; i < img->format->palette->ncolors; ++i)
	// 	{
	// 		palette[p++] = img->format->palette->colors[i].r;
	// 		palette[p++] = img->format->palette->colors[i].g;
	// 		palette[p++] = img->format->palette->colors[i].b;
	// 		palette[p++] = img->format->palette->colors[i].a;
	// 	}
	// }
	
	{
		ostringstream s;
		
		s << (bGenerateMipMaps? "glfwBuild2DMipmaps":"glTexImage2D");
		s << "(format " << GLToString(glTexFormat) <<
				", " << img->w << "x" <<  img->h <<
				", format " << GLToString(glImageFormat) <<
				", type " << GLToString(glImageType) <<
				", pixfmt " << pixfmt <<
				", imgpixfmt " << imgpixfmt <<
				")";
		LOG->Trace( "%s", s.str().c_str() );
	}

	FlushGLErrors();

	GLenum error;
	glTexImage2D(
		GL_TEXTURE_2D, 0, glTexFormat, 
		img->w, img->h, 0,
		glImageFormat, glImageType, img->pixels);
	
	error = glGetError();
	ASSERT_M( error == GL_NO_ERROR, GLToString(error) );

	if( bGenerateMipMaps )
		glGenerateMipmap(GL_TEXTURE_2D);
	error = glGetError();
	ASSERT_M( error == GL_NO_ERROR, GLToString(error) );


	/* Sanity check: */
	// if( pixfmt == FMT_PAL )
	// {
	// 	GLint size = 0;
	// 	glGetTexParameteriv(GL_TEXTURE_2D, GLenum(GL_TEXTURE_INDEX_SIZE_EXT), &size);
	// 	if(size != 8)
	// 		RageException::Throw("Thought paletted textures worked, but they don't.");
	// }

	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glFlush();

	if( FreeImg )
		delete img;
	return uTexHandle;
}

/* This doesn't support img being paletted if the surface itself isn't paletted.
 * This is only used for movies anyway, which are never paletted. */
void RageDisplay_GLFW::UpdateTexture( 
	unsigned uTexHandle, 
	RageSurface* img,
	int xoffset, int yoffset, int width, int height )
{
	glBindTexture( GL_TEXTURE_2D, uTexHandle );

	bool FreeImg;
	RagePixelFormat pixfmt = GetImgPixelFormat( img, FreeImg, width, height, false );

	glPixelStorei(GL_UNPACK_ROW_LENGTH, img->pitch / img->format->BytesPerPixel);

//	GLenum glTexFormat = GL_PIXFMT_INFO[pixfmt].internalfmt;
	GLenum glImageFormat = GL_PIXFMT_INFO[pixfmt].format;
	GLenum glImageType = GL_PIXFMT_INFO[pixfmt].type;

	glTexSubImage2D(GL_TEXTURE_2D, 0,
		xoffset, yoffset,
		width, height,
		glImageFormat, glImageType, img->pixels);

	/* Must unset PixelStore when we're done! */
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glFlush();

	if( FreeImg )
		delete img;
}

void RageDisplay_GLFW::SetPolygonMode( PolygonMode pm )
{
	GLenum m;
	switch( pm )
	{
	case POLYGON_FILL:	m = GL_FILL; break;
	case POLYGON_LINE:	m = GL_LINE; break;
	default:	ASSERT(0);	return;
	}
	glPolygonOffset(GL_FRONT_AND_BACK, m);
}

void RageDisplay_GLFW::SetLineWidth( float fWidth )
{
	glLineWidth( fWidth );
}

CString RageDisplay_GLFW::GetTextureDiagnostics( unsigned id ) const
{
	return "";
}

void RageDisplay_GLFW::SetAlphaTest( bool b )
{
}


/*
 * Although we pair texture formats (eg. GL_RGB8) and surface formats
 * (pairs of eg. GL_RGB8,GL_UNSIGNED_SHORT_5_5_5_1), it's possible for
 * a format to be supported for a texture format but not a surface
 * format.  This is abstracted, so you don't need to know about this
 * as a user calling CreateTexture.
 *
 * One case of this is if packed pixels aren't supported.  We can still
 * use 16-bit color modes, but we have to send it in 32-bit.  Almost
 * everything supports packed pixels.
 *
 * Another case of this is incomplete packed pixels support.  Some implementations
 * neglect GL_UNSIGNED_SHORT_*_REV. 
 */
bool RageDisplay_GLFW::SupportsSurfaceFormat( RagePixelFormat pixfmt )
{
	return true;
}


bool RageDisplay_GLFW::SupportsTextureFormat( RagePixelFormat pixfmt, bool realtime )
{
	/* If we support a pixfmt for texture formats but not for surface formats, then
	 * we'll have to convert the texture to a supported surface format before uploading.
	 * This is too slow for dynamic textures. */
	if( realtime && !SupportsSurfaceFormat( pixfmt ) )
		return false;

	switch( GL_PIXFMT_INFO[pixfmt].format )
	{
	case GL_COLOR_INDEX:
		return !!(&glColorTable);
	case GL_BGR:
	case GL_BGRA:
		return GL_BGRA;
	default:
		return true;
	}
}

void RageDisplay_GLFW::SetSphereEnvironmentMapping( bool b )
{
#ifndef __EMSCRIPTEN__
	if( b )
	{
		glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
		glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
		glEnable(GL_TEXTURE_GEN_S);
		glEnable(GL_TEXTURE_GEN_T);
	}
	else
	{
		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);
	}
#endif
}