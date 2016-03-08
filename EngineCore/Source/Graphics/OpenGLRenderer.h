
#ifndef _OPENGLRENDERER_H_
#define _OPENGLRENDERER_H_

#include "Renderer.h"

#define ROLLING_VB_SIZE (64 * 1024)
static const GLsizei glTypes[] = 
{
	GL_FLOAT,
	0, // No half float support
	GL_UNSIGNED_BYTE,
};

static const GLint internalFormats[] = 
{
	0,

	GL_R8,
	GL_RG8,
	GL_RGB8,
	GL_RGBA8,

	GL_R16,
	GL_RG16,
	GL_RGB16,
	GL_RGBA16,

	GL_R8_SNORM,
	GL_RG8_SNORM,
	GL_RGB8_SNORM,
	GL_RGBA8_SNORM,

	GL_R16_SNORM,
	GL_RG16_SNORM,
	GL_RGB16_SNORM,
	GL_RGBA16_SNORM,

	GL_R16F,
	GL_RG16F,
	GL_RGB16F,
	GL_RGBA16F,

	GL_R32F,
	GL_RG32F,
	GL_RGB32F,
	GL_RGBA32F,

	GL_R16I,
	GL_RG16I,
	GL_RGB16I,
	GL_RGBA16I,

	GL_R32I,
	GL_RG32I,
	GL_RGB32I,
	GL_RGBA32I,

	GL_R16UI,
	GL_RG16UI,
	GL_RGB16UI,
	GL_RGBA16UI,

	GL_R32UI,
	GL_RG32UI,
	GL_RGB32UI,
	GL_RGBA32UI,

	0, // RGBE8 not directly supported
	GL_RGB9_E5,
	GL_R11F_G11F_B10F,
	GL_RGB565,
	GL_RGBA4,
	GL_RGB10_A2,

	GL_DEPTH_COMPONENT16,
	GL_DEPTH_COMPONENT24,
	GL_DEPTH_COMPONENT32,
	GL_DEPTH_COMPONENT32F,

	GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,
	GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,
	GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,
	GL_COMPRESSED_RED_RGTC1,
	GL_COMPRESSED_RG_RGTC2,
};

static const GLint externalFormats[] = 
{
	0,

	GL_RED,
	GL_RG,
	GL_BGR,
	GL_BGRA,

	GL_RED,
	GL_RG,
	GL_BGR,
	GL_BGRA,

	GL_RED,
	GL_RG,
	GL_BGR,
	GL_BGRA,

	GL_RED,
	GL_RG,
	GL_BGR,
	GL_BGRA,

	GL_RED,
	GL_RG,
	GL_BGR,
	GL_BGRA,

	GL_RED,
	GL_RG,
	GL_BGR,
	GL_BGRA,

	GL_RED,
	GL_RG,
	GL_BGR,
	GL_BGRA,

	GL_RED,
	GL_RG,
	GL_BGR,
	GL_BGRA,

	GL_RED,
	GL_RG,
	GL_BGR,
	GL_BGRA,

	GL_RED,
	GL_RG,
	GL_BGR,
	GL_BGRA,

	0, // RGBE8 not directly supported
	GL_BGR,
	GL_BGR,
	GL_BGR,
	GL_BGRA,
	GL_BGRA,

	GL_DEPTH_COMPONENT,
	GL_DEPTH_COMPONENT,
	GL_DEPTH_COMPONENT,
	GL_DEPTH_COMPONENT,

	GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,
	GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,
	GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,
	GL_COMPRESSED_RED_RGTC1,
	GL_COMPRESSED_RG_RGTC2,
};

