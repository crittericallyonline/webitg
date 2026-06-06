/* RageDisplay_GLFW: OpenGL renderer. */

#ifndef RAGEDISPLAY_GLFW_H
#define RAGEDISPLAY_GLFW_H

class RageDisplay_GLFW: public RageDisplay
{
public:
	RageDisplay_GLFW();
	virtual ~RageDisplay_GLFW();
	CString Init( VideoModeParams p, bool bAllowUnacceleratedRenderer );
	void Update(float fDeltaTime);

	virtual CString GetApiDescription() const { return "OpenGL"; }

	bool IsSoftwareRenderer();
	void ResolutionChanged();
	const PixelFormatDesc *GetPixelFormatDesc(RagePixelFormat pf) const;

	bool BeginFrame();	
	void EndFrame();
	VideoModeParams GetVideoModeParams() const;
	void SetBlendMode( BlendMode mode );
	bool SupportsTextureFormat( RagePixelFormat pixfmt, bool realtime=false );
	unsigned CreateTexture( 
		RagePixelFormat pixfmt, 
		RageSurface* img,
		bool bGenerateMipMaps );
	void UpdateTexture( 
		unsigned uTexHandle, 
		RageSurface* img,
		int xoffset, int yoffset, int width, int height 
		);
	void DeleteTexture( unsigned uTexHandle );
	void ClearAllTextures();
	int GetNumTextureUnits();
	void SetTexture( int iTextureUnitIndex, RageTexture* pTexture );
	void SetTextureModeModulate();
	void SetTextureModeGlow();
	void SetTextureModeAdd();
	void SetTextureWrapping( bool b );
	int GetMaxTextureSize() const;
	void SetTextureFiltering( bool b);
	bool IsZWriteEnabled() const;
	bool IsZTestEnabled() const;
	void SetZWrite( bool b );
	void SetZBias( float f );
	void SetZTestMode( ZTestMode mode );
	void ClearZBuffer();
	void SetCullMode( CullMode mode );
	void SetAlphaTest( bool b );
	void SetMaterial( 
		const RageColor &emissive,
		const RageColor &ambient,
		const RageColor &diffuse,
		const RageColor &specular,
		float shininess
		);
	void SetLighting( bool b );
	void SetLightOff( int index );
	void SetLightDirectional( 
		int index, 
		const RageColor &ambient, 
		const RageColor &diffuse, 
		const RageColor &specular, 
		const RageVector3 &dir );

	void SetSphereEnvironmentMapping( bool b );

	RageCompiledGeometry* CreateCompiledGeometry();
	void DeleteCompiledGeometry( RageCompiledGeometry* p );

	// hacks for cell-shaded models
	virtual void SetPolygonMode( PolygonMode pm );
	virtual void SetLineWidth( float fWidth );

	CString GetTextureDiagnostics( unsigned id ) const;

protected:
	void DrawQuadsInternal( const RageSpriteVertex v[], int iNumVerts );
	void DrawQuadStripInternal( const RageSpriteVertex v[], int iNumVerts );
	void DrawFanInternal( const RageSpriteVertex v[], int iNumVerts );
	void DrawStripInternal( const RageSpriteVertex v[], int iNumVerts );
	void DrawTrianglesInternal( const RageSpriteVertex v[], int iNumVerts );
	void DrawCompiledGeometryInternal( const RageCompiledGeometry *p, int iMeshIndex );
	void DrawLineStripInternal( const RageSpriteVertex v[], int iNumVerts, float LineWidth );

	CString TryVideoMode( VideoModeParams params, bool &bNewDeviceOut );
	RageSurface* CreateScreenshot();
	void SetViewport(int shift_left, int shift_down);
	RagePixelFormat GetImgPixelFormat( RageSurface* &img, bool &FreeImg, int width, int height, bool bPalettedTexture );
	bool SupportsSurfaceFormat( RagePixelFormat pixfmt );
	
	void SendCurrentMatrices();
};

#endif