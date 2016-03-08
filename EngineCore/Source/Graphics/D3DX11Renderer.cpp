#define INITGUID

#include "CthughaStd.h"
#include "D3DX11Renderer.h"

#include "Shader.h"

#pragma warning (disable : 4996)

template <typename T, size_t N> char (&ArraySizeHelper(T (&arr)[N]))[N];
#define elementsOf(arr) (sizeof(ArraySizeHelper(arr)))

struct Constant 
{
	char *name;
	ubyte *vsData;
	ubyte *gsData;
	ubyte *tcsData;
	ubyte *tesData;
	ubyte *psData;
	ubyte *csData;
	int vsBuffer;
	int gsBuffer;
	int tcsBuffer;
	int tesBuffer;
	int psBuffer;
	int csBuffer;
};

int constantComp(const void *s0, const void *s1)
{
	return strcmp(((Constant *) s0)->name, ((Constant *) s1)->name);
}

struct Sampler 
{
	char *name;
	int vsIndex;
	int gsIndex;
	int tcsIndex;
	int tesIndex;
	int psIndex;
	int csIndex;
};

struct ImageBufferSlot
{

};

struct ObjectBuffer
{

};

struct Query
{
	ID3D11Query *ppQuery;
	unsigned long long result;
};

struct StructuredBuffer
{
	ID3D11Buffer* structuredBuffer;
	ID3D11ShaderResourceView* structuredBufferView;
	ID3D11UnorderedAccessView* structuredBufferViewRW;
	UINT flags;
	UINT sizePerElement;
	UINT size;
	D3D11_USAGE bufferUsage;
};

class Shader
{
public:

	ID3D11VertexShader *vertexShader;
	ID3D11PixelShader *pixelShader;
	ID3D11GeometryShader *geometryShader;
	ID3D11HullShader *hullShader;
	ID3D11DomainShader *domainShader;
	ID3D11ComputeShader *computeShader;
	ID3DBlob *inputSignature;

	ID3D11Buffer **vsConstants;
	ID3D11Buffer **gsConstants;
	ID3D11Buffer **hsConstants;
	ID3D11Buffer **dsConstants;
	ID3D11Buffer **psConstants;
	ID3D11Buffer **csConstants;
	UINT nVSCBuffers;
	UINT nGSCBuffers;
	UINT nHSCBuffers;
	UINT nDSCBuffers;
	UINT nPSCBuffers;
	UINT nCSCBuffers;

	Constant *constants;
	Sampler *textures;
	Sampler *samplers;
	Sampler *rwStructuredBuffer;
	Sampler *structuredBuffer;
	Sampler *uavTextures;

	UINT nConstants;
	UINT nTextures;
	UINT nSamplers;
	UINT nRWStructuredBuffers;
	UINT nStructuredBuffers;
	UINT nUavTextures;

	ubyte **vsConstMem;
	ubyte **gsConstMem;
	ubyte **hsConstMem;
	ubyte **dsConstMem;
	ubyte **psConstMem;
	ubyte **csConstMem;

	bool *vsDirty;
	bool *gsDirty;
	bool *hsDirty;
	bool *dsDirty;
	bool *psDirty;
	bool *csDirty;

	Shader()
	{
		vertexShader = NULL;
		pixelShader = NULL;
		geometryShader = NULL;
		hullShader = NULL;
		domainShader = NULL;
		computeShader = NULL;
	}
};

class Texture 
{
private:
	ID3D11Resource *texture;
	ID3D11ShaderResourceView *srv;
	ID3D11RenderTargetView   *rtv;
	ID3D11DepthStencilView   *dsv;
    ID3D11UnorderedAccessView **uav; //must be one uav for each mipmap levels, there aren´t uav support for texture Arrays
	ID3D11ShaderResourceView **srvArray;
	ID3D11RenderTargetView   **rtvArray;
	ID3D11DepthStencilView   **dsvArray;
	DXGI_FORMAT texFormat;
	DXGI_FORMAT srvFormat;
	DXGI_FORMAT rtvFormat;
	DXGI_FORMAT dsvFormat;
	int width, height, depth;
	UINT nMipMaps;
	int arraySize;	
	bool mod;

	//string name;

public:
	UINT flags;
	friend class Direct3D11Renderer;

	Texture()
	{
		texture = NULL;
		srv = NULL;
		rtv = NULL;
		dsv = NULL;
		srvArray = NULL;
		rtvArray = NULL;
		dsvArray = NULL;
		
		width = 0; height = 0; depth = 0;
		arraySize = 0;
		flags = 0;
	}

	~Texture()
	{
	}
};

int samplerComp(const void *s0, const void *s1)
{
	return strcmp(((Sampler *) s0)->name, ((Sampler *) s1)->name);
}

struct TTFFont
{
	//TTF_Font* pfont;
	SamplerStateID sampleState;
	UINT size;
};

struct VertexFormat 
{
	ID3D11InputLayout *inputLayout;
	UINT vertexSize[MAX_VERTEXSTREAM];
};

struct VertexBuffer 
{
	ID3D11Buffer *vertexBuffer;
	long size;
};

struct IndexBuffer 
{
	ID3D11Buffer *indexBuffer;
	UINT nIndices;
	UINT indexSize;
};

struct SamplerState 
{
	ID3D11SamplerState *samplerState;
};

struct BlendState 
{
	ID3D11BlendState *blendState;
};

struct DepthState 
{
	ID3D11DepthStencilState *dsState;
};

struct RasterizerState 
{
	ID3D11RasterizerState *rsState;
};

// Blending constants
const int ZERO                = D3D11_BLEND_ZERO;
const int ONE                 = D3D11_BLEND_ONE;
const int SRC_COLOR           = D3D11_BLEND_SRC_COLOR;
const int ONE_MINUS_SRC_COLOR = D3D11_BLEND_INV_SRC_COLOR;
const int DST_COLOR           = D3D11_BLEND_DEST_COLOR;
const int ONE_MINUS_DST_COLOR = D3D11_BLEND_INV_DEST_COLOR;
const int SRC_ALPHA           = D3D11_BLEND_SRC_ALPHA;
const int ONE_MINUS_SRC_ALPHA = D3D11_BLEND_INV_SRC_ALPHA;
const int DST_ALPHA           = D3D11_BLEND_DEST_ALPHA;
const int ONE_MINUS_DST_ALPHA = D3D11_BLEND_INV_DEST_ALPHA;
const int SRC_ALPHA_SATURATE  = D3D11_BLEND_SRC_ALPHA_SAT;

const int BM_ADD              = D3D11_BLEND_OP_ADD;
const int BM_SUBTRACT         = D3D11_BLEND_OP_SUBTRACT;
const int BM_REVERSE_SUBTRACT = D3D11_BLEND_OP_REV_SUBTRACT;
const int BM_MIN              = D3D11_BLEND_OP_MIN;
const int BM_MAX              = D3D11_BLEND_OP_MAX;

// Depth-test constants
const int NEVER    = D3D11_COMPARISON_NEVER;
const int LESS     = D3D11_COMPARISON_LESS;
const int EQUAL    = D3D11_COMPARISON_EQUAL;
const int LEQUAL   = D3D11_COMPARISON_LESS_EQUAL;
const int GREATER  = D3D11_COMPARISON_GREATER;
const int NOTEQUAL = D3D11_COMPARISON_NOT_EQUAL;
const int GEQUAL   = D3D11_COMPARISON_GREATER_EQUAL;
const int ALWAYS   = D3D11_COMPARISON_ALWAYS;

// Stencil-test constants
const int KEEP     = D3D11_STENCIL_OP_KEEP;
const int SET_ZERO = D3D11_STENCIL_OP_ZERO;
const int REPLACE  = D3D11_STENCIL_OP_REPLACE;
const int INVERT   = D3D11_STENCIL_OP_INVERT;
const int INCR     = D3D11_STENCIL_OP_INCR;
const int DECR     = D3D11_STENCIL_OP_DECR;
const int INCR_SAT = D3D11_STENCIL_OP_INCR_SAT;
const int DECR_SAT = D3D11_STENCIL_OP_DECR_SAT;

// Culling constants
const int CULL_NONE  = D3D11_CULL_NONE;
const int CULL_BACK  = D3D11_CULL_BACK;
const int CULL_FRONT = D3D11_CULL_FRONT;

// Fillmode constants
const int SOLID = D3D11_FILL_SOLID;
const int WIREFRAME = D3D11_FILL_WIREFRAME;

D3D11_MAP D3D11MapModes[] = { D3D11_MAP_READ, D3D11_MAP_WRITE, D3D11_MAP_READ_WRITE, D3D11_MAP_WRITE_DISCARD, D3D11_MAP_WRITE_NO_OVERWRITE };

HRESULT __stdcall CShaderInclude::Open(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes) {
	try 
	{
		std::string finalPath;
		switch (IncludeType) 
		{
		case D3D_INCLUDE_LOCAL:
			finalPath = pFileName;
				break;
		default:
			assert(0);
		}

		Resource IncludeShaderResource(finalPath);
		shared_ptr<ResHandle> IncludeShaderHandle = g_pApp->m_ResCache->GetHandle(&IncludeShaderResource);

		if (!IncludeShaderHandle)
		{
			CHG_ERROR("Not Found File: " + finalPath);
			return E_FAIL;
		}

		*ppData = ((ShaderLibraryResourceExtraData*)IncludeShaderHandle->GetExtra().get())->GetShaderLibraryData();
		*pBytes = IncludeShaderHandle->Size();
		
		return S_OK;
	}
	catch (std::exception& e) 
	{
		CHG_ERROR(e.what());

		return E_FAIL;
	}
}

HRESULT __stdcall CShaderInclude::Close(LPCVOID pData) 
{
	//char* buf = (char*)pData;
	//delete[] buf;
	return S_OK;
}


Direct3D11Renderer::Direct3D11Renderer(HWND hwnd, UINT height, UINT width, DXGI_FORMAT format, DXGI_FORMAT depthBufferFormat, UINT msaaSamples, bool fullscreen) : Renderer()
{
	HRESULT hr = S_OK;
	m_driverType = D3D_DRIVER_TYPE_NULL;
	m_featureLevel = D3D_FEATURE_LEVEL_11_0;

	UINT createDeviceFlags = 0;
#ifdef _DEBUG
    //createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_DRIVER_TYPE driverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };
    UINT numDriverTypes = ARRAYSIZE( driverTypes );

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };
	UINT numFeatureLevels = ARRAYSIZE( featureLevels );

    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory( &sd, sizeof( sd ) );
    sd.BufferCount = 1;
    sd.BufferDesc.Width = width;
    sd.BufferDesc.Height = height;
    sd.BufferDesc.Format = format;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hwnd;
    sd.SampleDesc.Count = msaaSamples;
    sd.SampleDesc.Quality = 0;
	sd.Windowed = !fullscreen;

    for( UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++ )
    {
        m_driverType = driverTypes[driverTypeIndex];
        hr = D3D11CreateDeviceAndSwapChain( NULL, m_driverType, NULL, createDeviceFlags, featureLevels, numFeatureLevels,
                                            D3D11_SDK_VERSION, &sd, &m_pSwapChain, &m_pd3dDevice, &m_featureLevel, &m_pImmediateContext );
        if( SUCCEEDED( hr ) )
            break;
    }
    if( FAILED( hr ) )
	{
        printf("Create DirectX 11 Renderer failed. Try to use other Renderer");
		return;
	}

	eventQuery = NULL;

	nImageUnits = 16;
	maxAnisotropic = 16;
	//Create DirectX 11 Device

//	textureLod = CHG_NEW float[nImageUnits];

	nMRTs = 8;
	if (nMRTs > MAX_MRTS) nMRTs = MAX_MRTS;

	numVertexBufferUsed = 0;
	plainShader = SHADER_NONE;
	plainVF = VF_NONE;
	texShader = SHADER_NONE;
	texVF = VF_NONE;
	lineShader = SHADER_NONE;
	lineVF = VF_NONE;
	rollingVB = VB_NONE;
	rollingVBOffset = 0;

	backBufferRTV = NULL;
	depthBufferDSV = NULL;

	setD3Ddefaults();
	resetToDefaults();

	//Create main depthStencilBuffer and renderTargetBuffer

	m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID *) &backBuffer);
	if (FAILED(m_pd3dDevice->CreateRenderTargetView(backBuffer, NULL, &backBufferRTV)))
	{
		CHG_ERROR("Create renderTargetBuffer error");
	}

	/*if (sampleBackBuffer)
	{
		if ((backBufferTexture = ((Direct3D10Renderer *) renderer)->addTexture(backBuffer)) == TEXTURE_NONE) return false;
		backBuffer->AddRef();
	}*/


	if (depthBufferFormat != DXGI_FORMAT_UNKNOWN)
	{
		// Create depth stencil texture
		D3D11_TEXTURE2D_DESC descDepth;
		descDepth.Width  = width;
		descDepth.Height = height;
		descDepth.MipLevels = 1;
		descDepth.ArraySize = 1;
		descDepth.Format = depthBufferFormat;
		descDepth.SampleDesc.Count = msaaSamples;
		descDepth.SampleDesc.Quality = 0;
		descDepth.Usage = D3D11_USAGE_DEFAULT;
		descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		descDepth.CPUAccessFlags = 0;
		descDepth.MiscFlags = 0;
		if (FAILED(m_pd3dDevice->CreateTexture2D(&descDepth, NULL, &depthBuffer)))
		{
			CHG_ERROR("Create depth Buffer error");
		}

		// Create the depth stencil view
		D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
		ZeroMemory( &descDSV, sizeof(descDSV) );
		descDSV.Format = descDepth.Format;
		descDSV.ViewDimension = msaaSamples > 1? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
		descDSV.Texture2D.MipSlice = 0;
		if (FAILED(m_pd3dDevice->CreateDepthStencilView(depthBuffer, &descDSV, &depthBufferDSV)))
		{
			CHG_ERROR("Create depth view error");
		}
	}

	m_pImmediateContext->OMSetRenderTargets(1, &backBufferRTV, depthBufferDSV);
	m_uNumRenderTargets = 1;

	// Setup the viewport
	D3D11_VIEWPORT viewport;
	viewport.Width  = (float)width;
	viewport.Height = (float)height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	m_pImmediateContext->RSSetViewports(1, &viewport);

	defaultRasterizeState = addRasterizerState(CULL_NONE);

	m_pShaderInclude = CHG_NEW CShaderInclude();

	//this->setFrameBuffer(backBufferRTV, depthBufferDSV);
}

Direct3D11Renderer::~Direct3D11Renderer()
{
	if (eventQuery) eventQuery->Release();

/*
	releaseFrameBufferSurfaces();
*/

	// Delete shaders
	for (UINT i = 0; i < shaders.size(); i++)
	{
		if (shaders[i].vertexShader  ) shaders[i].vertexShader->Release();
		if (shaders[i].geometryShader) shaders[i].geometryShader->Release();
		if (shaders[i].hullShader	 ) shaders[i].hullShader->Release();
		if (shaders[i].domainShader	 ) shaders[i].domainShader->Release();
		if (shaders[i].pixelShader   ) shaders[i].pixelShader->Release();
		if (shaders[i].computeShader ) shaders[i].computeShader->Release();
		if (shaders[i].inputSignature) shaders[i].inputSignature->Release();

		for (UINT k = 0; k < shaders[i].nVSCBuffers; k++)
		{
			shaders[i].vsConstants[k]->Release();
			delete shaders[i].vsConstMem[k];
		}
		for (UINT k = 0; k < shaders[i].nGSCBuffers; k++)
		{
			shaders[i].gsConstants[k]->Release();
			delete shaders[i].gsConstMem[k];
		}
		for (UINT k = 0; k < shaders[i].nHSCBuffers; k++)
		{
			shaders[i].hsConstants[k]->Release();
			delete shaders[i].hsConstMem[k];
		}
		for (UINT k = 0; k < shaders[i].nDSCBuffers; k++)
		{
			shaders[i].dsConstants[k]->Release();
			delete shaders[i].dsConstMem[k];
		}
		for (UINT k = 0; k < shaders[i].nPSCBuffers; k++)
		{
			shaders[i].psConstants[k]->Release();
			delete shaders[i].psConstMem[k];
		}
		for (UINT k = 0; k < shaders[i].nCSCBuffers; k++)
		{
			shaders[i].csConstants[k]->Release();
			delete shaders[i].csConstMem[k];
		}
		delete shaders[i].vsConstants;
		delete shaders[i].gsConstants;
		delete shaders[i].hsConstants;
		delete shaders[i].dsConstants;
		delete shaders[i].psConstants;
		delete shaders[i].csConstants;
		delete shaders[i].vsConstMem;
		delete shaders[i].gsConstMem;
		delete shaders[i].hsConstMem;
		delete shaders[i].dsConstMem;
		delete shaders[i].psConstMem;
		delete shaders[i].csConstMem;

		for (UINT k = 0; k < shaders[i].nConstants; k++)
		{
			delete shaders[i].constants[k].name;
		}
		delete shaders[i].constants;

		for (UINT k = 0; k < shaders[i].nTextures; k++)
		{
			delete shaders[i].textures[k].name;
		}
		delete shaders[i].textures;

		for (UINT k = 0; k < shaders[i].nUavTextures; k++)
		{
			delete shaders[i].uavTextures[k].name;
		}
		delete shaders[i].uavTextures;

		for (UINT k = 0; k < shaders[i].nSamplers; k++)
		{
			delete shaders[i].samplers[k].name;
		}
		delete shaders[i].samplers;

		for (UINT k = 0; k < shaders[i].nRWStructuredBuffers; k++)
		{
			delete shaders[i].rwStructuredBuffer[k].name;
		}
		delete shaders[i].rwStructuredBuffer;

		for (UINT k = 0; k < shaders[i].nStructuredBuffers; k++)
		{
			delete shaders[i].structuredBuffer[k].name;
		}
		delete shaders[i].structuredBuffer;

		delete shaders[i].vsDirty;
		delete shaders[i].gsDirty;
		delete shaders[i].hsDirty;
		delete shaders[i].dsDirty;
		delete shaders[i].psDirty;
		delete shaders[i].csDirty;
	}

    // Delete vertex formats
	for (UINT i = 0; i < vertexFormats.size(); i++)
	{
		if (vertexFormats[i].inputLayout) vertexFormats[i].inputLayout->Release();
	}

    // Delete vertex buffers
	for (UINT i = 0; i < vertexBuffers.size(); i++)
	{
		if (vertexBuffers[i].vertexBuffer) vertexBuffers[i].vertexBuffer->Release();
	}

	// Delete index buffers
	for (UINT i = 0; i < indexBuffers.size(); i++)
	{
		if (indexBuffers[i].indexBuffer) indexBuffers[i].indexBuffer->Release();
	}

	// Delete samplerstates
	for (UINT i = 0; i < samplerStates.size(); i++)
	{
		if (samplerStates[i].samplerState) samplerStates[i].samplerState->Release();
	}

	// Delete blendstates
	for (UINT i = 0; i < blendStates.size(); i++)
	{
		if (blendStates[i].blendState) blendStates[i].blendState->Release();
	}

	// Delete depthstates
	for (UINT i = 0; i < depthStates.size(); i++)
	{
		if (depthStates[i].dsState) depthStates[i].dsState->Release();
	}

	// Delete rasterizerstates
	for (UINT i = 0; i < rasterizerStates.size(); i++)
	{
		if (rasterizerStates[i].rsState) rasterizerStates[i].rsState->Release();
	}

	// Delete textures
	for (UINT i = 0; i < textures.size(); i++)
	{
		removeTexture(i);
	}

	// Delete Structured Buffers
	for (UINT i = 0; i < structuredBuffers.size(); i++)
	{
		removeStructuredBuffer(i);
	}

	SAFE_DELETE(m_pShaderInclude);

//	if (rollingVB) rollingVB->Release();
}

void Direct3D11Renderer::reset(const UINT flags)
{
	Renderer::reset(flags);

	if (flags & RESET_TEX)
	{
		for (UINT i = 0; i < MAX_TEXTUREUNIT; i++)
		{
			selectedTexturesVS[i].id = TEXTURE_NONE;
			selectedTexturesGS[i].id = TEXTURE_NONE;
			selectedTexturesTCS[i].id = TEXTURE_NONE;
			selectedTexturesTES[i].id = TEXTURE_NONE;
			selectedTexturesPS[i].id = TEXTURE_NONE;
			selectedTexturesCS[i].id = TEXTURE_NONE;
			selectedTexturesVS[i].type = TEXTURE_RESOURCE;
			selectedTexturesGS[i].type = TEXTURE_RESOURCE;
			selectedTexturesTCS[i].type = TEXTURE_RESOURCE;
			selectedTexturesTES[i].type = TEXTURE_RESOURCE;
			selectedTexturesPS[i].type = TEXTURE_RESOURCE;
			selectedTexturesCS[i].type = TEXTURE_RESOURCE;
			selectedTextureSlicesVS[i] = NO_SLICE;
			selectedTextureSlicesGS[i] = NO_SLICE;
			selectedTextureSlicesTCS[i] = NO_SLICE;
			selectedTextureSlicesTES[i] = NO_SLICE;
			selectedTextureSlicesPS[i] = NO_SLICE;
			selectedTextureSlicesCS[i] = NO_SLICE;
		}
	}

	if (flags & RESET_SS)
	{
		for (UINT i = 0; i < MAX_SAMPLERSTATE; i++)
		{
			selectedSamplerStatesVS[i] = SS_NONE;
			selectedSamplerStatesGS[i] = SS_NONE;
			selectedSamplerStatesTCS[i] = SS_NONE;
			selectedSamplerStatesTES[i] = SS_NONE;
			selectedSamplerStatesPS[i] = SS_NONE;
			selectedSamplerStatesCS[i] = SS_NONE;
		}
	}

	if (flags & RESET_RWSTRUCTUREDBUFFER)
	{
		for (UINT i = 0; i < MAX_RWSTRUCTUREBUFFERUNIT; i++)
		{
			selectedRWStructuredBufferVS[i].id = STB_NONE;
			selectedRWStructuredBufferVS[i].mipLevel = -1;
			selectedRWStructuredBufferGS[i].id = STB_NONE;
			selectedRWStructuredBufferGS[i].mipLevel = -1;
			selectedRWStructuredBufferPS[i].id = STB_NONE;
			selectedRWStructuredBufferPS[i].mipLevel = -1;
			selectedRWStructuredBufferCS[i].id = STB_NONE;
			selectedRWStructuredBufferCS[i].mipLevel = -1;
		}
	}

	apply();
}

void Direct3D11Renderer::resetToDefaults()
{
	Renderer::resetToDefaults();

	for (UINT i = 0; i < MAX_TEXTUREUNIT; i++)
	{
		currentTexturesVS[i].id = TEXTURE_NONE;
		currentTexturesGS[i].id = TEXTURE_NONE;
		currentTexturesTCS[i].id = TEXTURE_NONE;
		currentTexturesTES[i].id = TEXTURE_NONE;
		currentTexturesPS[i].id = TEXTURE_NONE;
        currentTexturesCS[i].id = TEXTURE_NONE;
        currentTexturesVS[i].type = TEXTURE_RESOURCE;
		currentTexturesGS[i].type = TEXTURE_RESOURCE;
		currentTexturesTCS[i].type = TEXTURE_RESOURCE;
		currentTexturesTES[i].type = TEXTURE_RESOURCE;
		currentTexturesPS[i].type = TEXTURE_RESOURCE;
        currentTexturesCS[i].type = TEXTURE_RESOURCE;
		currentTextureSlicesVS[i] = NO_SLICE;
		currentTextureSlicesGS[i] = NO_SLICE;
		currentTextureSlicesTCS[i] = NO_SLICE;
		currentTextureSlicesTES[i] = NO_SLICE;
		currentTextureSlicesPS[i] = NO_SLICE;
        currentTextureSlicesCS[i] = NO_SLICE;
	}

	for (UINT i = 0; i < MAX_SAMPLERSTATE; i++)
	{
		currentSamplerStatesVS[i] = SS_NONE;
		currentSamplerStatesGS[i] = SS_NONE;
		currentSamplerStatesTCS[i] = SS_NONE;
		currentSamplerStatesTES[i] = SS_NONE;
		currentSamplerStatesPS[i] = SS_NONE;
        currentSamplerStatesCS[i] = SS_NONE;
	}

	for (UINT i = 0; i < MAX_RWSTRUCTUREBUFFERUNIT; i++)
	{
		currentRWStructuredBufferVS[i].id = STB_NONE;
		currentRWStructuredBufferVS[i].mipLevel = -1;
		currentRWStructuredBufferGS[i].id = STB_NONE;
		currentRWStructuredBufferGS[i].mipLevel = -1;
		currentRWStructuredBufferPS[i].id = STB_NONE;
		currentRWStructuredBufferPS[i].mipLevel = -1;
		currentRWStructuredBufferCS[i].id = STB_NONE;
		currentRWStructuredBufferCS[i].mipLevel = -1;
	}

	// TODO: Fix ...
	currentRasterizerState = -2;


/*
	currentDepthRT = FB_DEPTH;
*/
}

void Direct3D11Renderer::setD3Ddefaults()
{
/*
	// Set some of my preferred defaults
	dev->SetRenderState(D3DRS_LIGHTING, FALSE);
	dev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	dev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

	if (maxAnisotropic > 1){
		for (uint i = 0; i < nImageUnits; i++){
			dev->SetSamplerState(i, D3DSAMP_MAXANISOTROPY, maxAnisotropic);
		}
	}
*/
}

