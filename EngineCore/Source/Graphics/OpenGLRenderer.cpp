
#include "CthughaStd.h"
#include "OpenGLRenderer.h"

#ifndef BUFFER_OFFSET
#define BUFFER_OFFSET(i) ((char *) NULL + (i))
#endif

typedef GLvoid (APIENTRY *UNIFORM_FUNC)(GLint location, GLsizei count, const void *value);
typedef GLvoid (APIENTRY *UNIFORM_MAT_FUNC)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);

class Texture 
{
public:

	union 
	{
		GLuint glTexID;
		GLuint glDepthID;
	};
	GLuint glTarget;
	FORMAT format;
	GLint internalFormat;
	GLint externalFormat;
	UINT flags;
	int width, height, depth;
	float lod;
	UINT nSlices;
	UINT arraySize;
	UINT nMipMaps;
	bool mod;
	//Add texture view in other version

	SamplerStateID samplerState;

	Texture()
	{

	}
};

ConstantType getConstantType(GLenum type)
{
	switch (type)
	{
		case GL_FLOAT:      return CONSTANT_FLOAT;
		case GL_FLOAT_VEC2: return CONSTANT_VEC2;
		case GL_FLOAT_VEC3: return CONSTANT_VEC3;
		case GL_FLOAT_VEC4: return CONSTANT_VEC4;
		case GL_INT:            return CONSTANT_INT;
		case GL_INT_VEC2:   return CONSTANT_IVEC2;
		case GL_INT_VEC3:   return CONSTANT_IVEC3;
		case GL_INT_VEC4:   return CONSTANT_IVEC4;
		case GL_BOOL:       return CONSTANT_BOOL;
		case GL_BOOL_VEC2:  return CONSTANT_BVEC2;
		case GL_BOOL_VEC3:  return CONSTANT_BVEC3;
		case GL_BOOL_VEC4:  return CONSTANT_BVEC4;
		case GL_FLOAT_MAT2: return CONSTANT_MAT2;
		case GL_FLOAT_MAT3: return CONSTANT_MAT3;
		case GL_FLOAT_MAT4: return CONSTANT_MAT4;
	}

	return (ConstantType) -1;
}

struct Constant 
{
	char *name;
	unsigned char *data;
	UINT index;
	ConstantType type;
	int nElements;
	bool dirty;
};

int constantComp(const void *s0, const void *s1)
{
	return strcmp(((Constant *) s0)->name, ((Constant *) s1)->name);
}

struct Sampler 
{
	char *name;
	UINT index;
};

int samplerComp(const void *s0, const void *s1)
{
	return strcmp(((Sampler *) s0)->name, ((Sampler *) s1)->name);
}

struct ImageBuffer 
{
	char *name;
	UINT index;
	UINT size;
};

int imageBufferComp(const void *s0, const void *s1)
{
	return strcmp(((ImageBuffer *) s0)->name, ((ImageBuffer *) s1)->name);
}

struct ObjectBufferEntry
{
	char *name;
	UINT index;
};

int objectBufferComp(const void *s0, const void *s1)
{
	return strcmp(((ObjectBufferEntry *) s0)->name, ((ObjectBufferEntry *) s1)->name);
}


class Shader 
{
public:

	GLuint program;
	GLuint vertexShader;
	GLuint tessellationControlShader;
	GLuint tessellationEvaluationShader;
    GLuint geometryShader;
	GLuint fragmentShader;
	GLuint computeShader;

	Constant *uniforms;
	Sampler  *samplers;
    ImageBuffer *imageBuffers;
	ObjectBufferEntry *objectBuffers;

	UINT nUniforms;
	UINT nSamplers;
    UINT nImageBuffers;
	UINT nObjectBuffers;
	UINT localSizeX, localSizeY, localSizeZ;

	Shader()
	{

	}
};

struct Attrib 
{
	int index;
	int stream;
	int size;
	AttributeFormat format;
	int offset;
};

#define MAX_ATTRIB 20
#define MAX_GENERIC 8
#define MAX_TEXCOORD 8

struct VertexFormat 
{
	Attrib attribs[MAX_ATTRIB];

	int vertexSize[MAX_VERTEXSTREAM];
	int numAttribs;
};

struct Query
{
    GLuint queryID;
};

struct VertexBuffer 
{
	GLuint vboVB;
	GLsizei size;
};

struct TTFFont
{
	TTF_Font* pfont;
	SamplerStateID sampleState;
	UINT size;
};

struct IndexBuffer 
{
	GLuint vboIB;

	UINT nIndices;
	UINT indexSize;
};

struct SamplerState 
{
	GLuint glSamplerID;
	GLint minFilter;
	GLint magFilter;
	GLint wrapS;
	GLint wrapT;
	GLint wrapR;
	GLint aniso;
	float lod;
};

struct BlendState 
{
	GLenum srcFactorRGB;
	GLenum dstFactorRGB;
	GLenum srcFactorAlpha;
	GLenum dstFactorAlpha;
	GLenum blendModeRGB;
	GLenum blendModeAlpha;
	int mask;
	bool blendEnable;
	bool alphaToCoverageEnable;
};

struct DepthState 
{
	int depthFunc;
	bool depthTest;
	bool depthWrite;
//	bool stencilTest;
//	uint8 stencilMask;
//	int stencilFunc;
//	int stencilFail;
//	int depthFail;
//	int stencilPass;
};

struct RasterizerState 
{
	int cullMode;
	int fillMode;
	bool multiSample;
	bool scissor;
};

struct ObjectBuffer
{
    ObjectBufferType type;
    UINT size;
    BufferAccess bufferAccess;
    GLuint glBufferID;
};

// Blending constants
const int ZERO                 = GL_ZERO;
const int ONE                  = GL_ONE;
const int SRC_COLOR            = GL_SRC_COLOR;
const int ONE_MINUS_SRC_COLOR  = GL_ONE_MINUS_SRC_COLOR;
const int DST_COLOR            = GL_DST_COLOR;
const int ONE_MINUS_DST_COLOR  = GL_ONE_MINUS_DST_COLOR;
const int SRC_ALPHA            = GL_SRC_ALPHA;
const int ONE_MINUS_SRC_ALPHA  = GL_ONE_MINUS_SRC_ALPHA;
const int DST_ALPHA            = GL_DST_ALPHA;
const int ONE_MINUS_DST_ALPHA  = GL_ONE_MINUS_DST_ALPHA;
const int SRC_ALPHA_SATURATE   = GL_SRC_ALPHA_SATURATE;

const int BM_ADD              = GL_FUNC_ADD_EXT;
const int BM_SUBTRACT         = GL_FUNC_SUBTRACT_EXT;
const int BM_REVERSE_SUBTRACT = GL_FUNC_REVERSE_SUBTRACT_EXT;
const int BM_MIN              = GL_MIN_EXT;
const int BM_MAX              = GL_MAX_EXT;

// Depth-test constants
const int NEVER    = GL_NEVER;
const int LESS     = GL_LESS;
const int EQUAL    = GL_EQUAL;
const int LEQUAL   = GL_LEQUAL;
const int GREATER  = GL_GREATER;
const int NOTEQUAL = GL_NOTEQUAL;
const int GEQUAL   = GL_GEQUAL;
const int ALWAYS   = GL_ALWAYS;

// Stencil-test constants
const int KEEP     = GL_KEEP;
const int SET_ZERO = GL_ZERO;
const int REPLACE  = GL_REPLACE;
const int INVERT   = GL_INVERT;
const int INCR     = GL_INCR_WRAP;
const int DECR     = GL_DECR_WRAP;
const int INCR_SAT = GL_INCR;
const int DECR_SAT = GL_DECR;

// Culling constants
const int CULL_NONE  = 0;
const int CULL_BACK  = GL_BACK;
const int CULL_FRONT = GL_FRONT;

// Fillmode constants
const int SOLID = GL_FILL;
const int WIREFRAME = GL_LINE;

void setGLdefaults()
{
	// Set some of my preferred defaults
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glFrontFace(GL_CCW);
	glPixelStorei(GL_PACK_ALIGNMENT,   1);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
}

#if defined(_WIN32)

OpenGLRenderer::OpenGLRenderer(SDL_Window* pSDLWindowHandle, SDL_GLContext GLMainContext) : Renderer()
{
	m_pWindowHandle = pSDLWindowHandle;
	m_MainContext = GLMainContext;

#elif defined(LINUX)

OpenGLRenderer::OpenGLRenderer(Window win, GLXContext glXc, Display *disp, int scr) : Renderer(){
	window = win;
	glxc = glXc;
	display = disp;
	screen = scr;

#elif defined(__APPLE__)

OpenGLRenderer::OpenGLRenderer(AGLContext aGlc) : Renderer(){
	aglc = aGlc;

#endif

	GLint units = 1;

	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &units);

	nImageUnits = units;

	memset(textureLod, 0, sizeof(textureLod));

	GLint mrt = 1;
	glGetIntegerv(GL_MAX_DRAW_BUFFERS, &mrt);
	nMRTs = mrt;

	if (nMRTs > MAX_MRTS) nMRTs = MAX_MRTS;

	for (UINT i = 0; i < nMRTs; i++)
	{
		drawBuffers[i] = GL_COLOR_ATTACHMENT0_EXT + i;
	}

	for (UINT i = 0; i < MAX_VERTEXSTREAM; i++)
	{
		enableVertexAttribs[i] = false;
	}

	vboChange = false;
	
	GLint aniso = 1;
	glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &aniso);
	maxAnisotropic = aniso;

	fbo = 0;
	currentFrontFace = GL_CW;
	m_VAO = 0;

	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);
	
	plainShader = SHADER_NONE;
	plainVF = VF_NONE;
	texShader = SHADER_NONE;
	texVF = VF_NONE;

	rollingVB = VB_NONE;
	rollingVBOffset = 0;

	m_textTexture = TEXTURE_NONE;
	
	setGLdefaults();
	resetToDefaults();

	memset(activeVertexFormat, VF_NONE, sizeof(activeVertexFormat));

	uniformFuncs[CONSTANT_FLOAT] = (void *) glUniform1fv;
	uniformFuncs[CONSTANT_VEC2]  = (void *) glUniform2fv;
	uniformFuncs[CONSTANT_VEC3]  = (void *) glUniform3fv;
	uniformFuncs[CONSTANT_VEC4]  = (void *) glUniform4fv;
	uniformFuncs[CONSTANT_INT]   = (void *) glUniform1iv;
	uniformFuncs[CONSTANT_IVEC2] = (void *) glUniform2iv;
	uniformFuncs[CONSTANT_IVEC3] = (void *) glUniform3iv;
	uniformFuncs[CONSTANT_IVEC4] = (void *) glUniform4iv;
	uniformFuncs[CONSTANT_BOOL]  = (void *) glUniform1iv;
	uniformFuncs[CONSTANT_BVEC2] = (void *) glUniform2iv;
	uniformFuncs[CONSTANT_BVEC3] = (void *) glUniform3iv;
	uniformFuncs[CONSTANT_BVEC4] = (void *) glUniform4iv;
	uniformFuncs[CONSTANT_MAT2]  = (void *) glUniformMatrix2fv;
	uniformFuncs[CONSTANT_MAT3]  = (void *) glUniformMatrix3fv;
	uniformFuncs[CONSTANT_MAT4]  = (void *) glUniformMatrix4fv;
}

