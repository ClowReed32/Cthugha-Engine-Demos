#ifndef _DIRECT3D10RENDERER_H_
#define _DIRECT3D10RENDERER_H_

#include "Renderer.h"

//#define SAFE_RELEASE(p) { if (p){ p->Release(); p = NULL; } }

#define ROLLING_VB_SIZE (64 * 1024)

#define STRUCTURED_BUFFER_RESOURCE 0
#define TEXTURE_RESOURCE 1

/*
#define VB_INVALID (-2)
*/

static DXGI_FORMAT formats[] = {
	DXGI_FORMAT_UNKNOWN,

	DXGI_FORMAT_R8_UNORM,
	DXGI_FORMAT_R8G8_UNORM,
	DXGI_FORMAT_UNKNOWN, // RGB8 not directly supported
	DXGI_FORMAT_R8G8B8A8_UNORM,

	DXGI_FORMAT_R16_UNORM,
	DXGI_FORMAT_R16G16_UNORM,
	DXGI_FORMAT_UNKNOWN, // RGB16 not directly supported
	DXGI_FORMAT_R16G16B16A16_UNORM,

	DXGI_FORMAT_R8_SNORM,
	DXGI_FORMAT_R8G8_SNORM,
	DXGI_FORMAT_UNKNOWN, // RGB8S not directly supported
	DXGI_FORMAT_R8G8B8A8_SNORM,

	DXGI_FORMAT_R16_SNORM,
	DXGI_FORMAT_R16G16_SNORM,
	DXGI_FORMAT_UNKNOWN, // RGB16S not directly supported
	DXGI_FORMAT_R16G16B16A16_SNORM,

	DXGI_FORMAT_R16_FLOAT,
	DXGI_FORMAT_R16G16_FLOAT,
	DXGI_FORMAT_UNKNOWN, // RGB16F not directly supported
	DXGI_FORMAT_R16G16B16A16_FLOAT,

	DXGI_FORMAT_R32_FLOAT,
	DXGI_FORMAT_R32G32_FLOAT,
	DXGI_FORMAT_R32G32B32_FLOAT,
	DXGI_FORMAT_R32G32B32A32_FLOAT,

	DXGI_FORMAT_R16_SINT,
	DXGI_FORMAT_R16G16_SINT,
	DXGI_FORMAT_UNKNOWN, // RGB16I not directly supported
	DXGI_FORMAT_R16G16B16A16_SINT,

	DXGI_FORMAT_R32_SINT,
	DXGI_FORMAT_R32G32_SINT,
	DXGI_FORMAT_R32G32B32_SINT,
	DXGI_FORMAT_R32G32B32A32_SINT,

	DXGI_FORMAT_R16_UINT,
	DXGI_FORMAT_R16G16_UINT,
	DXGI_FORMAT_UNKNOWN, // RGB16UI not directly supported
	DXGI_FORMAT_R16G16B16A16_UINT,

	DXGI_FORMAT_R32_UINT,
	DXGI_FORMAT_R32G32_UINT,
	DXGI_FORMAT_R32G32B32_UINT,
	DXGI_FORMAT_R32G32B32A32_UINT,

	DXGI_FORMAT_UNKNOWN, // RGBE8 not directly supported
	DXGI_FORMAT_R9G9B9E5_SHAREDEXP,
	DXGI_FORMAT_R11G11B10_FLOAT,
	DXGI_FORMAT_B5G6R5_UNORM,
	DXGI_FORMAT_UNKNOWN, // RGBA4 not directly supported
	DXGI_FORMAT_R10G10B10A2_UNORM,

	DXGI_FORMAT_D16_UNORM,
	DXGI_FORMAT_D24_UNORM_S8_UINT,
	DXGI_FORMAT_D24_UNORM_S8_UINT,
	DXGI_FORMAT_D32_FLOAT,

	DXGI_FORMAT_BC1_UNORM,
	DXGI_FORMAT_BC2_UNORM,
	DXGI_FORMAT_BC3_UNORM,
	DXGI_FORMAT_BC4_UNORM,
	DXGI_FORMAT_BC5_UNORM,
};

struct BufferResourceEntry
{
    TextureID id;
    int type;
};