/*bool createRenderTarget(LPDIRECT3DDEVICE9 dev, Texture &tex){
	if (isDepthFormat(tex.format)){
		if (tex.surfaces == NULL) tex.surfaces = CHG_NEW LPDIRECT3DSURFACE9;
		if (dev->CreateDepthStencilSurface(tex.width, tex.height, formats[tex.format], D3DMULTISAMPLE_NONE, 0, FALSE, tex.surfaces, NULL) != D3D_OK){
			delete tex.surfaces;

			ErrorMsg("Couldn't create depth surface");
			return false;
		}
	} else {
		bool mipMapped = (tex.filter.mipFilter != D3DTEXF_NONE);

		if (tex.flags & CUBEMAP){
			if (dev->CreateCubeTexture(tex.width, mipMapped? 0 : 1, tex.usage, formats[tex.format], D3DPOOL_DEFAULT, (LPDIRECT3DCUBETEXTURE9 *) &tex.texture, NULL) != D3D_OK){
				ErrorMsg("Couldn't create render target");
				return false;
			}

			if (tex.surfaces == NULL) tex.surfaces = CHG_NEW LPDIRECT3DSURFACE9[6];
			for (uint i = 0; i < 6; i++){
				((LPDIRECT3DCUBETEXTURE9) tex.texture)->GetCubeMapSurface((D3DCUBEMAP_FACES) i, 0, &tex.surfaces[i]);
			}
		} else {
			if (dev->CreateTexture(tex.width, tex.height, mipMapped? 0 : 1, tex.usage, formats[tex.format], D3DPOOL_DEFAULT, (LPDIRECT3DTEXTURE9 *) &tex.texture, NULL) != D3D_OK){
				ErrorMsg("Couldn't create render target");
				return false;
			}
			if (tex.surfaces == NULL) tex.surfaces = CHG_NEW LPDIRECT3DSURFACE9;
			((LPDIRECT3DTEXTURE9) tex.texture)->GetSurfaceLevel(0, tex.surfaces);
		}
	}

	return true;
}
*/

/*bool Direct3D10Renderer::resetDevice(){
	for (uint i = 0; i < textures.getCount(); i++){
		if (textures[i].surfaces){
			int n = (textures[i].flags & CUBEMAP)? 6 : 1;

			if (textures[i].texture) textures[i].texture->Release();
			for (int k = 0; k < n; k++){
				textures[i].surfaces[k]->Release();
			}
		}
	}

	if (!releaseFrameBufferSurfaces()) return false;

	if (dev->Reset(&d3dpp) != D3D_OK){
		ErrorMsg("Device reset failed");
		return false;
	}

	if (!createFrameBufferSurfaces()) return false;

	resetToDefaults();
	setD3Ddefaults();

	for (uint i = 0; i < textures.getCount(); i++){
		if (textures[i].surfaces){
			createRenderTarget(dev, textures[i]);
		}
	}

	return true;
}*/

TextureID Direct3D11Renderer::createTexture(ID3D11Resource *resource, UINT flags)
{
	Texture tex;
	memset(&tex, 0, sizeof(tex));

	tex.texture = resource;
	tex.srv = createSRV(resource);
	tex.flags = flags;

	D3D11_RESOURCE_DIMENSION type;
	resource->GetType(&type);

	switch (type)
	{
		case D3D11_RESOURCE_DIMENSION_TEXTURE1D:
			D3D11_TEXTURE1D_DESC desc1d;
			((ID3D11Texture1D *) resource)->GetDesc(&desc1d);

			tex.width  = desc1d.Width;
			tex.height = 1;
			tex.depth  = 1;
			break;

		case D3D11_RESOURCE_DIMENSION_TEXTURE2D:
			D3D11_TEXTURE2D_DESC desc2d;
			((ID3D11Texture2D *) resource)->GetDesc(&desc2d);

			tex.width  = desc2d.Width;
			tex.height = desc2d.Height;
			tex.depth  = 1;
			break;

		case D3D11_RESOURCE_DIMENSION_TEXTURE3D:
			D3D11_TEXTURE3D_DESC desc3d;
			((ID3D11Texture3D *) resource)->GetDesc(&desc3d);

			tex.width  = desc3d.Width;
			tex.height = desc3d.Height;
			tex.depth  = desc3d.Depth;
			break;
	}
	
	return textures.add(tex);;
}

TextureID Direct3D11Renderer::createTexture(char* rawData, FORMAT rawDataFormat, const int width, const int height, const int depth, const SamplerStateID samplerState, UINT flags)
{
    Texture tex;
    memset(&tex, 0, sizeof(tex));

    FORMAT format = rawDataFormat;
    UINT nMipMaps = 1;
    UINT nSlices = 1;
    UINT arraySize = 1;

    static D3D11_SUBRESOURCE_DATA texData[1024];
    D3D11_SUBRESOURCE_DATA *dest = texData;

    for (UINT n = 0; n < arraySize; n++)
    {
        for (UINT k = 0; k < nSlices; k++)
        {
            for (UINT i = 0; i < nMipMaps; i++)
            {
                UINT pitch, slicePitch;

                pitch = width * getBytesPerPixel(format);
                slicePitch = pitch * height;
                
                dest->pSysMem = rawData;
                dest->SysMemPitch = pitch;
                dest->SysMemSlicePitch = slicePitch;
                dest++;
            }
        }
    }

    tex.texFormat = formats[format];

    if (flags & SRGB)
    {
        // Change to the matching sRGB format
        switch (tex.texFormat)
        {
        case DXGI_FORMAT_R8G8B8A8_UNORM: tex.texFormat = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; break;
        case DXGI_FORMAT_BC1_UNORM: tex.texFormat = DXGI_FORMAT_BC1_UNORM_SRGB; break;
        case DXGI_FORMAT_BC2_UNORM: tex.texFormat = DXGI_FORMAT_BC2_UNORM_SRGB; break;
        case DXGI_FORMAT_BC3_UNORM: tex.texFormat = DXGI_FORMAT_BC3_UNORM_SRGB; break;
        }
    }

    HRESULT hr;

    if (height == 0)
    {
        D3D11_TEXTURE1D_DESC desc;
        desc.Width = width;
        desc.Format = tex.texFormat;
        desc.MipLevels = nMipMaps;
        desc.Usage = D3D11_USAGE_IMMUTABLE;
        desc.BindFlags = D3D10_BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags = 0;
        desc.ArraySize = 1;
        desc.MiscFlags = 0;

        hr = m_pd3dDevice->CreateTexture1D(&desc, texData, (ID3D11Texture1D **)&tex.texture);
    }
    else if (depth == 0)
    {
        D3D11_TEXTURE2D_DESC desc;
        desc.Width = width;
        desc.Height = height;
        desc.Format = tex.texFormat;
        desc.MipLevels = nMipMaps;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.Usage = D3D11_USAGE_IMMUTABLE;
        desc.BindFlags = D3D10_BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags = 0;

        desc.ArraySize = arraySize;
        desc.MiscFlags = 0;

        hr = m_pd3dDevice->CreateTexture2D(&desc, texData, (ID3D11Texture2D **)&tex.texture);
    }
    else if (depth != 0)
    {
        D3D11_TEXTURE3D_DESC desc;
        desc.Width = width;
        desc.Height = height;
        desc.Depth = depth;
        desc.Format = tex.texFormat;
        desc.MipLevels = nMipMaps;
        desc.Usage = D3D11_USAGE_IMMUTABLE;
        desc.BindFlags = D3D10_BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = 0;

        hr = m_pd3dDevice->CreateTexture3D(&desc, texData, (ID3D11Texture3D **)&tex.texture);
    }

    if (FAILED(hr))
    {
        printf("Couldn't create texture");
        return TEXTURE_NONE;
    }

    tex.srvFormat = tex.texFormat;
    tex.srv = createSRV(tex.texture, tex.srvFormat);
    tex.width = width;
    tex.height = height;
    tex.depth = depth;
    tex.nMipMaps = nMipMaps;

    int id = textures.add(tex);
    CHG_INFO("Create new texture: " + std::to_string(id));

    return id;
}

TextureID Direct3D11Renderer::createTexture(Image &img, const SamplerStateID samplerState, UINT flags)
{
	Texture tex;
	memset(&tex, 0, sizeof(tex));

	switch (img.getFormat()){
		case FORMAT_RGB8:
			img.convert(FORMAT_RGBA8);
//			img.convert(FORMAT_RGB10A2);
			break;
		case FORMAT_RGB16:
			img.convert(FORMAT_RGBA16);
			break;
		case FORMAT_RGB16F:
			img.convert(FORMAT_RGBA16F);
			break;
		case FORMAT_RGB32F:
			img.convert(FORMAT_RGBA32F);
			break;
	}

	FORMAT format = img.getFormat();
	UINT nMipMaps = img.getMipMapCount();
	UINT nSlices = img.isCube()? 6 : 1;
	UINT arraySize = img.getArraySize();

	static D3D11_SUBRESOURCE_DATA texData[1024];
	D3D11_SUBRESOURCE_DATA *dest = texData;

	for (UINT n = 0; n < arraySize; n++)
	{
		for (UINT k = 0; k < nSlices; k++)
		{
			for (UINT i = 0; i < nMipMaps; i++)
			{
				UINT pitch, slicePitch;

				if (isCompressedFormat(format))
				{
					pitch = ((img.getWidth(i) + 3) >> 2) * getBytesPerBlock(format);
					slicePitch = pitch * ((img.getHeight(i) + 3) >> 2);
				} 
				else 
				{
					pitch = img.getWidth(i) * getBytesPerPixel(format);
					slicePitch = pitch * img.getHeight(i);
				}

				dest->pSysMem = img.getPixels(i, n) + k * slicePitch;
				dest->SysMemPitch = pitch;
				dest->SysMemSlicePitch = slicePitch;
				dest++;
			}
		}
	}

	tex.texFormat = formats[format];

	if (flags & SRGB)
	{
		// Change to the matching sRGB format
		switch (tex.texFormat)
		{
			case DXGI_FORMAT_R8G8B8A8_UNORM: tex.texFormat = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; break;
			case DXGI_FORMAT_BC1_UNORM: tex.texFormat = DXGI_FORMAT_BC1_UNORM_SRGB; break;
			case DXGI_FORMAT_BC2_UNORM: tex.texFormat = DXGI_FORMAT_BC2_UNORM_SRGB; break;
			case DXGI_FORMAT_BC3_UNORM: tex.texFormat = DXGI_FORMAT_BC3_UNORM_SRGB; break;
		}
	}

	HRESULT hr;

	if (img.is1D())
	{
		D3D11_TEXTURE1D_DESC desc;
		desc.Width  = img.getWidth();
		desc.Format = tex.texFormat;
		desc.MipLevels = nMipMaps;
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.BindFlags = D3D10_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.ArraySize = 1;
		desc.MiscFlags = 0;

		hr = m_pd3dDevice->CreateTexture1D(&desc, texData, (ID3D11Texture1D **) &tex.texture);
	} 
	else if (img.is2D() || img.isCube())
	{
		D3D11_TEXTURE2D_DESC desc;
		desc.Width  = img.getWidth();
		desc.Height = img.getHeight();
		desc.Format = tex.texFormat;
		desc.MipLevels = nMipMaps;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.BindFlags = D3D10_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;

		if (img.isCube())
		{
			desc.ArraySize = 6 * arraySize;
			desc.MiscFlags = D3D10_RESOURCE_MISC_TEXTURECUBE;
		} 
		else 
		{
			desc.ArraySize = arraySize;
			desc.MiscFlags = 0;
		}

		hr = m_pd3dDevice->CreateTexture2D(&desc, texData, (ID3D11Texture2D **) &tex.texture);
	} 
	else if (img.is3D())
	{
		D3D11_TEXTURE3D_DESC desc;
		desc.Width  = img.getWidth();
		desc.Height = img.getHeight();
		desc.Depth  = img.getDepth();
		desc.Format = tex.texFormat;
		desc.MipLevels = nMipMaps;
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.BindFlags = D3D10_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;

		hr = m_pd3dDevice->CreateTexture3D(&desc, texData, (ID3D11Texture3D **) &tex.texture);
	}

	if (FAILED(hr))
	{
		printf("Couldn't create texture");
		return TEXTURE_NONE;
	}

	tex.srvFormat = tex.texFormat;
	tex.srv = createSRV(tex.texture, tex.srvFormat);
	tex.width = img.getWidth();
	tex.height = img.getHeight();
	tex.nMipMaps = nMipMaps;
	
    int id = textures.add(tex);
    CHG_INFO("Create new texture: " + std::to_string(id));

    return id;
}

TextureID Direct3D11Renderer::addRenderTarget(const int width, const int height, const int depth, const int mipMapCount, const int arraySize, const FORMAT format, const int msaaSamples, const SamplerStateID samplerState, UINT flags)
{
	Texture tex;
	memset(&tex, 0, sizeof(tex));

	tex.width  = width;
	tex.height = height;
	tex.depth  = depth;
	tex.arraySize = arraySize;
	tex.flags  = flags;
	tex.texFormat = formats[format];
	tex.nMipMaps = mipMapCount;

	if (flags & SRGB)
	{
		// Change to the matching sRGB format
		switch (tex.texFormat)
		{
			case DXGI_FORMAT_R8G8B8A8_UNORM: tex.texFormat = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; break;
			case DXGI_FORMAT_BC1_UNORM: tex.texFormat = DXGI_FORMAT_BC1_UNORM_SRGB; break;
			case DXGI_FORMAT_BC2_UNORM: tex.texFormat = DXGI_FORMAT_BC2_UNORM_SRGB; break;
			case DXGI_FORMAT_BC3_UNORM: tex.texFormat = DXGI_FORMAT_BC3_UNORM_SRGB; break;
		}
	}


	if (depth == 1)
	{
		D3D11_TEXTURE2D_DESC desc;
		desc.Width  = width;
		desc.Height = height;
		desc.Format = tex.texFormat;
		desc.MipLevels = mipMapCount;
		desc.SampleDesc.Count = msaaSamples;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		desc.CPUAccessFlags = 0;

		if (flags & CUBEMAP)
		{
			desc.ArraySize = 6;
			desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
		} 
		else 
		{
			desc.ArraySize = arraySize;
			desc.MiscFlags = 0;
		}
		if (flags & USE_MIPGEN)
		{
			desc.MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;
		}

		if (FAILED(m_pd3dDevice->CreateTexture2D(&desc, NULL, (ID3D11Texture2D **) &tex.texture)))
		{
			printf("Couldn't create render target");
			return TEXTURE_NONE;
		}
	} 
	else 
	{
		D3D11_TEXTURE3D_DESC desc;
		desc.Width  = width;
		desc.Height = height;
		desc.Depth  = depth;
		desc.Format = tex.texFormat;
		desc.MipLevels = mipMapCount;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		if (flags & UAV_TEXTURE)
			desc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;

		if (flags & USE_MIPGEN)
		{
			desc.MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;
		}

		if (FAILED(m_pd3dDevice->CreateTexture3D(&desc, NULL, (ID3D11Texture3D **) &tex.texture)))
		{
			printf("Couldn't create render target");
			return TEXTURE_NONE;
		}
	}


	tex.srvFormat = tex.texFormat;
	tex.rtvFormat = tex.texFormat;
	tex.srv = createSRV(tex.texture, tex.srvFormat);
	tex.rtv = createRTV(tex.texture, tex.rtvFormat);

	if (flags & UAV_TEXTURE)
		tex.uav = createUAV(tex.texture, tex.srvFormat);

	int sliceCount = (depth == 1)? arraySize : depth;

	if (flags & SAMPLE_SLICES)
	{
		tex.srvArray = CHG_NEW ID3D11ShaderResourceView *[sliceCount];
		for (int i = 0; i < sliceCount; i++)
		{
			tex.srvArray[i] = createSRV(tex.texture, tex.srvFormat, i);
		}
	}

	if (flags & RENDER_SLICES)
	{
		tex.rtvArray = CHG_NEW ID3D11RenderTargetView *[sliceCount];

		for (int i = 0; i < sliceCount; i++)
		{
			tex.rtvArray[i] = createRTV(tex.texture, tex.rtvFormat, i);
		}
	}
	
    int id = textures.add(tex);
    CHG_INFO("Create new Target Surface: " + std::to_string(id));

	return id;
}

TextureID Direct3D11Renderer::addRenderDepth(const int width, const int height, const int arraySize, const FORMAT format, const int msaaSamples, const SamplerStateID samplerState, UINT flags)
{
	Texture tex;
	memset(&tex, 0, sizeof(tex));

	tex.width  = width;
	tex.height = height;
	tex.depth  = 1;
	tex.arraySize = arraySize;
	tex.flags  = flags;
	tex.texFormat = tex.dsvFormat = formats[format];

	D3D11_TEXTURE2D_DESC desc;
	desc.Width  = width;
	desc.Height = height;
	desc.MipLevels = 1;
	desc.SampleDesc.Count = msaaSamples;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	desc.CPUAccessFlags = 0;

	if (flags & CUBEMAP)
	{
		desc.ArraySize = 6;
		desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
	} 
	else 
	{
		desc.ArraySize = arraySize;
		desc.MiscFlags = 0;
	}

	if (flags & SAMPLE_DEPTH)
	{
		switch (tex.dsvFormat)
		{
			case DXGI_FORMAT_D16_UNORM:
				tex.texFormat = DXGI_FORMAT_R16_TYPELESS;
				tex.srvFormat = DXGI_FORMAT_R16_UNORM;
				break;
			case DXGI_FORMAT_D24_UNORM_S8_UINT:
				tex.texFormat = DXGI_FORMAT_R24G8_TYPELESS;
				tex.srvFormat = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
				break;
			case DXGI_FORMAT_D32_FLOAT:
				tex.texFormat = DXGI_FORMAT_R32_TYPELESS;
				tex.srvFormat = DXGI_FORMAT_R32_FLOAT;
				break;
		}
		desc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
	}

	desc.Format = tex.texFormat;

	if (FAILED(m_pd3dDevice->CreateTexture2D(&desc, NULL, (ID3D11Texture2D **) &tex.texture)))
	{
		printf("Couldn't create depth target");
		return TEXTURE_NONE;
	}

	tex.dsv = createDSV(tex.texture, tex.dsvFormat);

	if (flags & RENDER_SLICES)
	{
		tex.dsvArray = CHG_NEW ID3D11DepthStencilView *[arraySize];
		for (int i = 0; i < arraySize; i++)
		{
			tex.dsvArray[i] = createDSV(tex.texture, tex.dsvFormat, i);
		}
	}

	if (flags & SAMPLE_DEPTH)
	{
		tex.srv = createSRV(tex.texture, tex.srvFormat);

		if (flags & SAMPLE_SLICES)
		{
			tex.srvArray = CHG_NEW ID3D11ShaderResourceView *[arraySize];
			for (int i = 0; i < arraySize; i++)
			{
				tex.srvArray[i] = createSRV(tex.texture, tex.srvFormat, i);
			}
		}
	}

    int id = textures.add(tex);
    CHG_INFO("Create new Depth Surface: " + std::to_string(id));

    return id;
}

bool Direct3D11Renderer::resizeRenderTarget(const TextureID renderTarget, const int width, const int height, const int depth, const int mipMapCount, const int arraySize)
{
	D3D11_RESOURCE_DIMENSION type;
	textures[renderTarget].texture->GetType(&type);

	switch (type)
	{
		case D3D11_RESOURCE_DIMENSION_TEXTURE2D:
			D3D11_TEXTURE2D_DESC desc2d;
			((ID3D11Texture2D *) textures[renderTarget].texture)->GetDesc(&desc2d);

			desc2d.Width     = width;
			desc2d.Height    = height;
			desc2d.ArraySize = arraySize;
			desc2d.MipLevels = mipMapCount;

			textures[renderTarget].texture->Release();
			if (FAILED(m_pd3dDevice->CreateTexture2D(&desc2d, NULL, (ID3D11Texture2D **) &textures[renderTarget].texture)))
			{
				printf("Couldn't create render target");
				return false;
			}
			break;

		default:
			return false;
	}

	if (textures[renderTarget].rtv)
	{
		textures[renderTarget].rtv->Release();
		textures[renderTarget].rtv = createRTV(textures[renderTarget].texture, textures[renderTarget].rtvFormat);
	}
	if (textures[renderTarget].dsv)
	{
		textures[renderTarget].dsv->Release();
		textures[renderTarget].dsv = createDSV(textures[renderTarget].texture, textures[renderTarget].dsvFormat);
	}
	if (textures[renderTarget].srv)
	{
		textures[renderTarget].srv->Release();
		textures[renderTarget].srv = createSRV(textures[renderTarget].texture, textures[renderTarget].srvFormat);
	}

	if (textures[renderTarget].rtvArray)
	{
		for (int i = 0; i < textures[renderTarget].arraySize; i++)
		{
			textures[renderTarget].rtvArray[i]->Release();
		}
		if (arraySize != textures[renderTarget].arraySize)
		{
			delete [] textures[renderTarget].rtvArray;
			textures[renderTarget].rtvArray = CHG_NEW ID3D11RenderTargetView *[arraySize];
		}
		for (int i = 0; i < arraySize; i++)
		{
			textures[renderTarget].rtvArray[i] = createRTV(textures[renderTarget].texture, textures[renderTarget].rtvFormat, i);
		}
	}
	if (textures[renderTarget].dsvArray)
	{
		for (int i = 0; i < textures[renderTarget].arraySize; i++)
		{
			textures[renderTarget].dsvArray[i]->Release();
		}
		if (arraySize != textures[renderTarget].arraySize)
		{
			delete [] textures[renderTarget].dsvArray;
			textures[renderTarget].dsvArray = CHG_NEW ID3D11DepthStencilView *[arraySize];
		}
		for (int i = 0; i < arraySize; i++)
		{
			textures[renderTarget].dsvArray[i] = createDSV(textures[renderTarget].texture, textures[renderTarget].dsvFormat, i);
		}
	}
	if (textures[renderTarget].srvArray)
	{
		for (int i = 0; i < textures[renderTarget].arraySize; i++)
		{
			textures[renderTarget].srvArray[i]->Release();
		}
		if (arraySize != textures[renderTarget].arraySize)
		{
			delete [] textures[renderTarget].srvArray;
			textures[renderTarget].srvArray = CHG_NEW ID3D11ShaderResourceView *[arraySize];
		}
		for (int i = 0; i < arraySize; i++)
		{
			textures[renderTarget].srvArray[i] = createSRV(textures[renderTarget].texture, textures[renderTarget].srvFormat, i);
		}
	}

	textures[renderTarget].width  = width;
	textures[renderTarget].height = height;
	textures[renderTarget].depth  = depth;
	textures[renderTarget].arraySize = arraySize;

	return true;
}

bool Direct3D11Renderer::generateMipMaps(const TextureID renderTarget)
{
	m_pImmediateContext->GenerateMips(textures[renderTarget].srv);

	return true;
}

void Direct3D11Renderer::removeVertexBuffer(const VertexBufferID vertexBuffer)
{
	if (vertexBuffers[vertexBuffer].vertexBuffer) vertexBuffers[vertexBuffer].vertexBuffer->Release();
	vertexBuffers.Remove(vertexBuffer);

    CHG_INFO("Remove Vertex Buffer: " + std::to_string(vertexBuffer));
}

void Direct3D11Renderer::removeIndexBuffer(const IndexBufferID indexBuffer)
{
	if (indexBuffers[indexBuffer].indexBuffer) indexBuffers[indexBuffer].indexBuffer->Release();
	indexBuffers.Remove(indexBuffer);

    CHG_INFO("Remove Index Buffer: " + std::to_string(indexBuffer));
}

void Direct3D11Renderer::removeTexture(const TextureID texture)
{
	SAFE_RELEASE(textures[texture].texture);
	SAFE_RELEASE(textures[texture].srv);
	SAFE_RELEASE(textures[texture].rtv);
	SAFE_RELEASE(textures[texture].dsv);
	delete[] textures[texture].uav;

	/*for (int i = 0; i < textures[texture].nMipMaps; i++)
	{
		if (textures[texture].uav)
			SAFE_RELEASE(textures[texture].uav[i]);
	}*/
	

	int sliceCount = (textures[texture].depth == 1)? textures[texture].arraySize : textures[texture].depth;

	if (textures[texture].srvArray)
	{
		for (int i = 0; i < sliceCount; i++)
		{
			textures[texture].srvArray[i]->Release();
		}
		delete [] textures[texture].srvArray;
		textures[texture].srvArray = NULL;
	}
	if (textures[texture].rtvArray)
	{
		for (int i = 0; i < sliceCount; i++)
		{
			textures[texture].rtvArray[i]->Release();
		}
		delete [] textures[texture].rtvArray;
		textures[texture].rtvArray = NULL;
	}
	if (textures[texture].dsvArray)
	{
		for (int i = 0; i < sliceCount; i++)
		{
			textures[texture].dsvArray[i]->Release();
		}
		delete [] textures[texture].dsvArray;
		textures[texture].dsvArray = NULL;
	}

	textures.Remove(texture);

    CHG_INFO("Remove Texture: " + std::to_string(texture));
}

void Direct3D11Renderer::removeStructuredBuffer(const StructuredBufferID structuredBuffer)
{
	SAFE_RELEASE(structuredBuffers[structuredBuffer].structuredBuffer);
	SAFE_RELEASE(structuredBuffers[structuredBuffer].structuredBufferView);
	SAFE_RELEASE(structuredBuffers[structuredBuffer].structuredBufferViewRW);
}