OpenGLRenderer::~OpenGLRenderer()
{
	// Restore states to default
	reset();
	apply();

	// Delete shaders
	for (UINT i = 0; i < shaders.size(); i++)
	{
		for (UINT j = 0; j < shaders[i].nSamplers; j++)
		{
			delete shaders[i].samplers[j].name;
		}
		for (UINT j = 0; j < shaders[i].nUniforms; j++)
		{
			delete shaders[i].uniforms[j].name;
			delete shaders[i].uniforms[j].data;
		}
        for (UINT j = 0; j < shaders[i].nImageBuffers; j++)
		{
            delete shaders[i].imageBuffers[j].name;
		}
		delete shaders[i].samplers;
		delete shaders[i].uniforms;
		glDeleteShader(shaders[i].vertexShader);
		glDeleteShader(shaders[i].fragmentShader);
		glDeleteProgram(shaders[i].program);
	}

    // Delete vertex buffers
	for (UINT i = 0; i < vertexBuffers.size(); i++)
	{
		glDeleteBuffers(1, &vertexBuffers[i].vboVB);
	}

	// Delete index buffers
	for (UINT i = 0; i < indexBuffers.size(); i++)
	{
		glDeleteBuffers(1, &indexBuffers[i].vboIB);
	}

	// Delete textures
	for (UINT i = 0; i < textures.size(); i++)
	{
		removeTexture(i);
	}

    // Delete TTF_Font
	for (UINT i = 0; i < TTFFonts.size(); i++)
	{
		TTF_CloseFont(TTFFonts[i].pfont);
	}

    // Delete Queries
    for (UINT i = 0; i < queries.size(); i++)
    {
        glDeleteQueries(1, &queries[i].queryID);
    }

	// Delete Fonts
	fonts.reset();
	

	if (fbo) glDeleteFramebuffers(1, &fbo);
}

void OpenGLRenderer::resetToDefaults()
{
	Renderer::resetToDefaults();

	for (UINT i = 0; i < MAX_TEXTUREUNIT; i++)
	{
		currentTextures[i] = TEXTURE_NONE;
	}

	for (UINT i = 0; i < MAX_SAMPLERSTATE; i++)
	{
		currentSamplerStates[i] = SS_NONE;
	}

	for (UINT i = 0; i < MAX_IMAGEBUFFERUNIT; i++)
	{
		currentImageBuffers[i].imageBufferID = TEXTURE_NONE;
	}
	
    for (UINT i = 0; i < MAX_OBJECTBUFFERUNIT; i++)
    {
        currentObjectBuffers[i] = TEXTURE_NONE;
    }
	
	currentSrcFactorRGB = currentSrcFactorAlpha = ONE;
	currentDstFactorRGB = currentDstFactorAlpha = ZERO;
	currentBlendModeRGB = currentBlendModeAlpha = BM_ADD;
	currentMask = ALL;
	currentBlendEnable = false;
	currentAlphaToCoverageEnable = false;

	currentDepthFunc = LEQUAL;
	currentDepthTestEnable = true;
	currentDepthWriteEnable = true;

	currentCullMode = CULL_NONE;
	currentFillMode = SOLID;
	currentMultiSampleEnable = true;
	currentScissorEnable = false;
}

void OpenGLRenderer::reset(const UINT flags)
{
	Renderer::reset(flags);

	if (flags & RESET_TEX)
	{
		for (UINT i = 0; i < MAX_TEXTUREUNIT; i++){
			selectedTextures[i] = TEXTURE_NONE;
		}
	}

	if (flags & RESET_SS)
	{
		for (UINT i = 0; i < MAX_SAMPLERSTATE; i++)
		{
			selectedSamplerStates[i] = SS_NONE;
		}
	}
	
	if (flags & RESET_IMAGE)
	{
		for (UINT i = 0; i < MAX_IMAGEBUFFERUNIT; i++)
		{
			selectedImageBuffers[i].imageBufferID = TEXTURE_NONE;
		}
	}

    if (flags & RESET_OBJECTBUFFER)
    {
        for (UINT i = 0; i < MAX_OBJECTBUFFERUNIT; i++)
        {
            selectedObjectBuffers[i] = TEXTURE_NONE;
        }
    }

	apply();
}

TextureID OpenGLRenderer::createTexture(Image &img, const SamplerStateID samplerState, UINT flags)
{
	Texture tex;
	memset(&tex, 0, sizeof(tex));

	switch (img.getFormat())
	{
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
	tex.internalFormat = internalFormats[format];
	tex.externalFormat = externalFormats[format];
	tex.format = format;
	tex.mod = false;
	
	if (img.is1D())
	{
		if(arraySize*nSlices > 1)
		{
			tex.width = img.getWidth();
			tex.height = 1;
			tex.depth = 1;
			tex.nMipMaps = nMipMaps;
			tex.arraySize = arraySize;
			tex.nSlices = nSlices;
			tex.glTarget = GL_TEXTURE_1D_ARRAY;

			glGenTextures(1, &tex.glTexID);
			glBindTexture(tex.glTarget, tex.glTexID);
			glTexStorage2D(GL_TEXTURE_1D_ARRAY, nMipMaps, tex.internalFormat, tex.width, nSlices*arraySize);
			
		}
		else
		{
			tex.width = img.getWidth();
			tex.height = 1;
			tex.depth = 1;
			tex.nMipMaps = nMipMaps;
			tex.arraySize = 1;
			tex.nSlices = 1;
			tex.glTarget = GL_TEXTURE_1D;

			glGenTextures(1, &tex.glTexID);
			glBindTexture(tex.glTarget, tex.glTexID);
			glTexStorage1D(GL_TEXTURE_1D, nMipMaps, tex.internalFormat, tex.width);

		}
	} 
	else if (img.is2D() || img.isCube())
	{
		if(arraySize*nSlices > 1)
		{
			tex.width = img.getWidth();
			tex.height = img.getHeight();
			tex.depth = 1;
			tex.nMipMaps = nMipMaps;
			tex.arraySize = arraySize;
			tex.nSlices = nSlices;
			tex.glTarget = GL_TEXTURE_2D_ARRAY;

			glGenTextures(1, &tex.glTexID);
			glBindTexture(tex.glTarget, tex.glTexID);
			glTexStorage3D(GL_TEXTURE_2D_ARRAY, nMipMaps, tex.internalFormat, tex.width, tex.height, nSlices*arraySize);
		}
		else
		{
			tex.width = img.getWidth();
			tex.height = img.getHeight();
			tex.depth = 1;
			tex.nMipMaps = nMipMaps;
			tex.arraySize = 1;
			tex.nSlices = 1;
			tex.glTarget = GL_TEXTURE_2D;

			glGenTextures(1, &tex.glTexID);
			glBindTexture(tex.glTarget, tex.glTexID);
			glTexStorage2D(GL_TEXTURE_2D, nMipMaps, tex.internalFormat, tex.width, tex.height);
		}
	} 
	else if (img.is3D())
	{
		tex.width = img.getWidth();
		tex.height = img.getHeight();
		tex.depth = img.getDepth();
		tex.nMipMaps = nMipMaps;
		tex.arraySize = 1;
		tex.nSlices = 1;
		tex.glTarget = GL_TEXTURE_3D;

		glGenTextures(1, &tex.glTexID);
		glBindTexture(tex.glTarget, tex.glTexID);
		glTexStorage3D(GL_TEXTURE_2D_ARRAY, nMipMaps, tex.internalFormat, tex.width, tex.height, nSlices*arraySize);
	}


	unsigned char* dest = NULL;

	for(UINT i = 0; i < nMipMaps; i++)
	{
		for(UINT j = 0; j < arraySize; j++)
		{
			for(UINT k = 0; k < nSlices; k++)
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

				dest = img.getPixels(i, j) + k * slicePitch;

				if(img.is1D())
				{
					if(arraySize*nSlices <= 1)
					{
						if(!isCompressedFormat(format))
							glTexSubImage1D(tex.glTarget, i, 0, img.getWidth(i), tex.externalFormat, GL_UNSIGNED_BYTE, dest);
						else
							glCompressedTexSubImage1D(tex.glTarget, i, 0, img.getWidth(i), tex.internalFormat, img.getSliceSize(), dest);
						
					}
					else
					{
						if(!isCompressedFormat(format))
							glTexSubImage2D(tex.glTarget, i, 0, j*nSlices + k, img.getWidth(i), img.getHeight(i), tex.externalFormat, GL_UNSIGNED_BYTE, dest);
						else
							glCompressedTexSubImage2D(tex.glTarget, i, 0, j*nSlices + k, img.getWidth(i), img.getHeight(i), tex.internalFormat, img.getSliceSize(), dest);;
					}
				}

				else if(img.is2D())
				{
					if(arraySize*nSlices <= 1)
					{
						if(!isCompressedFormat(format))
							glTexSubImage2D(tex.glTarget, i, 0, 0, img.getWidth(i), img.getHeight(i), tex.externalFormat, GL_UNSIGNED_BYTE, dest);
						else
							glCompressedTexSubImage2D(tex.glTarget, i, 0, 0, img.getWidth(i), img.getHeight(i), tex.externalFormat, img.getSliceSize(), dest);;
					}
					else
					{
						if(!isCompressedFormat(format))
							glTexSubImage3D(tex.glTarget, i, 0, 0, j*nSlices + k, img.getWidth(i), img.getHeight(i), img.getDepth(i), tex.externalFormat, GL_UNSIGNED_BYTE, dest);
						else
							glCompressedTexSubImage3D(tex.glTarget, i, 0, 0, j*nSlices + k, img.getWidth(i), img.getHeight(i), img.getDepth(i), tex.externalFormat, img.getSliceSize(), dest);;
					}
				}

				else if(img.is3D())
				{
					if(!isCompressedFormat(format))
						glTexSubImage3D(tex.glTarget, i, 0, 0, 0, img.getWidth(i), img.getHeight(i), img.getDepth(i), tex.externalFormat, GL_UNSIGNED_BYTE, dest);
					else
						glCompressedTexSubImage3D(tex.glTarget, i, 0, 0, 0, img.getWidth(i), img.getHeight(i), img.getDepth(i), tex.externalFormat, img.getSliceSize(), dest);;
				}
			}
		}
	}

	//Set default samplerStates	
	glTexParameteri(tex.glTarget , GL_TEXTURE_WRAP_S, GL_REPEAT);  
	glTexParameteri(tex.glTarget , GL_TEXTURE_WRAP_T, GL_REPEAT);  
	glTexParameteri(tex.glTarget , GL_TEXTURE_MIN_FILTER , GL_LINEAR);  
	glTexParameteri(tex.glTarget , GL_TEXTURE_MAG_FILTER , GL_LINEAR);

	glBindTexture(tex.glTarget, 0);

	textures.push_back(tex);

	return textures.size() - 1;
}