struct ImageSlot
{
	TextureID id;
	int mipLevel;
};

//This class allow to use include directive
class CShaderInclude : public ID3DInclude 
{
public:
	CShaderInclude() {}

	HRESULT __stdcall Open(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes);
	HRESULT __stdcall Close(LPCVOID pData);
};


class Direct3D11Renderer : public Renderer 
{
public:

	Direct3D11Renderer(HWND hwnd, UINT height, UINT width, DXGI_FORMAT format, DXGI_FORMAT depthFormat, UINT msaaSamples, bool fullscreen);
	~Direct3D11Renderer();

	void resetToDefaults();
	void reset(const UINT flags = RESET_ALL);
	void setD3Ddefaults();

//	bool resetDevice();

	TextureID createTexture(ID3D11Resource *resource, UINT flags = 0);
    TextureID createTexture(char* rawData, FORMAT rawDataFormat, const int width, const int height, const int depth, const SamplerStateID samplerState = SS_NONE, UINT flags = 0);
	TextureID createTexture(Image &img, const SamplerStateID samplerState = SS_NONE, UINT flags = 0);

	TextureID addRenderTarget(const int width, const int height, const int depth, const int mipMapCount, const int arraySize, const FORMAT format, const int msaaSamples = 1, const SamplerStateID samplerState = SS_NONE, UINT flags = 0);
	TextureID addRenderDepth(const int width, const int height, const int arraySize, const FORMAT format, const int msaaSamples = 1, const SamplerStateID samplerState = SS_NONE, UINT flags = 0);

	bool resizeRenderTarget(const TextureID renderTarget, const int width, const int height, const int depth, const int mipMapCount, const int arraySize);
	bool generateMipMaps(const TextureID renderTarget);

	void removeTexture(const TextureID texture);
	void removeVertexBuffer(const VertexBufferID vertexBuffer);
	void removeIndexBuffer(const IndexBufferID indexBuffer);
	void removeStructuredBuffer(const StructuredBufferID structuredBuffer);

	ShaderID createShader(const char *vsText, const char *gsText, const char *fsText, const char *csText, const char* tcsText, const char* tesText, const int vsLine, const int gsLine, const int fsLine, const int csLine,
		const int tcsLine, const int tesLine, const char *header = NULL, const char *extra = NULL, const char *fileName = NULL, const char **attributeNames = NULL, const int nAttributes = 0, const UINT flags = 0);
		
	VertexFormatID addVertexFormat(const FormatDesc *formatDesc, const UINT nAttribs, const ShaderID shader = SHADER_NONE);
	VertexBufferID addVertexBuffer(const long size, const BufferAccess bufferAccess, const void *data = NULL);
	IndexBufferID addIndexBuffer(const UINT nIndices, const UINT indexSize, const BufferAccess bufferAccess, const void *data = NULL);
	ObjectBufferID createObjectBuffer(const ObjectBufferType type, const UINT objectSize, const UINT nObjects, const BufferAccess bufferAccess, const void *data = NULL);

	StructuredBufferID createStructuredBuffer(const UINT elementSize, const UINT nElements, const bool bIsCPUWritable, const bool bIsGPUWritable, const UINT UAVFlags = 0, const void *data = NULL);
	StructuredBufferID createIndirectDrawArgBuffer();

	SamplerStateID addSamplerState(const Filter filter, const AddressMode s, const AddressMode t, const AddressMode r, const float lod = 0, const UINT maxAniso = 16, const int compareFunc = 0, const float *border_color = NULL);
	BlendStateID addBlendState(const int srcFactorRGB, const int destFactorRGB, const int srcFactorAlpha, const int destFactorAlpha, const int blendModeRGB, const int blendModeAlpha, const int mask = ALL, const bool alphaToCoverage = false);
	DepthStateID addDepthState(const bool depthTest, const bool depthWrite, const int depthFunc, const bool stencilTest, const UINT8 stencilReadMask, const UINT8 stencilWriteMask,
		const int stencilFuncFront, const int stencilFuncBack, const int stencilFailFront, const int stencilFailBack,
		const int depthFailFront, const int depthFailBack, const int stencilPassFront, const int stencilPassBack);
	RasterizerStateID addRasterizerState(const int cullMode, const int fillMode = SOLID, const bool multiSample = true, const bool scissor = false, const float depthBias = 0.0f, const float slopeDepthBias = 0.0f);
	