ShaderID Direct3D11Renderer::createShader(const char *vsText, const char *gsText, const char *fsText, const char *csText, const char* tcsText, const char* tesText, const int vsLine, const int gsLine, const int fsLine, const int csLine,
										  const int tcsLine, const int tesLine, const char *header, const char *extra, const char *fileName, const char **attributeNames, const int nAttributes, const UINT flags)
{
	if (vsText == NULL && fsText == NULL && gsText == NULL && csText == NULL) return SHADER_NONE;

	Shader shader;
	memset(&shader, 0, sizeof(shader));

	ID3DBlob *shaderBuf = NULL;
	ID3DBlob *errorsBuf = NULL;

	ID3D11ShaderReflection *vsRefl = NULL;
	ID3D11ShaderReflection *gsRefl = NULL;
	ID3D11ShaderReflection *tcsRefl = NULL;
	ID3D11ShaderReflection *tesRefl = NULL;
	ID3D11ShaderReflection *psRefl = NULL;
	ID3D11ShaderReflection *csRefl = NULL;

	UINT compileFlags = D3DCOMPILE_PACK_MATRIX_ROW_MAJOR | D3DCOMPILE_ENABLE_STRICTNESS;// | D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION;

#ifdef _DEBUG
	compileFlags |= D3DCOMPILE_PREFER_FLOW_CONTROL | D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	if (vsText != NULL)
	{
		std::string shaderString;
		if (extra != NULL) shaderString += extra;
		if (header != NULL) shaderString += header;

		char buf[256];
		int len = sprintf(buf, "#line %d\n", vsLine + 1);
		shaderString.append(buf, len);

		shaderString += vsText;


		if (SUCCEEDED(D3DCompile(shaderString.c_str(), shaderString.size(), fileName, NULL, m_pShaderInclude, "main", "vs_5_0", compileFlags, NULL, &shaderBuf, &errorsBuf)))
		{
			if (SUCCEEDED(m_pd3dDevice->CreateVertexShader(shaderBuf->GetBufferPointer(), shaderBuf->GetBufferSize(), NULL, &shader.vertexShader)))
			{
				D3DReflect( shaderBuf->GetBufferPointer(), shaderBuf->GetBufferSize(), IID_ID3D11ShaderReflection, (void**) &vsRefl);
				D3DGetInputSignatureBlob(shaderBuf->GetBufferPointer(), shaderBuf->GetBufferSize(), &shader.inputSignature);				
			}
		} 
		else 
		{
			printf((const char *) errorsBuf->GetBufferPointer());
		}
		SAFE_RELEASE(shaderBuf);
		SAFE_RELEASE(errorsBuf);

		if (shader.vertexShader == NULL) return SHADER_NONE;
	}

	if (gsText != NULL)
	{
		std::string shaderString;
		if (extra != NULL) shaderString += extra;
		if (header != NULL) shaderString += header;
		
		char buf[256];
		int len = sprintf(buf, "#line %d\n", gsLine + 1);
		shaderString.append(buf, len);

		shaderString += gsText;

		if (SUCCEEDED(D3DCompile(shaderString.c_str(), shaderString.size(), fileName, NULL, m_pShaderInclude, "main", "gs_5_0", compileFlags, NULL, &shaderBuf, &errorsBuf)))
		{
			if (SUCCEEDED(m_pd3dDevice->CreateGeometryShader(shaderBuf->GetBufferPointer(), shaderBuf->GetBufferSize(), NULL, &shader.geometryShader)))
			{
				D3DReflect( shaderBuf->GetBufferPointer(), shaderBuf->GetBufferSize(), IID_ID3D11ShaderReflection, (void**) &gsRefl);
			}
		} 
		else 
		{
			printf((const char *) errorsBuf->GetBufferPointer());
		}
		SAFE_RELEASE(shaderBuf);
		SAFE_RELEASE(errorsBuf);

		if (shader.geometryShader == NULL) return SHADER_NONE;
	}
	
	if (tcsText != NULL)
	{
		std::string shaderString;
		if (extra != NULL) shaderString += extra;
		if (header != NULL) shaderString += header;

		char buf[256];
		int len = sprintf(buf, "#line %d\n", tcsLine + 1);
		shaderString.append(buf, len);

		shaderString += tcsText;

		if (SUCCEEDED(D3DCompile(shaderString.c_str(), shaderString.size(), fileName, NULL, m_pShaderInclude, "main", "hs_5_0", compileFlags, NULL, &shaderBuf, &errorsBuf)))
		{
			if (SUCCEEDED(m_pd3dDevice->CreateHullShader(shaderBuf->GetBufferPointer(), shaderBuf->GetBufferSize(), NULL, &shader.hullShader)))
			{
				D3DReflect(shaderBuf->GetBufferPointer(), shaderBuf->GetBufferSize(), IID_ID3D11ShaderReflection, (void**) &tcsRefl);
			}
		}
		else
		{
			printf((const char *)errorsBuf->GetBufferPointer());
		}
		SAFE_RELEASE(shaderBuf);
		SAFE_RELEASE(errorsBuf);

		if (shader.hullShader == NULL) return SHADER_NONE;
	}

	if (tesText != NULL)
	{
		HRESULT hr;

		std::string shaderString;
		if (extra != NULL) shaderString += extra;
		if (header != NULL) shaderString += header;

		char buf[256];
		int len = sprintf(buf, "#line %d\n", tesLine + 1);
		shaderString.append(buf, len);

		shaderString += tesText;

		if (SUCCEEDED(D3DCompile(shaderString.c_str(), shaderString.size(), fileName, NULL, m_pShaderInclude, "main", "ds_5_0", compileFlags, NULL, &shaderBuf, &errorsBuf)))
		{
			if (SUCCEEDED(m_pd3dDevice->CreateDomainShader(shaderBuf->GetBufferPointer(), shaderBuf->GetBufferSize(), NULL, &shader.domainShader)))
			{
				hr = D3DReflect(shaderBuf->GetBufferPointer(), shaderBuf->GetBufferSize(), IID_ID3D11ShaderReflection, (void**) &tesRefl);
			}
		}
		else
		{
			printf((const char *)errorsBuf->GetBufferPointer());
		}
		SAFE_RELEASE(shaderBuf);
		SAFE_RELEASE(errorsBuf);

		if (shader.domainShader == NULL) return SHADER_NONE;
	}

	if (fsText != NULL)
	{
		std::string shaderString;
		if (extra != NULL) shaderString += extra;
		if (header != NULL) shaderString += header;
		
		char buf[256];
		int len = sprintf(buf, "#line %d\n", fsLine + 1);
		shaderString.append(buf, len);

		shaderString += fsText;

		if (SUCCEEDED(D3DCompile(shaderString.c_str(), shaderString.size(), fileName, NULL, m_pShaderInclude, "main", "ps_5_0", compileFlags, NULL, &shaderBuf, &errorsBuf)))
		{
			if (SUCCEEDED(m_pd3dDevice->CreatePixelShader(shaderBuf->GetBufferPointer(), shaderBuf->GetBufferSize(), NULL, &shader.pixelShader)))
			{
				D3DReflect( shaderBuf->GetBufferPointer(), shaderBuf->GetBufferSize(), IID_ID3D11ShaderReflection, (void**) &psRefl);
			}
		} 
		else
		{
			printf((const char *) errorsBuf->GetBufferPointer());
		}
		SAFE_RELEASE(shaderBuf);
		SAFE_RELEASE(errorsBuf);

		if (shader.pixelShader == NULL) return SHADER_NONE;
	}

	if (csText != NULL)
	{
		std::string shaderString;
		if (extra != NULL) shaderString += extra;
		if (header != NULL) shaderString += header;
		
		char buf[256];
		int len = sprintf(buf, "#line %d\n", csLine + 1);
		shaderString.append(buf, len);

		shaderString += csText;

		if (SUCCEEDED(D3DCompile(shaderString.c_str(), shaderString.size(), fileName, NULL, m_pShaderInclude, "main", "cs_5_0", compileFlags, NULL, &shaderBuf, &errorsBuf)))
		{
			if (SUCCEEDED(m_pd3dDevice->CreateComputeShader(shaderBuf->GetBufferPointer(), shaderBuf->GetBufferSize(), NULL, &shader.computeShader)))
			{
				D3DReflect( shaderBuf->GetBufferPointer(), shaderBuf->GetBufferSize(), IID_ID3D11ShaderReflection, (void**) &csRefl);
			}
		} 
		else
		{
			printf((const char *) errorsBuf->GetBufferPointer());
		}
		SAFE_RELEASE(shaderBuf);
		SAFE_RELEASE(errorsBuf);

		if (shader.computeShader == NULL) return SHADER_NONE;
	}

	D3D11_SHADER_DESC vsDesc, gsDesc, tcsDesc, tesDesc, psDesc, csDesc;
	if (vsRefl)
	{
		vsRefl->GetDesc(&vsDesc);

		if (vsDesc.ConstantBuffers)
		{
			shader.nVSCBuffers = vsDesc.ConstantBuffers;
			shader.vsConstants = (ID3D11Buffer**)malloc(sizeof(ID3D11Buffer*)*shader.nVSCBuffers);
			shader.vsConstMem = (ubyte**)malloc(sizeof(ubyte *)*shader.nVSCBuffers);
			shader.vsDirty = (bool*)malloc(sizeof(bool)*shader.nVSCBuffers);
		}
	}
	if (gsRefl)
	{
		gsRefl->GetDesc(&gsDesc);

		if (gsDesc.ConstantBuffers)
		{
			shader.nGSCBuffers = gsDesc.ConstantBuffers;
			shader.gsConstants = (ID3D11Buffer**)malloc(sizeof(ID3D11Buffer*)*shader.nGSCBuffers);
			shader.gsConstMem = (ubyte**)malloc(sizeof(ubyte *)*shader.nGSCBuffers);
			shader.gsDirty = (bool*)malloc(sizeof(bool)*shader.nGSCBuffers);
		}
	}
	if (tcsRefl)
	{
		tcsRefl->GetDesc(&tcsDesc);

		if (tcsDesc.ConstantBuffers)
		{
			shader.nHSCBuffers = tcsDesc.ConstantBuffers;
			shader.hsConstants = (ID3D11Buffer**)malloc(sizeof(ID3D11Buffer*)*shader.nHSCBuffers);
			shader.hsConstMem = (ubyte**)malloc(sizeof(ubyte *)*shader.nHSCBuffers);
			shader.hsDirty = (bool*)malloc(sizeof(bool)*shader.nHSCBuffers);
		}
	}
	if (tesRefl)
	{
		tesRefl->GetDesc(&tesDesc);

		if (tesDesc.ConstantBuffers)
		{
			shader.nDSCBuffers = tesDesc.ConstantBuffers;
			shader.dsConstants = (ID3D11Buffer**)malloc(sizeof(ID3D11Buffer*)*shader.nDSCBuffers);
			shader.dsConstMem = (ubyte**)malloc(sizeof(ubyte *)*shader.nDSCBuffers);
			shader.dsDirty = (bool*)malloc(sizeof(bool)*shader.nDSCBuffers);
		}
	}
	if (psRefl)
	{
		psRefl->GetDesc(&psDesc);

		if (psDesc.ConstantBuffers)
		{
			shader.nPSCBuffers = psDesc.ConstantBuffers;
			shader.psConstants = (ID3D11Buffer**)malloc(sizeof(ID3D11Buffer*)*shader.nPSCBuffers);
			shader.psConstMem = (ubyte**)malloc(sizeof(ubyte *)*shader.nPSCBuffers);
			shader.psDirty = (bool*)malloc(sizeof(bool)*shader.nPSCBuffers);
		}
	}
	if (csRefl)
	{
		csRefl->GetDesc(&csDesc);

		if (csDesc.ConstantBuffers)
		{
			shader.nCSCBuffers = csDesc.ConstantBuffers;
			shader.csConstants = (ID3D11Buffer**)malloc(sizeof(ID3D11Buffer*)*shader.nCSCBuffers);
			shader.csConstMem = (ubyte**)malloc(sizeof(ubyte *)*shader.nCSCBuffers);
			shader.csDirty = (bool*)malloc(sizeof(bool)*shader.nCSCBuffers);
		}
	}

	D3D11_SHADER_BUFFER_DESC sbDesc;

	D3D11_BUFFER_DESC cbDesc;
	cbDesc.Usage = D3D11_USAGE_DEFAULT;//D3D10_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = 0;//D3D10_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;

	std::vector<Constant> constants;
	UINT nVSConstants = 0;
	UINT nGSConstants = 0;
	UINT nTESConstants = 0;
	UINT nTCSConstants = 0;
	UINT nPSConstants = 0;
	UINT nCSConstants = 0;

	for (UINT i = 0; i < shader.nVSCBuffers; i++)
	{
		vsRefl->GetConstantBufferByIndex(i)->GetDesc(&sbDesc);

		if(sbDesc.Type != D3D11_CT_RESOURCE_BIND_INFO)
		{
			cbDesc.ByteWidth = sbDesc.Size;
			m_pd3dDevice->CreateBuffer(&cbDesc, NULL, &shader.vsConstants[nVSConstants]);

			shader.vsConstMem[nVSConstants] = CHG_NEW ubyte[sbDesc.Size];
			for (UINT k = 0; k < sbDesc.Variables; k++)
			{
				D3D11_SHADER_VARIABLE_DESC vDesc;
				vsRefl->GetConstantBufferByIndex(nVSConstants)->GetVariableByIndex(k)->GetDesc(&vDesc);

				Constant constant;
				size_t length = strlen(vDesc.Name);
				constant.name = CHG_NEW char[length + 1];
				strcpy(constant.name, vDesc.Name);
				constant.vsData = shader.vsConstMem[nVSConstants] + vDesc.StartOffset;
				constant.gsData = NULL;
				constant.tcsData = NULL;
				constant.tesData = NULL;
				constant.psData = NULL;
				constant.csData = NULL;
				constant.vsBuffer = nVSConstants;
				constant.gsBuffer = -1;
				constant.tcsBuffer = -1;
				constant.tesBuffer = -1;
				constant.psBuffer = -1;
				constant.csBuffer = -1;
				constants.push_back(constant);
			}

			shader.vsDirty[nVSConstants] = false;

			nVSConstants++;
		}		
	}

	if(shader.nVSCBuffers > 0 && shader.nVSCBuffers != nVSConstants)
	{
		shader.nVSCBuffers = nVSConstants;
		shader.vsConstants = (ID3D11Buffer**) realloc(shader.vsConstants, sizeof(ID3D11Buffer*)*shader.nVSCBuffers);
		shader.vsConstMem = (ubyte**) realloc(shader.vsConstMem, sizeof(ubyte *)*shader.nVSCBuffers);
		shader.vsDirty = (bool*) realloc(shader.vsDirty, sizeof(bool)*shader.nVSCBuffers);
	}

	UINT maxConst = constants.size();

	for (UINT i = 0; i < shader.nGSCBuffers; i++)
	{
		gsRefl->GetConstantBufferByIndex(i)->GetDesc(&sbDesc);

		if(sbDesc.Type != D3D11_CT_RESOURCE_BIND_INFO)
		{
			cbDesc.ByteWidth = sbDesc.Size;
			m_pd3dDevice->CreateBuffer(&cbDesc, NULL, &shader.gsConstants[nGSConstants]);

			shader.gsConstMem[nGSConstants] = CHG_NEW ubyte[sbDesc.Size];
			for (UINT k = 0; k < sbDesc.Variables; k++)
			{
				D3D11_SHADER_VARIABLE_DESC vDesc;
				gsRefl->GetConstantBufferByIndex(nGSConstants)->GetVariableByIndex(k)->GetDesc(&vDesc);

				int merge = -1;
				for (UINT j = 0; j < maxConst; j++)
				{
					if (strcmp(constants[j].name, vDesc.Name) == 0)
					{
						merge = j;
						break;
					}
				}

				if (merge < 0)
				{
					Constant constant;
					size_t length = strlen(vDesc.Name);
					constant.name = CHG_NEW char[length + 1];
					strcpy(constant.name, vDesc.Name);
					constant.vsData = NULL;
					constant.gsData = shader.gsConstMem[nGSConstants] + vDesc.StartOffset;
					constant.tcsData = NULL;
					constant.tesData = NULL;
					constant.psData = NULL;
					constant.csData = NULL;
					constant.vsBuffer = -1;
					constant.gsBuffer = nGSConstants;
					constant.tcsBuffer = -1;
					constant.tesBuffer = -1;
					constant.psBuffer = -1;
					constant.csBuffer = -1;
					constants.push_back(constant);
				} 
				else 
				{
					constants[merge].gsData = shader.gsConstMem[nGSConstants] + vDesc.StartOffset;
					constants[merge].gsBuffer = nGSConstants;
				}
			}

			shader.gsDirty[nGSConstants] = false;

			nGSConstants++;
		}
		
	}

	if(shader.nGSCBuffers > 0 && shader.nGSCBuffers != nGSConstants)
	{
		shader.nGSCBuffers = nGSConstants;
		shader.gsConstants = (ID3D11Buffer**) realloc(shader.gsConstants, sizeof(ID3D11Buffer*)*shader.nGSCBuffers);
		shader.gsConstMem = (ubyte**) realloc(shader.gsConstMem, sizeof(ubyte *)*shader.nGSCBuffers);
		shader.gsDirty = (bool*) realloc(shader.gsDirty, sizeof(bool)*shader.nGSCBuffers);
	}

	// Hull Shader Constants //////////////////////////////////////////////////////////////////////////////
	maxConst = constants.size();

	for (UINT i = 0; i < shader.nHSCBuffers; i++)
	{
		tcsRefl->GetConstantBufferByIndex(i)->GetDesc(&sbDesc);

		if (sbDesc.Type != D3D11_CT_RESOURCE_BIND_INFO)
		{
			cbDesc.ByteWidth = sbDesc.Size;
			m_pd3dDevice->CreateBuffer(&cbDesc, NULL, &shader.hsConstants[nTCSConstants]);

			shader.hsConstMem[nTCSConstants] = CHG_NEW ubyte[sbDesc.Size];
			for (UINT k = 0; k < sbDesc.Variables; k++)
			{
				D3D11_SHADER_VARIABLE_DESC vDesc;
				tcsRefl->GetConstantBufferByIndex(nTCSConstants)->GetVariableByIndex(k)->GetDesc(&vDesc);

				int merge = -1;
				for (UINT j = 0; j < maxConst; j++)
				{
					if (strcmp(constants[j].name, vDesc.Name) == 0)
					{
						merge = j;
						break;
					}
				}

				if (merge < 0)
				{
					Constant constant;
					size_t length = strlen(vDesc.Name);
					constant.name = CHG_NEW char[length + 1];
					strcpy(constant.name, vDesc.Name);
					constant.vsData = NULL;
					constant.gsData = NULL;
					constant.tcsData = shader.hsConstMem[nTCSConstants] + vDesc.StartOffset;
					constant.tesData = NULL;
					constant.psData = NULL;
					constant.csData = NULL;
					constant.vsBuffer = -1;
					constant.gsBuffer = -1;
					constant.tcsBuffer = nTCSConstants;
					constant.tesBuffer = -1;
					constant.psBuffer = -1;
					constant.csBuffer = -1;
					constants.push_back(constant);
				}
				else
				{
					constants[merge].tcsData = shader.hsConstMem[nTCSConstants] + vDesc.StartOffset;
					constants[merge].tcsBuffer = nTCSConstants;
				}
			}

			shader.hsDirty[nTCSConstants] = false;

			nTCSConstants++;
		}

	}

	if (shader.nHSCBuffers > 0 && shader.nHSCBuffers != nTCSConstants)
	{
		shader.nHSCBuffers = nTCSConstants;
		shader.hsConstants = (ID3D11Buffer**)realloc(shader.hsConstants, sizeof(ID3D11Buffer*)*shader.nHSCBuffers);
		shader.hsConstMem = (ubyte**)realloc(shader.hsConstMem, sizeof(ubyte *)*shader.nHSCBuffers);
		shader.hsDirty = (bool*)realloc(shader.hsDirty, sizeof(bool)*shader.nHSCBuffers);
	}

	// Domain Shader Constants //////////////////////////////////////////////////////////////////////////////
	maxConst = constants.size();

	for (UINT i = 0; i < shader.nDSCBuffers; i++)
	{
		tesRefl->GetConstantBufferByIndex(i)->GetDesc(&sbDesc);

		if (sbDesc.Type != D3D11_CT_RESOURCE_BIND_INFO)
		{
			cbDesc.ByteWidth = sbDesc.Size;
			m_pd3dDevice->CreateBuffer(&cbDesc, NULL, &shader.dsConstants[nTESConstants]);

			shader.dsConstMem[nTESConstants] = CHG_NEW ubyte[sbDesc.Size];
			for (UINT k = 0; k < sbDesc.Variables; k++)
			{
				D3D11_SHADER_VARIABLE_DESC vDesc;
				tesRefl->GetConstantBufferByIndex(nTESConstants)->GetVariableByIndex(k)->GetDesc(&vDesc);

				int merge = -1;
				for (UINT j = 0; j < maxConst; j++)
				{
					if (strcmp(constants[j].name, vDesc.Name) == 0)
					{
						merge = j;
						break;
					}
				}

				if (merge < 0)
				{
					Constant constant;
					size_t length = strlen(vDesc.Name);
					constant.name = CHG_NEW char[length + 1];
					strcpy(constant.name, vDesc.Name);
					constant.vsData = NULL;
					constant.gsData = NULL;
					constant.tcsData = NULL;
					constant.tesData = shader.dsConstMem[nTESConstants] + vDesc.StartOffset;
					constant.psData = NULL;
					constant.csData = NULL;
					constant.vsBuffer = -1;
					constant.gsBuffer = -1;
					constant.tcsBuffer = -1;
					constant.tesBuffer = nTESConstants;
					constant.psBuffer = -1;
					constant.csBuffer = -1;
					constants.push_back(constant);
				}
				else
				{
					constants[merge].tesData = shader.dsConstMem[nTESConstants] + vDesc.StartOffset;
					constants[merge].tesBuffer = nTESConstants;
				}
			}

			shader.dsDirty[nTESConstants] = false;

			nTESConstants++;
		}

	}

	if (shader.nDSCBuffers > 0 && shader.nDSCBuffers != nTESConstants)
	{
		shader.nDSCBuffers = nTESConstants;
		shader.dsConstants = (ID3D11Buffer**)realloc(shader.dsConstants, sizeof(ID3D11Buffer*)*shader.nDSCBuffers);
		shader.dsConstMem = (ubyte**)realloc(shader.dsConstMem, sizeof(ubyte *)*shader.nDSCBuffers);
		shader.dsDirty = (bool*)realloc(shader.dsDirty, sizeof(bool)*shader.nDSCBuffers);
	}

	maxConst = constants.size();
	for (UINT i = 0; i < shader.nPSCBuffers; i++)
	{
		psRefl->GetConstantBufferByIndex(i)->GetDesc(&sbDesc);

		if(sbDesc.Type != D3D11_CT_RESOURCE_BIND_INFO)
		{
			cbDesc.ByteWidth = sbDesc.Size;
			m_pd3dDevice->CreateBuffer(&cbDesc, NULL, &shader.psConstants[nPSConstants]);

			shader.psConstMem[nPSConstants] = CHG_NEW ubyte[sbDesc.Size];
			for (UINT k = 0; k < sbDesc.Variables; k++)
			{
				D3D11_SHADER_VARIABLE_DESC vDesc;
				psRefl->GetConstantBufferByIndex(nPSConstants)->GetVariableByIndex(k)->GetDesc(&vDesc);

				int merge = -1;
				for (UINT j = 0; j < maxConst; j++)
				{
					if (strcmp(constants[j].name, vDesc.Name) == 0)
					{
						merge = j;
						break;
					}
				}

				if (merge < 0)
				{
					Constant constant;
					size_t length = strlen(vDesc.Name);
					constant.name = CHG_NEW char[length + 1];
					strcpy(constant.name, vDesc.Name);
					constant.vsData = NULL;
					constant.gsData = NULL;
					constant.tcsData = NULL;
					constant.tesData = NULL;
					constant.psData = shader.psConstMem[nPSConstants] + vDesc.StartOffset;
					constant.csData = NULL;
					constant.vsBuffer = -1;
					constant.gsBuffer = -1;
					constant.tcsBuffer = -1;
					constant.tesBuffer = -1;
					constant.psBuffer = nPSConstants;
					constant.csBuffer = -1;
					constants.push_back(constant);
				}
				else
				{
					constants[merge].psData = shader.psConstMem[nPSConstants] + vDesc.StartOffset;
					constants[merge].psBuffer = nPSConstants;
				}
			}

			shader.psDirty[nPSConstants] = false;

			nPSConstants++;
		}				
	}

	if(shader.nPSCBuffers > 0 && shader.nPSCBuffers != nPSConstants)
	{
		shader.nPSCBuffers = nPSConstants;
		shader.psConstants = (ID3D11Buffer**) realloc(shader.psConstants, sizeof(ID3D11Buffer*)*shader.nPSCBuffers);
		shader.psConstMem = (ubyte**) realloc(shader.psConstMem, sizeof(ubyte *)*shader.nPSCBuffers);
		shader.psDirty = (bool*) realloc(shader.psDirty, sizeof(bool)*shader.nPSCBuffers);
	}

	maxConst = constants.size();
	for (UINT i = 0; i < shader.nCSCBuffers; i++)
	{
		csRefl->GetConstantBufferByIndex(i)->GetDesc(&sbDesc);

		if(sbDesc.Type != D3D11_CT_RESOURCE_BIND_INFO)
		{
			cbDesc.ByteWidth = sbDesc.Size;
			m_pd3dDevice->CreateBuffer(&cbDesc, NULL, &shader.csConstants[nCSConstants]);

			shader.csConstMem[nCSConstants] = CHG_NEW ubyte[sbDesc.Size];
			for (UINT k = 0; k < sbDesc.Variables; k++)
			{
				D3D11_SHADER_VARIABLE_DESC vDesc;
				csRefl->GetConstantBufferByIndex(nCSConstants)->GetVariableByIndex(k)->GetDesc(&vDesc);

				int merge = -1;
				for (UINT j = 0; j < maxConst; j++)
				{
					if (strcmp(constants[j].name, vDesc.Name) == 0)
					{
						merge = j;
						break;
					}
				}

				if (merge < 0)
				{
					Constant constant;
					size_t length = strlen(vDesc.Name);
					constant.name = CHG_NEW char[length + 1];
					strcpy(constant.name, vDesc.Name);
					constant.vsData = NULL;
					constant.gsData = NULL;
					constant.tcsData = NULL;
					constant.tesData = NULL;
					constant.psData = NULL;
					constant.csData = shader.csConstMem[nCSConstants] + vDesc.StartOffset;
					constant.vsBuffer = -1;
					constant.gsBuffer = -1;
					constant.tcsBuffer = -1;
					constant.tesBuffer = -1;
					constant.psBuffer = -1;
					constant.csBuffer = nCSConstants;
					constants.push_back(constant);
				}
				else
				{
					constants[merge].csData = shader.csConstMem[nCSConstants] + vDesc.StartOffset;
					constants[merge].csBuffer = nCSConstants;
				}
			}

			shader.csDirty[i] = false;

			nCSConstants++;
		}
	}

	if(shader.nCSCBuffers > 0 && shader.nCSCBuffers != nCSConstants)
	{
		shader.nCSCBuffers = nCSConstants;
		shader.csConstants = (ID3D11Buffer**) realloc(shader.csConstants, sizeof(ID3D11Buffer*)*shader.nCSCBuffers);
		shader.csConstMem = (ubyte**) realloc(shader.csConstMem, sizeof(ubyte *)*shader.nCSCBuffers);
		shader.csDirty = (bool*) realloc(shader.csDirty, sizeof(bool)*shader.nCSCBuffers);
	}

	shader.nConstants = constants.size();
	shader.constants = CHG_NEW Constant[shader.nConstants];
	memcpy(shader.constants, constants.data(), shader.nConstants * sizeof(Constant));
	qsort(shader.constants, shader.nConstants, sizeof(Constant), constantComp);

	UINT nMaxVSRes = vsRefl? vsDesc.BoundResources : 0;
	UINT nMaxGSRes = gsRefl? gsDesc.BoundResources : 0;
	UINT nMaxTCSRes = tcsRefl ? tcsDesc.BoundResources : 0;
	UINT nMaxTESRes = tesRefl ? tesDesc.BoundResources : 0;
	UINT nMaxPSRes = psRefl? psDesc.BoundResources : 0;
	UINT nMaxCSRes = csRefl? csDesc.BoundResources : 0;

    //Texture, Samples, buffer///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //Vertex Shader//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int maxResources = nMaxVSRes + nMaxGSRes + nMaxPSRes + nMaxCSRes;
	if (maxResources)
	{
		shader.textures = (Sampler *) malloc(maxResources * sizeof(Sampler));
		shader.samplers = (Sampler *) malloc(maxResources * sizeof(Sampler));
		shader.rwStructuredBuffer = (Sampler *) malloc(maxResources * sizeof(Sampler));
		shader.structuredBuffer = (Sampler *) malloc(maxResources * sizeof(Sampler));
		shader.uavTextures = (Sampler *) malloc(maxResources * sizeof(Sampler));
		shader.nTextures = 0;
		shader.nSamplers = 0;
		shader.nRWStructuredBuffers = 0;
		shader.nStructuredBuffers = 0;
		shader.nUavTextures = 0;

		D3D11_SHADER_INPUT_BIND_DESC siDesc;
		for (UINT i = 0; i < nMaxVSRes; i++)
		{
			vsRefl->GetResourceBindingDesc(i, &siDesc);

			if (siDesc.Type == D3D10_SIT_TEXTURE)
			{
				size_t length = strlen(siDesc.Name);
				shader.textures[shader.nTextures].name = CHG_NEW char[length + 1];
				strcpy(shader.textures[shader.nTextures].name, siDesc.Name);
				shader.textures[shader.nTextures].vsIndex = siDesc.BindPoint;
				shader.textures[shader.nTextures].gsIndex = -1;
				shader.textures[shader.nTextures].tcsIndex = -1;
				shader.textures[shader.nTextures].tesIndex = -1;
				shader.textures[shader.nTextures].psIndex = -1;
				shader.textures[shader.nTextures].csIndex = -1;
				shader.nTextures++;
			} 
			else if (siDesc.Type == D3D10_SIT_SAMPLER)
			{
				size_t length = strlen(siDesc.Name);
				shader.samplers[shader.nSamplers].name = CHG_NEW char[length + 1];
				strcpy(shader.samplers[shader.nSamplers].name, siDesc.Name);
				shader.samplers[shader.nSamplers].vsIndex = siDesc.BindPoint;
				shader.samplers[shader.nSamplers].gsIndex = -1;
				shader.samplers[shader.nSamplers].tcsIndex = -1;
				shader.samplers[shader.nSamplers].tesIndex = -1;
				shader.samplers[shader.nSamplers].psIndex = -1;
				shader.samplers[shader.nSamplers].csIndex = -1;
				shader.nSamplers++;
			}
			else if (siDesc.Type == D3D_SIT_UAV_RWSTRUCTURED)
			{
				size_t length = strlen(siDesc.Name);
				shader.rwStructuredBuffer[shader.nRWStructuredBuffers].name = CHG_NEW char[length + 1];
				strcpy(shader.rwStructuredBuffer[shader.nRWStructuredBuffers].name, siDesc.Name);
				shader.rwStructuredBuffer[shader.nRWStructuredBuffers].vsIndex = siDesc.BindPoint;
				shader.rwStructuredBuffer[shader.nRWStructuredBuffers].gsIndex = -1;
				shader.rwStructuredBuffer[shader.nRWStructuredBuffers].tcsIndex = -1;
				shader.rwStructuredBuffer[shader.nRWStructuredBuffers].tesIndex = -1;
				shader.rwStructuredBuffer[shader.nRWStructuredBuffers].psIndex = -1;
				shader.rwStructuredBuffer[shader.nRWStructuredBuffers].csIndex = -1;
				shader.nRWStructuredBuffers++;
			}
			else if (siDesc.Type == D3D_SIT_STRUCTURED)
			{
				size_t length = strlen(siDesc.Name);
				shader.structuredBuffer[shader.nStructuredBuffers].name = CHG_NEW char[length + 1];
				strcpy(shader.structuredBuffer[shader.nStructuredBuffers].name, siDesc.Name);
				shader.structuredBuffer[shader.nStructuredBuffers].vsIndex = siDesc.BindPoint;
				shader.structuredBuffer[shader.nStructuredBuffers].tcsIndex = -1;
				shader.structuredBuffer[shader.nStructuredBuffers].tesIndex = -1;
				shader.structuredBuffer[shader.nStructuredBuffers].gsIndex = -1;
				shader.structuredBuffer[shader.nStructuredBuffers].psIndex = -1;
				shader.structuredBuffer[shader.nStructuredBuffers].csIndex = -1;
				shader.nStructuredBuffers++;
			}
		}

		UINT maxTexture = shader.nTextures;
		UINT maxSampler = shader.nSamplers;
		UINT maxRWStructuredBuffer = shader.nRWStructuredBuffers;
		UINT maxStructuredBuffer = shader.nStructuredBuffers;
		UINT maxUAVTextures = shader.nUavTextures;

		for (UINT i = 0; i < nMaxGSRes; i++)
		{
			gsRefl->GetResourceBindingDesc(i, &siDesc);

			if (siDesc.Type == D3D10_SIT_TEXTURE)
			{
				int merge = -1;
				for (UINT j = 0; j < maxTexture; j++)
				{
					if (strcmp(shader.textures[j].name, siDesc.Name) == 0)
					{
						merge = j;
						break;
					}
				}
				if (merge < 0)
				{
					size_t length = strlen(siDesc.Name);
					shader.textures[shader.nTextures].name = CHG_NEW char[length + 1];
					strcpy(shader.textures[shader.nTextures].name, siDesc.Name);
					shader.textures[shader.nTextures].vsIndex = -1;
					shader.textures[shader.nTextures].gsIndex = siDesc.BindPoint;
					shader.textures[shader.nTextures].tcsIndex = -1;
					shader.textures[shader.nTextures].tesIndex = -1;
					shader.textures[shader.nTextures].psIndex = -1;
					shader.textures[shader.nTextures].csIndex = -1;
					shader.nTextures++;
				} 
				else 
				{
					shader.textures[merge].gsIndex = siDesc.BindPoint;
				}
			} 
			else if (siDesc.Type == D3D10_SIT_SAMPLER)
			{
				int merge = -1;
				for (UINT j = 0; j < maxSampler; j++)
				{
					if (strcmp(shader.samplers[j].name, siDesc.Name) == 0)
					{
						merge = j;
						break;
					}
				}
				if (merge < 0)
				{
					size_t length = strlen(siDesc.Name);
					shader.samplers[shader.nSamplers].name = CHG_NEW char[length + 1];
					strcpy(shader.samplers[shader.nSamplers].name, siDesc.Name);
					shader.samplers[shader.nSamplers].vsIndex = -1;
					shader.samplers[shader.nSamplers].gsIndex = siDesc.BindPoint;
					shader.samplers[shader.nSamplers].tcsIndex = -1;
					shader.samplers[shader.nSamplers].tesIndex = -1;
					shader.samplers[shader.nSamplers].psIndex = -1;
					shader.samplers[shader.nSamplers].csIndex = -1;
					shader.nSamplers++;
				} 
				else 
				{
					shader.samplers[merge].gsIndex = siDesc.BindPoint;
				}
			}
			else if (siDesc.Type == D3D_SIT_UAV_RWSTRUCTURED)
			{
				int merge = -1;
				for (UINT j = 0; j < maxRWStructuredBuffer; j++)
				{
					if (strcmp(shader.rwStructuredBuffer[j].name, siDesc.Name) == 0)
					{
						merge = j;
						break;
					}
				}
				if (merge < 0)
				{
					size_t length = strlen(siDesc.Name);
					shader.rwStructuredBuffer[shader.nRWStructuredBuffers].name = CHG_NEW char[length + 1];
					strcpy(shader.rwStructuredBuffer[shader.nRWStructuredBuffers].name, siDesc.Name);
					shader.rwStructuredBuffer[shader.nRWStructuredBuffers].vsIndex = -1;
					shader.rwStructuredBuffer[shader.nRWStructuredBuffers].tcsIndex = -1;
					shader.rwStructuredBuffer[shader.nRWStructuredBuffers].tesIndex = -1;
					shader.rwStructuredBuffer[shader.nRWStructuredBuffers].gsIndex = siDesc.BindPoint;
					shader.rwStructuredBuffer[shader.nRWStructuredBuffers].psIndex = -1;
					shader.rwStructuredBuffer[shader.nRWStructuredBuffers].csIndex = -1;
					shader.nRWStructuredBuffers++;
				} 
				else 
				{
					shader.rwStructuredBuffer[merge].gsIndex = siDesc.BindPoint;
				}
			}
			else if (siDesc.Type == D3D_SIT_STRUCTURED)
			{
				int merge = -1;
				for (UINT j = 0; j < maxStructuredBuffer; j++)
				{
					if (strcmp(shader.structuredBuffer[j].name, siDesc.Name) == 0)
					{
						merge = j;
						break;
					}
				}
				if (merge < 0)
				{
					size_t length = strlen(siDesc.Name);
					shader.structuredBuffer[shader.nStructuredBuffers].name = CHG_NEW char[length + 1];
					strcpy(shader.structuredBuffer[shader.nStructuredBuffers].name, siDesc.Name);
					shader.structuredBuffer[shader.nStructuredBuffers].vsIndex = -1;
					shader.structuredBuffer[shader.nStructuredBuffers].tcsIndex = -1;
					shader.structuredBuffer[shader.nStructuredBuffers].tesIndex = -1;
					shader.structuredBuffer[shader.nStructuredBuffers].gsIndex = siDesc.BindPoint;
					shader.structuredBuffer[shader.nStructuredBuffers].psIndex = -1;
					shader.structuredBuffer[shader.nStructuredBuffers].csIndex = -1;
					shader.nStructuredBuffers++;
				} 
				else 
				{
					shader.structuredBuffer[merge].gsIndex = siDesc.BindPoint;
				}
			}
		}

		maxTexture = shader.nTextures;
		maxSampler = shader.nSamplers;
		maxRWStructuredBuffer = shader.nRWStructuredBuffers;
		maxStructuredBuffer = shader.nStructuredBuffers;
		maxUAVTextures = shader.nUavTextures;

		// TCS textures bind //////////////////////////////////////////////////////////

		for (UINT i = 0; i < nMaxTCSRes; i++)
		{
			tcsRefl->GetResourceBindingDesc(i, &siDesc);

			if (siDesc.Type == D3D10_SIT_TEXTURE)
			{
				int merge = -1;
				for (UINT j = 0; j < maxTexture; j++)
				{
					if (strcmp(shader.textures[j].name, siDesc.Name) == 0)
					{
						merge = j;
						break;
					}
				}
				if (merge < 0)
				{
					size_t length = strlen(siDesc.Name);
					shader.textures[shader.nTextures].name = CHG_NEW char[length + 1];
					strcpy(shader.textures[shader.nTextures].name, siDesc.Name);
					shader.textures[shader.nTextures].vsIndex = -1;
					shader.textures[shader.nTextures].gsIndex = -1;
					shader.textures[shader.nTextures].tcsIndex = siDesc.BindPoint;
					shader.textures[shader.nTextures].tesIndex = -1;
					shader.textures[shader.nTextures].psIndex = -1;
					shader.textures[shader.nTextures].csIndex = -1;
					shader.nTextures++;
				}
				else
				{
					shader.textures[merge].tcsIndex = siDesc.BindPoint;
				}
			}
			else if (siDesc.Type == D3D10_SIT_SAMPLER)
			{
				int merge = -1;
				for (UINT j = 0; j < maxSampler; j++)
				{
					if (strcmp(shader.samplers[j].name, siDesc.Name) == 0)
					{
						merge = j;
						break;
					}
				}
				if (merge < 0)
				{
					size_t length = strlen(siDesc.Name);
					shader.samplers[shader.nSamplers].name = CHG_NEW char[length + 1];
					strcpy(shader.samplers[shader.nSamplers].name, siDesc.Name);
					shader.samplers[shader.nSamplers].vsIndex = -1;
					shader.samplers[shader.nSamplers].gsIndex = -1;
					shader.samplers[shader.nSamplers].tcsIndex = siDesc.BindPoint;
					shader.samplers[shader.nSamplers].tesIndex = -1;
					shader.samplers[shader.nSamplers].psIndex = -1;
					shader.samplers[shader.nSamplers].csIndex = -1;
					shader.nSamplers++;
				}
				else
				{
					shader.samplers[merge].tcsIndex = siDesc.BindPoint;
				}
			}
		}

		maxTexture = shader.nTextures;
		maxSampler = shader.nSamplers;

		// TCS textures bind //////////////////////////////////////////////////////////

		for (UINT i = 0; i < nMaxTESRes; i++)
		{
			tesRefl->GetResourceBindingDesc(i, &siDesc);

			if (siDesc.Type == D3D10_SIT_TEXTURE)
			{
				int merge = -1;
				for (UINT j = 0; j < maxTexture; j++)
				{
					if (strcmp(shader.textures[j].name, siDesc.Name) == 0)
					{
						merge = j;
						break;
					}
				}
				if (merge < 0)
				{
					size_t length = strlen(siDesc.Name);
					shader.textures[shader.nTextures].name = CHG_NEW char[length + 1];
					strcpy(shader.textures[shader.nTextures].name, siDesc.Name);
					shader.textures[shader.nTextures].vsIndex = -1;
					shader.textures[shader.nTextures].gsIndex = -1;
					shader.textures[shader.nTextures].tcsIndex = -1;
					shader.textures[shader.nTextures].tesIndex = siDesc.BindPoint;
					shader.textures[shader.nTextures].psIndex = -1;
					shader.textures[shader.nTextures].csIndex = -1;
					shader.nTextures++;
				}
				else
				{
					shader.textures[merge].tesIndex = siDesc.BindPoint;
				}
			}
			else if (siDesc.Type == D3D10_SIT_SAMPLER)
			{
				int merge = -1;
				for (UINT j = 0; j < maxSampler; j++)
				{
					if (strcmp(shader.samplers[j].name, siDesc.Name) == 0)
					{
						merge = j;
						break;
					}
				}
				if (merge < 0)
				{
					size_t length = strlen(siDesc.Name);
					shader.samplers[shader.nSamplers].name = CHG_NEW char[length + 1];
					strcpy(shader.samplers[shader.nSamplers].name, siDesc.Name);
					shader.samplers[shader.nSamplers].vsIndex = -1;
					shader.samplers[shader.nSamplers].gsIndex = -1;
					shader.samplers[shader.nSamplers].tcsIndex = -1;
					shader.samplers[shader.nSamplers].tesIndex = siDesc.BindPoint;
					shader.samplers[shader.nSamplers].psIndex = -1;
					shader.samplers[shader.nSamplers].csIndex = -1;
					shader.nSamplers++;
				}
				else
				{
					shader.samplers[merge].tesIndex = siDesc.BindPoint;
				}
			}
		}

		maxTexture = shader.nTextures;
		maxSampler = shader.nSamplers;

		// Pixel Shader ////////////////////////////////////////////////////////////////////////////////////

		for (UINT i = 0; i < nMaxPSRes; i++)
		{
			psRefl->GetResourceBindingDesc(i, &siDesc);

			if (siDesc.Type == D3D10_SIT_TEXTURE)
			{
				int merge = -1;
				for (UINT j = 0; j < maxTexture; j++)
				{
					if (strcmp(shader.textures[j].name, siDesc.Name) == 0)
					{
						merge = j;
						break;
					}
				}
				if (merge < 0)
				{
					if (strstr(siDesc.Name, "[") && strstr(siDesc.Name, "]"))
					{
						bool firstInArray = true;

						size_t length = strlen(siDesc.Name) - 3;
						char* varWithoutBracket = CHG_NEW char[length + 1];
						strncpy(varWithoutBracket, siDesc.Name, length);
						varWithoutBracket[length] = '\0';

						for (UINT j = 0; j < shader.nTextures; j++)
						{
							if (strcmp(shader.textures[j].name, varWithoutBracket) == 0)
							{
								firstInArray = false;
								break;
							}
						}

						if (firstInArray)
						{
							shader.textures[shader.nTextures].name = varWithoutBracket;
						}
						else
						{
							size_t length = strlen(siDesc.Name);
							shader.textures[shader.nTextures].name = CHG_NEW char[length + 1];
							strcpy(shader.textures[shader.nTextures].name, siDesc.Name);
							delete varWithoutBracket;
						}
					}
					else
					{
						size_t length = strlen(siDesc.Name);
						shader.textures[shader.nTextures].name = CHG_NEW char[length + 1];
						strcpy(shader.textures[shader.nTextures].name, siDesc.Name);
					}

					shader.textures[shader.nTextures].vsIndex = -1;
					shader.textures[shader.nTextures].gsIndex = -1;
					shader.textures[shader.nTextures].tcsIndex = -1;
					shader.textures[shader.nTextures].tesIndex = -1;
					shader.textures[shader.nTextures].psIndex = siDesc.BindPoint;
					shader.textures[shader.nTextures].csIndex = -1;
					shader.nTextures++;
				}

				else 
				{
					shader.textures[merge].psIndex = siDesc.BindPoint;
				}
			} 
			else if (siDesc.Type == D3D10_SIT_SAMPLER)
			{
				int merge = -1;
				for (UINT j = 0; j < maxSampler; j++)
				{
					if (strcmp(shader.samplers[j].name, siDesc.Name) == 0)
					{
						merge = j;
						break;
					}
				}
				if (merge < 0)
				{
					size_t length = strlen(siDesc.Name);
					shader.samplers[shader.nSamplers].name = CHG_NEW char[length + 1];
					strcpy(shader.samplers[shader.nSamplers].name, siDesc.Name);
					shader.samplers[shader.nSamplers].vsIndex = -1;
					shader.samplers[shader.nSamplers].gsIndex = -1;
					shader.samplers[shader.nSamplers].tcsIndex = -1;
					shader.samplers[shader.nSamplers].tesIndex = -1;
					shader.samplers[shader.nSamplers].psIndex = siDesc.BindPoint;
					shader.samplers[shader.nSamplers].csIndex = -1;
					shader.nSamplers++;
				} 
				else 
				{
					shader.samplers[merge].psIndex = siDesc.BindPoint;
				}
			}
			else if (siDesc.Type == D3D_SIT_UAV_RWSTRUCTURED)
			{
				int merge = -1;
				for (UINT j = 0; j < maxRWStructuredBuffer; j++)
				{
					if (strcmp(shader.rwStructuredBuffer[j].name, siDesc.Name) == 0)
					{
						merge = j;
						break;
					}
				}
				if (merge < 0)
				{
					if(strstr(siDesc.Name, "[0]"))
					{
						size_t length = strlen(siDesc.Name) - 3;
						shader.rwStructuredBuffer[shader.nRWStructuredBuffers].name = CHG_NEW char[length + 1];
						strncpy(shader.rwStructuredBuffer[shader.nRWStructuredBuffers].name, siDesc.Name, length);
						shader.rwStructuredBuffer[shader.nRWStructuredBuffers].name[length] = '\0';
					}
					else
					{
						size_t length = strlen(siDesc.Name);
						shader.rwStructuredBuffer[shader.nRWStructuredBuffers].name = CHG_NEW char[length + 1];
						strcpy(shader.rwStructuredBuffer[shader.nRWStructuredBuffers].name, siDesc.Name);
					}
						
					shader.rwStructuredBuffer[shader.nRWStructuredBuffers].vsIndex = -1;
					shader.rwStructuredBuffer[shader.nRWStructuredBuffers].tcsIndex = -1;
					shader.rwStructuredBuffer[shader.nRWStructuredBuffers].tesIndex = -1;
					shader.rwStructuredBuffer[shader.nRWStructuredBuffers].gsIndex = -1;
					shader.rwStructuredBuffer[shader.nRWStructuredBuffers].psIndex = siDesc.BindPoint;
					shader.rwStructuredBuffer[shader.nRWStructuredBuffers].csIndex = -1;
					shader.nRWStructuredBuffers++;
				} 
				else 
				{
					shader.rwStructuredBuffer[merge].psIndex = siDesc.BindPoint;
				}
			}
			else if (siDesc.Type == D3D_SIT_STRUCTURED)
			{
				int merge = -1;
				for (UINT j = 0; j < maxStructuredBuffer; j++)
				{
					if (strcmp(shader.structuredBuffer[j].name, siDesc.Name) == 0)
					{
						merge = j;
						break;
					}
				}
				if (merge < 0)
				{
					if(strstr(siDesc.Name, "[") && strstr(siDesc.Name, "]"))
					{
						bool firstInArray = true;

						size_t length = strlen(siDesc.Name) - 3;
						char* varWithoutBracket = CHG_NEW char[length + 1];
						strncpy(varWithoutBracket, siDesc.Name, length);
						varWithoutBracket[length] = '\0';

						for (UINT j = 0; j < shader.nStructuredBuffers; j++)
						{
							if (strcmp(shader.structuredBuffer[j].name, varWithoutBracket) == 0)
							{
								firstInArray = false;
								break;
							}
						}

						if(firstInArray)
						{
							shader.structuredBuffer[shader.nStructuredBuffers].name = varWithoutBracket;
						}
						else
						{
							size_t length = strlen(siDesc.Name);
							shader.structuredBuffer[shader.nStructuredBuffers].name = CHG_NEW char[length + 1];
							strcpy(shader.structuredBuffer[shader.nStructuredBuffers].name, siDesc.Name);
							delete varWithoutBracket;
						}
					}
					else
					{
						size_t length = strlen(siDesc.Name);
						shader.structuredBuffer[shader.nStructuredBuffers].name = CHG_NEW char[length + 1];
						strcpy(shader.structuredBuffer[shader.nStructuredBuffers].name, siDesc.Name);
					}

					shader.structuredBuffer[shader.nStructuredBuffers].vsIndex = -1;
					shader.structuredBuffer[shader.nStructuredBuffers].tcsIndex = -1;
					shader.structuredBuffer[shader.nStructuredBuffers].tesIndex = -1;
					shader.structuredBuffer[shader.nStructuredBuffers].gsIndex = -1;
					shader.structuredBuffer[shader.nStructuredBuffers].psIndex = siDesc.BindPoint;
					shader.structuredBuffer[shader.nStructuredBuffers].csIndex = -1;
					shader.nStructuredBuffers++;
				} 
				else 
				{
					shader.structuredBuffer[merge].psIndex = siDesc.BindPoint;
				}
			}
			else if (siDesc.Type == D3D_SIT_UAV_RWTYPED)
			{
				int merge = -1;
				for (UINT j = 0; j < maxUAVTextures; j++)
				{
					if (strcmp(shader.uavTextures[j].name, siDesc.Name) == 0)
					{
						merge = j;
						break;
					}
				}
				if (merge < 0)
				{
					if (strstr(siDesc.Name, "[0]"))
					{
						size_t length = strlen(siDesc.Name) - 3;
						shader.uavTextures[shader.nUavTextures].name = CHG_NEW char[length + 1];
						strncpy(shader.uavTextures[shader.nUavTextures].name, siDesc.Name, length);
						shader.uavTextures[shader.nUavTextures].name[length] = '\0';
					}
					else
					{
						size_t length = strlen(siDesc.Name);
						shader.uavTextures[shader.nUavTextures].name = CHG_NEW char[length + 1];
						strcpy(shader.uavTextures[shader.nUavTextures].name, siDesc.Name);
					}

					shader.uavTextures[shader.nUavTextures].vsIndex = -1;
					shader.uavTextures[shader.nUavTextures].tcsIndex = -1;
					shader.uavTextures[shader.nUavTextures].tesIndex = -1;
					shader.uavTextures[shader.nUavTextures].gsIndex = -1;
					shader.uavTextures[shader.nUavTextures].psIndex = siDesc.BindPoint;
					shader.uavTextures[shader.nUavTextures].csIndex = -1;
					shader.nUavTextures++;
				}
				else
				{
					shader.uavTextures[merge].psIndex = siDesc.BindPoint;
				}
			}
		}

		maxTexture = shader.nTextures;
		maxSampler = shader.nSamplers;
		maxRWStructuredBuffer = shader.nRWStructuredBuffers;
		maxStructuredBuffer = shader.nStructuredBuffers;
		maxUAVTextures = shader.nUavTextures;

		// Compute Shader ////////////////////////////////////////////////////////////////////////////////////

		for (UINT i = 0; i < nMaxCSRes; i++)
		{
			csRefl->GetResourceBindingDesc(i, &siDesc);

			if (siDesc.Type == D3D10_SIT_TEXTURE)
			{
				int merge = -1;
				for (UINT j = 0; j < maxTexture; j++)
				{
					if (strcmp(shader.textures[j].name, siDesc.Name) == 0)
					{
						merge = j;
						break;
					}
				}
				if (merge < 0)
				{
					size_t length = strlen(siDesc.Name);
					shader.textures[shader.nTextures].name = CHG_NEW char[length + 1];
					strcpy(shader.textures[shader.nTextures].name, siDesc.Name);
					shader.textures[shader.nTextures].vsIndex = -1;
					shader.textures[shader.nTextures].gsIndex = -1;
					shader.textures[shader.nTextures].tcsIndex = -1;
					shader.textures[shader.nTextures].tesIndex = -1;
					shader.textures[shader.nTextures].psIndex = -1;
					shader.textures[shader.nTextures].csIndex = siDesc.BindPoint;
					shader.nTextures++;
				} 
				else 
				{
					shader.textures[merge].csIndex = siDesc.BindPoint;
				}
			} 
			else if (siDesc.Type == D3D10_SIT_SAMPLER)
			{
				int merge = -1;
				for (UINT j = 0; j < maxSampler; j++)
				{
					if (strcmp(shader.samplers[j].name, siDesc.Name) == 0)
					{
						merge = j;
						break;
					}
				}
				if (merge < 0)
				{
					size_t length = strlen(siDesc.Name);
					shader.samplers[shader.nSamplers].name = CHG_NEW char[length + 1];
					strcpy(shader.samplers[shader.nSamplers].name, siDesc.Name);
					shader.samplers[shader.nSamplers].vsIndex = -1;
					shader.samplers[shader.nSamplers].gsIndex = -1;
					shader.samplers[shader.nSamplers].tcsIndex = -1;
					shader.samplers[shader.nSamplers].tesIndex = -1;
					shader.samplers[shader.nSamplers].psIndex = -1;
					shader.samplers[shader.nSamplers].csIndex = siDesc.BindPoint;
					shader.nSamplers++;
				} 
				else 
				{
					shader.samplers[merge].csIndex = siDesc.BindPoint;
				}
			}
			else if (siDesc.Type == D3D_SIT_UAV_RWSTRUCTURED || siDesc.Type == D3D_SIT_UAV_APPEND_STRUCTURED)
			{
				int merge = -1;
				for (UINT j = 0; j < maxRWStructuredBuffer; j++)
				{
					if (strcmp(shader.rwStructuredBuffer[j].name, siDesc.Name) == 0)
					{
						merge = j;
						break;
					}
				}
				if (merge < 0)
				{
					size_t length = strlen(siDesc.Name);
					shader.rwStructuredBuffer[shader.nRWStructuredBuffers].name = CHG_NEW char[length + 1];
					strcpy(shader.rwStructuredBuffer[shader.nRWStructuredBuffers].name, siDesc.Name);
					shader.rwStructuredBuffer[shader.nRWStructuredBuffers].vsIndex = -1;
					shader.rwStructuredBuffer[shader.nRWStructuredBuffers].tcsIndex = -1;
					shader.rwStructuredBuffer[shader.nRWStructuredBuffers].tesIndex = -1;
					shader.rwStructuredBuffer[shader.nRWStructuredBuffers].gsIndex = -1;
					shader.rwStructuredBuffer[shader.nRWStructuredBuffers].psIndex = -1;
					shader.rwStructuredBuffer[shader.nRWStructuredBuffers].csIndex = siDesc.BindPoint;
					shader.nRWStructuredBuffers++;
				} 
				else 
				{
					shader.rwStructuredBuffer[merge].csIndex = siDesc.BindPoint;
				}
			}
			else if (siDesc.Type == D3D_SIT_STRUCTURED)
			{
				int merge = -1;
				for (UINT j = 0; j < maxStructuredBuffer; j++)
				{
					if (strcmp(shader.structuredBuffer[j].name, siDesc.Name) == 0)
					{
						merge = j;
						break;
					}
				}
				if (merge < 0)
				{
					size_t length = strlen(siDesc.Name);
					shader.structuredBuffer[shader.nStructuredBuffers].name = CHG_NEW char[length + 1];
					strcpy(shader.structuredBuffer[shader.nStructuredBuffers].name, siDesc.Name);
					shader.structuredBuffer[shader.nStructuredBuffers].vsIndex = -1;
					shader.structuredBuffer[shader.nStructuredBuffers].tcsIndex = -1;
					shader.structuredBuffer[shader.nStructuredBuffers].tesIndex = -1;
					shader.structuredBuffer[shader.nStructuredBuffers].gsIndex = -1;
					shader.structuredBuffer[shader.nStructuredBuffers].psIndex = -1;
					shader.structuredBuffer[shader.nStructuredBuffers].csIndex = siDesc.BindPoint;
					shader.nStructuredBuffers++;
				} 
				else 
				{
					shader.structuredBuffer[merge].csIndex = siDesc.BindPoint;
				}
			}
			else if (siDesc.Type == D3D_SIT_UAV_RWTYPED)
			{
				int merge = -1;
				for (UINT j = 0; j < maxUAVTextures; j++)
				{
					if (strcmp(shader.uavTextures[j].name, siDesc.Name) == 0)
					{
						merge = j;
						break;
					}
				}
				if (merge < 0)
				{
					if (strstr(siDesc.Name, "[0]"))
					{
						size_t length = strlen(siDesc.Name) - 3;
						shader.uavTextures[shader.nUavTextures].name = CHG_NEW char[length + 1];
						strncpy(shader.uavTextures[shader.nUavTextures].name, siDesc.Name, length);
						shader.uavTextures[shader.nUavTextures].name[length] = '\0';
					}
					else
					{
						size_t length = strlen(siDesc.Name);
						shader.uavTextures[shader.nUavTextures].name = CHG_NEW char[length + 1];
						strcpy(shader.uavTextures[shader.nUavTextures].name, siDesc.Name);
					}

					shader.uavTextures[shader.nUavTextures].vsIndex = -1;
					shader.uavTextures[shader.nUavTextures].gsIndex = -1;
					shader.uavTextures[shader.nUavTextures].tcsIndex = -1;
					shader.uavTextures[shader.nUavTextures].tesIndex = -1;
					shader.uavTextures[shader.nUavTextures].psIndex = -1;
					shader.uavTextures[shader.nUavTextures].csIndex = siDesc.BindPoint;
					shader.nUavTextures++;
				}
				else
				{
					shader.uavTextures[merge].csIndex = siDesc.BindPoint;
				}
			}
		}

		shader.textures = (Sampler *) realloc(shader.textures, shader.nTextures * sizeof(Sampler));
		shader.samplers = (Sampler *) realloc(shader.samplers, shader.nSamplers * sizeof(Sampler));
		shader.rwStructuredBuffer = (Sampler *) realloc(shader.rwStructuredBuffer, shader.nRWStructuredBuffers * sizeof(Sampler));
		shader.structuredBuffer = (Sampler *) realloc(shader.structuredBuffer, shader.nStructuredBuffers * sizeof(Sampler));
		shader.uavTextures = (Sampler *)realloc(shader.uavTextures, shader.nUavTextures * sizeof(Sampler));
		qsort(shader.textures, shader.nTextures, sizeof(Sampler), samplerComp);
		qsort(shader.samplers, shader.nSamplers, sizeof(Sampler), samplerComp);
		qsort(shader.rwStructuredBuffer, shader.nRWStructuredBuffers, sizeof(Sampler), samplerComp);
		qsort(shader.structuredBuffer, shader.nStructuredBuffers, sizeof(Sampler), samplerComp);
		qsort(shader.uavTextures, shader.nUavTextures, sizeof(Sampler), samplerComp);
	}

	if (vsRefl) vsRefl->Release();
	if (gsRefl) gsRefl->Release();
	if (tcsRefl) tcsRefl->Release();
	if (tesRefl) tesRefl->Release();
	if (psRefl) psRefl->Release();
	if (csRefl) csRefl->Release();

	shaders.push_back(shader);

	return shaders.size() - 1;
}