static const GLint sources[] = 
{
	0,

	GL_UNSIGNED_BYTE,
	GL_UNSIGNED_BYTE,
	GL_UNSIGNED_BYTE,
	GL_UNSIGNED_BYTE,

	GL_UNSIGNED_BYTE,
	GL_UNSIGNED_BYTE,
	GL_UNSIGNED_BYTE,
	GL_UNSIGNED_BYTE,

	GL_UNSIGNED_BYTE,
	GL_UNSIGNED_BYTE,
	GL_UNSIGNED_BYTE,
	GL_UNSIGNED_BYTE,

	GL_UNSIGNED_BYTE,
	GL_UNSIGNED_BYTE,
	GL_UNSIGNED_BYTE,
	GL_UNSIGNED_BYTE,

	GL_FLOAT,
	GL_FLOAT,
	GL_FLOAT,
	GL_FLOAT,

	GL_FLOAT,
	GL_FLOAT,
	GL_FLOAT,
	GL_FLOAT,

	GL_UNSIGNED_BYTE,
	GL_UNSIGNED_BYTE,
	GL_UNSIGNED_BYTE,
	GL_UNSIGNED_BYTE,

	GL_UNSIGNED_BYTE,
	GL_UNSIGNED_BYTE,
	GL_UNSIGNED_BYTE,
	GL_UNSIGNED_BYTE,

	GL_UNSIGNED_BYTE,
	GL_UNSIGNED_BYTE,
	GL_UNSIGNED_BYTE,
	GL_UNSIGNED_BYTE,

	GL_UNSIGNED_BYTE,
	GL_UNSIGNED_BYTE,
	GL_UNSIGNED_BYTE,
	GL_UNSIGNED_BYTE,

	0, // RGBE8 not directly supported
	GL_UNSIGNED_BYTE,
	GL_UNSIGNED_BYTE,
	GL_UNSIGNED_BYTE,
	GL_UNSIGNED_BYTE,
	GL_UNSIGNED_BYTE,

	GL_FLOAT,
	GL_FLOAT,
	GL_FLOAT,
	GL_FLOAT,

	GL_UNSIGNED_BYTE,
	GL_UNSIGNED_BYTE,
	GL_UNSIGNED_BYTE,
	GL_UNSIGNED_BYTE,
	GL_UNSIGNED_BYTE,
};

static const GLenum srcFormats[] = { 0, GL_LUMINANCE, GL_LUMINANCE_ALPHA, GL_RGB, GL_RGBA };

static const GLenum srcObjectBufferType[] = 
{
    GL_ATOMIC_COUNTER_BUFFER,
    GL_TRANSFORM_FEEDBACK_BUFFER, 
    GL_UNIFORM_BUFFER,
    GL_SHADER_STORAGE_BUFFER
};

static const GLenum srcTypes[] = 
{
	0,
	// Unsigned formats
	GL_UNSIGNED_BYTE,
	GL_UNSIGNED_BYTE,
	GL_UNSIGNED_BYTE,
	GL_UNSIGNED_BYTE,

	GL_UNSIGNED_SHORT,
	GL_UNSIGNED_SHORT,
	GL_UNSIGNED_SHORT,
	GL_UNSIGNED_SHORT,

	// Signed formats
	0,
	0,
	0,
	0,

	0,
	0,
	0,
	0,

	// Float formats
	0,//GL_HALF_FLOAT,
	0,//GL_HALF_FLOAT,
	0,//GL_HALF_FLOAT,
	0,//GL_HALF_FLOAT,

	GL_FLOAT,
	GL_FLOAT,
	GL_FLOAT,
	GL_FLOAT,

	// Signed integer formats
	0,
	0,
	0,
	0,

	0,
	0,
	0,
	0,

	// Unsigned integer formats
	0,
	0,
	0,
	0,

	0,
	0,
	0,
	0,

	// Packed formats
	0, // RGBE8 not directly supported
	0, // RGBE9E5 not supported
	0, // RG11B10F not supported
	GL_UNSIGNED_SHORT_5_6_5,
	GL_UNSIGNED_SHORT_4_4_4_4_REV,
	GL_UNSIGNED_INT_2_10_10_10_REV,

	// Depth formats
	GL_UNSIGNED_SHORT,
	GL_UNSIGNED_INT,
	GL_UNSIGNED_INT_24_8_EXT,
	0, // D32F not supported

	// Compressed formats
	0,
	0,
	0,
	0,
	0,
};