TextureID OpenGLRenderer::addRenderTarget(const int width, const int height, const int depth, const int mipMapCount, const int arraySize, const FORMAT format, const int msaaSamples, const SamplerStateID samplerState, UINT flags)
{
	Texture tex;
	memset(&tex, 0, sizeof(tex));

	tex.width  = width;
	tex.height = height;
	tex.depth  = depth;
	tex.arraySize = arraySize;
	tex.flags  = flags;
	tex.internalFormat = internalFormats[format];
	tex.externalFormat = externalFormats[format];
	tex.format = format;
	tex.nSlices = arraySize;
	tex.nMipMaps = mipMapCount;

	/*if (flags & SRGB)
	{
		// Change to the matching sRGB format
		switch (tex.texFormat)
		{
			case DXGI_FORMAT_R8G8B8A8_UNORM: tex.texFormat = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; break;
			case DXGI_FORMAT_BC1_UNORM: tex.texFormat = DXGI_FORMAT_BC1_UNORM_SRGB; break;
			case DXGI_FORMAT_BC2_UNORM: tex.texFormat = DXGI_FORMAT_BC2_UNORM_SRGB; break;
			case DXGI_FORMAT_BC3_UNORM: tex.texFormat = DXGI_FORMAT_BC3_UNORM_SRGB; break;
		}
	}*/


	if (depth == 1)
	{
		if (flags & CUBEMAP)
		{
			tex.arraySize = 6;			
		} 
		
		/*if (flags & USE_MIPGEN)
		{
			desc.MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;
		}*/

		if(tex.arraySize > 1)
		{
			tex.glTarget = GL_TEXTURE_2D_ARRAY;
			glGenTextures(1, &tex.glTexID);
			glBindTexture(tex.glTarget, tex.glTexID);
			//glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, tex.format, tex.width, tex.height, arraySize);

			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            for(unsigned int mip = 0; mip < mipMapCount; mip++)
            {
			    glTexImage3D(GL_TEXTURE_2D_ARRAY, mip, tex.internalFormat, tex.width >> mip, tex.height >> mip, tex.arraySize, 0, tex.externalFormat, sources[format], 0);
            }
		}
		else
		{
			tex.glTarget = GL_TEXTURE_2D;
			glGenTextures(1, &tex.glTexID);
			glBindTexture(tex.glTarget, tex.glTexID);
			//glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, tex.internalFormat, tex.width, tex.height, arraySize);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            for(unsigned int mip = 0; mip < mipMapCount; mip++)
            {
			    glTexImage2D(GL_TEXTURE_2D, mip, tex.internalFormat, tex.width >> mip, tex.height >> mip, 0, tex.externalFormat, sources[format], 0);
            }
		}
	} 
	else 
	{
		/*if (flags & USE_MIPGEN)
		{
			desc.MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;
		}*/

		tex.glTarget = GL_TEXTURE_3D;
		glGenTextures(1, &tex.glTexID);
		glBindTexture(tex.glTarget, tex.glTexID);

		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		//glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, tex.internalFormat, tex.width, tex.height, arraySize);
        for(unsigned int mip = 0; mip < mipMapCount; mip++)
        {
		    glTexImage3D(GL_TEXTURE_3D, mip, tex.internalFormat, tex.width >> mip, tex.height >> mip, tex.depth >> mip, 0, tex.externalFormat, sources[format], 0);
        }
		tex.glTarget = GL_TEXTURE_3D; 
	}

	glBindTexture(tex.glTarget, 0);

	textures.push_back(tex);

	return textures.size() - 1;

	/*if (isCompressedFormat(format) || arraySize > 1) return false;

	// TODO: Implement
	if (mipMapCount > 1) return false;

//	if (flags & CUBEMAP) flags |= CLAMP_ST;

	Texture tex;
	memset(&tex, 0, sizeof(tex));
	tex.glTarget = (flags & CUBEMAP)? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D;
	tex.flags  = flags;
	tex.internalFormat = format;

	glGenTextures(1, &tex.glTexID);
	glBindTexture(tex.glTarget, tex.glTexID);
	setupSampler(tex.glTarget, samplerStates[samplerState]);
	tex.samplerState = samplerState;

	textures.push_back(tex);
	TextureID rt = textures.size() - 1;
	setRenderTargetSize(rt, width, height);

	glBindTexture(tex.glTarget, 0);

	return rt;*/
}

TextureID OpenGLRenderer::addRenderDepth(const int width, const int height, const int arraySize, const FORMAT format, const int msaaSamples, const SamplerStateID samplerState, UINT flags)
{
	Texture tex;
	memset(&tex, 0, sizeof(tex));

	tex.width  = width;
	tex.height = height;
	tex.depth  = 1;
	tex.arraySize = arraySize;
	tex.flags  = flags;
	tex.internalFormat = internalFormats[format];
	tex.nSlices = 1;
	tex.nMipMaps = 1;
	tex.glTarget = GL_RENDERBUFFER; 

	/*if (flags & CUBEMAP)
	{
		desc.ArraySize = 6;
		desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
	} 
	else 
	{
		desc.ArraySize = arraySize;
		desc.MiscFlags = 0;
	}*/

	/*if (flags & SAMPLE_DEPTH)
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
	}*/

	glGenRenderbuffers(1, &tex.glDepthID);
	glBindRenderbuffer(GL_RENDERBUFFER, tex.glDepthID);
	glRenderbufferStorage(GL_RENDERBUFFER, tex.internalFormat, width, height);

	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	textures.push_back(tex);

	return textures.size() - 1;

}

void OpenGLRenderer::setRenderTargetSize(const TextureID renderTarget, const int width, const int height)
{
	textures[renderTarget].width  = width;
	textures[renderTarget].height = height;

	if (textures[renderTarget].glTarget == GL_RENDERBUFFER)
	{
		GLint format = textures[renderTarget].format;

		// Bind render buffer
		glBindRenderbuffer(GL_RENDERBUFFER, textures[renderTarget].glDepthID);
		glRenderbufferStorage(GL_RENDERBUFFER, internalFormats[format], width, height);

		// Restore renderbuffer
		glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);

	} 
	else 
	{
		GLint format = textures[renderTarget].format;

		// Allocate all required surfaces.
		glBindTexture(textures[renderTarget].glTarget, textures[renderTarget].glTexID);
		if (textures[renderTarget].flags & CUBEMAP)
		{
			for (int i = GL_TEXTURE_CUBE_MAP_POSITIVE_X; i <= GL_TEXTURE_CUBE_MAP_NEGATIVE_Z; i++)
				glTexImage2D(i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, NULL);
		} 
		else 
		{
			glTexImage2D(textures[renderTarget].glTarget, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, NULL);
		}

		glBindTexture(textures[renderTarget].glTarget, 0);
	}
}

bool OpenGLRenderer::resizeRenderTarget(const TextureID renderTarget, const int width, const int height, const int depth, const int mipMapCount, const int arraySize)
{
	if (depth > 1 || arraySize > 1 || mipMapCount > 1) return false;

	if (width != textures[renderTarget].width || height != textures[renderTarget].height)
	{
		setRenderTargetSize(renderTarget, width, height);
	}

	return true;
}

bool OpenGLRenderer::generateMipMaps(const TextureID renderTarget)
{
	// TODO: Implement
	return false;
}

void OpenGLRenderer::removeTexture(const TextureID texture)
{
	if (textures[texture].glTarget)
	{
		if (textures[texture].glTarget == GL_RENDERBUFFER)
		{
			glDeleteRenderbuffers(1, &textures[texture].glDepthID);
		} 
		else 
		{
			glDeleteTextures(1, &textures[texture].glTexID);
		}
		textures[texture].glTarget = 0;
	}
}

int samplerCompare(const void *sampler0, const void *sampler1)
{
	return strcmp(((Sampler *) sampler0)->name, ((Sampler *) sampler1)->name);
}

