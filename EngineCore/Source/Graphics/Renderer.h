#ifndef _RENDERER_H_
#define _RENDERER_H_

// Some useful alias /////////////////////
typedef int AtlasTextureID;
typedef int TextureID;
typedef int ShaderID;
typedef int VertexBufferID;
typedef int IndexBufferID;
typedef int VertexFormatID;
typedef int SamplerStateID;
typedef int BlendStateID;
typedef int DepthStateID;
typedef int RasterizerStateID;
typedef int FontID;
typedef int QueryID;
typedef int ObjectBufferID;
typedef int StructuredBufferID;

///////////////////////////////////////////

// classes and structs "pre"-declaration ////////////////
/*
* Must be define on specific renderer implementation
*/
class Texture;
class Shader;
struct VertexBuffer;
struct IndexBuffer;
struct VertexFormat;
struct SamplerState;
struct BlendState;
struct DepthState;
struct RasterizerState;
struct ImageBufferSlot;
struct Query;
struct ObjectBuffer;
struct StructuredBuffer;
/////////////////////////////////////////////////////////


//Structured Buffer flags//////////////////////////////////////////////////////////////////////////

enum ResourceView
{
	RESOURCE_VIEW,
	UNORDERED_ACCESS_VIEW,
	UNORDERED_RESOURCE_VIEW,
};

//DirectX 11 UAV Flags
enum BUFFER_UAV_FLAG
{
	BUFFER_UAV_FLAG_RAW = 0x1,
	BUFFER_UAV_FLAG_APPEND = 0x2,
	BUFFER_UAV_FLAG_COUNTER = 0x4
};

//////////////////////////////////////////////////////////////////////////////////////////////////////

// Shader enumerators //////////////////////////
enum ConstantType 
{
	CONSTANT_FLOAT,
	CONSTANT_VEC2,
	CONSTANT_VEC3,
	CONSTANT_VEC4,
	CONSTANT_INT,
	CONSTANT_IVEC2,
	CONSTANT_IVEC3,
	CONSTANT_IVEC4,
	CONSTANT_BOOL,
	CONSTANT_BVEC2,
	CONSTANT_BVEC3,
	CONSTANT_BVEC4,
	CONSTANT_MAT2,
	CONSTANT_MAT3,
	CONSTANT_MAT4,

	CONSTANT_TYPE_COUNT
};

enum ObjectBufferType
{
    ATOMIC_COUNTER_BUFFER,
    TRANSFORM_FEEDBACK_BUFFER, 
    UNIFORM_BUFFER,
    SHADER_STORAGE_BUFFER
};

extern int constantTypeSizes[CONSTANT_TYPE_COUNT];
//////////////////////////////////////////////////

// Texture sample filters //////
enum Filter 
{
	NEAREST,
	LINEAR,
	BILINEAR,
	TRILINEAR,
	BILINEAR_ANISO,
	TRILINEAR_ANISO,
};

enum AddressMode 
{
	WRAP,
	CLAMP,
	BORDER,
	MIRROR,
};

inline bool hasMipmaps(const Filter filter){ return (filter >= LINEAR); }
inline bool hasAniso(const Filter filter){ return (filter >= BILINEAR_ANISO); }

///////////////////////////////////

// TTF font structs ///////////////
struct Character 
{
	float x0, y0;
	float x1, y1;
	float ratioX;
	float ratioY;
	TextureID page;
	float xOffset;
	float yOffset;
	float xadvance;
};

struct Kernel
{
	DWORD first;
	DWORD second;
	short amount;
};

struct TexFont 
{
	Character chars[256];
	TextureID texture;
};

struct TTFFont;

///////////////////////////////////

struct TexVertex 
{
	TexVertex(const Vec2 p, const Vec2 t)
	{
		position = p;
		texCoord = t;
	}
	Vec2 position;
	Vec2 texCoord;
};

#define MAKEQUAD(x0, y0, x1, y1, o)\
	Vec2(x0 + o, y0 + o),\
	Vec2(x0 + o, y1 - o),\
	Vec2(x1 - o, y0 + o),\
	Vec2(x1 - o, y1 - o),