struct ImageBufferSlot
{
	TextureID imageBufferID;
	FORMAT imageBufferFormat;
	ImageBufferAccess imageBufferAccess;
    int mipLevel;
};

class OpenGLRenderer : public Renderer {
public:

#if defined(_WIN32)
	OpenGLRenderer(SDL_Window* pSDLWindowHandle, SDL_GLContext GLMainContext);
#elif defined(LINUX)
	OpenGLRenderer(Window win, GLXContext glXc, Display *disp, int scr);
#elif defined(__APPLE__)
	OpenGLRenderer(AGLContext aGlc);
#endif
	~OpenGLRenderer();
	void resetToDefaults();
	void reset(const UINT flags = RESET_ALL);

	TextureID createTexture(Image &img, const SamplerStateID samplerState = SS_NONE, UINT flags = 0);

	TextureID addRenderTarget(const int width, const int height, const int depth, const int mipMapCount, const int arraySize, const FORMAT format, const int msaaSamples = 1, const SamplerStateID samplerState = SS_NONE, UINT flags = 0);
	TextureID addRenderDepth(const int width, const int height, const int arraySize, const FORMAT format, const int msaaSamples = 1, const SamplerStateID samplerState = SS_NONE, UINT flags = 0);
	void setRenderTargetSize(const TextureID renderTarget, const int width, const int height);
	bool resizeRenderTarget(const TextureID renderTarget, const int width, const int height, const int depth, const int mipMapCount, const int arraySize);
	bool generateMipMaps(const TextureID renderTarget);

	void removeTexture(const TextureID texture);

	ShaderID createShader(const char *vsText, const char *gsText, const char *fsText, const char *csText, const char* tcsText, const char* tesText, const int vsLine, const int gsLine, const int fsLine, const int csLine,
		const int tcsLine, const int tesLine, const char *header = NULL, const char *extra = NULL, const char *fileName = NULL, const char **attributeNames = NULL, const int nAttributes = 0, const UINT flags = 0);

	bool getShaderWorkGroupSize(const int shaderID, UINT* localGroupSize);
	
	VertexFormatID addVertexFormat(const FormatDesc *formatDesc, const UINT nAttribs, const ShaderID shader = SHADER_NONE);
	VertexBufferID addVertexBuffer(const long size, const BufferAccess bufferAccess, const void *data = NULL);
	IndexBufferID addIndexBuffer(const UINT nIndices, const UINT indexSize, const BufferAccess bufferAccess, const void *data = NULL);
	
	 ObjectBufferID createObjectBuffer(const ObjectBufferType type, const UINT objectSize, const UINT nObjects, const BufferAccess bufferAccess, const void *data = NULL);
	 
	SamplerStateID addSamplerState(const Filter filter, const AddressMode s, const AddressMode t, const AddressMode r, const float lod = 0, const UINT maxAniso = 16, const int compareFunc = 0, const float *border_color = NULL);
	BlendStateID addBlendState(const int srcFactorRGB, const int destFactorRGB, const int srcFactorAlpha, const int destFactorAlpha, const int blendModeRGB, const int blendModeAlpha, const int mask = ALL, const bool alphaToCoverage = false);
	DepthStateID addDepthState(const bool depthTest, const bool depthWrite, const int depthFunc, const bool stencilTest, const UINT8 stencilReadMask, const UINT8 stencilWriteMask,
		const int stencilFuncFront, const int stencilFuncBack, const int stencilFailFront, const int stencilFailBack,
		const int depthFailFront, const int depthFailBack, const int stencilPassFront, const int stencilPassBack);
	RasterizerStateID addRasterizerState(const int cullMode, const int fillMode = SOLID, const bool multiSample = true, const bool scissor = false, const float depthBias = 0.0f, const float slopeDepthBias = 0.0f);
	FontID addFont(const char *ttfFontFile, const UINT size, const SamplerStateID samplerState = -1);
	FontID addFont(char *rawBuffer, unsigned int rawSize, const UINT size, const SamplerStateID samplerState = -1);