bool Direct3D11Renderer::getShaderWorkGroupSize(const int shaderID, UINT* localGroupSize)
{
	return true;
}

D3D11_USAGE imageBufferUsages[] =
{
	D3D11_USAGE_DEFAULT,
    D3D11_USAGE_IMMUTABLE,
    D3D11_USAGE_DYNAMIC,
    D3D11_USAGE_STAGING
};

StructuredBufferID Direct3D11Renderer::createIndirectDrawArgBuffer()
{
	StructuredBuffer structuredBuffer;

	structuredBuffer.flags = D3D11_BIND_UNORDERED_ACCESS;
	structuredBuffer.size = sizeof(UINT)* 5;
	structuredBuffer.sizePerElement = sizeof(UINT);
	structuredBuffer.bufferUsage = D3D11_USAGE_DEFAULT;

	D3D11_BUFFER_DESC bufferDesc;

	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.ByteWidth = sizeof(UINT)* 5;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;
	bufferDesc.StructureByteStride = sizeof(float);

	if (FAILED(m_pd3dDevice->CreateBuffer(&bufferDesc, NULL, &structuredBuffer.structuredBuffer)))
		return -1;

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	ZeroMemory(&uavDesc, sizeof(uavDesc));
	uavDesc.Format = DXGI_FORMAT_R32_UINT;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.Flags = 0;
	uavDesc.Buffer.NumElements = 5;

	if (FAILED(m_pd3dDevice->CreateUnorderedAccessView(structuredBuffer.structuredBuffer, &uavDesc, &structuredBuffer.structuredBufferViewRW)))
		return -1;

	structuredBuffer.structuredBufferView = NULL;

	structuredBuffers.push_back(structuredBuffer);

	return structuredBuffers.size() - 1;
}