ShaderID OpenGLRenderer::createShader(const char *vsText, const char *gsText, const char *fsText, const char *csText, const char* tcsText, const char* tesText, const int vsLine, const int gsLine, const int fsLine, const int csLine,
									  const int tcsLine, const int tesLine, const char *header, const char *extra, const char *fileName, const char **attributeNames, const int nAttributes, const UINT flags)
{

	if (vsText == NULL && fsText == NULL && gsText == NULL && csText == NULL) return SHADER_NONE;

	Shader shader;
	std::string shaderString;
	char line[16];
	GLint vsResult, tcsResult, tesResult, gsResult, fsResult, csResult, linkResult;
	char infoLog[2048];
	GLint len, infoLogPos = 0;

	memset(&shader, 0, sizeof(Shader));

	// Compile to the highest supported language version
	static char versionString[16];
	static GLint versionLen;
	static bool first = true;
	if (first)
	{
		const char *version = (const char *) glGetString(GL_SHADING_LANGUAGE_VERSION);
		if (version)
		{
			int major = atoi(version);
			int minor = atoi(strchr(version, '.') + 1);
			versionLen = sprintf(versionString, "#version %u%02u\n", major, minor);		
		}
		first = false;
	}	

	/////////VERTEX SHADER///////////////////////////////////////////////////////////////////////////////////////////////////

	shaderString.append(versionString, versionLen);	

	// A couple of defines to make life easier for HLSL users
	shaderString += 
		"#define saturate(x) clamp(x,0.0,1.0)\n"
		"#define lerp mix\n\0";

	if (extra) shaderString += extra;
	if (header) shaderString += header;
	
	shader.vertexShader = 0;
    shader.geometryShader = 0;
	shader.fragmentShader = 0;
	shader.computeShader = 0;
	shader.program = glCreateProgram();

	// Compile the vertex shader
	if (vsText != NULL)
	{	
		shader.vertexShader = glCreateShader(GL_VERTEX_SHADER);
		len = sprintf(line, "#line %d\n", vsLine);
		shaderString.append(line, len);
		shaderString += vsText;
		char const * vertexShaderSource = shaderString.c_str();
		glShaderSource(shader.vertexShader, 1, &vertexShaderSource, NULL);
		glCompileShader(shader.vertexShader);
		glGetShaderiv(shader.vertexShader, GL_COMPILE_STATUS, &vsResult);
		if (vsResult)
		{
			glAttachShader(shader.program, shader.vertexShader);
		} 
		else 
		{
			infoLogPos += sprintf(infoLog + infoLogPos, "Vertex shader error:\n");
		}
		glGetShaderInfoLog(shader.vertexShader, sizeof(infoLog) - infoLogPos, &len, infoLog + infoLogPos);
		infoLogPos += len;
	} 
	else vsResult = GL_TRUE;

	////////TESSELLATION CONTROL SHADER////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	shaderString.resize(0);

	shaderString.append(versionString, versionLen);

	// A couple of defines to make life easier for HLSL users
	shaderString += 
		"#define saturate(x) clamp(x,0.0,1.0)\n"
		"#define lerp mix\n\0";

	if (extra) shaderString += extra;
	if (header) shaderString += header;

	// Compile the fragment shader
	if (tcsText != NULL)
	{
		shader.tessellationControlShader = glCreateShader(GL_TESS_CONTROL_SHADER);
		len = sprintf(line, "#line %d\n", tcsLine);
		shaderString.append(line, len);
		shaderString += tcsText;
		char const * tesselationControlShaderSource = shaderString.c_str();
		glShaderSource(shader.tessellationControlShader, 1, &tesselationControlShaderSource, NULL);
		glCompileShader(shader.tessellationControlShader);
		glGetShaderiv(shader.tessellationControlShader, GL_COMPILE_STATUS, &tcsResult);
		if (tcsResult)
		{
			glAttachShader(shader.program, shader.tessellationControlShader);
		} 
		else 
		{
			infoLogPos += sprintf(infoLog + infoLogPos, "Tesselation control shader error:\n");
		}
		glGetShaderInfoLog(shader.tessellationControlShader, sizeof(infoLog) - infoLogPos, &len, infoLog + infoLogPos);
		infoLogPos += len;
	} 
	else tcsResult = GL_TRUE;

	////////TESSELLATION EVALUATION SHADER////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	shaderString.resize(0);

	shaderString.append(versionString, versionLen);

	// A couple of defines to make life easier for HLSL users
	shaderString += 
		"#define saturate(x) clamp(x,0.0,1.0)\n"
		"#define lerp mix\n\0";

	if (extra) shaderString += extra;
	if (header) shaderString += header;

	// Compile the fragment shader
	if (tesText != NULL)
	{
		shader.tessellationEvaluationShader = glCreateShader(GL_TESS_EVALUATION_SHADER);
		len = sprintf(line, "#line %d\n", tesLine);
		shaderString.append(line, len);
		shaderString += tesText;
		char const * tessellationEvaluationShaderSource = shaderString.c_str();
		glShaderSource(shader.tessellationEvaluationShader, 1, &tessellationEvaluationShaderSource, NULL);
		glCompileShader(shader.tessellationEvaluationShader);
		glGetShaderiv(shader.tessellationEvaluationShader, GL_COMPILE_STATUS, &tesResult);
		if (tesResult)
		{
			glAttachShader(shader.program, shader.tessellationEvaluationShader);
		} 
		else 
		{
			infoLogPos += sprintf(infoLog + infoLogPos, "Tessellation evaluation shader error:\n");
		}
		glGetShaderInfoLog(shader.tessellationEvaluationShader, sizeof(infoLog) - infoLogPos, &len, infoLog + infoLogPos);
		infoLogPos += len;
	} 
	else tesResult = GL_TRUE;

	////////GEOMETRY SHADER////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    shaderString.resize(0);

	shaderString.append(versionString, versionLen);

	// A couple of defines to make life easier for HLSL users
	shaderString += 
		"#define saturate(x) clamp(x,0.0,1.0)\n"
		"#define lerp mix\n\0";

	if (extra) shaderString += extra;
	if (header) shaderString += header;

	// Compile the fragment shader
	if (gsText != NULL)
	{
		shader.geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
		len = sprintf(line, "#line %d\n", gsLine);
		shaderString.append(line, len);
		shaderString += gsText;
		char const * geometryShaderSource = shaderString.c_str();
		glShaderSource(shader.geometryShader, 1, &geometryShaderSource, NULL);
		glCompileShader(shader.geometryShader);
		glGetShaderiv(shader.geometryShader, GL_COMPILE_STATUS, &gsResult);
		if (gsResult)
		{
			glAttachShader(shader.program, shader.geometryShader);
		} 
		else 
		{
			infoLogPos += sprintf(infoLog + infoLogPos, "Geometry shader error:\n");
		}
		glGetShaderInfoLog(shader.geometryShader, sizeof(infoLog) - infoLogPos, &len, infoLog + infoLogPos);
		infoLogPos += len;
	} 
	else gsResult = GL_TRUE;

	/////////FRAGMENT SHADER//////////////////////////////////////////////////////////////////////////////////////////////////////////////

	shaderString.resize(0);

	shaderString.append(versionString, versionLen);

	// A couple of defines to make life easier for HLSL users
	shaderString += 
		"#define saturate(x) clamp(x,0.0,1.0)\n"
		"#define lerp mix\n\0";

	if (extra) shaderString += extra;
	if (header) shaderString += header;

	// Compile the fragment shader
	if (fsText != NULL)
	{
		shader.fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		len = sprintf(line, "#line %d\n", fsLine);
		shaderString.append(line, len);
		shaderString += fsText;
		char const * fragmentShaderSource = shaderString.c_str();
		glShaderSource(shader.fragmentShader, 1, &fragmentShaderSource, NULL);
		glCompileShader(shader.fragmentShader);
		glGetShaderiv(shader.fragmentShader, GL_COMPILE_STATUS, &fsResult);
		if (fsResult)
		{
			glAttachShader(shader.program, shader.fragmentShader);
		} 
		else 
		{
			infoLogPos += sprintf(infoLog + infoLogPos, "Fragment shader error:\n");
		}
		glGetShaderInfoLog(shader.fragmentShader, sizeof(infoLog) - infoLogPos, &len, infoLog + infoLogPos);
		infoLogPos += len;
	} 
	else fsResult = GL_TRUE;

	////////COMPUTE SHADER////////////////////////////////////////////////////////////////////////////////////////////////

	shaderString.resize(0);

	shaderString.append(versionString, versionLen);

	if (extra) shaderString += extra;
	if (header) shaderString += header;

	// Compile the fragment shader
	if (csText != NULL)
	{
		shader.computeShader = glCreateShader(GL_COMPUTE_SHADER);
		len = sprintf(line, "#line %d\n", csLine);
		shaderString.append(line, len);
		shaderString += csText;
		char const * computeShaderSource = shaderString.c_str();
		glShaderSource(shader.computeShader, 1, &computeShaderSource, NULL);
		glCompileShader(shader.computeShader);
		glGetShaderiv(shader.computeShader, GL_COMPILE_STATUS, &csResult);
		if (csResult)
		{
			glAttachShader(shader.program, shader.computeShader);
		} 
		else 
		{
			infoLogPos += sprintf(infoLog + infoLogPos, "Compute shader error:\n");
		}
		glGetShaderInfoLog(shader.computeShader, sizeof(infoLog) - infoLogPos, &len, infoLog + infoLogPos);
		infoLogPos += len;
	} 
	else csResult = GL_TRUE;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Link the shaders
	if (vsResult && tcsResult && tesResult && gsResult && fsResult && csResult)
	{
		for (int i = 0; i < nAttributes; i++)
		{
			if (attributeNames[i]) glBindAttribLocation(shader.program, i, attributeNames[i]);
		}

		glLinkProgram(shader.program);
		glGetProgramiv(shader.program, GL_LINK_STATUS, &linkResult);
		glGetProgramInfoLog(shader.program, sizeof(infoLog) - infoLogPos, &len, infoLog + infoLogPos);
		infoLogPos += len;

		//outputDebugString(infoLog);

		if (linkResult)
		{
			GLuint currProgram = (currentShader == SHADER_NONE)? 0 : shaders[currentShader].program;

			glUseProgram(shader.program);

			GLint uniformCount, maxLength;
			glGetProgramiv(shader.program, GL_ACTIVE_UNIFORMS, &uniformCount);
			glGetProgramiv(shader.program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxLength);

			if(csText != NULL)
			{
				int workGroupSize[3];
				glGetProgramiv(shader.program, GL_COMPUTE_WORK_GROUP_SIZE, workGroupSize);

				shader.localSizeX = workGroupSize[0];
				shader.localSizeY = workGroupSize[1];
				shader.localSizeZ = workGroupSize[2];
			}

			if(tesText != NULL)
			{
				glPatchParameteri(GL_PATCH_VERTICES, 3);
			}

			Sampler  *samplers = (Sampler  *) malloc(uniformCount * sizeof(Sampler));
			Constant *uniforms = (Constant *) malloc(uniformCount * sizeof(Constant));
            ImageBuffer *imageBuffers = (ImageBuffer *) malloc(uniformCount * sizeof(ImageBuffer));
			ObjectBufferEntry *objectBuffers = (ObjectBufferEntry *) malloc(uniformCount * sizeof(ObjectBufferEntry));

			int nSamplers = 0;
			int nUniforms = 0;
            int nImageBuffers = 0;
			int nObjectBuffers = 0;
			int atomicCounterIndex = 0;
			int storageBufferIndex = 0;
			char *name = CHG_NEW char[maxLength];

			for (int i = 0; i < uniformCount; i++)
			{
				GLenum type;
				GLint length, size;
				glGetActiveUniform(shader.program, i, maxLength, &length, &size, &type, name);

				if (type >= GL_SAMPLER_1D && type <= GL_SAMPLER_2D_RECT_SHADOW)
				{
					// Assign samplers to image units
					GLint location = glGetUniformLocation(shader.program, name);
					glUniform1i(location, nSamplers);

					if(size > 1)
					{
						if(size > uniformCount)
						{
							samplers = (Sampler  *) realloc(samplers, size * uniformCount * sizeof(Sampler));
						}

						name[length - 3] = '\0';
						length -= 3;	
					}

					for (UINT iElement = 0; iElement < size; ++iElement) 
					{
						if(iElement > 0)
						{
							samplers[nSamplers].name = CHG_NEW char[length + 6];

							strcpy(samplers[nSamplers].name, name);
							strcat(samplers[nSamplers].name, "Array");
						}
						else
						{
							samplers[nSamplers].name = CHG_NEW char[length + 1];
							strcpy(samplers[nSamplers].name, name);
						}

						samplers[nSamplers].index = nSamplers;
						nSamplers++;
					}
				}
                else if(type >= GL_IMAGE_1D && type <= GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY)
                {
                    // Assign samplers to image units
					GLint location = glGetUniformLocation(shader.program, name);
					glUniform1i(location, nImageBuffers);

					if(size > 1)
					{
						if(size > uniformCount)
						{
							imageBuffers = (ImageBuffer *) realloc(imageBuffers, size * uniformCount * sizeof(ImageBuffer));
						}

						name[length - 3] = '\0';
						length -= 3;						
					}

					for (UINT iElement = 0; iElement < size; ++iElement) 
					{
						if(iElement > 0)
						{
							imageBuffers[nImageBuffers].name = CHG_NEW char[length + 6];

							strcpy(imageBuffers[nImageBuffers].name, name);
							strcat(imageBuffers[nImageBuffers].name, "Array");
						}
						else
						{
							imageBuffers[nImageBuffers].name = CHG_NEW char[length + 1];
							strcpy(imageBuffers[nImageBuffers].name, name);
						}

						imageBuffers[nImageBuffers].index = nImageBuffers;
						imageBuffers[nImageBuffers].size = size;
						nImageBuffers++;
					}
                }
				else if(type == GL_UNSIGNED_INT_ATOMIC_COUNTER)
				{
					GLint location = -1;
					glGetActiveAtomicCounterBufferiv(shader.program, atomicCounterIndex, GL_ATOMIC_COUNTER_BUFFER_BINDING, &location);

					//glUniform1i(location, nObjectBuffers);

					objectBuffers[nObjectBuffers].name = CHG_NEW char[length + 1];
					strcpy(objectBuffers[nObjectBuffers].name, name);
					objectBuffers[nObjectBuffers].index = location;

					nObjectBuffers++;
				}
				else if(type == GL_SHADER_STORAGE_BUFFER)
				{
					GLint location = -1;
					GLenum props[1] = {GL_BUFFER_BINDING};
					glGetProgramResourceiv(shader.program, GL_SHADER_STORAGE_BLOCK, storageBufferIndex, 1, props, 1, NULL, &location);

					//glUniform1i(location, nObjectBuffers);

					objectBuffers[nObjectBuffers].name = CHG_NEW char[length + 1];
					strcpy(objectBuffers[nObjectBuffers].name, name);
					objectBuffers[nObjectBuffers].index = location;

					nObjectBuffers++;
					storageBufferIndex++;
				}
				else 
				{
					// Store all non-gl uniforms
					if (strncmp(name, "gl_", 3) != 0)
					{
						char *bracket = strchr(name, '[');
						if (bracket == NULL || (bracket[1] == '0' && bracket[2] == ']'))
						{
							if (bracket)
							{
								*bracket = '\0';
								length = (GLint) (bracket - name);
							}

							uniforms[nUniforms].name = CHG_NEW char[length + 1];
							uniforms[nUniforms].index = glGetUniformLocation(shader.program, name);
							uniforms[nUniforms].type = getConstantType(type);
							uniforms[nUniforms].nElements = size;
							strcpy(uniforms[nUniforms].name, name);
							nUniforms++;
						} 
						else if (bracket != NULL && bracket[1] > '0')
						{
							*bracket = '\0';
							for (int i = nUniforms - 1; i >= 0; i--)
							{
								if (strcmp(uniforms[i].name, name) == 0)
								{
									int index = atoi(bracket + 1) + 1;
									if (index > uniforms[i].nElements)
									{
										uniforms[i].nElements = index;
									}
								}
							}
						}
					}
				}
			}

			// Restore the current program
			glUseProgram(currProgram);

			delete[] name;

			// Shorten arrays to actual count
			samplers = (Sampler  *) realloc(samplers, nSamplers * sizeof(Sampler));
			uniforms = (Constant *) realloc(uniforms, nUniforms * sizeof(Constant));
            imageBuffers = (ImageBuffer *) realloc(imageBuffers, nImageBuffers * sizeof(ImageBuffer));
			objectBuffers = (ObjectBufferEntry *) realloc(objectBuffers, nObjectBuffers * sizeof(ObjectBufferEntry));
			qsort(samplers, nSamplers, sizeof(Sampler),  samplerComp);
			qsort(uniforms, nUniforms, sizeof(Constant), constantComp);
            qsort(imageBuffers, nImageBuffers, sizeof(ImageBuffer), imageBufferComp);
			qsort(objectBuffers, nObjectBuffers, sizeof(ObjectBuffer), objectBufferComp);

			for (int i = 0; i < nUniforms; i++)
			{
				int constantSize = constantTypeSizes[uniforms[i].type] * uniforms[i].nElements;
				uniforms[i].data = CHG_NEW unsigned char[constantSize];
				memset(uniforms[i].data, 0, constantSize);
				uniforms[i].dirty = false;
			}
			shader.uniforms  = uniforms;
			shader.samplers  = samplers;
            shader.imageBuffers = imageBuffers;
			shader.objectBuffers = objectBuffers;
			shader.nUniforms = nUniforms;
			shader.nSamplers = nSamplers;
            shader.nImageBuffers = nImageBuffers;
			shader.nObjectBuffers = nObjectBuffers;

			shaders.push_back(shader);

			return shaders.size() - 1;
		}
	}

	printf("\n%s\n", infoLog);
	return SHADER_NONE;
}

bool OpenGLRenderer::getShaderWorkGroupSize(const int shaderID, UINT* localGroupSize)
{
	if(shaderID == SHADER_NONE || localGroupSize == NULL) return false;

	localGroupSize[0] = 1;
	localGroupSize[1] = 1;
	localGroupSize[2] = 1;

	return true;
}