	int getSamplerUnit(const ShaderID shader, const char *samplerName) const;
	int getImageBufferUnit(const ShaderID shader, const char *imageBufferName) const;
	int getObjectBufferUnit(const ShaderID shader, const char *objectBufferName) const;
	
	GLenum getExternalFormat(GLenum internalFormat);
	void setTexture(const TextureID texture){ selectedTextures[0] = texture; }
	void setTexture(const char *textureName, const TextureID texture);
	void setTexture(const int textureUnit, const TextureID texture);
	void setTextureArray(const int initIndex, const TextureID *textures, const int n_textures);
	void setTextureArray(const char *textureName, const TextureID *textures, const int arraySize);
	void setTexture(const char *textureName, const TextureID texture, const SamplerStateID samplerState);
	void setTextureSlice(const char *textureName, const TextureID texture, const int slice);
	void applyTextures();

	void setImageBuffer(const char *ImageBufferName, const TextureID texture, const int mipLevel, const FORMAT formatImageBuffer, const ImageBufferAccess imageBufferAccess);
	void setImageBufferArray(const char *ImageBufferName, const TextureID *texture, const int *mipLevel, const FORMAT *formatImageBuffer, const ImageBufferAccess *imageBufferAccess, const UINT offset, const UINT arraySize);
	void applyImageBuffers();

	void setObjectBuffer(const char *objectBufferName, const ObjectBufferID objectBufferID);
	void setObjectBuffer(const int bindingSlot, const ObjectBufferID objectBufferID);
    void applyObjectBuffers();

	void setSamplerState(const char *samplerName, const SamplerStateID samplerState);
	void applySamplerStates();

	void setShaderConstantRaw(const char *name, const void *data, const int size);
	void applyConstants();
	bool hasTessellation(const ShaderID shaderID);

//	void changeTexture(const uint imageUnit, const TextureID texture);
	void changeRenderTargets(const TextureID *colorRTs, const UINT nRenderTargets, const TextureID depthRT = TEXTURE_NONE, const int depthSlice = NO_SLICE, const int *slices = NULL, const UINT flags = DEFAULT_SIZE);
	void changeToMainFramebuffer();
	void changeShader(const ShaderID shader);
	void changeVertexFormat(const VertexFormatID vertexFormat);
	void changeVertexBuffer(UINT StartSlot, UINT NumBuffers, const VertexBufferID* vertexBuffer, const UINT* offset = 0);
	void changeIndexBuffer(const IndexBufferID indexBuffer);

//	void changeSamplerState(const uint samplerUnit, const SamplerStateID samplerState);
	void changeBlendState(const BlendStateID blendState, const UINT sampleMask = ~0);
	void changeDepthState(const DepthStateID depthState, const UINT stencilRef = 0);
	void changeRasterizerState(const RasterizerStateID rasterizerState);

/*
	void changeShaderConstant1i(const char *name, const int constant);
	void changeShaderConstant1f(const char *name, const float constant);
	void changeShaderConstant2f(const char *name, const vec2 &constant);
	void changeShaderConstant3f(const char *name, const vec3 &constant);
	void changeShaderConstant4f(const char *name, const vec4 &constant);
	void changeShaderConstant3x3f(const char *name, const mat3 &constant);
	void changeShaderConstant4x4f(const char *name, const mat4 &constant);
	void changeShaderConstantArray1f(const char *name, const float *constant, const uint count);
	void changeShaderConstantArray2f(const char *name, const vec2 *constant, const uint count);
	void changeShaderConstantArray3f(const char *name, const vec3 *constant, const uint count);
	void changeShaderConstantArray4f(const char *name, const vec4 *constant, const uint count);
*/

    //Queries///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    QueryID createQuery();
    void beginQuery(QueryID id);
	void endQuery(QueryID id);
    bool queryResultAvailable(QueryID id);
    UINT getResultQuery(QueryID id);
	void deleteQueries();
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void clear(const bool clearColor, const bool clearDepth, const bool clearStencil, const float *color = NULL, const float depth = 1.0f, const UINT stencil = 0);