StructuredBufferID Direct3D11Renderer::createStructuredBuffer(const UINT elementSize, const UINT nElements, const bool bIsCPUWritable, const bool bIsGPUWritable, const UINT UAVFlags, const void *data)
{
	StructuredBuffer structuredBuffer;

	memset(&structuredBuffer, 0, sizeof(StructuredBuffer));

	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.ByteWidth = elementSize * nElements;
	bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bufferDesc.StructureByteStride = elementSize;
	structuredBuffer.size = elementSize * nElements;
	structuredBuffer.sizePerElement = elementSize;
	structuredBuffer.flags = UAVFlags;

	if ((!bIsCPUWritable) && (!bIsGPUWritable))
	{
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		structuredBuffer.bufferUsage = D3D11_USAGE_IMMUTABLE;
	}
	else if (bIsCPUWritable && (!bIsGPUWritable))
	{
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		structuredBuffer.bufferUsage = D3D11_USAGE_DYNAMIC;
	}
	else if ((!bIsCPUWritable) && bIsGPUWritable)
	{
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.BindFlags = (D3D11_BIND_SHADER_RESOURCE |
			D3D11_BIND_UNORDERED_ACCESS);
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		structuredBuffer.bufferUsage = D3D11_USAGE_DEFAULT;
	}
	else
	{
		assert((!(bIsCPUWritable && bIsGPUWritable)));
	}

	D3D11_SUBRESOURCE_DATA bufferInitData;
	ZeroMemory((&bufferInitData), sizeof(bufferInitData));
	bufferInitData.pSysMem = data;

	//Create Buffer
	if (FAILED(m_pd3dDevice->CreateBuffer(&bufferDesc, (data) ? (&bufferInitData) : NULL, &structuredBuffer.structuredBuffer)))
		return -1;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.ElementWidth = nElements;

	if (FAILED(m_pd3dDevice->CreateShaderResourceView(structuredBuffer.structuredBuffer, &srvDesc, &structuredBuffer.structuredBufferView)))
	{
		return -1;
	}

	if (bIsGPUWritable)
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
		ZeroMemory((&uavDesc), sizeof(uavDesc));
		uavDesc.Format = DXGI_FORMAT_UNKNOWN;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		uavDesc.Buffer.NumElements = nElements;
		uavDesc.Buffer.Flags = UAVFlags;
		if (FAILED(m_pd3dDevice->CreateUnorderedAccessView(structuredBuffer.structuredBuffer, &uavDesc, &structuredBuffer.structuredBufferViewRW)))
			return -1;
	}

	structuredBuffers.push_back(structuredBuffer);

	return structuredBuffers.size() - 1;
}