VertexFormatID OpenGLRenderer::addVertexFormat(const FormatDesc *formatDesc, const UINT nAttribs, const ShaderID shader)
{
	VertexFormat vertexFormat;

	memset(&vertexFormat, 0, sizeof(vertexFormat));

	UINT numAttrib = 0;

	for (UINT i = 0; i < nAttribs; i++)
	{
		int stream = formatDesc[i].stream;

		switch (formatDesc[i].type)
		{
		case TYPE_GENERIC:
		case TYPE_TANGENT:
		case TYPE_BINORMAL:
		case TYPE_VERTEX:
		case TYPE_NORMAL:
		case TYPE_TEXCOORD:
			vertexFormat.attribs[numAttrib].index = i;
			vertexFormat.attribs[numAttrib].stream = stream;
			vertexFormat.attribs[numAttrib].size   = formatDesc[i].size;
			vertexFormat.attribs[numAttrib].offset = vertexFormat.vertexSize[stream];
			vertexFormat.attribs[numAttrib].format = formatDesc[i].format;
			numAttrib++;
			break;
		}
		
		vertexFormat.vertexSize[stream] += formatDesc[i].size * getFormatSize(formatDesc[i].format);
	}

	vertexFormat.numAttribs = numAttrib;

	vertexFormats.push_back(vertexFormat);

	return vertexFormats.size() - 1;
}

GLenum usages[] = 
{
	GL_STATIC_DRAW,
	GL_DYNAMIC_DRAW,
	GL_STREAM_DRAW,
};

GLenum imageBufferUsages[] =
{
	GL_WRITE_ONLY,
	GL_READ_ONLY,
	GL_READ_WRITE
};

VertexBufferID OpenGLRenderer::addVertexBuffer(const long size, const BufferAccess bufferAccess, const void *data)
{
	VertexBuffer vb;
	vb.size = size;

	glGenBuffers(1, &vb.vboVB);
	glBindBuffer(GL_ARRAY_BUFFER, vb.vboVB);
	glBufferData(GL_ARRAY_BUFFER, size, data, usages[bufferAccess]);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	vertexBuffers.push_back(vb);

	return vertexBuffers.size() - 1;
}

IndexBufferID OpenGLRenderer::addIndexBuffer(const UINT nIndices, const UINT indexSize, const BufferAccess bufferAccess, const void *data)
{
	IndexBuffer ib;

	ib.nIndices = nIndices;
	ib.indexSize = indexSize;

	UINT size = nIndices * indexSize;
	glGenBuffers(1, &ib.vboIB);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib.vboIB);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, usages[bufferAccess]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	indexBuffers.push_back(ib);

	return indexBuffers.size() - 1;
}

ObjectBufferID OpenGLRenderer::createObjectBuffer(const ObjectBufferType type, const UINT objectSize, const UINT nObjects, const BufferAccess bufferAccess, const void *data)
{
    ObjectBuffer ob;

    ob.bufferAccess = bufferAccess;
    ob.size = objectSize*nObjects;
    ob.type = type;

    glGenBuffers(1, &ob.glBufferID);
    glBindBuffer(srcObjectBufferType[ob.type], ob.glBufferID);
    glBufferData(srcObjectBufferType[ob.type], ob.size, data, usages[ob.bufferAccess]);
    glBindBuffer(srcObjectBufferType[ob.type], 0);

    objectBuffers.push_back(ob);

    return objectBuffers.size() - 1;
}
GLint minFilters[] = 
{
	GL_NEAREST,
	GL_LINEAR,
	GL_LINEAR_MIPMAP_NEAREST,
	GL_LINEAR_MIPMAP_LINEAR,
	GL_LINEAR_MIPMAP_NEAREST,
	GL_LINEAR_MIPMAP_LINEAR,
};

SamplerStateID OpenGLRenderer::addSamplerState(const Filter filter, const AddressMode s, const AddressMode t, const AddressMode r, const float lod, const UINT maxAniso, const int compareFunc, const float *border_color)
{
	if (compareFunc != 0 || border_color != NULL) return SS_NONE;

	SamplerState samplerState;

	samplerState.minFilter = minFilters[filter];
	samplerState.magFilter = (filter == NEAREST)? GL_NEAREST : GL_LINEAR;
	samplerState.wrapS = (s == WRAP)? GL_REPEAT : GL_CLAMP_TO_EDGE;
	samplerState.wrapT = (t == WRAP)? GL_REPEAT : GL_CLAMP_TO_EDGE;
	samplerState.wrapR = (r == WRAP)? GL_REPEAT : GL_CLAMP_TO_EDGE;
	samplerState.aniso = hasAniso(filter)? std::min((UINT) maxAnisotropic, maxAniso) : 1;
	samplerState.lod = lod;

	glGenSamplers(1, &samplerState.glSamplerID);
	glSamplerParameteri(samplerState.glSamplerID, GL_TEXTURE_MAG_FILTER, samplerState.minFilter);
	glSamplerParameteri(samplerState.glSamplerID, GL_TEXTURE_MAG_FILTER, samplerState.magFilter);
	glSamplerParameteri(samplerState.glSamplerID, GL_TEXTURE_WRAP_S, samplerState.wrapS);
	glSamplerParameteri(samplerState.glSamplerID, GL_TEXTURE_WRAP_T, samplerState.wrapT);
	glSamplerParameteri(samplerState.glSamplerID, GL_TEXTURE_WRAP_R, samplerState.wrapR);
	glSamplerParameterf(samplerState.glSamplerID, GL_TEXTURE_MAX_ANISOTROPY_EXT, samplerState.aniso);


	samplerStates.push_back(samplerState);

	return samplerStates.size() - 1;
}

BlendStateID OpenGLRenderer::addBlendState(const int srcFactorRGB, const int destFactorRGB, const int srcFactorAlpha, const int destFactorAlpha, const int blendModeRGB, const int blendModeAlpha, const int mask, const bool alphaToCoverage)
{
	BlendState blendState;

	blendState.srcFactorRGB   = srcFactorRGB;
	blendState.dstFactorRGB   = destFactorRGB;
	blendState.srcFactorAlpha = srcFactorAlpha;
	blendState.dstFactorAlpha = destFactorAlpha;
	blendState.blendModeRGB   = blendModeRGB;
	blendState.blendModeAlpha = blendModeAlpha;
	blendState.mask = mask;
	blendState.blendEnable = (srcFactorRGB != ONE || destFactorRGB != ZERO || srcFactorAlpha != ONE || destFactorAlpha != ZERO);
	blendState.alphaToCoverageEnable = alphaToCoverage;

	blendStates.push_back(blendState);

	return blendStates.size() - 1;
}

DepthStateID OpenGLRenderer::addDepthState(const bool depthTest, const bool depthWrite, const int depthFunc, const bool stencilTest, const UINT8 stencilReadMask, const UINT8 stencilWriteMask,
						   const int stencilFuncFront, const int stencilFuncBack, const int stencilFailFront, const int stencilFailBack,
						   const int depthFailFront, const int depthFailBack, const int stencilPassFront, const int stencilPassBack)
{
	//Future Work : Implement stencil test
	DepthState depthState;

	depthState.depthTest  = depthTest;
	depthState.depthWrite = depthWrite;
	depthState.depthFunc  = depthFunc;

	depthStates.push_back(depthState);

	return depthStates.size() - 1;
}

RasterizerStateID OpenGLRenderer::addRasterizerState(const int cullMode, const int fillMode, const bool multiSample, const bool scissor, const float depthBias, const float slopeDepthBias)
{
	RasterizerState rasterizerState;

	rasterizerState.cullMode = cullMode;
	rasterizerState.fillMode = fillMode;
	rasterizerState.multiSample = multiSample;
	rasterizerState.scissor = scissor;

	rasterizerStates.push_back(rasterizerState);

	return rasterizerStates.size() - 1;
}

FontID OpenGLRenderer::addFont(const char *ttfFontFile, const UINT size, const SamplerStateID samplerState)
{
	TTFFont font;

	font.pfont = TTF_OpenFont(ttfFontFile, size);

	if(!font.pfont)
	{
		printf("Dont load font");
		return -1;
	}

	font.sampleState = samplerState;
	font.size = size;

	TTFFonts.push_back(font);

	return TTFFonts.size() - 1;
}

FontID OpenGLRenderer::addFont(char *rawBuffer, unsigned int rawSize, const UINT size, const SamplerStateID samplerState)
{
	TTFFont font;

	SDL_RWops *rw = SDL_RWFromMem(rawBuffer, rawSize);

	font.pfont = TTF_OpenFontRW(rw, 1, size);

	if(!font.pfont)
	{
		printf("Dont load font");
		return -1;
	}

	font.sampleState = samplerState;
	font.size = size;

	TTFFonts.push_back(font);

	return TTFFonts.size() - 1;
}