	FontID addFont(const char *ttfFontFile, const UINT size, const SamplerStateID samplerState = -1);
	FontID addFont(char *rawBuffer, unsigned int rawSize, const UINT size, const SamplerStateID samplerState = -1);

	bool getShaderWorkGroupSize(const int shaderID, UINT* localGroupSize);

	void setTexture(const char *textureName, const TextureID texture);
	void setTexture(const int textureUnit, const TextureID texture); //Deprecated
	void setTextureArray(const int initIndex, const TextureID *textures, const int n_textures) { }; //Deprecated
	void setTextureArray(const char *textureName, const TextureID *textures, const int arraySize);
	void setTexture(const char *textureName, const TextureID texture, const SamplerStateID samplerState);
	void setTextureSlice(const char *textureName, const TextureID texture, const int slice);
	void applyTextures();

	void setImageBuffer(const char *ImageBufferName, const TextureID texture, const int mipLevel, const FORMAT formatImageBuffer, const ImageBufferAccess imageBufferAccess);
	void setImageBufferArray(const char *ImageBufferName, const TextureID *texture, const int *mipLevel, const FORMAT *formatImageBuffer, const ImageBufferAccess *imageBufferAccess, const UINT offset, const UINT arraySize);
	void applyImageBuffers() { };

	void setObjectBuffer(const char *objectBufferName, const ObjectBufferID objectBufferID) { };
	void setObjectBuffer(const int bindingSlot, const ObjectBufferID objectBufferID) { };
	void applyObjectBuffers() { };

	void setSamplerState(const char *samplerName, const SamplerStateID samplerState);
	void applySamplerStates();

	void setShaderConstantRaw(const char *name, const void *data, const int size);
	void applyConstants();

	void setStructuredBuffer(const char *structuredBufferName, const StructuredBufferID structuredBufferID);
	void setStructuredBufferArray(const char *structuredBufferName, const StructuredBufferID *structuredBufferID, int offset, int n_StructuredBuffers);
	void applyStructuredBuffers();
	void applyRWStructuredBuffers();

//	void changeTexture(const uint imageUnit, const TextureID textureID);
	void changeRenderTargets(const TextureID *colorRTs, const UINT nRenderTargets, const TextureID depthRT = TEXTURE_NONE, const int depthSlice = NO_SLICE, const int *slices = NULL, const UINT flags = DEFAULT_SIZE);
	void changeToMainFramebuffer();
	void changeShader(const ShaderID shaderID);
	void changeVertexFormat(const VertexFormatID vertexFormatID);
	void changeVertexBuffer(UINT StartSlot, UINT NumBuffers, const VertexBufferID* vertexBuffer, const UINT* offset = 0);
	void changeIndexBuffer(const IndexBufferID indexBufferID);
	void changeCullFace(const int cullFace);

//	void changeSamplerState(const uint samplerUnit, const SamplerStateID samplerState);
	void changeBlendState(const BlendStateID blendState, const UINT sampleMask = ~0);
	void changeDepthState(const DepthStateID depthState, const UINT stencilRef = 0);
	void changeRasterizerState(const RasterizerStateID rasterizerState);

	//Queries///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    QueryID createQuery();
    void beginQuery(QueryID id);
	void endQuery(QueryID id);
    bool queryResultAvailable(QueryID id);
	unsigned long long  getResultQuery(QueryID id);
	void deleteQueries();
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void clear(const bool clearColor, const bool clearDepth, const bool clearStencil, const float *color, const float depth, const UINT stencil);
	void clearRenderTarget(const TextureID renderTarget, const Color &color, const int slice = NO_SLICE);
	void clearDepthTarget(const TextureID depthTarget, const float depth = 1.0f, const int slice = NO_SLICE);

	bool hasTessellation(const ShaderID shaderID);