VertexFormatID Direct3D11Renderer::addVertexFormat(const FormatDesc *formatDesc, const UINT nAttribs, const ShaderID shader)
{
	static const DXGI_FORMAT formats[][4] = {
		DXGI_FORMAT_R32_FLOAT, DXGI_FORMAT_R32G32_FLOAT, DXGI_FORMAT_R32G32B32_FLOAT, DXGI_FORMAT_R32G32B32A32_FLOAT,
		DXGI_FORMAT_R16_FLOAT, DXGI_FORMAT_R16G16_FLOAT, DXGI_FORMAT_UNKNOWN,         DXGI_FORMAT_R16G16B16A16_FLOAT,
		DXGI_FORMAT_R8_UNORM,  DXGI_FORMAT_R8G8_UNORM,   DXGI_FORMAT_UNKNOWN,         DXGI_FORMAT_R8G8B8A8_UNORM,
	};

	static const char *semantics[] = 
	{
		NULL,
		"Position",
		"Texcoord",
		"Normal",
		"Tangent",
		"Binormal",
		"Color",
	};


	int index[7];
	memset(index, 0, sizeof(index));

	VertexFormat vf;
	memset(vf.vertexSize, 0, sizeof(vf.vertexSize));

	D3D11_INPUT_ELEMENT_DESC *desc = CHG_NEW D3D11_INPUT_ELEMENT_DESC[nAttribs];

	// Fill the vertex element array
	for (UINT i = 0; i < nAttribs; i++)
	{
		int stream = formatDesc[i].stream;
		int size = formatDesc[i].size;
		desc[i].InputSlot = stream;
		desc[i].AlignedByteOffset = vf.vertexSize[stream];
		desc[i].SemanticName = semantics[formatDesc[i].type];
		desc[i].SemanticIndex = index[formatDesc[i].type]++;
		desc[i].Format = formats[formatDesc[i].format][size - 1];
		desc[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		desc[i].InstanceDataStepRate = 0;

		vf.vertexSize[stream] += size * getFormatSize(formatDesc[i].format);
	}

	HRESULT hr = m_pd3dDevice->CreateInputLayout(desc, nAttribs, shaders[shader].inputSignature->GetBufferPointer(), shaders[shader].inputSignature->GetBufferSize(), &vf.inputLayout);
	delete [] desc;

	if (FAILED(hr))
	{
		printf("Couldn't create vertex declaration");
		return VF_NONE;
	}

	vertexFormats.push_back(vf);

	return vertexFormats.size() - 1;
}

D3D11_USAGE usage[] = 
{
	D3D11_USAGE_IMMUTABLE,
	D3D11_USAGE_DEFAULT,
	D3D11_USAGE_DYNAMIC,
};

VertexBufferID Direct3D11Renderer::addVertexBuffer(const long size, const BufferAccess bufferAccess, const void *data)
{
	VertexBuffer vb;
	vb.size = size;

	D3D11_BUFFER_DESC desc;
	ZeroMemory( &desc, sizeof(desc) );
	desc.Usage = usage[bufferAccess];
	desc.ByteWidth = size;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = (bufferAccess == DYNAMIC)? D3D11_CPU_ACCESS_WRITE : 0;
	desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vbData;
	ZeroMemory( &vbData, sizeof(vbData) );
	vbData.pSysMem = data;
	vbData.SysMemPitch = 0;
	vbData.SysMemSlicePitch = 0;

	if (FAILED(m_pd3dDevice->CreateBuffer(&desc, data? &vbData : NULL, &vb.vertexBuffer)))
	{
        printf("Couldn't create vertex buffer");
		return VB_NONE;
	}

    int id = vertexBuffers.add(vb);

    CHG_INFO("Create new Vertex Buffer: " + std::to_string(id));

    return id;
}

IndexBufferID Direct3D11Renderer::addIndexBuffer(const UINT nIndices, const UINT indexSize, const BufferAccess bufferAccess, const void *data)
{
	IndexBuffer ib;
	ib.indexSize = indexSize;
	ib.nIndices = nIndices;

	D3D11_BUFFER_DESC desc;
	ZeroMemory( &desc, sizeof(desc) );
	desc.Usage = usage[bufferAccess];
	desc.ByteWidth = nIndices * indexSize;
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	desc.CPUAccessFlags = (bufferAccess == DYNAMIC)? D3D11_CPU_ACCESS_WRITE : 0;
	desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA ibData;
	ZeroMemory( &ibData, sizeof(ibData) );
	ibData.pSysMem = data;
	ibData.SysMemPitch = 0;
	ibData.SysMemSlicePitch = 0;

	if (FAILED(m_pd3dDevice->CreateBuffer(&desc, data? &ibData : NULL, &ib.indexBuffer)))
	{
        printf("Couldn't create vertex buffer");
		return IB_NONE;
	}
	
    int id = indexBuffers.add(ib);

    CHG_INFO("Create new Index Buffer: " + std::to_string(id));

    return id;
}

ObjectBufferID Direct3D11Renderer::createObjectBuffer(const ObjectBufferType type, const UINT objectSize, const UINT nObjects, const BufferAccess bufferAccess, const void *data)
{
	return -1;
}

D3D11_FILTER filters[] = 
{
	D3D11_FILTER_MIN_MAG_MIP_POINT,
	D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT,
	D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT,
	D3D11_FILTER_MIN_MAG_MIP_LINEAR,
	D3D11_FILTER_ANISOTROPIC,
	D3D11_FILTER_ANISOTROPIC,
};

D3D11_TEXTURE_ADDRESS_MODE address_modes[] = 
{
	D3D11_TEXTURE_ADDRESS_WRAP,
	D3D11_TEXTURE_ADDRESS_CLAMP,
	D3D11_TEXTURE_ADDRESS_BORDER,
	D3D11_TEXTURE_ADDRESS_MIRROR,
};

SamplerStateID Direct3D11Renderer::addSamplerState(const Filter filter, const AddressMode s, const AddressMode t, const AddressMode r, const float lod, const UINT maxAniso, const int compareFunc, const float *border_color)
{
	SamplerState samplerState;

	D3D11_SAMPLER_DESC desc;
	desc.Filter = filters[filter];
	if (compareFunc){
		desc.Filter = (D3D11_FILTER) (desc.Filter | 0x80);
	}
	desc.ComparisonFunc = (D3D11_COMPARISON_FUNC) compareFunc;
	desc.AddressU = address_modes[s];
	desc.AddressV = address_modes[t];
	desc.AddressW = address_modes[r];
	desc.MipLODBias = lod;
	desc.MaxAnisotropy = hasAniso(filter)? maxAniso : 1;
	if (border_color)
	{
		desc.BorderColor[0] = border_color[0];
		desc.BorderColor[1] = border_color[1];
		desc.BorderColor[2] = border_color[2];
		desc.BorderColor[3] = border_color[3];
	}
	else
	{
		desc.BorderColor[0] = 0;
		desc.BorderColor[1] = 0;
		desc.BorderColor[2] = 0;
		desc.BorderColor[3] = 0;
	}
	desc.MinLOD = 0;
	desc.MaxLOD = hasMipmaps(filter)? D3D10_FLOAT32_MAX : 0;

	if (FAILED(m_pd3dDevice->CreateSamplerState(&desc, &samplerState.samplerState)))
	{
		printf("Couldn't create samplerstate");
		return SS_NONE;
	}

	samplerStates.push_back(samplerState);

	return samplerStates.size() - 1;
}

BlendStateID Direct3D11Renderer::addBlendState(const int srcFactorRGB, const int destFactorRGB, const int srcFactorAlpha, const int destFactorAlpha, const int blendModeRGB, const int blendModeAlpha, const int mask, const bool alphaToCoverage)
{
	BlendState blendState;

	BOOL blendEnable =
		srcFactorRGB != D3D11_BLEND_ONE || destFactorRGB != D3D11_BLEND_ZERO ||
		srcFactorAlpha != D3D11_BLEND_ONE || destFactorAlpha != D3D11_BLEND_ZERO;

	D3D11_BLEND_DESC desc;
	ZeroMemory( &desc, sizeof(desc) );

	desc.AlphaToCoverageEnable = (BOOL) alphaToCoverage;
	desc.RenderTarget[0].BlendOp = (D3D11_BLEND_OP) blendModeAlpha;
	desc.RenderTarget[0].SrcBlend = (D3D11_BLEND) srcFactorRGB;
	desc.RenderTarget[0].DestBlend = (D3D11_BLEND) destFactorRGB;
	desc.RenderTarget[0].BlendOpAlpha = (D3D11_BLEND_OP) blendModeAlpha;
	desc.RenderTarget[0].SrcBlendAlpha = (D3D11_BLEND) srcFactorAlpha;
	desc.RenderTarget[0].DestBlendAlpha = (D3D11_BLEND) destFactorAlpha;

	memset(&desc.RenderTarget[0].BlendEnable, 0, sizeof(desc.RenderTarget[0].BlendEnable));
	memset(&desc.RenderTarget[0].RenderTargetWriteMask, 0, sizeof(desc.RenderTarget[0].RenderTargetWriteMask));
	desc.RenderTarget[0].BlendEnable = blendEnable;
	desc.RenderTarget[0].RenderTargetWriteMask = mask;

	if (FAILED(m_pd3dDevice->CreateBlendState(&desc, &blendState.blendState)))
	{
		printf("Couldn't create blendstate");
		return BS_NONE;
	}

	blendStates.push_back(blendState);

	return blendStates.size() - 1;
}

DepthStateID Direct3D11Renderer::addDepthState(const bool depthTest, const bool depthWrite, const int depthFunc, const bool stencilTest, const UINT8 stencilReadMask, const UINT8 stencilWriteMask,
		const int stencilFuncFront, const int stencilFuncBack, const int stencilFailFront, const int stencilFailBack,
		const int depthFailFront, const int depthFailBack, const int stencilPassFront, const int stencilPassBack)
{

	DepthState depthState;

	D3D11_DEPTH_STENCIL_DESC desc;
	desc.DepthEnable = (BOOL) depthTest;
	desc.DepthWriteMask = depthWrite? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
	desc.DepthFunc = (D3D11_COMPARISON_FUNC) depthFunc;
	desc.StencilEnable = (BOOL) stencilTest;
	desc.StencilReadMask  = stencilReadMask;
	desc.StencilWriteMask = stencilWriteMask;
	desc.BackFace. StencilFunc = (D3D11_COMPARISON_FUNC) stencilFuncBack;
	desc.FrontFace.StencilFunc = (D3D11_COMPARISON_FUNC) stencilFuncFront;
	desc.BackFace. StencilDepthFailOp = (D3D11_STENCIL_OP) depthFailBack;
	desc.FrontFace.StencilDepthFailOp = (D3D11_STENCIL_OP) depthFailFront;
	desc.BackFace. StencilFailOp = (D3D11_STENCIL_OP) stencilFailBack;
	desc.FrontFace.StencilFailOp = (D3D11_STENCIL_OP) stencilFailFront;
	desc.BackFace. StencilPassOp = (D3D11_STENCIL_OP) stencilPassBack;
	desc.FrontFace.StencilPassOp = (D3D11_STENCIL_OP) stencilPassFront;

	if (FAILED(m_pd3dDevice->CreateDepthStencilState(&desc, &depthState.dsState)))
	{
		printf("Couldn't create depthstate");
		return DS_NONE;
	}

	depthStates.push_back(depthState);

	return depthStates.size() - 1;
}

RasterizerStateID Direct3D11Renderer::addRasterizerState(const int cullMode, const int fillMode, const bool multiSample, const bool scissor, const float depthBias, const float slopeDepthBias)
{
	RasterizerState rasterizerState;

	D3D11_RASTERIZER_DESC desc;
	desc.CullMode = (D3D11_CULL_MODE) cullMode;
	desc.FillMode = (D3D11_FILL_MODE) fillMode;
	desc.FrontCounterClockwise = FALSE;
	desc.DepthBias = (INT) depthBias;
	desc.DepthBiasClamp = 0.0f;
	desc.SlopeScaledDepthBias = slopeDepthBias;
	desc.AntialiasedLineEnable = FALSE;
	desc.DepthClipEnable = TRUE;
	desc.MultisampleEnable = (BOOL) multiSample;
	desc.ScissorEnable = (BOOL) scissor;

	if (FAILED(m_pd3dDevice->CreateRasterizerState(&desc, &rasterizerState.rsState)))
	{
		printf("Couldn't create rasterizerstate");
		return RS_NONE;
	}

	rasterizerStates.push_back(rasterizerState);

	return rasterizerStates.size() - 1;
}

FontID Direct3D11Renderer::addFont(char *rawBuffer, UINT rawSize, const UINT size, const SamplerStateID samplerState)
{
	/*TTFFont font;

	SDL_RWops *rw = SDL_RWFromMem(rawBuffer, rawSize);

	font.pfont = TTF_OpenFontRW(rw, 1, size);

	if (!font.pfont)
	{
		printf("Dont load font");
		return -1;
	}

	font.sampleState = samplerState;
	font.size = size;

	TTFFonts.push_back(font);*/

	return TTFFonts.size() - 1;
}

FontID Direct3D11Renderer::addFont(const char *ttfFontFile, const UINT size, const SamplerStateID samplerState)
{
	/*TTFFont font;

	font.pfont = TTF_OpenFont(ttfFontFile, size);

	if (!font.pfont)
	{
		printf("Dont load font");
		return -1;
	}

	font.sampleState = samplerState;
	font.size = size;

	TTFFonts.push_back(font);*/

	return TTFFonts.size() - 1;
}

const Sampler *getSampler(const Sampler *samplers, const int count, const char *name)
{
	int minSampler = 0;
	int maxSampler = count - 1;

	// Do a quick lookup in the sorted table with a binary search
	while (minSampler <= maxSampler)
	{
		int currSampler = (minSampler + maxSampler) >> 1;
        int res = strcmp(name, samplers[currSampler].name);

		if (res == 0)
		{
			return samplers + currSampler;
		} 
		else if (res > 0)
		{
            minSampler = currSampler + 1;
		}
		else 
		{
            maxSampler = currSampler - 1;
		}
	}

	return NULL;
}

void Direct3D11Renderer::setTexture(const char *textureName, const TextureID texture)
{
	//ASSERT(selectedShader != SHADER_NONE);
	if(selectedShader == SHADER_NONE)
		return;

	const Sampler *s = getSampler(shaders[selectedShader].textures, shaders[selectedShader].nTextures, textureName);
	if (s)
	{
		if (s->vsIndex >= 0)
		{
            selectedTexturesVS[s->vsIndex].id = texture;
            selectedTexturesVS[s->vsIndex].type = TEXTURE_RESOURCE;
			selectedTextureSlicesVS[s->vsIndex] = NO_SLICE;
		}
		if (s->gsIndex >= 0)
		{
			selectedTexturesGS[s->gsIndex].id = texture;
            selectedTexturesGS[s->gsIndex].type = TEXTURE_RESOURCE;
			selectedTextureSlicesGS[s->gsIndex] = NO_SLICE;
		}
		if (s->tcsIndex >= 0)
		{
			selectedTexturesTCS[s->tcsIndex].id = texture;
			selectedTexturesTCS[s->tcsIndex].type = TEXTURE_RESOURCE;
			selectedTextureSlicesTCS[s->tcsIndex] = NO_SLICE;
		}
		if (s->tesIndex >= 0)
		{
			selectedTexturesTES[s->tesIndex].id = texture;
			selectedTexturesTES[s->tesIndex].type = TEXTURE_RESOURCE;
			selectedTextureSlicesTES[s->tesIndex] = NO_SLICE;
		}
		if (s->psIndex >= 0)
		{
			selectedTexturesPS[s->psIndex].id = texture;
            selectedTexturesPS[s->psIndex].type = TEXTURE_RESOURCE;
			selectedTextureSlicesPS[s->psIndex] = NO_SLICE;
		}
        if (s->csIndex >= 0)
		{
			selectedTexturesCS[s->csIndex].id = texture;
            selectedTexturesCS[s->csIndex].type = TEXTURE_RESOURCE;
			selectedTextureSlicesCS[s->csIndex] = NO_SLICE;
		}
	}
#ifdef _DEBUG
	else 
	{
		char str[256];
		sprintf(str, "Invalid texture \"%s\"", textureName);
		//outputDebugString(str);
	}
#endif
}

void Direct3D11Renderer::setTexture(const int textureUnit, const TextureID texture)
{
	//NOTHING
}

void Direct3D11Renderer::setTextureArray(const char* textureName, const TextureID *textures, const int n_textures)
{
	if (selectedShader == SHADER_NONE)
		return;

	const Sampler *s = getSampler(shaders[selectedShader].textures, shaders[selectedShader].nTextures, textureName);
	if (s)
	{
		/*if (s->vsIndex >= 0)
		{
			for (int i = offset; i < arraySize + offset; i++)
			{
				selectedRWStructuredBufferVS[s->vsIndex + i].id = texture[i];
				selectedRWStructuredBufferVS[s->vsIndex + i].mipLevel = mipLevel[i];
			}
		}

		if (s->gsIndex >= 0)
		{
			for (int i = offset; i < arraySize + offset; i++)
			{
				selectedRWStructuredBufferGS[s->gsIndex + i].id = texture[i];
				selectedRWStructuredBufferGS[s->gsIndex + i].mipLevel = mipLevel[i];
			}
		}*/
		if (s->psIndex >= 0)
		{
			for (int i = 0; i < n_textures; i++)
			{
				selectedTexturesPS[s->psIndex + i].id = textures[i];
				selectedTexturesPS[s->psIndex + i].type = TEXTURE_RESOURCE;
			}
		}
		/*if (s->csIndex >= 0)
		{
			for (int i = offset; i < arraySize + offset; i++)
			{
				selectedRWStructuredBufferCS[s->csIndex + i].id = texture[i];
				selectedRWStructuredBufferCS[s->csIndex + i].mipLevel = mipLevel[i];
			}
		}*/
	}
}

void Direct3D11Renderer::setTexture(const char *textureName, const TextureID texture, const SamplerStateID samplerState)
{
	//ASSERT(selectedShader != SHADER_NONE);
	if(selectedShader == SHADER_NONE)
		return;

	const Sampler *s = getSampler(shaders[selectedShader].textures, shaders[selectedShader].nTextures, textureName);
	if (s)
	{
		if (s->vsIndex >= 0)
		{
			selectedTexturesVS[s->vsIndex].id = texture;
            selectedTexturesVS[s->vsIndex].type = TEXTURE_RESOURCE;
			selectedTextureSlicesVS[s->vsIndex] = NO_SLICE;
			selectedSamplerStatesVS[s->vsIndex] = samplerState;
		}
		if (s->gsIndex >= 0)
		{
			selectedTexturesGS[s->gsIndex].id = texture;
            selectedTexturesGS[s->gsIndex].type = TEXTURE_RESOURCE;
			selectedTextureSlicesGS[s->gsIndex] = NO_SLICE;
			selectedSamplerStatesGS[s->gsIndex] = samplerState;
		}
		if (s->tcsIndex >= 0)
		{
			selectedTexturesTCS[s->tcsIndex].id = texture;
			selectedTexturesTCS[s->tcsIndex].type = TEXTURE_RESOURCE;
			selectedTextureSlicesTCS[s->tcsIndex] = NO_SLICE;
			selectedSamplerStatesTCS[s->tcsIndex] = samplerState;
		}
		if (s->tesIndex >= 0)
		{
			selectedTexturesTES[s->tesIndex].id = texture;
			selectedTexturesTES[s->tesIndex].type = TEXTURE_RESOURCE;
			selectedTextureSlicesTES[s->tesIndex] = NO_SLICE;
			selectedSamplerStatesTES[s->tesIndex] = samplerState;
		}
		if (s->psIndex >= 0)
		{
			selectedTexturesPS[s->psIndex].id = texture;
            selectedTexturesPS[s->psIndex].type = TEXTURE_RESOURCE;
			selectedTextureSlicesPS[s->psIndex] = NO_SLICE;
			selectedSamplerStatesPS[s->psIndex] = samplerState;
		}
        if (s->csIndex >= 0)
		{
			selectedTexturesCS[s->csIndex].id = texture;
            selectedTexturesCS[s->csIndex].type = TEXTURE_RESOURCE;
			selectedTextureSlicesCS[s->csIndex] = NO_SLICE;
			selectedSamplerStatesCS[s->csIndex] = samplerState;
		}
	}
#ifdef _DEBUG
	else 
	{
		char str[256];
		sprintf(str, "Invalid texture \"%s\"", textureName);
		//outputDebugString(str);
	}
#endif
}

void Direct3D11Renderer::setTextureSlice(const char *textureName, const TextureID texture, const int slice)
{
	//ASSERT(selectedShader != SHADER_NONE);
	if(selectedShader == SHADER_NONE)
		return;

	const Sampler *s = getSampler(shaders[selectedShader].textures, shaders[selectedShader].nTextures, textureName);
	if (s)
	{
		if (s->vsIndex >= 0)
		{
			selectedTexturesVS[s->vsIndex].id = texture;
            selectedTexturesVS[s->vsIndex].type = TEXTURE_RESOURCE;
			selectedTextureSlicesVS[s->vsIndex] = slice;
		}
		if (s->gsIndex >= 0)
		{
			selectedTexturesGS[s->gsIndex].id = texture;
            selectedTexturesGS[s->gsIndex].type = TEXTURE_RESOURCE;
			selectedTextureSlicesGS[s->gsIndex] = slice;
		}
		if (s->tcsIndex >= 0)
		{
			selectedTexturesTCS[s->tcsIndex].id = texture;
			selectedTexturesTCS[s->tcsIndex].type = TEXTURE_RESOURCE;
			selectedTextureSlicesTCS[s->tcsIndex] = slice;
		}
		if (s->tesIndex >= 0)
		{
			selectedTexturesTES[s->tesIndex].id = texture;
			selectedTexturesTES[s->tesIndex].type = TEXTURE_RESOURCE;
			selectedTextureSlicesTES[s->tesIndex] = slice;
		}
		if (s->psIndex >= 0)
		{
			selectedTexturesPS[s->psIndex].id = texture;
            selectedTexturesPS[s->psIndex].type = TEXTURE_RESOURCE;
			selectedTextureSlicesPS[s->psIndex] = slice;
		}
        if (s->csIndex >= 0)
		{
			selectedTexturesCS[s->csIndex].id = texture;
            selectedTexturesCS[s->csIndex].type = TEXTURE_RESOURCE;
			selectedTextureSlicesCS[s->csIndex] = slice;
		}
	}
#ifdef _DEBUG
	else 
	{
		char str[256];
		sprintf(str, "Invalid texture \"%s\"", textureName);
		//outputDebugString(str);
	}
#endif
}


bool Direct3D11Renderer::fillSRV(ID3D11ShaderResourceView **dest, int &min, int &max, const BufferResourceEntry selectedTextures[], BufferResourceEntry currentTextures[], const int selectedTextureSlices[], int currentTextureSlices[], const Texture *textures, const StructuredBuffer *structuredBuffers)
{
	min = 0;
	do 
	{
        if (selectedTextures[min].id != currentTextures[min].id || selectedTextureSlices[min] != currentTextureSlices[min] || selectedTextures[min].type != selectedTextures[min].type)
		{
			max = MAX_TEXTUREUNIT;
			do 
			{
				max--;
			} while (selectedTextures[max].id == currentTextures[max].id && selectedTextureSlices[max] == currentTextureSlices[max] && selectedTextures[min].type == selectedTextures[min].type);

			for (int i = min; i <= max; i++)
			{
				if (selectedTextures[i].id != TEXTURE_NONE)
				{
					if (selectedTextureSlices[i] == NO_SLICE)
					{
                        if (selectedTextures[i].type == TEXTURE_RESOURCE)
                        {
                            *dest++ = textures[selectedTextures[i].id].srv;
                        }
                        else if(selectedTextures[i].type == STRUCTURED_BUFFER_RESOURCE)
                        {
                            *dest++ = structuredBuffers[selectedTextures[i].id].structuredBufferView;
                        }
					} 
					else 
					{                        
				        *dest++ = textures[selectedTextures[i].id].srvArray[selectedTextureSlices[i]];                        
					}
				} 
				else 
				{
					*dest++ = NULL;
				}
				currentTextures[i] = selectedTextures[i];
				currentTextureSlices[i] = selectedTextureSlices[i];
			}
			return true;
		}
		min++;
	} while (min < MAX_TEXTUREUNIT);

	return false;
}

void Direct3D11Renderer::applyTextures()
{
	ID3D11ShaderResourceView *srViews[MAX_TEXTUREUNIT];

	int min, max;
    if (fillSRV(srViews, min, max, selectedTexturesVS, currentTexturesVS, selectedTextureSlicesVS, currentTextureSlicesVS, textures.data(), structuredBuffers.data()))
	{
		m_pImmediateContext->VSSetShaderResources(min, max - min + 1, srViews);
	}
	if (fillSRV(srViews, min, max, selectedTexturesGS, currentTexturesGS, selectedTextureSlicesGS, currentTextureSlicesGS, textures.data(), structuredBuffers.data()))
	{
		m_pImmediateContext->GSSetShaderResources(min, max - min + 1, srViews);
	}
	if (fillSRV(srViews, min, max, selectedTexturesTCS, currentTexturesTCS, selectedTextureSlicesTCS, currentTextureSlicesTCS, textures.data(), structuredBuffers.data()))
	{
		m_pImmediateContext->HSSetShaderResources(min, max - min + 1, srViews);
	}
	if (fillSRV(srViews, min, max, selectedTexturesTES, currentTexturesTES, selectedTextureSlicesTES, currentTextureSlicesTES, textures.data(), structuredBuffers.data()))
	{
		m_pImmediateContext->DSSetShaderResources(min, max - min + 1, srViews);
	}
	if (fillSRV(srViews, min, max, selectedTexturesPS, currentTexturesPS, selectedTextureSlicesPS, currentTextureSlicesPS, textures.data(), structuredBuffers.data()))
	{
		m_pImmediateContext->PSSetShaderResources(min, max - min + 1, srViews);
	}
    if (fillSRV(srViews, min, max, selectedTexturesCS, currentTexturesCS, selectedTextureSlicesCS, currentTextureSlicesCS, textures.data(), structuredBuffers.data()))
	{
		m_pImmediateContext->CSSetShaderResources(min, max - min + 1, srViews);
	}
}

void Direct3D11Renderer::setImageBuffer(const char *ImageBufferName, const TextureID texture, const int mipLevel, const FORMAT formatImageBuffer, const ImageBufferAccess imageBufferAccess)
{
	//ASSERT(selectedShader != SHADER_NONE);
	if (selectedShader == SHADER_NONE)
		return;

	const Sampler *RWs = getSampler(shaders[selectedShader].uavTextures, shaders[selectedShader].nUavTextures, ImageBufferName);
	if (RWs)
	{
		if (RWs->vsIndex >= 0)
		{
			selectedRWStructuredBufferVS[RWs->vsIndex].id = texture;
			selectedRWStructuredBufferVS[RWs->vsIndex].mipLevel = mipLevel;
		}
			
		if (RWs->gsIndex >= 0)
		{
			selectedRWStructuredBufferGS[RWs->gsIndex].id = texture;
			selectedRWStructuredBufferGS[RWs->gsIndex].mipLevel = mipLevel;
		}
		if (RWs->psIndex >= 0)
		{
			selectedRWStructuredBufferPS[RWs->psIndex].id = texture;
			selectedRWStructuredBufferPS[RWs->psIndex].mipLevel = mipLevel;
		}
		if (RWs->csIndex >= 0)
		{
			selectedRWStructuredBufferCS[RWs->csIndex].id = texture;
			selectedRWStructuredBufferCS[RWs->csIndex].mipLevel = mipLevel;
		}
	}
}

void Direct3D11Renderer::setImageBufferArray(const char *ImageBufferName, const TextureID *texture, const int *mipLevel, const FORMAT *formatImageBuffer, const ImageBufferAccess *imageBufferAccess, const UINT offset, const UINT arraySize)
{
	if (selectedShader == SHADER_NONE)
		return;

	const Sampler *RWs = getSampler(shaders[selectedShader].uavTextures, shaders[selectedShader].nUavTextures, ImageBufferName);
	if (RWs)
	{
		if (RWs->vsIndex >= 0)
		{
			for (UINT i = offset; i < arraySize + offset; i++)
			{
				selectedRWStructuredBufferVS[RWs->vsIndex + i].id = texture[i];
				selectedRWStructuredBufferVS[RWs->vsIndex + i].mipLevel = mipLevel[i];
			}
		}

		if (RWs->gsIndex >= 0)
		{
			for (UINT i = offset; i < arraySize + offset; i++)
			{
				selectedRWStructuredBufferGS[RWs->gsIndex + i].id = texture[i];
				selectedRWStructuredBufferGS[RWs->gsIndex + i].mipLevel = mipLevel[i];
			}
		}
		if (RWs->psIndex >= 0)
		{
			for (UINT i = offset; i < arraySize + offset; i++)
			{
				selectedRWStructuredBufferPS[RWs->psIndex + i].id = texture[i];
				selectedRWStructuredBufferPS[RWs->psIndex + i].mipLevel = mipLevel[i];
			}			
		}
		if (RWs->csIndex >= 0)
		{
			for (UINT i = offset; i < arraySize + offset; i++)
			{
				selectedRWStructuredBufferCS[RWs->csIndex + i].id = texture[i];
				selectedRWStructuredBufferCS[RWs->csIndex + i].mipLevel = mipLevel[i];
			}
		}
	}
}

void Direct3D11Renderer::setStructuredBuffer(const char *structuredBufferName, const StructuredBufferID structuredBufferID)
{
	//ASSERT(selectedShader != SHADER_NONE);
	if(selectedShader == SHADER_NONE)
		return;

	const Sampler *RWs = getSampler(shaders[selectedShader].rwStructuredBuffer, shaders[selectedShader].nRWStructuredBuffers, structuredBufferName);
	if (RWs)
	{
		if (RWs->vsIndex >= 0) selectedRWStructuredBufferVS[RWs->vsIndex].id = structuredBufferID;
		if (RWs->gsIndex >= 0) selectedRWStructuredBufferGS[RWs->gsIndex].id = structuredBufferID;
		if (RWs->psIndex >= 0) selectedRWStructuredBufferPS[RWs->psIndex].id = structuredBufferID;
		if (RWs->csIndex >= 0) selectedRWStructuredBufferCS[RWs->csIndex].id = structuredBufferID;
	}

	const Sampler *s = getSampler(shaders[selectedShader].structuredBuffer, shaders[selectedShader].nStructuredBuffers, structuredBufferName);
	if (s)
	{
		if (s->vsIndex >= 0)
        {
            selectedTexturesVS[s->vsIndex].id = structuredBufferID;
            selectedTexturesVS[s->vsIndex].type = STRUCTURED_BUFFER_RESOURCE;
        }
		if (s->gsIndex >= 0) 
        {
            selectedTexturesGS[s->gsIndex].id = structuredBufferID;
            selectedTexturesGS[s->gsIndex].type = STRUCTURED_BUFFER_RESOURCE;
        }
		if (s->psIndex >= 0) 
        {
            selectedTexturesPS[s->psIndex].id = structuredBufferID;
            selectedTexturesPS[s->psIndex].type = STRUCTURED_BUFFER_RESOURCE;
        }
        if (s->csIndex >= 0) 
        {
            selectedTexturesCS[s->csIndex].id = structuredBufferID;
            selectedTexturesCS[s->csIndex].type = STRUCTURED_BUFFER_RESOURCE;
        }
	}
}

void Direct3D11Renderer::setStructuredBufferArray(const char *structuredBufferName, const StructuredBufferID *structuredBufferID, int offset, int n_StructuredBuffers)
{
	if(selectedShader == SHADER_NONE)
	return;

	const Sampler *RWs = getSampler(shaders[selectedShader].rwStructuredBuffer, shaders[selectedShader].nRWStructuredBuffers, structuredBufferName);
	if (RWs)
	{
		if (RWs->vsIndex >= 0) 
			for(int i=offset; i < n_StructuredBuffers + offset; i++)
				selectedRWStructuredBufferVS[RWs->vsIndex + i].id = structuredBufferID[i];
		
		if (RWs->gsIndex >= 0) 
			for(int i=offset; i < n_StructuredBuffers + offset; i++)
				selectedRWStructuredBufferGS[RWs->gsIndex + i].id = structuredBufferID[i];

		if (RWs->psIndex >= 0) 
			for(int i=offset; i < n_StructuredBuffers + offset; i++)
				selectedRWStructuredBufferPS[RWs->psIndex + i].id = structuredBufferID[i];

        if (RWs->csIndex >= 0) 
			for(int i=offset; i < n_StructuredBuffers + offset; i++)
				selectedRWStructuredBufferCS[RWs->csIndex + i].id = structuredBufferID[i];
	}

	const Sampler *s = getSampler(shaders[selectedShader].structuredBuffer, shaders[selectedShader].nStructuredBuffers, structuredBufferName);
	if (s)
	{
		if (s->vsIndex >= 0) 
			for(int i=offset; i < n_StructuredBuffers + offset; i++)
            {
				selectedTexturesVS[s->vsIndex + i].id = structuredBufferID[i];
                selectedTexturesVS[s->vsIndex + i].type = STRUCTURED_BUFFER_RESOURCE;
            }

		if (s->gsIndex >= 0) 
			for(int i=offset; i < n_StructuredBuffers + offset; i++)
            {
				selectedTexturesGS[s->gsIndex + i].id = structuredBufferID[i];
                selectedTexturesGS[s->gsIndex + i].type = STRUCTURED_BUFFER_RESOURCE;
            }

		if (s->psIndex >= 0) 
			for(int i=offset; i < n_StructuredBuffers + offset; i++)
            {
				selectedTexturesPS[s->psIndex + i].id = structuredBufferID[i];
                selectedTexturesPS[s->psIndex + i].type = STRUCTURED_BUFFER_RESOURCE;
            }

        if (s->csIndex >= 0) 
			for(int i=offset; i < n_StructuredBuffers + offset; i++)
            {
				selectedTexturesCS[s->csIndex + i].id = structuredBufferID[i];
                selectedTexturesCS[s->csIndex + i].type = STRUCTURED_BUFFER_RESOURCE;
            }
	}
}

bool Direct3D11Renderer::fillSB(ID3D11ShaderResourceView **dest, int &min, int &max, const StructuredBufferID selectedStructuredBuffer[], StructuredBufferID currentStructuredBuffer[], const StructuredBuffer *structuredBuffers)
{
	min = 0;
	do 
	{
		if (selectedStructuredBuffer[min] != currentStructuredBuffer[min])
		{
			max = MAX_STRUCTUREBUFFERUNIT;
			do 
			{
				max--;
			} while (selectedStructuredBuffer[max] == currentStructuredBuffer[max]);

			for (int i = min; i <= max; i++)
			{
				if (selectedStructuredBuffer[i] != STB_NONE)
				{
					*dest++ = structuredBuffers[selectedStructuredBuffer[i]].structuredBufferView;
				}
				else 
				{
					*dest++ = NULL;
				}
				currentStructuredBuffer[i] = selectedStructuredBuffer[i];
			}
			return true;
		}
		min++;
	} while (min < MAX_STRUCTUREBUFFERUNIT);

	return false;
}

bool Direct3D11Renderer::fillRSB(ID3D11UnorderedAccessView **dest, int &min, int &max, const ImageSlot selectedRWStructuredBuffer[], ImageSlot currentRWStructuredBuffer[], const StructuredBuffer *structuredBuffers, const Texture *textures)
{
	min = 0;
	do 
	{
		if (selectedRWStructuredBuffer[min].id != currentRWStructuredBuffer[min].id || selectedRWStructuredBuffer[min].mipLevel != currentRWStructuredBuffer[min].mipLevel)
		{
			max = MAX_RWSTRUCTUREBUFFERUNIT;
			do 
			{
				max--;
			} while (selectedRWStructuredBuffer[max].id == currentRWStructuredBuffer[max].id && selectedRWStructuredBuffer[max].mipLevel == currentRWStructuredBuffer[max].mipLevel);

			for (int i = min; i <= max; i++)
			{
				if (selectedRWStructuredBuffer[i].mipLevel == -1)
				{
					if (selectedRWStructuredBuffer[i].id != STB_NONE)
					{
						*dest++ = structuredBuffers[selectedRWStructuredBuffer[i].id].structuredBufferViewRW;
					}
					else
					{
						*dest++ = NULL;
					}
					currentRWStructuredBuffer[i] = selectedRWStructuredBuffer[i];
					currentRWStructuredBuffer[i].mipLevel = -1;
				}
				else
				{
					if (selectedRWStructuredBuffer[i].id != STB_NONE)
					{
						*dest++ = textures[selectedRWStructuredBuffer[i].id].uav[selectedRWStructuredBuffer[i].mipLevel];
					}
					else
					{
						*dest++ = NULL;
					}
					currentRWStructuredBuffer[i] = selectedRWStructuredBuffer[i];
				}
			}
			return true;
		}
		min++;
	} while (min < MAX_RWSTRUCTUREBUFFERUNIT);

	return false;
}

void Direct3D11Renderer::applyStructuredBuffers()
{
	/*ID3D11ShaderResourceView *buffers[MAX_STRUCTUREBUFFERUNIT];

	int min, max;
	if (fillSB(buffers, min, max, selectedTexturesVS, currentTexturesVS, structuredBuffers.data()))
	{
		m_pImmediateContext->VSSetShaderResources(min, max - min + 1, buffers);
	}
	if (fillSB(buffers, min, max, selectedTexturesGS, currentTexturesGS, structuredBuffers.data()))
	{
		m_pImmediateContext->GSSetShaderResources(min, max - min + 1, buffers);
	}
	if (fillSB(buffers, min, max, selectedTexturesPS, currentTexturesPS, structuredBuffers.data()))
	{
		m_pImmediateContext->PSSetShaderResources(min, max - min + 1, buffers);
	}
    if (fillSB(buffers, min, max, selectedTexturesCS, currentTexturesCS, structuredBuffers.data()))
	{
		m_pImmediateContext->CSSetShaderResources(min, max - min + 1, buffers);
	}*/
}

void Direct3D11Renderer::applyRWStructuredBuffers()
{
	ID3D11UnorderedAccessView *buffers[MAX_RWSTRUCTUREBUFFERUNIT];

	UINT initialCounter = 0;

	int min, max;
	if (fillRSB(buffers, min, max, selectedRWStructuredBufferVS, currentRWStructuredBufferVS, structuredBuffers.data(), textures.data()))
	{
		//m_pImmediateContext->VSSetUnorderedAccessViews(min, max - min + 1, buffers); ----> DX11_1
	}
	if (fillRSB(buffers, min, max, selectedRWStructuredBufferGS, currentRWStructuredBufferGS, structuredBuffers.data(), textures.data()))
	{
		//m_pImmediateContext->GSSetUnorderedAccessViews(min, max - min + 1, buffers); ----> DX11_1
	}
	if (fillRSB(buffers, min, max, selectedRWStructuredBufferPS, currentRWStructuredBufferPS, structuredBuffers.data(), textures.data()))
	{
		m_pImmediateContext->OMSetRenderTargetsAndUnorderedAccessViews(D3D11_KEEP_RENDER_TARGETS_AND_DEPTH_STENCIL, NULL, NULL, min, max - min + 1, buffers, NULL);
	}
	if (fillRSB(buffers, min, max, selectedRWStructuredBufferCS, currentRWStructuredBufferCS, structuredBuffers.data(), textures.data()))
	{
		m_pImmediateContext->CSSetUnorderedAccessViews(min, max - min + 1, buffers, &initialCounter);
	}
}

void Direct3D11Renderer::resetPixelUAV()
{
    for(int i=0; i < MAX_RWSTRUCTUREBUFFERUNIT; i++)
    {
		selectedRWStructuredBufferPS[i].id = STB_NONE;
        selectedRWStructuredBufferPS[i].mipLevel = -1;
    }

    ID3D11UnorderedAccessView *buffers[MAX_RWSTRUCTUREBUFFERUNIT];
	int min, max;

	if (fillRSB(buffers, min, max, selectedRWStructuredBufferPS, currentRWStructuredBufferPS, structuredBuffers.data(), textures.data()))
	{
		m_pImmediateContext->OMSetRenderTargetsAndUnorderedAccessViews(D3D11_KEEP_RENDER_TARGETS_AND_DEPTH_STENCIL, NULL, NULL, min, max - min + 1, buffers, NULL);
	}
}

bool Direct3D11Renderer::hasTessellation(ShaderID shaderID)
{
	if (shaders[shaderID].hullShader != NULL && shaders[shaderID].domainShader != NULL)
		return true;

	return false;
}

void Direct3D11Renderer::setSamplerState(const char *samplerName, const SamplerStateID samplerState)
{
	//ASSERT(selectedShader != SHADER_NONE);
	if(selectedShader == SHADER_NONE)
		return;

	const Sampler *s = getSampler(shaders[selectedShader].samplers, shaders[selectedShader].nSamplers, samplerName);
	if (s)
	{
		if (s->vsIndex >= 0) selectedSamplerStatesVS[s->vsIndex] = samplerState;
		if (s->gsIndex >= 0) selectedSamplerStatesGS[s->gsIndex] = samplerState;
		if (s->tcsIndex >= 0) selectedSamplerStatesTCS[s->tcsIndex] = samplerState;
		if (s->tesIndex >= 0) selectedSamplerStatesTES[s->tesIndex] = samplerState;
		if (s->psIndex >= 0) selectedSamplerStatesPS[s->psIndex] = samplerState;
        if (s->csIndex >= 0) selectedSamplerStatesCS[s->csIndex] = samplerState;
	}
#ifdef _DEBUG
	else 
	{
		char str[256];
		sprintf(str, "Invalid samplerstate \"%s\"", samplerName);
		//outputDebugString(str);
	}
#endif
}

bool Direct3D11Renderer::fillSS(ID3D11SamplerState **dest, int &min, int &max, const SamplerStateID selectedSamplerStates[], SamplerStateID currentSamplerStates[], const SamplerState *samplerStates)
{
	min = 0;
	do 
	{
		if (selectedSamplerStates[min] != currentSamplerStates[min])
		{
			max = MAX_SAMPLERSTATE;
			do 
			{
				max--;
			} while (selectedSamplerStates[max] == currentSamplerStates[max]);

			for (int i = min; i <= max; i++)
			{
				if (selectedSamplerStates[i] != SS_NONE)
				{
					*dest++ = samplerStates[selectedSamplerStates[i]].samplerState;
				}
				else 
				{
					*dest++ = NULL;
				}
				currentSamplerStates[i] = selectedSamplerStates[i];
			}
			return true;
		}
		min++;
	} while (min < MAX_SAMPLERSTATE);

	return false;
}

void Direct3D11Renderer::applySamplerStates()
{
	ID3D11SamplerState *samplers[MAX_SAMPLERSTATE];

	int min, max;
	if (fillSS(samplers, min, max, selectedSamplerStatesVS, currentSamplerStatesVS, samplerStates.data()))
	{
		m_pImmediateContext->VSSetSamplers(min, max - min + 1, samplers);
	}
	if (fillSS(samplers, min, max, selectedSamplerStatesGS, currentSamplerStatesGS, samplerStates.data()))
	{
		m_pImmediateContext->GSSetSamplers(min, max - min + 1, samplers);
	}
	if (fillSS(samplers, min, max, selectedSamplerStatesTCS, currentSamplerStatesTCS, samplerStates.data()))
	{
		m_pImmediateContext->HSSetSamplers(min, max - min + 1, samplers);
	}
	if (fillSS(samplers, min, max, selectedSamplerStatesTES, currentSamplerStatesTES, samplerStates.data()))
	{
		m_pImmediateContext->DSSetSamplers(min, max - min + 1, samplers);
	}
	if (fillSS(samplers, min, max, selectedSamplerStatesPS, currentSamplerStatesPS, samplerStates.data()))
	{
		m_pImmediateContext->PSSetSamplers(min, max - min + 1, samplers);
	}
    if (fillSS(samplers, min, max, selectedSamplerStatesCS, currentSamplerStatesCS, samplerStates.data()))
	{
		m_pImmediateContext->CSSetSamplers(min, max - min + 1, samplers);
	}
}

void Direct3D11Renderer::setShaderConstantRaw(const char *name, const void *data, const int size)
{
	int minConstant = 0;
	int maxConstant = shaders[selectedShader].nConstants - 1;
	Constant *constants = shaders[selectedShader].constants;

	// Do a quick lookup in the sorted table with a binary search
	while (minConstant <= maxConstant)
	{
		int currConstant = (minConstant + maxConstant) >> 1;
		int res = strcmp(name, constants[currConstant].name);
		if (res == 0)
		{
			Constant *c = constants + currConstant;

			if (c->vsData)
			{
				if (memcmp(c->vsData, data, size))
				{
					memcpy(c->vsData, data, size);
					shaders[selectedShader].vsDirty[c->vsBuffer] = true;
				}
			}
			if (c->gsData)
			{
				if (memcmp(c->gsData, data, size))
				{
					memcpy(c->gsData, data, size);
					shaders[selectedShader].gsDirty[c->gsBuffer] = true;
				}
			}
			if (c->tcsData)
			{
				if (memcmp(c->tcsData, data, size))
				{
					memcpy(c->tcsData, data, size);
					shaders[selectedShader].hsDirty[c->tcsBuffer] = true;
				}
			}
			if (c->tesData)
			{
				if (memcmp(c->tesData, data, size))
				{
					memcpy(c->tesData, data, size);
					shaders[selectedShader].dsDirty[c->tesBuffer] = true;
				}
			}
			if (c->psData)
			{
				if (memcmp(c->psData, data, size))
				{
					memcpy(c->psData, data, size);
					shaders[selectedShader].psDirty[c->psBuffer] = true;
				}
			}
			if (c->csData)
			{
				if (memcmp(c->csData, data, size))
				{
					memcpy(c->csData, data, size);
					shaders[selectedShader].csDirty[c->csBuffer] = true;
				}
			}

			return;

		} 
		else if (res > 0)
		{
			minConstant = currConstant + 1;
		} 
		else 
		{
			maxConstant = currConstant - 1;
		}
	}

#ifdef _DEBUG
	char str[256];
	sprintf(str, "Invalid constant \"%s\"", name);
	//outputDebugString(str);
#endif
}

void Direct3D11Renderer::applyConstants()
{
	if (currentShader != SHADER_NONE)
	{
		Shader *shader = &shaders[currentShader];

		for (UINT i = 0; i < shader->nVSCBuffers; i++)
		{
			if (shader->vsDirty[i])
			{
				m_pImmediateContext->UpdateSubresource(shader->vsConstants[i], 0, NULL, shader->vsConstMem[i], 0, 0);
				shader->vsDirty[i] = false;
			}
		}
		for (UINT i = 0; i < shader->nGSCBuffers; i++)
		{
			if (shader->gsDirty[i])
			{
				m_pImmediateContext->UpdateSubresource(shader->gsConstants[i], 0, NULL, shader->gsConstMem[i], 0, 0);
				shader->gsDirty[i] = false;
			}
		}
		for (UINT i = 0; i < shader->nHSCBuffers; i++)
		{
			if (shader->hsDirty[i])
			{
				m_pImmediateContext->UpdateSubresource(shader->hsConstants[i], 0, NULL, shader->hsConstMem[i], 0, 0);
				shader->hsDirty[i] = false;
			}
		}
		for (UINT i = 0; i < shader->nDSCBuffers; i++)
		{
			if (shader->dsDirty[i])
			{
				m_pImmediateContext->UpdateSubresource(shader->dsConstants[i], 0, NULL, shader->dsConstMem[i], 0, 0);
				shader->dsDirty[i] = false;
			}
		}
		for (UINT i = 0; i < shader->nPSCBuffers; i++)
		{
			if (shader->psDirty[i])
			{
				m_pImmediateContext->UpdateSubresource(shader->psConstants[i], 0, NULL, shader->psConstMem[i], 0, 0);
				shader->psDirty[i] = false;
			}
		}
        for (UINT i = 0; i < shader->nCSCBuffers; i++)
		{
			if (shader->csDirty[i])
			{
				m_pImmediateContext->UpdateSubresource(shader->csConstants[i], 0, NULL, shader->csConstMem[i], 0, 0);
				shader->csDirty[i] = false;
			}
		}
	}
}

void Direct3D11Renderer::changeRenderTargets(const TextureID *colorRTs, const UINT nRenderTargets, const TextureID depthRT, const int depthSlice, const int *slices, const UINT flags)
{
    //Reset UAV bound in pixel shader
    resetPixelUAV();

	// Reset bound textures
	for (int i = 0; i < MAX_TEXTUREUNIT; i++)
	{
		selectedTexturesVS[i].id = TEXTURE_NONE;
		selectedTexturesGS[i].id = TEXTURE_NONE;
		selectedTexturesPS[i].id = TEXTURE_NONE;
		selectedTexturesCS[i].id = TEXTURE_NONE;

        selectedTexturesVS[i].type = TEXTURE_RESOURCE;
		selectedTexturesGS[i].type = TEXTURE_RESOURCE;
		selectedTexturesPS[i].type = TEXTURE_RESOURCE;
		selectedTexturesCS[i].type = TEXTURE_RESOURCE;
	}
	applyTextures();

	ID3D11RenderTargetView *rtv[16];
	ID3D11DepthStencilView *dsv;

	if (depthRT == FB_DEPTH)
	{
		dsv = depthBufferDSV;
	} 
	else if (depthRT == TEXTURE_NONE)
	{
		dsv = NULL;
	} 
	else if (depthSlice == NO_SLICE)
	{
		dsv = textures[depthRT].dsv;
	} 
	else 
	{
		dsv = textures[depthRT].dsvArray[depthSlice];
	}
	currentDepthRT = depthRT;
	currentDepthSlice = depthSlice;

	for (UINT i = 0; i < nRenderTargets; i++)
	{
		TextureID rt = colorRTs[i];
		int slice = NO_SLICE;
		if (slices == NULL || slices[i] == NO_SLICE)
		{
			if (rt == FB_COLOR)
			{
				rtv[i] = backBufferRTV;
			} 
			else 
			{
				rtv[i] = textures[rt].rtv;
			}
		} 
		else 
		{
			slice = slices[i];
			rtv[i] = textures[rt].rtvArray[slice];
		}

		currentColorRT[i] = rt;
		currentColorRTSlice[i] = slice;
	}

	for (UINT i = nRenderTargets; i < MAX_MRTS; i++)
	{
		currentColorRT[i] = TEXTURE_NONE;
		currentColorRTSlice[i] = NO_SLICE;
	}

	m_pImmediateContext->OMSetRenderTargets(nRenderTargets, rtv, dsv);
	m_uNumRenderTargets = nRenderTargets;

	D3D11_VIEWPORT vp;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;

	if (nRenderTargets > 0)
	{
		TextureID rt = colorRTs[0];
		if (rt == FB_COLOR)
		{
			vp.Width  = (float)viewportWidth;
			vp.Height = (float)viewportHeight;
		} 
		else 
		{
			if (flags & DEFAULT_SIZE)
			{
				vp.Width = (float)textures[rt].width;
				vp.Height = (float)textures[rt].height;
			}
			else
			{
				vp.Width = (float)viewportWidth;
				vp.Height = (float)viewportHeight;
				vp.TopLeftX = (float)viewportPosX;
				vp.TopLeftY = (float)viewportPosY;
			}
		}
	} 
	else if (depthRT != TEXTURE_NONE)
	{
		if (flags & DEFAULT_SIZE)
		{
			vp.Width = (float)textures[depthRT].width;
			vp.Height = (float)textures[depthRT].height;
		}
		else
		{
			vp.Width = (float)viewportWidth;
			vp.Height = (float)viewportHeight;
			vp.TopLeftX = (float)viewportPosX;
			vp.TopLeftY = (float)viewportPosY;
		}
	}

	m_pImmediateContext->RSSetViewports(1, &vp);
}

void Direct3D11Renderer::changeToMainFramebuffer()
{
    //Reset UAV bound in pixel shader
    resetPixelUAV();

	// Reset bound textures
	for (int i = 0; i < MAX_TEXTUREUNIT; i++)
	{
		selectedTexturesVS[i].id = TEXTURE_NONE;
		selectedTexturesGS[i].id = TEXTURE_NONE;
		selectedTexturesPS[i].id = TEXTURE_NONE;
        selectedTexturesCS[i].id = TEXTURE_NONE;

        selectedTexturesVS[i].type = TEXTURE_RESOURCE;
		selectedTexturesGS[i].type = TEXTURE_RESOURCE;
		selectedTexturesPS[i].type = TEXTURE_RESOURCE;
        selectedTexturesCS[i].type = TEXTURE_RESOURCE;
	}
	applyTextures();

	m_pImmediateContext->OMSetRenderTargets(1, &backBufferRTV, depthBufferDSV);

	D3D11_VIEWPORT vp = { 0, 0, (float)viewportWidth, (float)viewportHeight, 0.0f, 1.0f };
	m_pImmediateContext->RSSetViewports(1, &vp);

	currentColorRT[0] = FB_COLOR;
	currentColorRTSlice[0] = NO_SLICE;

	for (UINT i = 1; i < MAX_MRTS; i++)
	{
		currentColorRT[i] = TEXTURE_NONE;
		currentColorRTSlice[i] = NO_SLICE;
	}
	currentDepthRT = FB_DEPTH;
	currentDepthSlice = NO_SLICE;
}

void Direct3D11Renderer::changeShader(const ShaderID shaderID)
{
	if (shaderID != currentShader)
	{
		if (shaderID == SHADER_NONE)
		{
			m_pImmediateContext->VSSetShader(NULL, NULL, NULL);
			m_pImmediateContext->GSSetShader(NULL, NULL, NULL);
			m_pImmediateContext->HSSetShader(NULL, NULL, NULL);
			m_pImmediateContext->DSSetShader(NULL, NULL, NULL);
			m_pImmediateContext->PSSetShader(NULL, NULL, NULL);
			m_pImmediateContext->CSSetShader(NULL, NULL, NULL);
		} 
		else 
		{
			m_pImmediateContext->VSSetShader(shaders[shaderID].vertexShader, NULL, NULL);
			m_pImmediateContext->GSSetShader(shaders[shaderID].geometryShader, NULL, NULL);
			m_pImmediateContext->HSSetShader(shaders[shaderID].hullShader, NULL, NULL);
			m_pImmediateContext->DSSetShader(shaders[shaderID].domainShader, NULL, NULL);
			m_pImmediateContext->PSSetShader(shaders[shaderID].pixelShader, NULL, NULL);
			m_pImmediateContext->CSSetShader(shaders[shaderID].computeShader, NULL, NULL);

			if (shaders[shaderID].nVSCBuffers) m_pImmediateContext->VSSetConstantBuffers(0, shaders[shaderID].nVSCBuffers, shaders[shaderID].vsConstants);
			if (shaders[shaderID].nGSCBuffers) m_pImmediateContext->GSSetConstantBuffers(0, shaders[shaderID].nGSCBuffers, shaders[shaderID].gsConstants);
			if (shaders[shaderID].nHSCBuffers) m_pImmediateContext->HSSetConstantBuffers(0, shaders[shaderID].nHSCBuffers, shaders[shaderID].hsConstants);
			if (shaders[shaderID].nDSCBuffers) m_pImmediateContext->DSSetConstantBuffers(0, shaders[shaderID].nDSCBuffers, shaders[shaderID].dsConstants);
			if (shaders[shaderID].nPSCBuffers) m_pImmediateContext->PSSetConstantBuffers(0, shaders[shaderID].nPSCBuffers, shaders[shaderID].psConstants);
            if (shaders[shaderID].nCSCBuffers) m_pImmediateContext->CSSetConstantBuffers(0, shaders[shaderID].nCSCBuffers, shaders[shaderID].csConstants);
		}

		currentShader = shaderID;
	}
}

void Direct3D11Renderer::changeVertexFormat(const VertexFormatID vertexFormatID)
{
	if (vertexFormatID != currentVertexFormat)
	{
		if (vertexFormatID == VF_NONE)
		{
			m_pImmediateContext->IASetInputLayout(NULL);
		} 
		else 
		{
			m_pImmediateContext->IASetInputLayout(vertexFormats[vertexFormatID].inputLayout);
		}

		currentVertexFormat = vertexFormatID;
	}
}
//---------------------------------------------------------------------------------------------------->POR AQUI
/*void Direct3D11Renderer::changeVertexBuffer(const int stream, const VertexBufferID vertexBufferID, const intptr offset)
{
	if (vertexBufferID != currentVertexBuffers[stream] || offset != currentOffsets[stream])
	{
		UINT strides[1];
		UINT offsets[1];
		if (vertexBufferID == VB_NONE)
		{
			strides[0] = 0;
			offsets[0] = 0;
			ID3D11Buffer *null[] = { NULL };
			m_pImmediateContext->IASetVertexBuffers(stream, 1, null, strides, offsets);
		} 
		else 
		{
			strides[0] = vertexFormats[currentVertexFormat].vertexSize[stream];
			offsets[0] = (UINT) offset;
			m_pImmediateContext->IASetVertexBuffers(stream, 1, &vertexBuffers[vertexBufferID].vertexBuffer, strides, offsets);
		}

		currentVertexBuffers[stream] = vertexBufferID;
		currentOffsets[stream] = offset;
	}
}*/

void Direct3D11Renderer::changeVertexBuffer(UINT StartSlot, UINT NumBuffers, const VertexBufferID* vertexBufferID, const UINT* offset)
{
	if (vertexBufferID[0] != currentVertexBuffers[0] || offset[0] != currentOffsets[0] || vertexBufferID[1] != currentVertexBuffers[1] || offset[1] != currentOffsets[1] || 
		vertexBufferID[2] != currentVertexBuffers[2] || offset[2] != currentOffsets[2] || vertexBufferID[3] != currentVertexBuffers[3] || offset[3] != currentOffsets[3] || 
		vertexBufferID[4] != currentVertexBuffers[4] || offset[4] != currentOffsets[4] || vertexBufferID[5] != currentVertexBuffers[5] || offset[5] != currentOffsets[5] || 
		vertexBufferID[6] != currentVertexBuffers[6] || offset[6] != currentOffsets[6] || vertexBufferID[7] != currentVertexBuffers[7] || offset[7] != currentOffsets[7])
	{
		UINT strides[8];
		UINT offsets[8];
		ID3D11Buffer* ptrVertexBuffers[8];

		for(UINT i=0; i<NumBuffers; i++)
		{
			if (vertexBufferID[i] == VB_NONE)
			{
				strides[i] = 0;
				offsets[i] = 0;
				ptrVertexBuffers[i] = NULL;
				//m_pImmediateContext->IASetVertexBuffers(i, 1, null, strides, offsets);
			} 
			else 
			{
				strides[i] = vertexFormats[currentVertexFormat].vertexSize[i];
				offsets[i] = (UINT) offset[i];
				ptrVertexBuffers[i] = vertexBuffers[vertexBufferID[i]].vertexBuffer;
			}

			currentVertexBuffers[i] = vertexBufferID[i];
			currentOffsets[i] = offset[i];

		}

		m_pImmediateContext->IASetVertexBuffers(0, NumBuffers, ptrVertexBuffers, strides, offsets);
		
	}
}

void Direct3D11Renderer::changeIndexBuffer(const IndexBufferID indexBufferID)
{
	if (indexBufferID != currentIndexBuffer)
	{
		if (indexBufferID == IB_NONE)
		{
			m_pImmediateContext->IASetIndexBuffer(NULL, DXGI_FORMAT_UNKNOWN, 0);
		} 
		else 
		{
			DXGI_FORMAT format = indexBuffers[indexBufferID].indexSize < 4? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
			m_pImmediateContext->IASetIndexBuffer(indexBuffers[indexBufferID].indexBuffer, format, 0);
		}

		currentIndexBuffer = indexBufferID;
	}
}

void Direct3D11Renderer::changeBlendState(const BlendStateID blendState, const UINT sampleMask)
{
	if (blendState != currentBlendState || sampleMask != currentSampleMask)
	{
		if (blendState == BS_NONE)
		{
			m_pImmediateContext->OMSetBlendState(NULL, Vec4(0, 0, 0, 0), sampleMask);
		} 
		else 
		{
			m_pImmediateContext->OMSetBlendState(blendStates[blendState].blendState, Vec4(0, 0, 0, 0), sampleMask);
		}

		currentBlendState = blendState;
		currentSampleMask = sampleMask;
	}
}

void Direct3D11Renderer::changeDepthState(const DepthStateID depthState, const UINT stencilRef)
{
	if (depthState != currentDepthState || stencilRef != currentStencilRef)
	{
		if (depthState == DS_NONE)
		{
			m_pImmediateContext->OMSetDepthStencilState(NULL, stencilRef);
		} 
		else 
		{
			m_pImmediateContext->OMSetDepthStencilState(depthStates[depthState].dsState, stencilRef);
		}

		currentDepthState = depthState;
		currentStencilRef = stencilRef;
	}
}

void Direct3D11Renderer::changeRasterizerState(const RasterizerStateID rasterizerState)
{
	if (rasterizerState != currentRasterizerState)
	{
		if (rasterizerState == RS_NONE)
		{
			m_pImmediateContext->RSSetState(NULL);
		} 
		else 
		{
			m_pImmediateContext->RSSetState(rasterizerStates[rasterizerState].rsState);
		}

		currentRasterizerState = rasterizerState;
	}
}

//Queries///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
QueryID Direct3D11Renderer::createQuery()
{
	Query query;
	D3D11_QUERY_DESC queryDesc;

	ZeroMemory(&queryDesc, sizeof(D3D11_QUERY_DESC));
	queryDesc.Query = D3D11_QUERY_OCCLUSION;

	if (FAILED(m_pd3dDevice->CreateQuery(&queryDesc, &query.ppQuery)))
	{
		CHG_WARNING("Create Query Error");
		return -1;
	}

	query.result = 0;

	queries.push_back(query);

	return queries.size() - 1;
}

void Direct3D11Renderer::beginQuery(QueryID id)
{
	m_pImmediateContext->Begin(queries[id].ppQuery);
}

void Direct3D11Renderer::endQuery(QueryID id)
{
	m_pImmediateContext->End(queries[id].ppQuery);
}

bool Direct3D11Renderer::queryResultAvailable(QueryID id)
{
	if (FAILED(m_pImmediateContext->GetData(queries[id].ppQuery, &queries[id].result, queries[id].ppQuery->GetDataSize(), 0)))
	{
		return false;
	}

	return true;
}

unsigned long long Direct3D11Renderer::getResultQuery(QueryID id)
{
	return queries[id].result;
}

void Direct3D11Renderer::deleteQueries()
{
	for (UINT i = 0; i < queries.size(); i++)
	{
		Query query = queries[i];

		if (query.ppQuery)
			query.ppQuery->Release();
	}

	queries.clear();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Direct3D11Renderer::clear(const bool clearColor, const bool clearDepth, const bool clearStencil, const float *color, const float depth, const UINT stencil)
{
	if (clearColor)
	{
		if (currentColorRT[0] == FB_COLOR)
		{
			m_pImmediateContext->ClearRenderTargetView(backBufferRTV, color);
		}

		for (int i = 0; i < MAX_MRTS; i++)
		{
			if (currentColorRT[i] >= 0)
			{
				if (currentColorRTSlice[i] == NO_SLICE)
				{
					m_pImmediateContext->ClearRenderTargetView(textures[currentColorRT[i]].rtv, color);
				} 
				else 
				{
					m_pImmediateContext->ClearRenderTargetView(textures[currentColorRT[i]].rtvArray[currentColorRTSlice[i]], color);
				}
			}
		}
	}
	if (clearDepth || clearStencil)
	{
		UINT clearFlags = 0;
		if (clearDepth)   clearFlags |= D3D11_CLEAR_DEPTH;
		if (clearStencil) clearFlags |= D3D11_CLEAR_STENCIL;

		if (currentDepthRT == FB_DEPTH)
		{
			m_pImmediateContext->ClearDepthStencilView(depthBufferDSV, clearFlags, depth, stencil);
		}
		else if (currentDepthRT >= 0)
		{
			if (currentDepthSlice == NO_SLICE)
			{
				m_pImmediateContext->ClearDepthStencilView(textures[currentDepthRT].dsv, clearFlags, depth, stencil);
			} 
			else 
			{
				m_pImmediateContext->ClearDepthStencilView(textures[currentDepthRT].dsvArray[currentDepthSlice], clearFlags, depth, stencil);
			}
		}
	}
}

void Direct3D11Renderer::clearRenderTarget(const TextureID renderTarget, const Vec4 &color, const int slice)
{
	if (slice == NO_SLICE)
	{
		m_pImmediateContext->ClearRenderTargetView(textures[renderTarget].rtv, color);
	} 
	else 
	{
		m_pImmediateContext->ClearRenderTargetView(textures[renderTarget].rtvArray[slice], color);
	}
}

void Direct3D11Renderer::clearDepthTarget(const TextureID depthTarget, const float depth, const int slice)
{
	if (slice == NO_SLICE)
	{
		m_pImmediateContext->ClearDepthStencilView(textures[depthTarget].dsv, D3D11_CLEAR_DEPTH, depth, 0);
	} 
	else 
	{
		m_pImmediateContext->ClearDepthStencilView(textures[depthTarget].dsvArray[slice], D3D11_CLEAR_DEPTH, depth, 0);
	}
}


const D3D11_PRIMITIVE_TOPOLOGY d3dPrim[] = 
{
	D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
	D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED, // Triangle fans not supported
	D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
	D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED, // Quads not supported
	D3D11_PRIMITIVE_TOPOLOGY_LINELIST,
	D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP,
	D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED, // Line loops not supported
	D3D11_PRIMITIVE_TOPOLOGY_POINTLIST,
	D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST,
};

void Direct3D11Renderer::drawArrays(const Primitives primitives, const int firstVertex, const int nVertices)
{
	m_pImmediateContext->IASetPrimitiveTopology(d3dPrim[primitives]);
	m_pImmediateContext->Draw(nVertices, firstVertex);
}

void Direct3D11Renderer::drawElements(const Primitives primitives, const int firstIndex, const int nIndices, const int firstVertex, const int nVertices)
{
	m_pImmediateContext->IASetPrimitiveTopology(d3dPrim[primitives]);
	m_pImmediateContext->DrawIndexed(nIndices, firstIndex, 0);
}

void Direct3D11Renderer::drawElementsInstancedIndirect(const Primitives primitive, StructuredBufferID bufferArgs, UINT offset)
{
	m_pImmediateContext->IASetPrimitiveTopology(d3dPrim[primitive]);
	m_pImmediateContext->DrawIndexedInstancedIndirect(structuredBuffers[bufferArgs].structuredBuffer, offset);
}

void Direct3D11Renderer::setup2DMode(const float left, const float right, const float top, const float bottom)
{
	scaleBias2D.x = 2.0f / (right - left);
	scaleBias2D.y = 2.0f / (top - bottom);
	scaleBias2D.z = -1.0f;
	scaleBias2D.w =  1.0f;

    setShaderConstant4f("scaleBias", scaleBias2D);
}

const char *lineVS =
"float4x4 View;"
"float4x4 Proj;"
"float4 main(float4 position: Position): SV_Position {"
"	position = mul(mul(float4(position.xyz, 1.0f), View), Proj);"
"	return position;"
"}";

const char *linePS =
"float4 color;"
"float4 main() : SV_Target {"
"	return color;"
"}";

const char *plainVS =
"float4 scaleBias;"
"float4 main(float4 position: Position): SV_Position {"
"	position.xy = position.xy * scaleBias.xy + scaleBias.zw;"
"	return position;"
"}";

const char *plainPS =
"float4 color;"
"float4 main() : SV_Target {"
"	return color;"
"}";

const char *texDefs =
"struct VsIn {"
"	float4 position: Position;"
"	float2 texCoord: TexCoord;"
"};"
"struct PsIn {"
"	float4 position: SV_Position;"
"	float2 texCoord: TexCoord;"
"};\n";

const char *texVS =
"float4 scaleBias;"
"PsIn main(VsIn In){"
"	PsIn Out;"
"	Out.position = In.position;"
"	Out.position.xy = Out.position.xy * scaleBias.xy + scaleBias.zw;"
"	Out.texCoord = In.texCoord;"
"	return Out;"
"}";

const char *texPS =
"Texture2D Base: register(t0);"
"SamplerState base: register(s0);"
"float4 color;"
"float4 main(PsIn In) : SV_Target {"
"	return Base.Sample(base, In.texCoord) * color;"
"}";

void Direct3D11Renderer::drawPlain(const Primitives primitives, Vec2 *vertices, const UINT nVertices, const BlendStateID blendState, const DepthStateID depthState, const Vec4 *color)
{
	int size = nVertices * sizeof(Vec2);

	UINT stride = sizeof(Vec2);
	UINT offset[1];
	offset[0] =	copyToRollingVB(vertices, size);

	if (plainShader == SHADER_NONE)
	{
		plainShader = createShader(plainVS, NULL, plainPS, NULL, NULL, NULL, 0, 0, 0, 0, 0, 0);

		FormatDesc format[] = { 0, TYPE_VERTEX, FORMAT_FLOAT, 2 };
		plainVF = addVertexFormat(format, elementsOf(format), plainShader);
	}

	Vec4 col = Vec4(1, 1, 1, 1);
	if (color) col = *color;

	reset();
	setShader(plainShader);
	setShaderConstant4f("scaleBias", scaleBias2D);
	setShaderConstant4f("color", col);
	setBlendState(blendState);
	setDepthState(depthState);
	setRasterizerState(defaultRasterizeState);
	setVertexFormat(plainVF);
	setVertexBuffer(0, 1, &rollingVB, offset);
	apply();

	m_pImmediateContext->IASetPrimitiveTopology(d3dPrim[primitives]);
	m_pImmediateContext->Draw(nVertices, 0);
}

void Direct3D11Renderer::drawLine(Vec3 vPoint1, Vec3 vPoint2, Mat4x4 mView, Mat4x4 mProj, const BlendStateID blendState, const DepthStateID depthState, const Vec4 *color)
{
	int size = 2 * sizeof(Vec3);
	Vec3 vertices[2];

	vertices[0] = vPoint1;
	vertices[1] = vPoint2;

	UINT stride = sizeof(Vec3);
	UINT offset[1];
	offset[0] = copyToRollingVB(vertices, size);

	if (lineShader == SHADER_NONE)
	{
		lineShader = createShader(lineVS, NULL, linePS, NULL, NULL, NULL, 0, 0, 0, 0, 0, 0);

		FormatDesc format[] = { 0, TYPE_VERTEX, FORMAT_FLOAT, 3 };
		lineVF = addVertexFormat(format, elementsOf(format), lineShader);
	}

	Vec4 col = Vec4(1, 1, 1, 1);
	if (color) col = *color;

	reset();
	setShader(lineShader);
	setShaderConstant4x4f("View", mView);
	setShaderConstant4x4f("Proj", mProj);
	setShaderConstant4f("color", col);
	setBlendState(blendState);
	setDepthState(depthState);
	setRasterizerState(defaultRasterizeState);
	setVertexFormat(lineVF);
	setVertexBuffer(0, 1, &rollingVB, offset);
	apply();

	m_pImmediateContext->IASetPrimitiveTopology(d3dPrim[PRIM_LINES]);
	m_pImmediateContext->Draw(2, 0);
}

void Direct3D11Renderer::drawTextured(const Primitives primitives, TexVertex *vertices, const UINT nVertices, const TextureID texture, const SamplerStateID samplerState, const BlendStateID blendState, const DepthStateID depthState, const Vec4 *color)
{
	int size = nVertices * sizeof(TexVertex);

	UINT stride = sizeof(TexVertex);
	UINT offset[1];
	offset[0] =	copyToRollingVB(vertices, size);

	if (texShader == SHADER_NONE)
	{
		texShader = createShader(texVS, NULL, texPS, NULL, NULL, NULL, 0, 0, 0, 0, 0, 0, texDefs);

		FormatDesc format[] = 
		{
			0, TYPE_VERTEX,   FORMAT_FLOAT, 2,
			0, TYPE_TEXCOORD, FORMAT_FLOAT, 2,
		};
		texVF = addVertexFormat(format, elementsOf(format), texShader);
	}

	Vec4 col = Vec4(1, 1, 1, 1);
	if (color) col = *color;

	reset();
	setShader(texShader);
	setShaderConstant4f("scaleBias", scaleBias2D);
	setShaderConstant4f("color", col);
	setTexture("Base", texture);
	setSamplerState("base", samplerState);
	setBlendState(blendState);
	setDepthState(depthState);
	setRasterizerState(defaultRasterizeState);
	setVertexFormat(texVF);
	setVertexBuffer(0, 1, &rollingVB, offset);
	apply();

	m_pImmediateContext->IASetPrimitiveTopology(d3dPrim[primitives]);
	m_pImmediateContext->Draw(nVertices, 0);
}

ID3D11UnorderedAccessView **Direct3D11Renderer::createUAV(ID3D11Resource *resource, DXGI_FORMAT format, const int firstSlice, const int sliceCount)
{
	ID3D11UnorderedAccessView **uav = NULL;

	D3D11_RESOURCE_DIMENSION type;
	resource->GetType(&type);

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	ZeroMemory(&uavDesc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));

	switch (type)
	{
	case D3D11_RESOURCE_DIMENSION_TEXTURE1D:
		D3D11_TEXTURE1D_DESC desc1d;
		((ID3D11Texture1D *)resource)->GetDesc(&desc1d);

		uavDesc.Format = (format != DXGI_FORMAT_UNKNOWN) ? format : desc1d.Format;
		if (desc1d.ArraySize > 1)
		{
			printf("no support for array Testures");
			return NULL;
		}
		else
		{
			uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE1D;

			uav = CHG_NEW ID3D11UnorderedAccessView*[desc1d.MipLevels];

			for (UINT i = 0; i < desc1d.MipLevels; i++)
			{
				uavDesc.Texture1D.MipSlice = i;
								
				if (FAILED(m_pd3dDevice->CreateUnorderedAccessView(resource, &uavDesc, &uav[i])))
				{
					delete[] uav;
					return NULL;
				}
					
			}

			return uav;
		}
		break;

	case D3D11_RESOURCE_DIMENSION_TEXTURE2D:
		D3D11_TEXTURE2D_DESC desc2d;
		((ID3D11Texture2D *)resource)->GetDesc(&desc2d);

		uavDesc.Format = (format != DXGI_FORMAT_UNKNOWN) ? format : desc2d.Format;
		if (desc2d.ArraySize > 1)
		{
			printf("no support for array Testures");
			return NULL;
		}
		else
		{
			uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;

			uav = CHG_NEW ID3D11UnorderedAccessView*[desc2d.MipLevels];

			for (UINT i = 0; i < desc2d.MipLevels; i++)
			{
				uavDesc.Texture2D.MipSlice = i;

				if (FAILED(m_pd3dDevice->CreateUnorderedAccessView(resource, &uavDesc, &uav[i])))
				{
					delete[] uav;
					return NULL;
				}

			}

			return uav;
		}
		break;

	case D3D11_RESOURCE_DIMENSION_TEXTURE3D:
		D3D11_TEXTURE3D_DESC desc3d;
		((ID3D11Texture3D *)resource)->GetDesc(&desc3d);

		uavDesc.Format = (format != DXGI_FORMAT_UNKNOWN) ? format : desc3d.Format;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE3D;
		uavDesc.Texture3D.WSize = -1;
		
		uav = CHG_NEW ID3D11UnorderedAccessView*[desc3d.MipLevels];

		for (UINT i = 0; i < desc3d.MipLevels; i++)
		{
			uavDesc.Texture3D.MipSlice = i;

			if (FAILED(m_pd3dDevice->CreateUnorderedAccessView(resource, &uavDesc, &uav[i])))
			{
				delete[] uav;
				return NULL;
			}

		}

		return uav;

		break;
	default:
		printf("Unsupported type");
		return NULL;
	}

	return NULL;
}

ID3D11ShaderResourceView *Direct3D11Renderer::createSRV(ID3D11Resource *resource, DXGI_FORMAT format, const int firstSlice, const int sliceCount)
{
	D3D11_RESOURCE_DIMENSION type;
	resource->GetType(&type);
	
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ID3D11ShaderResourceView *srv;
	
	switch (type)
	{
		case D3D11_RESOURCE_DIMENSION_TEXTURE1D:
			D3D11_TEXTURE1D_DESC desc1d;
			((ID3D11Texture1D *) resource)->GetDesc(&desc1d);

			srvDesc.Format = (format != DXGI_FORMAT_UNKNOWN)? format : desc1d.Format;
			if (desc1d.ArraySize > 1)
			{
				srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
				srvDesc.Texture1DArray.FirstArraySlice = 0;
				srvDesc.Texture1DArray.ArraySize = desc1d.ArraySize;
				srvDesc.Texture1DArray.MostDetailedMip = 0;
				srvDesc.Texture1DArray.MipLevels = desc1d.MipLevels;
			} 
			else 
			{
				srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
				srvDesc.Texture1D.MostDetailedMip = 0;
				srvDesc.Texture1D.MipLevels = desc1d.MipLevels;
			}
			break;
		
		case D3D11_RESOURCE_DIMENSION_TEXTURE2D:
			D3D11_TEXTURE2D_DESC desc2d;
			((ID3D11Texture2D *) resource)->GetDesc(&desc2d);

			srvDesc.Format = (format != DXGI_FORMAT_UNKNOWN)? format : desc2d.Format;
			if (desc2d.ArraySize > 1){
				if (desc2d.MiscFlags & D3D11_RESOURCE_MISC_TEXTURECUBE)
				{
					srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
					srvDesc.TextureCube.MostDetailedMip = 0;
					srvDesc.TextureCube.MipLevels = desc2d.MipLevels;
				} 
				else
				{
					srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;

					if (firstSlice < 0){
						srvDesc.Texture2DArray.FirstArraySlice = 0;
						srvDesc.Texture2DArray.ArraySize = desc2d.ArraySize;
					} 
					else 
					{
						srvDesc.Texture2DArray.FirstArraySlice = firstSlice;
						if (sliceCount < 0){
							srvDesc.Texture2DArray.ArraySize = 1;
						}
						else 
						{
							srvDesc.Texture2DArray.ArraySize = sliceCount;
						}
					}
					srvDesc.Texture2DArray.MostDetailedMip = 0;
					srvDesc.Texture2DArray.MipLevels = desc2d.MipLevels;
				}
			} 
			else 
			{
				srvDesc.ViewDimension = (desc2d.SampleDesc.Count > 1)? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D;
				srvDesc.Texture2D.MostDetailedMip = 0;
				srvDesc.Texture2D.MipLevels = desc2d.MipLevels;
			}
			break;

		case D3D11_RESOURCE_DIMENSION_TEXTURE3D:
			D3D11_TEXTURE3D_DESC desc3d;
			((ID3D11Texture3D *) resource)->GetDesc(&desc3d);

			srvDesc.Format = (format != DXGI_FORMAT_UNKNOWN)? format : desc3d.Format;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;

			srvDesc.Texture3D.MostDetailedMip = 0;
			srvDesc.Texture3D.MipLevels = desc3d.MipLevels;

			break;
		default:
			printf("Unsupported type");
			return NULL;
	}

	if (FAILED(m_pd3dDevice->CreateShaderResourceView(resource, &srvDesc, &srv)))
	{
		printf("CreateShaderResourceView failed");
		return NULL;
	}

	return srv;
}

ID3D11RenderTargetView *Direct3D11Renderer::createRTV(ID3D11Resource *resource, DXGI_FORMAT format, const int firstSlice, const int sliceCount)
{
	D3D11_RESOURCE_DIMENSION type;
	resource->GetType(&type);

	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	ID3D11RenderTargetView *rtv;

	switch (type)
	{
		case D3D11_RESOURCE_DIMENSION_TEXTURE2D:
			D3D11_TEXTURE2D_DESC desc2d;
			((ID3D11Texture2D *) resource)->GetDesc(&desc2d);

			rtvDesc.Format = (format != DXGI_FORMAT_UNKNOWN)? format : desc2d.Format;
			if (desc2d.ArraySize > 1)
			{
				rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
				if (firstSlice < 0)
				{
					rtvDesc.Texture2DArray.FirstArraySlice = 0;
					rtvDesc.Texture2DArray.ArraySize = desc2d.ArraySize;
				} else 
				{
					rtvDesc.Texture2DArray.FirstArraySlice = firstSlice;
					if (sliceCount < 0)
					{
						rtvDesc.Texture2DArray.ArraySize = 1;
					} 
					else 
					{
						rtvDesc.Texture2DArray.ArraySize = sliceCount;
					}
				}
				rtvDesc.Texture2DArray.MipSlice = 0;
			} 
			else 
			{
				rtvDesc.ViewDimension = (desc2d.SampleDesc.Count > 1)? D3D11_RTV_DIMENSION_TEXTURE2DMS : D3D11_RTV_DIMENSION_TEXTURE2D;
				rtvDesc.Texture2D.MipSlice = 0;
			}
			break;

		case D3D11_RESOURCE_DIMENSION_TEXTURE3D:
			D3D11_TEXTURE3D_DESC desc3d;
			((ID3D11Texture3D *) resource)->GetDesc(&desc3d);

			rtvDesc.Format = (format != DXGI_FORMAT_UNKNOWN)? format : desc3d.Format;
			rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE3D;
			if (firstSlice < 0)
			{
				rtvDesc.Texture3D.FirstWSlice = 0;
				rtvDesc.Texture3D.WSize = desc3d.Depth;
			} 
			else 
			{
				rtvDesc.Texture3D.FirstWSlice = firstSlice;
				if (sliceCount < 0)
				{
					rtvDesc.Texture3D.WSize = 1;
				} 
				else 
				{
					rtvDesc.Texture3D.WSize = sliceCount;
				}
			}
			rtvDesc.Texture3D.MipSlice = 0;
			break;
		
		default:
			printf("Unsupported type");
			return NULL;
	}

	if (FAILED(m_pd3dDevice->CreateRenderTargetView(resource, &rtvDesc, &rtv)))
	{
		printf("CreateRenderTargetView failed");
		return NULL;
	}

	return rtv;
}

ID3D11DepthStencilView *Direct3D11Renderer::createDSV(ID3D11Resource *resource, DXGI_FORMAT format, const int firstSlice, const int sliceCount)
{
	D3D11_RESOURCE_DIMENSION type;
	resource->GetType(&type);

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	ZeroMemory( &dsvDesc, sizeof(dsvDesc) );

	ID3D11DepthStencilView *dsv;

	switch (type)
	{
		case D3D11_RESOURCE_DIMENSION_TEXTURE2D:
			D3D11_TEXTURE2D_DESC desc2d;
			((ID3D11Texture2D *) resource)->GetDesc(&desc2d);

			dsvDesc.Format = (format != DXGI_FORMAT_UNKNOWN)? format : desc2d.Format;
			if (desc2d.ArraySize > 1)
			{
				dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
				if (firstSlice < 0)
				{
					dsvDesc.Texture2DArray.FirstArraySlice = 0;
					dsvDesc.Texture2DArray.ArraySize = desc2d.ArraySize;
					dsvDesc.Texture2DArray.MipSlice = 0;
				} 
				else 
				{
					dsvDesc.Texture2DArray.FirstArraySlice = firstSlice;
					if (sliceCount < 0)
					{
						dsvDesc.Texture2DArray.ArraySize = 1;
					} 
					else 
					{
						dsvDesc.Texture2DArray.ArraySize = sliceCount;
					}
					dsvDesc.Texture2DArray.MipSlice = 0;
				}
			} 
			else 
			{
				dsvDesc.ViewDimension = (desc2d.SampleDesc.Count > 1)? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
				dsvDesc.Texture2D.MipSlice = 0;
			}
			break;

		default:
			printf("Unsupported type");
			return NULL;
	}

	if (FAILED(m_pd3dDevice->CreateDepthStencilView(resource, &dsvDesc, &dsv)))
	{
		printf("CreateDepthStencilView failed");
		return NULL;
	}

	return dsv;
}

ubyte *Direct3D11Renderer::mapRollingVB(const UINT size)
{
	//ASSERT(size <= ROLLING_VB_SIZE);
	if(size >= ROLLING_VB_SIZE)
		return NULL;

	if (rollingVB == VB_NONE) rollingVB = addVertexBuffer(ROLLING_VB_SIZE, DEFAULT);

	ubyte *data = NULL;
	D3D11_MAP flag = D3D11_MAP_WRITE_NO_OVERWRITE;

	if (rollingVBOffset + size > ROLLING_VB_SIZE)
	{
		flag = D3D11_MAP_WRITE_DISCARD;
		rollingVBOffset = 0;
	}

	m_pImmediateContext->Map(vertexBuffers[rollingVB].vertexBuffer, D3D11CalcSubresource( 0, 0, 0 ), flag, 0, (D3D11_MAPPED_SUBRESOURCE*) &data);
	//vertexBuffers[rollingVB].vertexBuffer->Map(flag, 0, (void **) &data);

	return data + rollingVBOffset;
}

void Direct3D11Renderer::unmapRollingVB(const UINT size)
{
	m_pImmediateContext->Unmap(vertexBuffers[rollingVB].vertexBuffer, D3D11CalcSubresource( 0, 0, 0 ));
	//vertexBuffers[rollingVB].vertexBuffer->Unmap();

	rollingVBOffset += size;
}

UINT Direct3D11Renderer::copyToRollingVB(const void *src, const UINT size)
{
	//ASSERT(size <= ROLLING_VB_SIZE);
	if(size >= ROLLING_VB_SIZE)
		return NULL;

	if (rollingVB == VB_NONE) rollingVB = addVertexBuffer(ROLLING_VB_SIZE, DYNAMIC);

	D3D11_MAPPED_SUBRESOURCE resource;
	D3D11_MAP flag = D3D11_MAP_WRITE_NO_OVERWRITE;
	if (rollingVBOffset + size > ROLLING_VB_SIZE)
	{
		flag = D3D11_MAP_WRITE_DISCARD;
		rollingVBOffset = 0;
	}

	UINT offset = rollingVBOffset;
	m_pImmediateContext->Map(vertexBuffers[rollingVB].vertexBuffer, D3D11CalcSubresource( 0, 0, 0 ), flag, 0, &resource);
	//vertexBuffers[rollingVB].vertexBuffer->Map(flag, 0, (void **) &data);
		ubyte *data = (ubyte*)resource.pData;
		memcpy(data + offset, src, size);
	m_pImmediateContext->Unmap(vertexBuffers[rollingVB].vertexBuffer, D3D11CalcSubresource( 0, 0, 0 ));
	//vertexBuffers[rollingVB].vertexBuffer->Unmap();
/*
	D3D10_BOX box;
	box.left   = offset;
	box.right  = offset + size;
	box.top    = 0;
	box.bottom = 1;
	box.front  = 0;
	box.back   = 1;

	device->UpdateSubresource(vertexBuffers[rollingVB].vertexBuffer, 0, &box, src, 0, 0);
*/
	rollingVBOffset += size;

	return offset;
}

ID3D11Resource *Direct3D11Renderer::getResource(const TextureID texture) const 
{
	return textures[texture].texture;
}

void Direct3D11Renderer::flush()
{
	m_pImmediateContext->Flush();
}

void Direct3D11Renderer::endFrame()
{
	m_pSwapChain->Present(0, 0);
}

void Direct3D11Renderer::finish()
{
	if (eventQuery == NULL)
	{
		D3D11_QUERY_DESC desc;
		desc.Query = D3D11_QUERY_EVENT;
		desc.MiscFlags = 0;
		m_pd3dDevice->CreateQuery(&desc, &eventQuery);
	}

	m_pImmediateContext->End(eventQuery);

	m_pImmediateContext->Flush();

	BOOL result = FALSE;
	do 
	{
		m_pImmediateContext->GetData(eventQuery, &result, sizeof(BOOL), 0);
	} while (!result);
}

void Direct3D11Renderer::dispatchCompute(const UINT num_groups_x, const UINT num_groups_y, const UINT num_groups_z)
{
	 m_pImmediateContext->Dispatch( num_groups_x, num_groups_y, num_groups_z );
}

void* Direct3D11Renderer::mapBuffer(const StructuredBufferID objectBuffer, const MapMode mapMode)
{
	void* pMapData;

	D3D11_MAPPED_SUBRESOURCE MappedResource;
	ZeroMemory(&MappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	m_pImmediateContext->Map(structuredBuffers[objectBuffer].structuredBuffer, 0, D3D11MapModes[mapMode], 0, &MappedResource);
	pMapData = MappedResource.pData;

	return pMapData;
}

void Direct3D11Renderer::unMapBuffer(const StructuredBufferID objectBuffer)
{
	m_pImmediateContext->Unmap(structuredBuffers[objectBuffer].structuredBuffer, 0);
}

void* Direct3D11Renderer::mapVertexBuffer(const VertexBufferID objectBuffer, const MapMode mapMode)
{
	void* pMapData;

	D3D11_MAPPED_SUBRESOURCE MappedResource;
	ZeroMemory(&MappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	m_pImmediateContext->Map(vertexBuffers[objectBuffer].vertexBuffer, 0, D3D11MapModes[mapMode], 0, &MappedResource);
	pMapData = MappedResource.pData;

	return pMapData;
}

void Direct3D11Renderer::unMapVertexBuffer(const VertexBufferID objectBuffer)
{
	m_pImmediateContext->Unmap(vertexBuffers[objectBuffer].vertexBuffer, 0);
}