int OpenGLRenderer::getSamplerUnit(const ShaderID shader, const char *samplerName) const 
{
	//ASSERT(shader != SHADER_NONE);
	if(shader == SHADER_NONE) return -1;

	Sampler *samplers = shaders[shader].samplers;
	int minSampler = 0;
	int maxSampler = shaders[shader].nSamplers - 1;

	// Do a quick lookup in the sorted table with a binary search
	while (minSampler <= maxSampler)
	{
		int currSampler = (minSampler + maxSampler) >> 1;
        int res = strcmp(samplerName, samplers[currSampler].name);
		if (res == 0)
		{
			return samplers[currSampler].index;
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

	return -1;
}

int OpenGLRenderer::getImageBufferUnit(const ShaderID shader, const char *imageBufferName) const
{
	if(shader == SHADER_NONE) return -1;

	ImageBuffer *imageBuffers = shaders[shader].imageBuffers;
	int minImageBuffer = 0;
	int maxImageBuffer = shaders[shader].nImageBuffers - 1;

	// Do a quick lookup in the sorted table with a binary search
	while (minImageBuffer <= maxImageBuffer)
	{
		int currImageBuffer = (minImageBuffer + maxImageBuffer) >> 1;
        int res = strcmp(imageBufferName, imageBuffers[currImageBuffer].name);
		if (res == 0)
		{
			return imageBuffers[currImageBuffer].index;
		} 
		else if (res > 0)
		{
            minImageBuffer = currImageBuffer + 1;
		} 
		else 
		{
            maxImageBuffer = currImageBuffer - 1;
		}
	}

	return -1;
}


int OpenGLRenderer::getObjectBufferUnit(const ShaderID shader, const char *objectBufferName) const
{
    if(shader == SHADER_NONE) return -1;

    ObjectBufferEntry *objectBuffers = shaders[shader].objectBuffers;
	int minObjectBuffer = 0;
	int maxObjectBuffer = shaders[shader].nObjectBuffers - 1;

	// Do a quick lookup in the sorted table with a binary search
	while (minObjectBuffer <= maxObjectBuffer)
	{
		int currObjectBuffer = (minObjectBuffer + maxObjectBuffer) >> 1;
        int res = strcmp(objectBufferName, objectBuffers[currObjectBuffer].name);
		if (res == 0)
		{
			return objectBuffers[currObjectBuffer].index;
		} 
		else if (res > 0)
		{
            minObjectBuffer = currObjectBuffer + 1;
		} 
		else 
		{
            maxObjectBuffer = currObjectBuffer - 1;
		}
	}

	return -1;
}

/*
void OpenGLRenderer::changeTexture(const uint imageUnit, const TextureID texture){
	TextureID currTex = currentTextures[imageUnit];

	if (texture != currTex){
		glActiveTexture(GL_TEXTURE0 + imageUnit);
		if (texture == TEXTURE_NONE){
			glDisable(textures[currTex].glTarget);
		} else {
			if (currTex == TEXTURE_NONE){
				glEnable(textures[texture].glTarget);
			} else if (textures[texture].glTarget != textures[currTex].glTarget){
				glDisable(textures[currTex].glTarget);
				glEnable(textures[texture].glTarget);
			}
			glBindTexture(textures[texture].glTarget, textures[texture].glTexID);
			//if (textures[textureID].lod != textureLod[imageUnit]){
			//	glTexEnvf(GL_TEXTURE_FILTER_CONTROL_EXT, GL_TEXTURE_LOD_BIAS_EXT, textureLod[imageUnit] = textures[textureID].lod);
			//}
		}

		currentTextures[imageUnit] = texture;
	}
}

void OpenGLRenderer::changeSamplerState(const uint samplerUnit, const SamplerStateID samplerState){
	TextureID currTex = currentTextures[samplerUnit];
	if (currTex != TEXTURE_NONE){
		if (samplerState != textures[currTex].samplerState){
			ASSERT(samplerState != SS_NONE);
			
			glActiveTexture(GL_TEXTURE0 + samplerUnit);
			setupSampler(textures[currTex].glTarget, samplerStates[samplerState]);

			textures[currTex].samplerState = samplerState;
		}
	}
}

*/
void OpenGLRenderer::setTexture(const char *textureName, const TextureID texture)
{
	//ASSERT(selectedShader != SHADER_NONE);
	if(selectedShader == SHADER_NONE) 
		return;

	int unit = getSamplerUnit(selectedShader, textureName);
	if (unit >= 0)
	{
		selectedTextures[unit] = texture;
	}
}

void OpenGLRenderer::setTexture(const char *textureName, const TextureID texture, const SamplerStateID samplerState)
{
	//ASSERT(selectedShader != SHADER_NONE);
	if(selectedShader == SHADER_NONE)
		return;

	int unit = getSamplerUnit(selectedShader, textureName);
	if (unit >= 0)
	{
		selectedTextures[unit] = texture;
		selectedSamplerStates[unit] = samplerState;
	}
}

void OpenGLRenderer::setTexture(const int textureUnit, const TextureID texture)
{
	if(selectedShader == SHADER_NONE)
		return;

	if (textureUnit >= 0)
	{
		selectedTextures[textureUnit] = texture;
	}
}

void OpenGLRenderer::setTextureArray(const int initIndex, const TextureID *textures, const int n_textures)
{
	if(selectedShader == SHADER_NONE)
		return;

	for(int i=initIndex; i<(initIndex + n_textures); i++)
	{
		selectedTextures[i] = textures[i - initIndex];
	}
}

void OpenGLRenderer::setTextureArray(const char *textureName, const TextureID *textures, const int arraySize)
{
	if(selectedShader == SHADER_NONE)
		return;

	int unit = getSamplerUnit(selectedShader, textureName);

	if(unit >= 0)
	{
		for(int i=0; i<arraySize; i++)
		{
			selectedTextures[unit + i] = textures[i];
		}
	}
}

void OpenGLRenderer::setTextureSlice(const char *textureName, const TextureID texture, const int slice)
{
	return;
}

void OpenGLRenderer::applyTextures()
{
	for (UINT i = 0; i < MAX_TEXTUREUNIT; i++)
	{
		TextureID texture = selectedTextures[i];
		TextureID currTex = currentTextures[i];

		if (texture != currTex || (texture > -1 && textures[texture].mod))
		{
			glActiveTexture(GL_TEXTURE0 + i);
			if (texture == TEXTURE_NONE)
			{
				glBindTexture(textures[currTex].glTarget, 0);
			} 
			else 
			{
				glBindTexture(textures[texture].glTarget, textures[texture].glTexID);
				textures[texture].mod = false;
			}

			currentTextures[i] = texture;
		}
	}

	glActiveTexture(GL_TEXTURE0);
}

void OpenGLRenderer::setImageBuffer(const char *ImageBufferName, const TextureID texture, const int mipLevel, const FORMAT formatImageBuffer, const ImageBufferAccess imageBufferAccess)
{
	if(selectedShader == SHADER_NONE) 
		return;

	int unit = getImageBufferUnit(selectedShader, ImageBufferName);
	if (unit >= 0)
	{
		selectedImageBuffers[unit].imageBufferID = texture;
		selectedImageBuffers[unit].imageBufferFormat = formatImageBuffer;
		selectedImageBuffers[unit].imageBufferAccess = imageBufferAccess;
        selectedImageBuffers[unit].mipLevel = mipLevel;
	}
}

void OpenGLRenderer::setImageBufferArray(const char *ImageBufferName, const TextureID *texture, const int *mipLevel, const FORMAT *formatImageBuffer, const ImageBufferAccess *imageBufferAccess, const UINT offset, const UINT arraySize)
{
	if(selectedShader == SHADER_NONE) 
		return;

	int unit = getImageBufferUnit(selectedShader, ImageBufferName);

	if(unit >= 0)
	{
		for(int i=offset; i<arraySize; i++)
		{
			selectedImageBuffers[unit + i].imageBufferID = texture[i];
			selectedImageBuffers[unit + i].imageBufferFormat = formatImageBuffer[i];
			selectedImageBuffers[unit + i].imageBufferAccess = imageBufferAccess[i];
			selectedImageBuffers[unit + i].mipLevel = mipLevel[i];
		}
	}

}
void OpenGLRenderer::applyImageBuffers()
{
	for (UINT i = 0; i < MAX_IMAGEBUFFERUNIT; i++)
	{
		ImageBufferSlot imageBuffer = selectedImageBuffers[i];
		ImageBufferSlot currImageBuff = currentImageBuffers[i];

		if (imageBuffer.imageBufferID != currImageBuff.imageBufferID || (imageBuffer.imageBufferID > -1 && textures[imageBuffer.imageBufferID].mod)
            || imageBuffer.imageBufferFormat != currImageBuff.imageBufferFormat || imageBuffer.imageBufferAccess != currImageBuff.imageBufferAccess
            || imageBuffer.mipLevel != currImageBuff.mipLevel)
		{
			if (imageBuffer.imageBufferID == TEXTURE_NONE)
			{
				glBindImageTexture(i, 0, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA8);
			} 
			else 
			{
				glBindImageTexture(i, textures[imageBuffer.imageBufferID].glTexID, imageBuffer.mipLevel, GL_TRUE, 0, imageBufferUsages[imageBuffer.imageBufferAccess], internalFormats[imageBuffer.imageBufferFormat]);
				textures[imageBuffer.imageBufferID].mod = false;
			}

			currentImageBuffers[i] = imageBuffer;
		}
	}	
}

void OpenGLRenderer::setObjectBuffer(const char *objectBufferName, const ObjectBufferID objectBufferID)
{
    if(selectedShader == SHADER_NONE) 
		return;

	int unit = getObjectBufferUnit(selectedShader, objectBufferName);

	if(unit >= 0)
	{		
		selectedObjectBuffers[unit] = objectBufferID;
	}
}

void OpenGLRenderer::setObjectBuffer(const int bindSlot, const ObjectBufferID objectBufferID)
{
	if(selectedShader == SHADER_NONE) 
		return;

	if(bindSlot >= 0)
	{
		selectedObjectBuffers[bindSlot] = objectBufferID;
	}
}

void OpenGLRenderer::applyObjectBuffers()
{
    for (UINT i = 0; i < MAX_OBJECTBUFFERUNIT; i++)
	{
		ObjectBufferID objectBuffer = selectedObjectBuffers[i];
		ObjectBufferID currObjectBuff = currentObjectBuffers[i];

		if (objectBuffer != currObjectBuff)
		{
			//textures[imageBuffer.imageBufferID].mod = false;

			if (objectBuffer == TEXTURE_NONE)
			{
                glBindBufferBase(srcObjectBufferType[objectBuffers[currObjectBuff].type], i, 0);
			} 
			else 
			{
                glBindBufferBase(srcObjectBufferType[objectBuffers[objectBuffer].type], i, objectBuffers[objectBuffer].glBufferID);
			}

			currentObjectBuffers[i] = objectBuffer;
		}
	}	
}

void OpenGLRenderer::setSamplerState(const char *samplerName, const SamplerStateID samplerState)
{
	if(selectedShader == SHADER_NONE)
		return;

	int unit = getSamplerUnit(selectedShader, samplerName);
	if (unit >= 0)
	{
		selectedSamplerStates[unit] = samplerState;
	}
}

void OpenGLRenderer::applySamplerStates()
{
	for(UINT i = 0; i < MAX_SAMPLERSTATE; i++)
	{
		if(selectedSamplerStates[i] != currentSamplerStates[i])
		{
			if (selectedSamplerStates[i] == SS_NONE)
			{
				glBindSampler(i, 0);
			}
			else
			{
				glBindSampler(i, samplerStates[selectedSamplerStates[i]].glSamplerID);				
			}

			currentSamplerStates[i] = selectedSamplerStates[i];
		}
	}
}

void OpenGLRenderer::setShaderConstantRaw(const char *name, const void *data, const int size)
{
	int minUniform = 0;
	int maxUniform = shaders[selectedShader].nUniforms - 1;
	Constant *uniforms = shaders[selectedShader].uniforms;

	// Do a quick lookup in the sorted table with a binary search
	while (minUniform <= maxUniform)
	{
		int currUniform = (minUniform + maxUniform) >> 1;
		int res = strcmp(name, uniforms[currUniform].name);
		if (res == 0)
		{
			Constant *uni = &uniforms[currUniform];

			if (memcmp(uni->data, data, size))
			{
				memcpy(uni->data, data, size);
				uni->dirty = true;
			}
			return;

		} 
		else if (res > 0)
		{
			minUniform = currUniform + 1;
		} 
		else 
		{
			maxUniform = currUniform - 1;
		}
	}
}

bool OpenGLRenderer::hasTessellation(ShaderID shaderID)
{
	if(shaders[shaderID].tessellationEvaluationShader > 0 && shaders[shaderID].tessellationControlShader > 0)
		return true;
	
	return false;
}

void OpenGLRenderer::applyConstants()
{
	if (currentShader != SHADER_NONE)
	{
		for (UINT i = 0; i < shaders[currentShader].nUniforms; i++)
		{
			Constant *uni = shaders[currentShader].uniforms + i;
			if (uni->dirty)
			{
				if (uni->type >= CONSTANT_MAT2)
				{
					((UNIFORM_MAT_FUNC) uniformFuncs[uni->type])(uni->index, uni->nElements, GL_FALSE, (float *) uni->data);
				}
				else 
				{
					((UNIFORM_FUNC) uniformFuncs[uni->type])(uni->index, uni->nElements, (float *) uni->data);
				}
				uni->dirty = false;
			}
		}
	}
}

QueryID OpenGLRenderer::createQuery()
{
    Query query;

    glGenQueries(1, &query.queryID);
    queries.push_back(query);

    return queries.size() - 1;
}

void OpenGLRenderer::beginQuery(QueryID id)
{
    glBeginQuery(GL_SAMPLES_PASSED, queries[id].queryID);
}

void OpenGLRenderer::endQuery(QueryID id)
{
    glEndQuery(GL_SAMPLES_PASSED);
}

bool OpenGLRenderer::queryResultAvailable(QueryID id)
{
    GLuint available = GL_FALSE;

	//UINT time1 = g_pApp->getTick();

    glGetQueryObjectuiv(queries[id].queryID, GL_QUERY_RESULT_AVAILABLE, &available);

	//UINT elapsedTime1 = g_pApp->getTick() - time1;
	//printf("QueryResultAvailable: %d ---- %s\n", elapsedTime1, available==GL_TRUE ? "Disponible" : "No Disponible");

    return available == GL_TRUE;
}

UINT OpenGLRenderer::getResultQuery(QueryID id)
{
    GLuint sampleCount = 1;

    glGetQueryObjectuiv(queries[id].queryID, GL_QUERY_RESULT, &sampleCount);

    return sampleCount;
}

void OpenGLRenderer::deleteQueries()
{
	UINT deleteQueriesArray[20];
	UINT count = 0;

	while(queries.size() > count)
	{
		int QueriesPackLength = ((((int)queries.size() - count) - 20) > 0) ? 20 : (int)queries.size() - count;

		for(int i=0; i<QueriesPackLength; i++)
		{
			deleteQueriesArray[i] = queries[count + i].queryID;
		}

		glDeleteQueries(QueriesPackLength, deleteQueriesArray);

		count += QueriesPackLength;
	}

	queries.clear();
	
}

void OpenGLRenderer::changeRenderTargets(const TextureID *colorRTs, const UINT nRenderTargets, const TextureID depthRT, const int depthSlice, const int *slices, const UINT flags)
{
	// Reset states to default before switching render target
	reset();
	apply();

	if (fbo == 0) 
		glGenFramebuffers(1, &fbo);

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	for (UINT i = 0; i < nRenderTargets; i++)
	{
		TextureID colorRT = colorRTs[i];

		if (textures[colorRT].flags & CUBEMAP)
		{
			if (colorRT != currentColorRT[i] || currentColorRTSlice[i] != slices[i])
			{
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_CUBE_MAP_POSITIVE_X + slices[i], textures[colorRT].glTexID, 0);
				currentColorRTSlice[i] = slices[i];
			}
		} 
		else 
		{
			if (colorRT != currentColorRT[i])
			{
				glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, textures[colorRT].glTexID, 0);
				currentColorRT[i] = colorRT;
			}
		}
	}
	
	if (nRenderTargets != nCurrentRenderTargets)
	{
		for (UINT i = nRenderTargets; i < nCurrentRenderTargets; i++)
		{
			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, 0, 0);
			currentColorRT[i] = TEXTURE_NONE;
			currentColorRTSlice[i] = NO_SLICE;
		}
		
		if (nRenderTargets == 0)
		{
			glDrawBuffer(GL_NONE);
			//glReadBuffer(GL_NONE);
		}
		else 
		{
			glDrawBuffers(nRenderTargets, drawBuffers);
			//glReadBuffer(GL_COLOR_ATTACHMENT0);
		}
		nCurrentRenderTargets = nRenderTargets;
	}

	if (depthRT != currentDepthRT)
	{
		if (depthRT >= 0 && textures[depthRT].glTarget != GL_RENDERBUFFER)
		{
			glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, textures[depthRT].glTexID, 0);
			if (isStencilFormat(textures[depthRT].format))
			{
				glFramebufferTexture(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, textures[depthRT].glTexID, 0);
			} 
			else 
			{
				glFramebufferTexture(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, 0, 0);
			}
		} 
		else 
		{
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, (depthRT < 0)? 0 : textures[depthRT].glTexID);
			if (depthRT >= 0 && isStencilFormat(textures[depthRT].format))
			{
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, textures[depthRT].glTexID);
			} 
			else 
			{
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, 0);
			}
		}
		currentDepthRT = depthRT;
	}

	if (nRenderTargets > 0)
	{
		if (textures[colorRTs[0]].flags & CUBEMAP)
		{
			changeFrontFace(GL_CW);
		}
		else 
		{
			changeFrontFace(GL_CCW);
		}

        if(flags & DEFAULT_SIZE)
        {
		    glViewport(0, 0, textures[colorRTs[0]].width, textures[colorRTs[0]].height);
        }
        else
        {
            glViewport(viewportPosX, viewportPosY, viewportWidth, viewportHeight);
        }
	} 
	else if(depthRT >= 0)
	{
		changeFrontFace(GL_CCW);

        if(flags & DEFAULT_SIZE)
        {
		    glViewport(0, 0, textures[depthRT].width, textures[depthRT].height);
        }
        else
        {
            glViewport(viewportPosX, viewportPosY, viewportWidth, viewportHeight);
        }
	}
	
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		CHG_ERROR("FrameBuffer not complete!");
}