	void drawArrays(const Primitives primitives, const int firstVertex, const int nVertices);
	void drawElements(const Primitives primitives, const int firstIndex, const int nIndices, const int firstVertex, const int nVertices);
	void drawElementsInstancedIndirect(const Primitives primitive, StructuredBufferID bufferArgs, UINT offset);

	void setup2DMode(const float left, const float right, const float top, const float bottom);
	void drawLine(Vec3 vPoint1, Vec3 vPoint2, Mat4x4 mView, Mat4x4 mProj, const BlendStateID blendState, const DepthStateID depthState, const Vec4 *color = NULL);
	void drawPlain(const Primitives primitives, Vec2 *vertices, const UINT nVertices, const BlendStateID blendState, const DepthStateID depthState, const Vec4 *color = NULL);
	void drawTextured(const Primitives primitives, TexVertex *vertices, const UINT nVertices, const TextureID texture, const SamplerStateID samplerState, const BlendStateID blendState, const DepthStateID depthState, const Vec4 *color = NULL);

	void endFrame();

	void dispatchCompute(const UINT num_groups_x, const UINT num_groups_y, const UINT num_groups_z);
	void memoryBarrier(UINT flags) { }

	void resetAtomicCounter(ObjectBufferID atomicCounterBuffer, UINT value) { }

	void setFrameBuffer(ID3D11RenderTargetView *colorRTV, ID3D11DepthStencilView *depthDSV)
	{
		backBufferRTV  = colorRTV;
		depthBufferDSV = depthDSV;
	}

	ID3D11Resource *getResource(const TextureID texture) const;

	void flush();
	void finish();

	//Map/UnMap Buffer
	void* mapBuffer(const StructuredBufferID objectBuffer, const MapMode mapMode);
	void unMapBuffer(const StructuredBufferID objectBuffer);
	void* mapVertexBuffer(const VertexBufferID objectBuffer, const MapMode mapMode);
	void unMapVertexBuffer(const VertexBufferID objectBuffer);

protected:
	ID3D11ShaderResourceView *createSRV(ID3D11Resource *resource, DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN, const int firstSlice = -1, const int sliceCount = -1);
	ID3D11RenderTargetView   *createRTV(ID3D11Resource *resource, DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN, const int firstSlice = -1, const int sliceCount = -1);
	ID3D11DepthStencilView   *createDSV(ID3D11Resource *resource, DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN, const int firstSlice = -1, const int sliceCount = -1);
	ID3D11UnorderedAccessView **createUAV(ID3D11Resource *resource, DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN, const int firstSlice = -1, const int sliceCount = -1);

	ID3D11Texture2D *backBuffer;
	ID3D11Texture2D *depthBuffer;
	ID3D11RenderTargetView *backBufferRTV;
	ID3D11DepthStencilView *depthBufferDSV;

	BufferResourceEntry currentTexturesVS[MAX_TEXTUREUNIT], selectedTexturesVS[MAX_TEXTUREUNIT];
	BufferResourceEntry currentTexturesGS[MAX_TEXTUREUNIT], selectedTexturesGS[MAX_TEXTUREUNIT];
	BufferResourceEntry currentTexturesTCS[MAX_TEXTUREUNIT], selectedTexturesTCS[MAX_TEXTUREUNIT];
	BufferResourceEntry currentTexturesTES[MAX_TEXTUREUNIT], selectedTexturesTES[MAX_TEXTUREUNIT];
	BufferResourceEntry currentTexturesPS[MAX_TEXTUREUNIT], selectedTexturesPS[MAX_TEXTUREUNIT];
    BufferResourceEntry currentTexturesCS[MAX_TEXTUREUNIT], selectedTexturesCS[MAX_TEXTUREUNIT];

	int currentTextureSlicesVS[MAX_TEXTUREUNIT], selectedTextureSlicesVS[MAX_TEXTUREUNIT];
	int currentTextureSlicesGS[MAX_TEXTUREUNIT], selectedTextureSlicesGS[MAX_TEXTUREUNIT];
	int currentTextureSlicesTCS[MAX_TEXTUREUNIT], selectedTextureSlicesTCS[MAX_TEXTUREUNIT];
	int currentTextureSlicesTES[MAX_TEXTUREUNIT], selectedTextureSlicesTES[MAX_TEXTUREUNIT];
	int currentTextureSlicesPS[MAX_TEXTUREUNIT], selectedTextureSlicesPS[MAX_TEXTUREUNIT];
    int currentTextureSlicesCS[MAX_TEXTUREUNIT], selectedTextureSlicesCS[MAX_TEXTUREUNIT];