#define MAKETEXQUAD(x0, y0, x1, y1, o)\
	TexVertex(Vec2(x0 + o, y0 + o), Vec2(0.0f, 0.0f)),\
	TexVertex(Vec2(x0 + o, y1 - o), Vec2(0.0f, 1.0f)),\
	TexVertex(Vec2(x1 - o, y0 + o), Vec2(1.0f, 0.0f)),\
	TexVertex(Vec2(x1 - o, y1 - o), Vec2(1.0f, 1.0f)),

#define MAKERECT(x0, y0, x1, y1, lw)\
	Vec2(x0, y0),\
	Vec2(x0 + lw, y0 + lw),\
	Vec2(x1, y0),\
	Vec2(x1 - lw, y0 + lw),\
	Vec2(x1, y1),\
	Vec2(x1 - lw, y1 - lw),\
	Vec2(x0, y1),\
	Vec2(x0 + lw, y1 - lw),\
	Vec2(x0, y0),\
	Vec2(x0 + lw, y0 + lw),


#define TEXTURE_NONE  (-1)
#define SHADER_NONE   (-1)
#define BLENDING_NONE (-1)
#define VF_NONE   (-1)
#define VB_NONE   (-1)
#define IB_NONE   (-1)
#define SS_NONE   (-1)
#define BS_NONE   (-1)
#define DS_NONE   (-1)
#define RS_NONE   (-1)
#define FONT_NONE (-1)
#define QUERY_NONE (-1)
#define STB_NONE   (-1)

#define FB_COLOR (-2)
#define FB_DEPTH (-2)
#define NO_SLICE (-1)

#define DONTCARE (-2)

// Texture flags
#define CUBEMAP       0x1
#define HALF_FLOAT    0x2
#define SRGB          0x4
#define SAMPLE_DEPTH  0x8
#define SAMPLE_SLICES 0x10
#define RENDER_SLICES 0x20
#define USE_MIPGEN    0x40

// Texture Target flags
#define DEFAULT_SIZE 0x1
#define VIEWPORT_SIZE 0x2
#define SPARSE_TEXTURE 0x80
#define UAV_TEXTURE	  0x100

// Shader flags
#define ASSEMBLY 0x1

// Mask constants
#define RED   0x1
#define GREEN 0x2
#define BLUE  0x4
#define ALPHA 0x8

#define ALL (RED | GREEN | BLUE | ALPHA)
#define NONE 0


// reset() flags
#define RESET_ALL    0xFFFF
#define RESET_SHADER 0x1
#define RESET_VF     0x2
#define RESET_VB     0x4
#define RESET_IB     0x8
#define RESET_DS     0x10
#define RESET_BS     0x20
#define RESET_RS     0x40
#define RESET_SS     0x80
#define RESET_TEX    0x100
#define RESET_IMAGE  0x200
#define RESET_OBJECTBUFFER 0x400
#define RESET_STRUCTUREDBUFFER 0x800
#define RESET_RWSTRUCTUREDBUFFER 0x1000

// Draw Text flags
#define F_TOP 0xC
#define F_BOTTOM 0xC0
#define F_LEFT 0x30
#define F_RIGHT 0x3

#define MAX_STORAGE_BLOCK 10

//Compute flags
//#define VERTEX_ATTRIB_ARRAY_BARRIER_BIT
//#define ELEMENT_ARRAY_BARRIER_BIT
//#define UNIFORM_BARRIER_BIT
#define TEXTURE_FETCH_BARRIER_BIT			0x8
#define SHADER_IMAGE_ACCESS_BARRIER_BIT		0x20
//#define COMMAND_BARRIER_BIT
//#define PIXEL_BUFFER_BARRIER_BIT
//#define TEXTURE_UPDATE_BARRIER_BIT 
//#define BUFFER_UPDATE_BARRIER_BIT
//#define FRAMEBUFFER_BARRIER_BIT
//#define TRANSFORM_FEEDBACK_BARRIER_BIT 
//#define ATOMIC_COUNTER_BARRIER_BIT
#define SHADER_STORAGE_BARRIER_BIT			0x2000
//#define ALL_BARRIER_BITS

// Buffer access type /////////////////
enum BufferAccess {
	STATIC,
	DEFAULT,
	DYNAMIC,
};

enum ImageBufferAccess
{
	DEFAULT_ACCESS,
	READ_ONLY,
	WRITE_ONLY,
	READ_WRITE
};

enum MapMode
{
	MAP_READ,
	MAP_WRITE,
	MAP_READ_WRITE,
	MAP_WRITE_DISCARD,
	MAP_WRITE_NO_OVERWRITE
};