void OpenGLRenderer::changeToMainFramebuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(viewportPosX, viewportPosY, viewportWidth, viewportHeight);
	changeFrontFace(GL_CW);
}

void OpenGLRenderer::changeShader(const ShaderID shader)
{
	if (shader != currentShader)
	{
		if (shader == SHADER_NONE)
		{
			glUseProgram(0);
		}
		else
		{
			glUseProgram(shaders[shader].program);
		}
		currentShader = shader;
	}
}

void OpenGLRenderer::changeVertexFormat(const VertexFormatID vertexFormat)
{
	GLuint indexVBO = 0;

	if (vertexFormat != currentVertexFormat || vboChange)
	{
		vboChange = false;

		for(UINT i = 0; i < MAX_VERTEXSTREAM; i++)
		{
			if(enableVertexAttribs[i])
			{
				glDisableVertexAttribArray(i);
				enableVertexAttribs[i] = false;
			}
		}

		static VertexFormat zero;
		VertexFormat *curr = &zero, *sel = &zero;

		if (currentVertexFormat != VF_NONE) curr = &vertexFormats[currentVertexFormat];
		if (vertexFormat != VF_NONE) sel = &vertexFormats[vertexFormat];

		//Not support mixed vertex attrib in a buffer

		for(int i=0; i < vertexFormats[vertexFormat].numAttribs; i++)
		{
			glEnableVertexAttribArray(sel->attribs[i].index);
			enableVertexAttribs[sel->attribs[i].index] = true;

			if(indexVBO != vertexBuffers[currentVertexBuffers[sel->attribs[i].stream]].vboVB)
			{
				glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers[currentVertexBuffers[sel->attribs[i].stream]].vboVB);
				indexVBO = vertexBuffers[currentVertexBuffers[sel->attribs[i].stream]].vboVB;
			}

			glVertexAttribPointer(
				vertexFormats[vertexFormat].attribs[i].index,                  
				vertexFormats[vertexFormat].attribs[i].size,					// size
				glTypes[vertexFormats[vertexFormat].attribs[i].format],			// type
				GL_FALSE,           // normalized?
				vertexFormats[vertexFormat].vertexSize[sel->attribs[i].stream], // stride
				(GLvoid*)(vertexFormats[vertexFormat].attribs[i].offset + currentOffsets[sel->attribs[i].stream])            // array buffer offset
			);
		}		

		currentVertexFormat = vertexFormat;
	}
}

void OpenGLRenderer::changeVertexBuffer(UINT StartSlot, UINT NumBuffers, const VertexBufferID* vertexBufferID, const UINT* offset)
{
	if (vertexBufferID[0] != currentVertexBuffers[0] || offset[0] != currentOffsets[0] || vertexBufferID[1] != currentVertexBuffers[1] || offset[1] != currentOffsets[1] || 
		vertexBufferID[2] != currentVertexBuffers[2] || offset[2] != currentOffsets[2] || vertexBufferID[3] != currentVertexBuffers[3] || offset[3] != currentOffsets[3] || 
		vertexBufferID[4] != currentVertexBuffers[4] || offset[4] != currentOffsets[4] || vertexBufferID[5] != currentVertexBuffers[5] || offset[5] != currentOffsets[5] || 
		vertexBufferID[6] != currentVertexBuffers[6] || offset[6] != currentOffsets[6] || vertexBufferID[7] != currentVertexBuffers[7] || offset[7] != currentOffsets[7])
	{
		UINT strides[8];
		UINT offsets[8];
		
		vboChange = true;
		
		for(UINT i=0; i<NumBuffers; i++)
		{
			currentVertexBuffers[i] = vertexBufferID[i];
			currentOffsets[i] = offset[i];
		}		
	}
}

void OpenGLRenderer::changeIndexBuffer(const IndexBufferID indexBuffer)
{
	if (indexBuffer != currentIndexBuffer)
	{
		if (indexBuffer == IB_NONE)
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		} 
		else 
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffers[indexBuffer].vboIB);
		}

		currentIndexBuffer = indexBuffer;
	}
}

void OpenGLRenderer::changeBlendState(const BlendStateID blendState, const UINT sampleMask)
{
	if (blendState != currentBlendState)
	{
		if (blendState == BS_NONE || !blendStates[blendState].blendEnable)
		{
			if (currentBlendEnable)
			{
				glDisable(GL_BLEND);
				currentBlendEnable = false;
			}
		} 
		else 
		{
			if (blendStates[blendState].blendEnable)
			{
				if (!currentBlendEnable)
				{
					glEnable(GL_BLEND);
					currentBlendEnable = true;
				}
				if (blendStates[blendState].srcFactorRGB   != currentSrcFactorRGB ||
					blendStates[blendState].dstFactorRGB   != currentDstFactorRGB ||
					blendStates[blendState].srcFactorAlpha != currentSrcFactorAlpha ||
					blendStates[blendState].dstFactorAlpha != currentDstFactorAlpha){

					glBlendFuncSeparate(
						currentSrcFactorRGB   = blendStates[blendState].srcFactorRGB,
						currentDstFactorRGB   = blendStates[blendState].dstFactorRGB,
						currentSrcFactorAlpha = blendStates[blendState].srcFactorAlpha,
						currentDstFactorAlpha = blendStates[blendState].dstFactorAlpha);
				}
				if (blendStates[blendState].blendModeRGB   != currentBlendModeRGB ||
					blendStates[blendState].blendModeAlpha != currentBlendModeAlpha)
				{
					glBlendEquationSeparate(
						currentBlendModeRGB   = blendStates[blendState].blendModeRGB,
						currentBlendModeAlpha = blendStates[blendState].blendModeAlpha);
				}
			}
		}

		int mask = ALL;
		bool alphaToCoverage = false;
		if (blendState != BS_NONE)
		{
			mask = blendStates[blendState].mask;
			alphaToCoverage = blendStates[blendState].alphaToCoverageEnable;
		}

		if (mask != currentMask)
		{
			glColorMask((mask & RED) & 1, ((mask & GREEN) >> 1) & 1, ((mask & BLUE) >> 2) & 1, ((mask & ALPHA) >> 3) & 1);

			currentMask = mask;
		}

		if (alphaToCoverage != currentAlphaToCoverageEnable)
		{
			if (alphaToCoverage)
				glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
			else
				glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);

			currentAlphaToCoverageEnable = alphaToCoverage;
		}

		currentBlendState = blendState;
	}
}

void OpenGLRenderer::changeDepthState(const DepthStateID depthState, const UINT stencilRef)
{
	// stencilRef currently not used

	if (depthState != currentDepthState)
	{
		if (depthState == DS_NONE)
		{
			if (!currentDepthTestEnable)
			{
				glEnable(GL_DEPTH_TEST);
				currentDepthTestEnable = true;
			}

			if (!currentDepthWriteEnable)
			{
				glDepthMask(GL_TRUE);
				currentDepthWriteEnable = true;
			}

			if (currentDepthFunc != LEQUAL)
			{
				glDepthFunc(currentDepthFunc = LEQUAL);
			}
		} 
		else 
		{
			if (depthStates[depthState].depthTest)
			{
				if (!currentDepthTestEnable)
				{
					glEnable(GL_DEPTH_TEST);
					currentDepthTestEnable = true;
				}
				if (depthStates[depthState].depthWrite != currentDepthWriteEnable)
				{
					glDepthMask((currentDepthWriteEnable = depthStates[depthState].depthWrite)? GL_TRUE : GL_FALSE);
				}
				if (depthStates[depthState].depthFunc != currentDepthFunc)
				{
					glDepthFunc(currentDepthFunc = depthStates[depthState].depthFunc);
				}
			} 
			else 
			{
				if (currentDepthTestEnable)
				{
					glDisable(GL_DEPTH_TEST);
					currentDepthTestEnable = false;
				}
			}
		}

		currentDepthState = depthState;
	}
}

void OpenGLRenderer::changeRasterizerState(const RasterizerStateID rasterizerState)
{
	if (rasterizerState != currentRasterizerState)
	{
		RasterizerState state;
		if (rasterizerState == RS_NONE)
		{
			state.cullMode = CULL_NONE;
			state.fillMode = SOLID;
			state.multiSample = true;
			state.scissor = false;
		} 
		else 
		{
			state = rasterizerStates[rasterizerState];
		}


		if (state.cullMode != currentCullMode)
		{
			if (state.cullMode == CULL_NONE)
			{
				glDisable(GL_CULL_FACE);
			}
			else 
			{
				if (currentCullMode == CULL_NONE) glEnable(GL_CULL_FACE);
				glCullFace(state.cullMode);
			}

			currentCullMode = state.cullMode;
		}

		if (state.fillMode != currentFillMode)
		{
			glPolygonMode(GL_FRONT_AND_BACK, currentFillMode = state.fillMode);
		}

		if (state.multiSample != currentMultiSampleEnable)
		{
			if (state.multiSample){
				glEnable(GL_MULTISAMPLE);
			} 
			else 
			{
				glDisable(GL_MULTISAMPLE);
			}
			currentMultiSampleEnable = state.multiSample;
		}

		if (state.scissor != currentScissorEnable)
		{
			if (state.scissor)
			{
				glEnable(GL_SCISSOR_TEST);
			} 
			else 
			{
				glDisable(GL_SCISSOR_TEST);
			}
			currentScissorEnable = state.scissor;
		}

		currentRasterizerState = rasterizerState;
	}
}