	SamplerStateID currentSamplerStatesVS[MAX_SAMPLERSTATE], selectedSamplerStatesVS[MAX_SAMPLERSTATE];
	SamplerStateID currentSamplerStatesGS[MAX_SAMPLERSTATE], selectedSamplerStatesGS[MAX_SAMPLERSTATE];
	SamplerStateID currentSamplerStatesTCS[MAX_SAMPLERSTATE], selectedSamplerStatesTCS[MAX_SAMPLERSTATE];
	SamplerStateID currentSamplerStatesTES[MAX_SAMPLERSTATE], selectedSamplerStatesTES[MAX_SAMPLERSTATE];
	SamplerStateID currentSamplerStatesPS[MAX_SAMPLERSTATE], selectedSamplerStatesPS[MAX_SAMPLERSTATE];
    SamplerStateID currentSamplerStatesCS[MAX_SAMPLERSTATE], selectedSamplerStatesCS[MAX_SAMPLERSTATE];

	ImageSlot currentRWStructuredBufferVS[MAX_RWSTRUCTUREBUFFERUNIT], selectedRWStructuredBufferVS[MAX_RWSTRUCTUREBUFFERUNIT];
	ImageSlot currentRWStructuredBufferGS[MAX_RWSTRUCTUREBUFFERUNIT], selectedRWStructuredBufferGS[MAX_RWSTRUCTUREBUFFERUNIT];
	ImageSlot currentRWStructuredBufferPS[MAX_RWSTRUCTUREBUFFERUNIT], selectedRWStructuredBufferPS[MAX_RWSTRUCTUREBUFFERUNIT];
	ImageSlot currentRWStructuredBufferCS[MAX_RWSTRUCTUREBUFFERUNIT], selectedRWStructuredBufferCS[MAX_RWSTRUCTUREBUFFERUNIT];

private:
	bool fillSRV(ID3D11ShaderResourceView **dest, int &min, int &max, const BufferResourceEntry selectedTextures[], BufferResourceEntry currentTextures[], const TextureID selectedTextureSlices[], TextureID currentTextureSlices[], const Texture *textures, const StructuredBuffer *structuredBuffers);
	bool fillSS(ID3D11SamplerState **dest, int &min, int &max, const SamplerStateID selectedSamplerStates[], SamplerStateID currentSamplerStates[], const SamplerState *samplerStates);
	bool fillSB(ID3D11ShaderResourceView **dest, int &min, int &max, const StructuredBufferID selectedStructuredBuffer[], StructuredBufferID currentStructuredBuffer[], const StructuredBuffer *structuredBuffers);
	bool fillRSB(ID3D11UnorderedAccessView **dest, int &min, int &max, const ImageSlot selectedRWStructuredBuffer[], ImageSlot currentRWStructuredBuffer[], const StructuredBuffer *structuredBuffers, const Texture *textures);

    void resetPixelUAV();

	ubyte *mapRollingVB(const UINT size);
	void unmapRollingVB(const UINT size);
	UINT copyToRollingVB(const void *src, const UINT size);

	VertexBufferID rollingVB;
	int rollingVBOffset;

	RasterizerStateID defaultRasterizeState;
	ShaderID plainShader, texShader, lineShader;
	VertexFormatID plainVF, texVF, lineVF;

	UINT m_uNumRenderTargets;

	Vec4 scaleBias2D;

	ID3D11Query *eventQuery;

	//DirectX11 device member
	D3D_DRIVER_TYPE m_driverType;
	D3D_FEATURE_LEVEL m_featureLevel;
	ID3D11Device* m_pd3dDevice;
	ID3D11DeviceContext* m_pImmediateContext;
	IDXGISwapChain* m_pSwapChain;

	CShaderInclude* m_pShaderInclude;
};

#endif // _DIRECT3D11RENDERER_H_