///////////////////////////////////////

enum Primitives {
	PRIM_TRIANGLES      = 0,
	PRIM_TRIANGLE_FAN   = 1,
	PRIM_TRIANGLE_STRIP = 2,
	PRIM_QUADS          = 3,
	PRIM_LINES          = 4,
	PRIM_LINE_STRIP     = 5,
	PRIM_LINE_LOOP      = 6,
	PRIM_POINTS         = 7,
	PRIM_PATCHES		= 8,
};

enum AttributeType {
	TYPE_GENERIC  = 0,
	TYPE_VERTEX   = 1,
	TYPE_TEXCOORD = 2,
	TYPE_NORMAL   = 3,
	TYPE_TANGENT  = 4,
	TYPE_BINORMAL = 5,
	TYPE_COLOR	  = 6,
};

enum AttributeFormat {
	FORMAT_FLOAT = 0,
	FORMAT_HALF  = 1,
	FORMAT_UBYTE = 2,
};

struct FormatDesc {
	int stream;
	AttributeType type;
	AttributeFormat format;
	int size;
};

#define MAX_MRTS 8
#define MAX_VERTEXSTREAM 8
#define MAX_TEXTUREUNIT  16
#define MAX_IMAGEBUFFERUNIT 16
#define MAX_SAMPLERSTATE 16
#define MAX_OBJECTBUFFERUNIT 16
#define MAX_RWSTRUCTUREBUFFERUNIT 7
#define MAX_STRUCTUREBUFFERUNIT 16

// Blending constants
extern const int ZERO;
extern const int ONE;
extern const int SRC_COLOR;
extern const int ONE_MINUS_SRC_COLOR;
extern const int DST_COLOR;
extern const int ONE_MINUS_DST_COLOR;
extern const int SRC_ALPHA;
extern const int ONE_MINUS_SRC_ALPHA;
extern const int DST_ALPHA;
extern const int ONE_MINUS_DST_ALPHA;
extern const int SRC_ALPHA_SATURATE;

extern const int BM_ADD;
extern const int BM_SUBTRACT;
extern const int BM_REVERSE_SUBTRACT;
extern const int BM_MIN;
extern const int BM_MAX;

// Depth-test constants
extern const int NEVER;
extern const int LESS;
extern const int EQUAL;
extern const int LEQUAL;
extern const int GREATER;
extern const int NOTEQUAL;
extern const int GEQUAL;
extern const int ALWAYS;

// Stencil-test constants
extern const int KEEP;
extern const int SET_ZERO;
extern const int REPLACE;
extern const int INVERT;
extern const int INCR;
extern const int DECR;
extern const int INCR_SAT;
extern const int DECR_SAT;

// Culling constants
extern const int CULL_NONE;
extern const int CULL_BACK;
extern const int CULL_FRONT;

// Fillmode constants
extern const int SOLID;
extern const int WIREFRAME;

////////////////////////////////////////////////////
//
// Class Renderer
//
////////////////////////////////////////////////////

/*
 *	This class abstracts and simplifies low level rendering operations
 */

class Renderer 
{
public:
	Renderer();
	virtual ~Renderer();

	virtual void resetToDefaults();
	virtual void reset(const UINT flags = RESET_ALL);
	void apply();

	// Texture Methods. Create a texture from file or memory ///////////////////////////////////////////////////////////////////////////////////////////////
	TextureID createTexture(const char *fileName, const bool useMipMaps, const SamplerStateID samplerState = SS_NONE, UINT flags = 0);
	TextureID createTexture(char *rawBuffer, unsigned int rawSize, std::string extension, const bool useMipMaps, const SamplerStateID samplerState = SS_NONE, UINT flags = 0);
	virtual TextureID createTexture(char* rawData, FORMAT rawDataFormat, const int width, const int height, const int depth, const SamplerStateID samplerState = SS_NONE, UINT flags = 0) = 0;
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Render Targets Methods. Create Render Targets and Depth buffer. /////////////////////////////////////////////////////////////////////////////////////
	TextureID addRenderTarget(const int width, const int height, const FORMAT format, const SamplerStateID samplerState = SS_NONE, UINT flags = 0)
	{
		return addRenderTarget(width, height, 1, 1, 1, format, 1, samplerState, flags);
	}
	TextureID addRenderDepth(const int width, const int height, const int depthBits)
	{
		return addRenderDepth(width, height, 1, depthBits <= 16? FORMAT_D16 : FORMAT_D24);
	}