	void drawArrays(const Primitives primitives, const int firstVertex, const int nVertices);
	void drawElements(const Primitives primitives, const int firstIndex, const int nIndices, const int firstVertex, const int nVertices);

	void setup2DMode(const float left, const float right, const float top, const float bottom);
	void drawPlain(const Primitives primitives, Vec2 *vertices, const UINT nVertices, const BlendStateID blendState, const DepthStateID depthState, const Vec4 *color = NULL);
	void drawTextured(const Primitives primitives, TexVertex *vertices, const UINT nVertices, const TextureID texture, const SamplerStateID samplerState, const BlendStateID blendState, const DepthStateID depthState, const Vec4 *color = NULL);
	bool drawText(const char *str, float x, float y, const FontID font, const SamplerStateID samplerState, const BlendStateID blendState, const DepthStateID depthState, Color *textColor = NULL, const Vec4 *rect = NULL, const BYTE flags = 0);

	void dispatchCompute(const UINT num_groups_x, const UINT num_groups_y, const UINT num_groups_z);
    void memoryBarrier(UINT flags);

	void flush();
	void finish();

	ubyte *mapRollingVB(const UINT size);
	void unmapRollingVB(const UINT size);
	UINT copyToRollingVB(const void *src, const UINT size);

	//Reset AtomicCounter
    void resetAtomicCounter(ObjectBufferID atomicCounterBuffer, UINT value);

	//Map/UnMap Buffer
	void* mapBuffer(const ObjectBufferID objectBuffer, const ImageBufferAccess bufferAccess);
	void unMapBuffer();

protected:
	void changeFrontFace(const GLenum frontFace);
//	void setupFilter(const Texture &tex, const Filter filter, const uint flags);
	void setupSampler(GLenum glTarget, const SamplerState &ss);

#if defined(WIN32)
	SDL_Window* m_pWindowHandle;
	SDL_GLContext m_MainContext;
#elif defined(LINUX)
	Window window;
    GLXContext glxc;
	Display *display;
	int screen;
#elif defined(__APPLE__)
	AGLContext aglc;
#endif

	TextureID currentTextures[MAX_TEXTUREUNIT], selectedTextures[MAX_TEXTUREUNIT];
	ImageBufferSlot currentImageBuffers[MAX_IMAGEBUFFERUNIT], selectedImageBuffers[MAX_IMAGEBUFFERUNIT];
	ObjectBufferID currentObjectBuffers[MAX_OBJECTBUFFERUNIT], selectedObjectBuffers[MAX_OBJECTBUFFERUNIT];
	SamplerStateID currentSamplerStates[MAX_SAMPLERSTATE], selectedSamplerStates[MAX_SAMPLERSTATE];
	float textureLod[MAX_TEXTUREUNIT];
	bool enableVertexAttribs[MAX_VERTEXSTREAM];

	VertexFormatID activeVertexFormat[MAX_VERTEXSTREAM];

	GLuint fbo;
	GLenum drawBuffers[MAX_MRTS];
	GLenum currentFrontFace;

	GLenum currentSrcFactorRGB;
	GLenum currentDstFactorRGB;
	GLenum currentSrcFactorAlpha;
	GLenum currentDstFactorAlpha;
	GLenum currentBlendModeRGB;
	GLenum currentBlendModeAlpha;
	int currentMask;
	bool currentBlendEnable;
	bool currentAlphaToCoverageEnable;

	int currentDepthFunc;
	bool currentDepthTestEnable;
	bool currentDepthWriteEnable;

	bool currentMultiSampleEnable;
	bool currentScissorEnable;
	int currentCullMode;
	int currentFillMode;

	VertexBufferID rollingVB;
	int rollingVBOffset;

	ShaderID plainShader, texShader;
	VertexFormatID plainVF, texVF;

	Vec4 scaleBias2D;

	GLuint m_VAO;

	void *uniformFuncs[CONSTANT_TYPE_COUNT];

	//TTF Font attributes
	TextureID m_textTexture;
	SDL_Surface* m_ptextSurface; //Comprobar si acumula memoria
	
private:
	bool vboChange;
};

#endif // _OPENGLRENDERER_H_