const GLenum glPrim[] = 
{
	GL_TRIANGLES,
	GL_TRIANGLE_FAN,
	GL_TRIANGLE_STRIP,
	GL_QUADS,
	GL_LINES,
	GL_LINE_STRIP,
	GL_LINE_LOOP,
	GL_POINTS,
	GL_PATCHES,
};

void OpenGLRenderer::clear(const bool clearColor, const bool clearDepth, const bool clearStencil, const float *color, const float depth, const UINT stencil)
{
	GLbitfield cleits = 0;

	if (clearColor)
	{
		cleits |= GL_COLOR_BUFFER_BIT;
		glClearColor(color[0], color[1], color[2], color[3]);

		if (currentMask != ALL)
		{
			glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
			currentMask = ALL;
		}
	}
	if (clearDepth)
	{
		cleits |= GL_DEPTH_BUFFER_BIT;
		glClearDepth(depth);
		if (!currentDepthWriteEnable)
		{
			glDepthMask(GL_TRUE);
			currentDepthWriteEnable = true;
		}
	}
	if (clearStencil)
	{
		cleits |= GL_STENCIL_BUFFER_BIT;
		glClearStencil(stencil);
	}

	if (cleits)
	{
		glClear(cleits);
	}
}

void OpenGLRenderer::drawArrays(const Primitives primitives, const int firstVertex, const int nVertices)
{
	glDrawArrays(glPrim[primitives], firstVertex, nVertices);

	nDrawCalls++;
}

void OpenGLRenderer::drawElements(const Primitives primitives, const int firstIndex, const int nIndices, const int firstVertex, const int nVertices)
{
	UINT indexSize = indexBuffers[currentIndexBuffer].indexSize;

	glDrawElements(glPrim[primitives], nIndices, indexSize == 2? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, BUFFER_OFFSET(indexSize * firstIndex));

	nDrawCalls++;
}

bool OpenGLRenderer::drawText(const char *str, float x, float y, const FontID font, const SamplerStateID samplerState, const BlendStateID blendState, const DepthStateID depthState, Color *textColor, const Vec4 *rect, const BYTE flags)
{
	if(strcmp(str, "") == 0)
		return true;	

	Vec4 col = Vec4(1, 0, 0, 0);
	if (textColor) col = *textColor;

	SDL_Color color;

	color.r = 255 * col.x;
	color.g = 255 * col.y;
	color.b = 255 * col.z;
	color.a = 255 * col.w;

	if(m_textTexture == TEXTURE_NONE)
	{
		Texture tex;

		glGenTextures(1, &tex.glTexID);
		tex.glTarget = GL_TEXTURE_2D;
		textures.push_back(tex);
		m_textTexture = textures.size() - 1;
	}

	m_ptextSurface = TTF_RenderUTF8_Blended_Wrapped(TTFFonts[font].pfont, str, color, g_pApp->GetScreenSize().x);

	if(m_ptextSurface == NULL)
	{
        printf("Error: Draw TTF text");
		return false;
	}

	GLenum textFormat;
    if(m_ptextSurface->format->BytesPerPixel == 4)
    {
        // alpha
        if(m_ptextSurface->format->Rmask == 0x000000ff)
            textFormat = GL_RGBA;
        else
            textFormat = GL_BGRA;
    }

	glBindTexture(GL_TEXTURE_2D, textures[m_textTexture].glTexID);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_ptextSurface->w, m_ptextSurface->h, 0, textFormat, GL_UNSIGNED_BYTE, m_ptextSurface->pixels);
	glBindTexture(GL_TEXTURE_2D, 0);
	textures[m_textTexture].mod = true;

	if(rect)
	{
		if((flags & F_TOP) != 0)
		{
			y = rect->y;
		}
		else if((flags & F_BOTTOM) != 0)
		{
			y = rect->w - (float)m_ptextSurface->h;
		}
		else
		{
			y = (rect->y + (rect->w - rect->y)*0.5f) - m_ptextSurface->h*0.5f;
		}

		if((flags & F_LEFT) != 0)
		{
			x = rect->x;
		}
		else if((flags & F_RIGHT) != 0)
		{
			x = rect->x - (float)m_ptextSurface->w;
		}
		else
		{
			x = (rect->x + (rect->z - rect->x)*0.5f) - m_ptextSurface->w*0.5f;
		}
	}

	TexVertex textPoint[] = 
	{
		TexVertex( Vec2(x,   y  ),  Vec2(0.0f, 0.0f)),
		TexVertex( Vec2(x,   y + m_ptextSurface->h),  Vec2(0.0f, 1.0f)),
		TexVertex( Vec2(x + m_ptextSurface->w, y),  Vec2(1.0f, 0.0f)),
		TexVertex( Vec2(x + m_ptextSurface->w, y + m_ptextSurface->h),  Vec2(1.0f, 1.0f)),
	};

	drawTextured(PRIM_TRIANGLE_STRIP, textPoint, 4, m_textTexture, samplerState, blendState, depthState);

	SDL_FreeSurface(m_ptextSurface);

	return true;
}

void OpenGLRenderer::setup2DMode(const float left, const float right, const float top, const float bottom)
{
	scaleBias2D.x = 2.0f / (right - left);
	scaleBias2D.y = 2.0f / (top - bottom);
	scaleBias2D.z = -1.0f;
	scaleBias2D.w =  1.0f;
}

ubyte* OpenGLRenderer::mapRollingVB(const UINT size)
{
	//ASSERT(size <= ROLLING_VB_SIZE);
	if(size >= ROLLING_VB_SIZE)
		return NULL;

	if (rollingVB == VB_NONE) rollingVB = addVertexBuffer(ROLLING_VB_SIZE, DEFAULT);

	ubyte *data = NULL;

	if (rollingVBOffset + size > ROLLING_VB_SIZE)
	{
		rollingVBOffset = 0;
	}

	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers[rollingVB].vboVB);

	data = (ubyte*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	return data + rollingVBOffset;
}

void OpenGLRenderer::unmapRollingVB(const UINT size)
{
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers[rollingVB].vboVB);

	glUnmapBuffer(GL_ARRAY_BUFFER);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	rollingVBOffset += size;
}

UINT OpenGLRenderer::copyToRollingVB(const void *src, const UINT size)
{
	//ASSERT(size <= ROLLING_VB_SIZE);
	if(size >= ROLLING_VB_SIZE)
		return NULL;

	if (rollingVB == VB_NONE) rollingVB = addVertexBuffer(ROLLING_VB_SIZE, DYNAMIC);

	if (rollingVBOffset + size > ROLLING_VB_SIZE)
	{
		rollingVBOffset = 0;
	}

	UINT offset = rollingVBOffset;

	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers[rollingVB].vboVB);

	ubyte *data = (ubyte*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
		memcpy(data + offset, src, size);
	glUnmapBuffer(GL_ARRAY_BUFFER);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	rollingVBOffset += size;

	return offset;
}

void OpenGLRenderer::resetAtomicCounter(ObjectBufferID atomicCounterBuffer, UINT value)
{
    ObjectBuffer atomicCounter = objectBuffers[atomicCounterBuffer];

    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, atomicCounter.glBufferID);
    GLuint* ptr = (GLuint*)glMapBufferRange(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint),
                                            GL_MAP_WRITE_BIT | 
                                            GL_MAP_INVALIDATE_BUFFER_BIT | 
                                            GL_MAP_UNSYNCHRONIZED_BIT);
	//printf("Graphics Counter: %d\n", ptr[0]);
    ptr[0] = value;
    glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0); 
}

const char *plainVS =
"uniform vec4 scaleBias;"
"layout(location = 0) in vec2 Position;"
"void main() {"
"	gl_Position.xy = Position.xy * scaleBias.xy + scaleBias.zw;"
"}";

const char *plainPS =
"uniform vec4 color;"
"out vec4 outColor;"
"void main() {"
"	outColor = color;"
"}";

const char *texVS =
"uniform vec4 scaleBias;"
"layout(location = 0) in vec2 Position;"
"layout(location = 1) in vec2 texCoord;"
"out vec2 outTexCoord;"
"void main(){"
"	gl_Position.xy = Position.xy * scaleBias.xy + scaleBias.zw;"
"	outTexCoord = texCoord;"
"}";

const char *texPS =
"uniform sampler2D base;"
"uniform vec4 color;"
"in vec2 outTexCoord;"
"out vec4 outColor;"
"void main() {"
"	outColor = texture( base, outTexCoord ) * color;"
"}";

void OpenGLRenderer::drawPlain(const Primitives primitives, Vec2 *vertices, const UINT nVertices, const BlendStateID blendState, const DepthStateID depthState, const Vec4 *color)
{
	int size = nVertices * sizeof(Vec2);

	UINT stride = sizeof(Vec2);
	UINT* offset = CHG_NEW UINT[1];
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
	setVertexFormat(plainVF);
	setVertexBuffer(0, 1, &rollingVB, offset);
	apply();

	glDrawArrays(glPrim[primitives], 0, nVertices);

	delete offset;
}

void OpenGLRenderer::drawTextured(const Primitives primitives, TexVertex *vertices, const UINT nVertices, const TextureID texture, const SamplerStateID samplerState, const BlendStateID blendState, const DepthStateID depthState, const Vec4 *color)
{
	int size = nVertices * sizeof(TexVertex);

	UINT stride = sizeof(TexVertex);
	UINT* offset = CHG_NEW UINT[1];
	offset[0] =	copyToRollingVB(vertices, size);

	if (texShader == SHADER_NONE)
	{
		texShader = createShader(texVS, NULL, texPS, NULL, NULL, NULL, 0, 0, 0, 0, 0, 0);

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
	setTexture("base", texture, samplerState);
	setBlendState(blendState);
	setDepthState(depthState);
	setVertexFormat(texVF);
	setVertexBuffer(0, 1, &rollingVB, offset);
	apply();

	glDrawArrays(glPrim[primitives], 0, nVertices);

	delete offset;
}

void OpenGLRenderer::dispatchCompute(const UINT num_groups_x, const UINT num_groups_y, const UINT num_groups_z)
{
	glDispatchCompute((GLuint)num_groups_x, (GLuint)num_groups_y, (GLuint)num_groups_z);
}

void OpenGLRenderer::memoryBarrier(UINT flags)
{
	glMemoryBarrier(flags);
}

void OpenGLRenderer::flush()
{
	glFlush();
}

void OpenGLRenderer::finish()
{
	glFinish();
}

void OpenGLRenderer::changeFrontFace(const GLenum frontFace)
{
	if (frontFace != currentFrontFace)
	{
		glFrontFace(currentFrontFace = frontFace);
	}
}

void OpenGLRenderer::setupSampler(GLenum glTarget, const SamplerState &ss)
{
	// Set requested wrapping modes
	glTexParameteri(glTarget, GL_TEXTURE_WRAP_S, ss.wrapS);
	if (glTarget != GL_TEXTURE_1D) glTexParameteri(glTarget, GL_TEXTURE_WRAP_T, ss.wrapT);
	if (glTarget == GL_TEXTURE_3D) glTexParameteri(glTarget, GL_TEXTURE_WRAP_R, ss.wrapR);

	// Set requested filter modes
	glTexParameteri(glTarget, GL_TEXTURE_MAG_FILTER, ss.magFilter);
	glTexParameteri(glTarget, GL_TEXTURE_MIN_FILTER, ss.minFilter);

	// Setup anisotropic filtering
	if (ss.aniso > 1)
	{
		glTexParameteri(glTarget, GL_TEXTURE_MAX_ANISOTROPY_EXT, ss.aniso);
	}
}

void* OpenGLRenderer::mapBuffer(const ObjectBufferID objectBuffer, const ImageBufferAccess bufferAccess)
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, objectBuffers[objectBuffer].glBufferID);
	return glMapBuffer(GL_SHADER_STORAGE_BUFFER, imageBufferUsages[bufferAccess]);
}

void OpenGLRenderer::unMapBuffer()
{
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}