	virtual TextureID addRenderDepth(const int width, const int height, const int arraySize, const FORMAT format, const int msaaSamples = 1, const SamplerStateID samplerState = SS_NONE, UINT flags = 0) = 0;
	virtual TextureID addRenderTarget(const int width, const int height, const int depth, const int mipMapCount, const int arraySize, const FORMAT format, 
		const int msaaSamples = 1, const SamplerStateID samplerState = SS_NONE, UINT flags = 0) = 0;
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	virtual bool resizeRenderTarget(const TextureID renderTarget, const int width, const int height, const int depth, const int mipMapCount, const int arraySize) = 0;
	virtual bool generateMipMaps(const TextureID renderTarget) = 0;

	// Shaders Methods. Read Shader data from file and memory and compile. //////////////////////////////////////////////////////////////////////////////////////
	ShaderID createShader(const char *fileName, const UINT flags = 0);
	ShaderID createShader(const char *fileName, const char *extra, const UINT flags = 0);
	ShaderID createShader(const char *fileName, const char **attributeNames, const int nAttributes, const char *extra = NULL, const UINT flags = 0);
	ShaderID createShader(char *rawBuffer, unsigned int rawSize, const UINT flags = 0);
	ShaderID createShader(char *rawBuffer, unsigned int rawSize, const char *extra, const UINT flags = 0);
	ShaderID createShader(char *rawBuffer, unsigned int rawSize, const char **attributeNames, const int nAttributes, const char *extra = NULL, const UINT flags = 0);
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Vertex formats, vertex buffers and index buffers creator ///////////////////////////////////////////////////////////////////////////////////
	int getFormatSize(const AttributeFormat format) const;
	virtual VertexFormatID addVertexFormat(const FormatDesc *formatDesc, const UINT nAttribs, const ShaderID shader = SHADER_NONE) = 0; 
	virtual VertexBufferID addVertexBuffer(const long size, const BufferAccess bufferAccess, const void *data = NULL) = 0;
	virtual IndexBufferID addIndexBuffer(const UINT nIndices, const UINT indexSize, const BufferAccess bufferAccess, const void *data = NULL) = 0;

	virtual ObjectBufferID createObjectBuffer(const ObjectBufferType type, const UINT objectSize, const UINT nObjects, const BufferAccess bufferAccess, const void *data = NULL) = 0;
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	// Structured Buffers creator ////////////////////////////////////////////////////////////////////////
	virtual StructuredBufferID createStructuredBuffer(const UINT elementSize, const UINT nElements, const bool bIsCPUWritable, const bool bIsGPUWritable, const UINT UAVFlags = 0, const void *data = NULL) = 0;
	virtual StructuredBufferID createIndirectDrawArgBuffer() = 0;
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	
	// Create render states ////////////////////////////////////////////////////////////////////////////////
	virtual SamplerStateID addSamplerState(const Filter filter, const AddressMode s, const AddressMode t, const AddressMode r, const float lod = 0, const UINT maxAniso = 16, 
		const int compareFunc = 0, const float *border_color = NULL) = 0;
	BlendStateID addBlendState(const int srcFactor, const int destFactor, const int blendMode = BM_ADD, const int mask = ALL, const bool alphaToCoverage = false)
	{
		return addBlendState(srcFactor, destFactor, srcFactor, destFactor, blendMode, blendMode, mask, alphaToCoverage);
	}
	DepthStateID addDepthState(const bool depthTest, const bool depthWrite, const int depthFunc = LEQUAL, const bool stencilTest = false,
		const UINT8 stencilMask = 0xFF, const int stencilFunc = ALWAYS, const int stencilFail = KEEP, const int depthFail = KEEP, const int stencilPass = KEEP)
	{
		return addDepthState(depthTest, depthWrite, depthFunc, stencilTest, stencilMask, stencilMask, stencilFunc, stencilFunc, stencilFail, stencilFail, depthFail, depthFail, stencilPass, stencilPass);
	}
	virtual RasterizerStateID addRasterizerState(const int cullMode, const int fillMode = SOLID, const bool multiSample = true, const bool scissor = false, const float depthBias = 0.0f, 
		const float slopeDepthBias = 0.0f) = 0;
	//////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Set Texture slot in shaders /////////////////////////////////////////////////////////////////////////////
	virtual void setTexture(const char *textureName, const TextureID texture) = 0;
	virtual void setTexture(const int textureUnit, const TextureID texture) = 0;
	virtual void setTexture(const char *textureName, const TextureID texture, const SamplerStateID samplerState) = 0;
	virtual void setTextureArray(const int initIndex, const TextureID *textures, const int n_textures) = 0; //Warning function
	virtual void setTextureArray(const char *textureName, const TextureID *textures, const int arraySize) = 0;
	virtual void setTextureSlice(const char *textureName, const TextureID texture, const int slice) = 0;
	virtual void setImageBuffer(const char *ImageBufferName, const TextureID texture, const int mipLevel, const FORMAT formatImageBuffer, const ImageBufferAccess imageBufferAccess) = 0;
	virtual void setImageBufferArray(const char *ImageBufferName, const TextureID *texture, const int *mipLevel, const FORMAT *formatImageBuffer, const ImageBufferAccess *imageBufferAccess, 
		const UINT offset, const UINT arraySize) = 0;
    virtual void setObjectBuffer(const char *objectBufferName, const ObjectBufferID objectBufferID) = 0;
	virtual void setObjectBuffer(const int bindingSlot, const ObjectBufferID objectBufferID) = 0;	
	virtual void setStructuredBuffer(const char *structuredBufferName, const StructuredBufferID structuredBufferID) = 0;
	virtual void setStructuredBufferArray(const char *structuredBufferName, const StructuredBufferID *structuredBufferID, int offset, int n_StructuredBuffers) = 0;
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	// Apply change on shaders /////////////////
	virtual void applyTextures() = 0;
	virtual void applyImageBuffers() = 0;
    virtual void applyObjectBuffers() = 0;
	virtual void applyStructuredBuffers() = 0;
	virtual void applyRWStructuredBuffers() = 0;
	/////////////////////////////////////////////

	// Set Render States ///////////////////////////////////////////////////////////////
	virtual void setSamplerState(const char *samplerName, const SamplerStateID samplerState) = 0;
	void setBlendState(const BlendStateID blendState, const UINT sampleMask = ~0)
	{
		selectedBlendState = blendState;
		selectedSampleMask = sampleMask;
	}
	void setDepthState(const DepthStateID depthState, const UINT stencilRef = 0)
	{
		selectedDepthState = depthState;
		selectedStencilRef = stencilRef;
	}
	void setRasterizerState(const RasterizerStateID rasterizerState)
	{
		selectedRasterizerState = rasterizerState;
	}
	//////////////////////////////////////////////////////////////////////////////////////

	// Apply Render States //////////////////////////////////////////////////////////////
	virtual void applySamplerStates() = 0;
	virtual void changeBlendState(const BlendStateID blendState, const UINT sampleMask = ~0) = 0;
	virtual void changeDepthState(const DepthStateID depthState, const UINT stencilRef = 0) = 0;
	virtual void changeRasterizerState(const RasterizerStateID rasterizerState) = 0;
	/////////////////////////////////////////////////////////////////////////////////////

	// Remove textures and buffers //////////////////////////////////////////////////////
	virtual void removeTexture(const TextureID texture) = 0;
	virtual void removeVertexBuffer(const VertexBufferID vertexBuffer) = 0;
	virtual void removeIndexBuffer(const IndexBufferID indexBuffer) = 0;
	/////////////////////////////////////////////////////////////////////////////////////

	// Create bitmap and ttf font ///////////////////////////////////////////////////////
	FontID addFont(TexFont texFont);
	virtual FontID addFont(const char *ttfFontFile, const UINT size, const SamplerStateID samplerState = -1) = 0;
	virtual FontID addFont(char *rawBuffer, unsigned int rawSize, const UINT size, const SamplerStateID samplerState = -1) = 0;
	/////////////////////////////////////////////////////////////////////////////////////

	// Set Shader on renderer ///////////////////////////////////
	virtual bool hasTessellation(const ShaderID shaderID) = 0;
    bool activeShaderHasTessellation();
	
	void setShader(const ShaderID shader)
	{
		selectedShader = shader;
	}
	/////////////////////////////////////////////////////////////

	// Set vertex format on shader and set vertex and index buffers ////////////////////////////////////
	void setVertexFormat(const VertexFormatID VertexFormat)
	{
		selectedVertexFormat = VertexFormat;
	}

	void setVertexBuffer(UINT StartSlot, UINT NumBuffers, const VertexBufferID* vertexBuffer, const UINT* offset = 0)
	{
		UINT slot = StartSlot;
		numVertexBufferUsed = NumBuffers;

		for(UINT i=0; i < NumBuffers; i++)
		{
			selectedVertexBuffers[slot] = vertexBuffer[i];
			selectedOffsets[slot] = offset[i];

			slot++;
		}
	}

	void setIndexBuffer(const IndexBufferID indexBuffer)
	{
		selectedIndexBuffer = indexBuffer;
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////

	// Set constant buffers components /////////////////////////////////////////////////////////
	void setShaderConstant1i(const char *name, const int constant);
	void setShaderConstant1f(const char *name, const float constant);
	void setShaderConstant2f(const char *name, const Vec2 &constant);
	void setShaderConstant3f(const char *name, const Vec3 &constant);
	void setShaderConstant4f(const char *name, const Vec4 &constant);
	void setShaderConstant4x4f(const char *name, const Mat4x4 &constant);
	void setShaderConstantStruct(const char *name, const void* constant, const int elementSize);
	void setShaderConstantArray1i(const char *name, const int *constant, const UINT count);
	void setShaderConstantArray1f(const char *name, const float *constant, const UINT count);
	void setShaderConstantArray2f(const char *name, const Vec2  *constant, const UINT count);
	void setShaderConstantArray3f(const char *name, const Vec3  *constant, const UINT count);
	void setShaderConstantArray4f(const char *name, const Vec4  *constant, const UINT count);
	void setShaderConstantArray4x4f(const char *name, const Mat4x4 *constant, const UINT count);
	void setShaderConstantArrayStruct(const char *name, const void* constant, const int elementSize, const UINT count);
	/////////////////////////////////////////////////////////////////////////////////////////////

	// Apply Constant buffers on shaders ///////////
	virtual void applyConstants() = 0;
	////////////////////////////////////////////////

	// Apply Render Targets //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void changeRenderTarget(const TextureID colorRT, const TextureID depthRT = TEXTURE_NONE, const int slice = NO_SLICE, const UINT flags = DEFAULT_SIZE)
	{
		if(colorRT < 0)
			changeRenderTargets(&colorRT, 0, depthRT, &slice, flags);
		else
			changeRenderTargets(&colorRT, 1, depthRT, &slice, flags);
	}
	void changeRenderTargets(const TextureID *colorRTs, const UINT nRenderTargets, const TextureID depthRT, const int *faces, const UINT flags = DEFAULT_SIZE)
	{
		changeRenderTargets(colorRTs, nRenderTargets, depthRT, NO_SLICE, faces, flags);
	}
	virtual void changeRenderTargets(const TextureID *colorRTs, const UINT nRenderTargets, const TextureID depthRT = TEXTURE_NONE, const int depthSlice = NO_SLICE, const int *slices = NULL, 
		const UINT flags = DEFAULT_SIZE) = 0;
	virtual void changeToMainFramebuffer() = 0;

	void setViewport(const int width, const int height, const int posX = 0, const int posY = 0);
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Apply shader ////////////////////////////////////////
	virtual void changeShader(const ShaderID shader) = 0;
	////////////////////////////////////////////////////////
	
	// Apply geometry buffers ///////////////////////////////////////////////////////////////////////
	virtual void changeVertexFormat(const VertexFormatID vertexFormat) = 0;
	virtual void changeVertexBuffer(UINT StartSlot, UINT NumBuffers, const VertexBufferID* vertexBuffer, const UINT* offset = 0) = 0;
	virtual void changeIndexBuffer(const IndexBufferID indexBuffer) = 0;
	/////////////////////////////////////////////////////////////////////////////////////////////////

	// Clear and draw primitives methods ////////////////////////////////////////////////////////////////////////////////////////////////////
	void clear(const bool clearColor, const bool clearDepth, const float *color = NULL, const float depth = 1.0f)
	{
		clear(clearColor, clearDepth, false, color, depth, 0);
	}
	virtual void clear(const bool clearColor, const bool clearDepth, const bool clearStencil, const float *color = NULL, const float depth = 1.0f, const UINT stencil = 0) = 0;

	virtual void drawArrays(const Primitives primitives, const int firstVertex, const int nVertices) = 0;
	virtual void drawElements(const Primitives primitives, const int firstIndex, const int nIndices, const int firstVertex, const int nVertices) = 0;
	virtual void drawElementsInstancedIndirect(const Primitives primitive, StructuredBufferID bufferArgs, UINT offset) = 0;
	virtual void drawElementsInstanced(const Primitives primitive, const int firstIndex, const int nIndices, const int firstVertex, const int nVertices, UINT uNumInstances, UINT offset) = 0;
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // Compute Shader ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void dispatchCompute(const UINT num_groups_x, const UINT num_groups_y, const UINT num_groups_z) = 0;
    virtual void memoryBarrier(UINT flags) = 0;
	virtual bool getShaderWorkGroupSize(const int shaderID, UINT* localGroupSize) = 0;
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Queries ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual QueryID createQuery() = 0;
    virtual void beginQuery(QueryID id) = 0;
	virtual void endQuery(QueryID id) = 0;
    virtual bool queryResultAvailable(QueryID id) = 0;
	virtual unsigned long long  getResultQuery(QueryID id) = 0;
	virtual void deleteQueries() = 0;
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	// Simple draw methods for GUIs /////////////////////////////////////////////////////////////////////////////////////////////////////////
	virtual void setup2DMode(const float left, const float right, const float top, const float bottom) = 0;
	virtual void drawPlain(const Primitives primitives, Vec2 *vertices, const UINT nVertices, const BlendStateID blendState, const DepthStateID depthState, const Vec4 *color = NULL) = 0;
	virtual void drawLine(Vec3 vPoint1, Vec3 vPoint2, Mat4x4 mView, Mat4x4 mProj, const BlendStateID blendState, const DepthStateID depthState, const Vec4 *color = NULL) = 0;
	virtual void drawTextured(const Primitives primitives, TexVertex *vertices, const UINT nVertices, const TextureID texture, const SamplerStateID samplerState, 
		const BlendStateID blendState, const DepthStateID depthState, const Vec4 *color = NULL) = 0;
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Bitmap text methods ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	float getTextWidth(const FontID font, const char *str, int length = -1) const;
	float getTextHeight(const FontID font, const char *str, int length = -1) const;
	UINT getTextQuads(const char *str) const;
	void fillTextBuffer(TexVertex *dest, const char *str, float x, float y, const float charWidth, const float charHeight, const FontID font) const;
	bool drawText(const char *str, float x, float y, const float charWidth, const float charHeight, const FontID font, const SamplerStateID samplerState, const BlendStateID blendState, 
		const DepthStateID depthState, Color* textColor = NULL);
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
    //Reset AtomicCounter
    virtual void resetAtomicCounter(ObjectBufferID atomicCounterBuffer, UINT value) = 0;

	// Map/UnMap Buffer //////////////////////////////////////////////////////////////////////////
	virtual void* mapBuffer(const StructuredBufferID objectBuffer, const MapMode mapMode) = 0;
	virtual void unMapBuffer(const StructuredBufferID objectBuffer) = 0;
	virtual void* mapVertexBuffer(const VertexBufferID objectBuffer, const MapMode mapMode) = 0;
	virtual void unMapVertexBuffer(const VertexBufferID objectBuffer) = 0;
	//////////////////////////////////////////////////////////////////////////////////////////////

	// Finish frame /////////////
	virtual void endFrame() = 0;
	/////////////////////////////
	
	// Gets /////////////////////////////////////////////////////////////////////
	ShaderID getCurrentShaderID() { return currentShader; }
	ShaderID getSelectedShaderID() { return selectedShader; }
	VertexFormatID getSelectedVertexFormatID() { return selectedVertexFormat; }
	VertexFormatID getCurrentVertexFormatID() { return currentVertexFormat; }
	/////////////////////////////////////////////////////////////////////////////

#ifdef PROFILE
	// Profiling
	void profileFrameStart(const float frameTime);
	void profileFrameEnd();

	void profileBegin(const char *name);
	void profileNext(const char *name);
	void profileEnd();

	const char *getProfileString() const { return profileString; }
#endif

	// Synchronization
	virtual void flush() = 0;
	virtual void finish() = 0;

protected:
	// Elements loaded and created ///////////////////
	Array <Texture> textures;
	Array <Shader> shaders;
	Array <VertexFormat> vertexFormats;
	Array <VertexBuffer> vertexBuffers;
	Array <IndexBuffer> indexBuffers;
	Array <SamplerState> samplerStates;
	Array <BlendState> blendStates;
	Array <DepthState> depthStates;
	Array <RasterizerState> rasterizerStates;
	Array <TexFont> fonts;
	Array <TTFFont> TTFFonts;
	Array <ObjectBuffer> objectBuffers;
	Array <Query> queries;
	Array <StructuredBuffer> structuredBuffers;
	//////////////////////////////////////////////////

	UINT nImageUnits, nMRTs;
	int maxAnisotropic;

	// Renderer states /////////////////////////////////////////////////////////////////////////////////
	ShaderID currentShader;
	ShaderID selectedShader;

	VertexFormatID currentVertexFormat; 
	VertexFormatID selectedVertexFormat;
	VertexBufferID currentVertexBuffers[MAX_VERTEXSTREAM], selectedVertexBuffers[MAX_VERTEXSTREAM];
	UINT selectedOffsets[MAX_VERTEXSTREAM];
	UINT currentOffsets[MAX_VERTEXSTREAM];
	UINT numVertexBufferUsed;
	IndexBufferID currentIndexBuffer, selectedIndexBuffer;

	DepthStateID currentDepthState, selectedDepthState;
	UINT currentStencilRef, selectedStencilRef;
	BlendStateID currentBlendState, selectedBlendState;
	UINT currentSampleMask, selectedSampleMask;
	RasterizerStateID currentRasterizerState, selectedRasterizerState;

	TextureID currentColorRT[MAX_MRTS], currentDepthRT;
	int currentColorRTSlice[MAX_MRTS], currentDepthSlice;
	UINT nCurrentRenderTargets;

	int viewportPosX, viewportPosY, viewportWidth, viewportHeight;
	/////////////////////////////////////////////////////////////////////////////////////////////////////

	// Statistics counters
	UINT nDrawCalls;

	// Abstract Protected Methods implements on specific renderer implementation
	// Texture Methods. Create a texture from file or memory ///////////////////////////////////////////////////////////////////////////////////////////////
	virtual TextureID createTexture(Image &img, const SamplerStateID samplerState = SS_NONE, UINT flags = 0) = 0;
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Shaders Methods. Read Shader data from file and memory and compile. //////////////////////////////////////////////////////////////////////////////////////
	virtual ShaderID createShader(const char *vsText, const char *gsText, const char *fsText, const char *csText, const char* tcsText, const char* tesText, const int vsLine, const int gsLine, const int fsLine, const int csLine,
		const int tcsLine, const int tesLine, const char *header = NULL, const char *extra = NULL, const char *fileName = NULL, const char **attributeNames = NULL, const int nAttributes = 0, const UINT flags = 0) = 0;
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Create render states ////////////////////////////////////////////////////////////////////////////////
	virtual BlendStateID addBlendState(const int srcFactorRGB, const int destFactorRGB, const int srcFactorAlpha, const int destFactorAlpha, const int blendModeRGB, const int blendModeAlpha, const int mask = ALL, const bool alphaToCoverage = false) = 0;
	virtual DepthStateID addDepthState(const bool depthTest, const bool depthWrite, const int depthFunc, const bool stencilTest, const UINT8 stencilReadMask, const UINT8 stencilWriteMask,
		const int stencilFuncFront, const int stencilFuncBack, const int stencilFailFront, const int stencilFailBack,
		const int depthFailFront, const int depthFailBack, const int stencilPassFront, const int stencilPassBack) = 0;
	//////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Set constant buffers components /////////////////////////////////////////////////////////
	virtual void setShaderConstantRaw(const char *name, const void *data, const int size) = 0;
	/////////////////////////////////////////////////////////////////////////////////////////////

#ifdef PROFILE
	// Profiling
	float profileStartTime;
	bool profileFrame;
	bool profileReset;

	char profileString[512];
	int profileStringIndex;
	float profileTimes[32];
	int profileTimeIndex;
	int profileFrameCount;
#endif


private:
	TexVertex *fontBuffer;
	UINT fontBufferCount;

#ifdef DEBUG
	bool wasReset;
#endif
};

#endif // _RENDERER_H_
