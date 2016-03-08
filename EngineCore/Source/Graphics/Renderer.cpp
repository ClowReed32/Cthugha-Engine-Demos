#include "CthughaStd.h"
#include "Renderer.h"
//#include "BitMapFontLoader.h"
//#include "Util/String.h"

int constantTypeSizes[CONSTANT_TYPE_COUNT] = 
{
	sizeof(float),
	sizeof(Vec2),
	sizeof(Vec3),
	sizeof(Vec4),
	sizeof(int),
	sizeof(int) * 2,
	sizeof(int) * 3,
	sizeof(int) * 4,
	sizeof(int),
	sizeof(int) * 2,
	sizeof(int) * 3,
	sizeof(int) * 4,
	16,
	36,
	sizeof(Mat4x4),
};


Renderer::Renderer()
{
	nImageUnits = 1;
	nMRTs = 1;
	maxAnisotropic = 1;

	viewportPosX = 0;
	viewportPosY = 0;
	viewportWidth  = 0;
	viewportHeight = 0;

	fontBuffer = NULL;
	fontBufferCount = 0;

#ifdef PROFILE
	profileFrame = false;
	profileReset = true;
	profileString[0] = '\0';
#endif
}

Renderer::~Renderer()
{
//	delete textureLod;
	delete[] fontBuffer;
}

void Renderer::resetToDefaults()
{
	currentShader = SHADER_NONE;

	currentVertexFormat = VF_NONE;

	for (UINT i = 0; i < MAX_VERTEXSTREAM; i++)
	{
		currentVertexBuffers[i] = VB_NONE;
		currentOffsets[i] = 0;
	}
	currentIndexBuffer = IB_NONE;

	currentDepthState = DS_NONE;
	currentStencilRef = 0;
	currentBlendState = BS_NONE;
	currentSampleMask = ~0;
	currentRasterizerState = RS_NONE;


	currentColorRT[0] = FB_COLOR;
	currentColorRTSlice[0] = NO_SLICE;
	for (UINT i = 1; i < nMRTs; i++)
	{
		currentColorRT[i] = TEXTURE_NONE;
		currentColorRTSlice[i] = NO_SLICE;
	}
	currentDepthRT = FB_DEPTH;
	currentDepthSlice = NO_SLICE;
	nCurrentRenderTargets = 1;

	//reset();

#ifdef DEBUG
	wasReset = false;
#endif
}

void Renderer::reset(const UINT flags)
{
	if (flags & RESET_SHADER) selectedShader = SHADER_NONE;

	if (flags & RESET_VF) selectedVertexFormat = VF_NONE;

	if (flags & RESET_VB)
	{
		for (UINT i = 0; i < MAX_VERTEXSTREAM; i++)
		{
			selectedVertexBuffers[i] = VB_NONE;
			selectedOffsets[i] = 0;
		}
	}
	if (flags & RESET_IB) selectedIndexBuffer = IB_NONE;

	if (flags & RESET_DS)
	{
		selectedDepthState = DS_NONE;
		selectedStencilRef = 0;
	}
	if (flags & RESET_BS)
	{
		selectedBlendState = BS_NONE;
		selectedSampleMask = ~0;
	}
	if (flags & RESET_RS) selectedRasterizerState = RS_NONE;

#ifdef DEBUG
	wasReset = true;
#endif
}

void Renderer::apply()
{

#ifdef DEBUG
	/*if (!wasReset){
		CHG_WARNING("Warning: reset() not called before apply()");
	}
	wasReset = false;*/
#endif


	if (selectedShader != DONTCARE)
	{
		changeShader(selectedShader);
		applyConstants();
	}

	applyTextures();

    applyImageBuffers();

	applyObjectBuffers();
	
	applyRWStructuredBuffers();

	applySamplerStates();

	changeVertexFormat(selectedVertexFormat);

	changeVertexBuffer(0, numVertexBufferUsed, selectedVertexBuffers, selectedOffsets);

	if (selectedIndexBuffer != DONTCARE) changeIndexBuffer(selectedIndexBuffer);

	if (selectedDepthState != DONTCARE)
	{
		changeDepthState(selectedDepthState, selectedStencilRef);
	}
	if (selectedBlendState != DONTCARE)
	{
		changeBlendState(selectedBlendState, selectedSampleMask);
	}
	if (selectedRasterizerState != DONTCARE) changeRasterizerState(selectedRasterizerState);

//	reset();
}

TextureID Renderer::createTexture(const char *fileName, const bool useMipMaps, const SamplerStateID samplerState, UINT flags){
	Image img;

	UINT loadFlags = 0;
	if (!useMipMaps) loadFlags |= DONT_LOAD_MIPMAPS;

	if (img.loadImage(fileName, loadFlags))
	{
		if (img.getFormat() == FORMAT_RGBE8) img.unpackImage();
		if (useMipMaps && img.getMipMapCount() <= 1) img.createMipMaps();
		return createTexture(img, samplerState, flags);
	} 
	else 
	{
		char str[256];
		sprintf_s(str, "Couldn't open \"%s\"", fileName);

		//ErrorMsg(str);
		return NULL;
	}
}

TextureID Renderer::createTexture(char *rawBuffer, unsigned int rawSize, std::string extension, const bool useMipMaps, const SamplerStateID samplerState, UINT flags){
	Image img;

	UINT loadFlags = 0;
	if (!useMipMaps) loadFlags |= DONT_LOAD_MIPMAPS;

	if (img.loadImageFromMemory(rawBuffer, rawSize, extension, loadFlags))
	{
		if (img.getFormat() == FORMAT_RGBE8) img.unpackImage();
		if (useMipMaps && img.getMipMapCount() <= 1) img.createMipMaps();
		return createTexture(img, samplerState, flags);
	} 
	else 
	{
		char str[256];
		sprintf_s(str, "Couldn't create Image");

		//ErrorMsg(str);
		return TEXTURE_NONE;
	}
}

/*TextureID Renderer::addCubemap(const char **fileNames, const bool useMipMaps, const SamplerStateID samplerState, const int nArraySlices, uint flags){
	Image img;
	if (img.loadSlicedImage(fileNames, 0, nArraySlices)){
		if (img.getFormat() == FORMAT_RGBE8) img.unpackImage();
		if (useMipMaps && img.getMipMapCount() <= 1) img.createMipMaps();
		return addTexture(img, samplerState, flags);
	} else {
		char str[1024];
		int n = sprintf(str, "Couldn't open cubemap:\n");
		for (int i = 0; i < 6 * nArraySlices; i++){
			n += sprintf(str + n, "%s\n", fileNames[i]);
		}

		ErrorMsg(str);
		return TEXTURE_NONE;
	}
}*/

ShaderID Renderer::createShader(char *rawBuffer, unsigned int rawSize, const UINT flags)
{
	return createShader(rawBuffer, rawSize, NULL, 0, NULL, flags);
}

ShaderID Renderer::createShader(char *rawBuffer, unsigned int rawSize, const char *extra, const UINT flags)
{
	return createShader(rawBuffer, rawSize, NULL, 0, extra, flags);
}

ShaderID Renderer::createShader(char *rawBuffer, unsigned int rawSize, const char **attributeNames, const int nAttributes, const char *extra, const UINT flags)
{
	ShaderID res = SHADER_NONE;

	if (rawBuffer == NULL)
	{
		printf_s("NULL mem pointer");
		//ErrorMsg(String("Couldn't load \"") + fileName + "\"");
	} 
	else 
	{
		// Find file size
		unsigned int length = rawSize;		
		char *shaderText = (char*)malloc(length + 1);
		memcpy(shaderText, rawBuffer, length+1);
		shaderText[length] = '\0';

		char *vs = strstr(shaderText, "[Vertex shader]");
		char *gs = strstr(shaderText, "[Geometry shader]");
		char *fs = strstr(shaderText, "[Fragment shader]");
		char *cs = strstr(shaderText, "[Compute shader]");
		char *tcs = strstr(shaderText, "[Tessellation Control shader]");
		char *tes = strstr(shaderText, "[Tessellation Evaluation shader]");

		char *header = (shaderText[0] != '[')? shaderText : NULL;

		int vsLine = 0;
		if (vs != NULL){
			*vs = '\0';
			vs += 15;
			while (*vs == '\r' || *vs == '\n') vs++;

			char *str = shaderText;
			while (str < vs){
                if (*str == '\n') vsLine++;
				str++;
			}
		}

		int gsLine = 0;
		if (gs != NULL){
			*gs = '\0';
			gs += 17;
			while (*gs == '\r' || *gs == '\n') gs++;

			char *str = shaderText;
			while (str < gs){
                if (*str == '\n') gsLine++;
				str++;
			}
		}

		int fsLine = 0;
		if (fs != NULL)
		{
			*fs = '\0';
			fs += 17;
			while (*fs == '\r' || *fs == '\n') fs++;

			char *str = shaderText;
			while (str < fs)
			{
                if (*str == '\n') fsLine++;
				str++;
			}
		}

		int csLine = 0;
		if (cs != NULL)
		{
			*cs = '\0';
			cs += 16;
			while (*cs == '\r' || *cs == '\n') cs++;

			char *str = shaderText;
			while (str < cs)
			{
                if (*str == '\n') csLine++;
				str++;
			}
		}

		int tcsLine = 0;
		if (tcs != NULL)
		{
			*tcs = '\0';
			tcs += 29;
			while (*tcs == '\r' || *tcs == '\n') tcs++;

			char *str = shaderText;
			while (str < tcs)
			{
                if (*str == '\n') tcsLine++;
				str++;
			}
		}

		int tesLine = 0;
		if (tes != NULL)
		{
			*tes = '\0';
			tes += 32;
			while (*tes == '\r' || *tes == '\n') tes++;

			char *str = shaderText;
			while (str < tes)
			{
                if (*str == '\n') tesLine++;
				str++;
			}
		}
		
		
		res = createShader(vs, gs, fs, cs, tcs, tes, vsLine, gsLine, fsLine, csLine, tcsLine, tesLine, header, extra, NULL, attributeNames, nAttributes, flags);
		free(shaderText);
	}
	return res;
}

ShaderID Renderer::createShader(const char *fileName, const UINT flags)
{
	return createShader(fileName, NULL, 0, NULL, flags);
}

ShaderID Renderer::createShader(const char *fileName, const char *extra, const UINT flags)
{
	return createShader(fileName, NULL, 0, extra, flags);
}

bool Renderer::activeShaderHasTessellation()
{
    return (currentShader >= 0) ? hasTessellation(currentShader) : false;
}

ShaderID Renderer::createShader(const char *fileName, const char **attributeNames, const int nAttributes, const char *extra, const UINT flags)
{
	FILE *file = fopen(fileName, "rb");
	ShaderID res = SHADER_NONE;

	if (file == NULL)
	{
		char str[256];
		sprintf_s(str, "Couldn't open \"%s\"", fileName);
		//ErrorMsg(String("Couldn't load \"") + fileName + "\"");
	} else {
#ifdef DEBUG
		char str[66];
		str[0] = '\n';
		memset(str + 1, '-', sizeof(str) - 2);
		str[sizeof(str) - 1] = '\0';
		size_t lfn = strlen(fileName);
		size_t start = (sizeof(str) - lfn) / 2;

		str[start - 1]   = '[';
		str[start + lfn] = ']';
		strncpy(str + start, fileName, lfn);
		CHG_ERROR(str);
#endif

		// Find file size
		fseek(file, 0, SEEK_END);
		int length = ftell(file);
		fseek(file, 0, SEEK_SET);

		char *shaderText = CHG_NEW char[length + 1];
		fread(shaderText, length, 1, file);
		fclose(file);
		shaderText[length] = '\0';

		char *vs = strstr(shaderText, "[Vertex shader]");
		char *gs = strstr(shaderText, "[Geometry shader]");
		char *fs = strstr(shaderText, "[Fragment shader]");
		char *cs = strstr(shaderText, "[Compute shader]");
		char *tcs = strstr(shaderText, "[Tessellation Control shader]");
		char *tes = strstr(shaderText, "[Tessellation Evaluation shader]");

		char *header = (shaderText[0] != '[')? shaderText : NULL;

		int vsLine = 0;
		if (vs != NULL){
			*vs = '\0';
			vs += 15;
			while (*vs == '\r' || *vs == '\n') vs++;

			char *str = shaderText;
			while (str < vs){
                if (*str == '\n') vsLine++;
				str++;
			}
		}

		int gsLine = 0;
		if (gs != NULL){
			*gs = '\0';
			gs += 17;
			while (*gs == '\r' || *gs == '\n') gs++;

			char *str = shaderText;
			while (str < gs){
                if (*str == '\n') gsLine++;
				str++;
			}
		}

		int fsLine = 0;
		if (fs != NULL)
		{
			*fs = '\0';
			fs += 17;
			while (*fs == '\r' || *fs == '\n') fs++;

			char *str = shaderText;
			while (str < fs)
			{
                if (*str == '\n') fsLine++;
				str++;
			}
		}

		int csLine = 0;
		if (cs != NULL)
		{
			*cs = '\0';
			cs += 16;
			while (*cs == '\r' || *cs == '\n') cs++;

			char *str = shaderText;
			while (str < cs)
			{
                if (*str == '\n') csLine++;
				str++;
			}
		}

		int tcsLine = 0;
		if (tcs != NULL)
		{
			*tcs = '\0';
			tcs += 29;
			while (*tcs == '\r' || *tcs == '\n') tcs++;

			char *str = shaderText;
			while (str < tcs)
			{
                if (*str == '\n') tcsLine++;
				str++;
			}
		}

		int tesLine = 0;
		if (tes != NULL)
		{
			*tes = '\0';
			tes += 32;
			while (*tes == '\r' || *tes == '\n') tes++;

			char *str = shaderText;
			while (str < tes)
			{
                if (*str == '\n') tesLine++;
				str++;
			}
		}
		
		
		res = createShader(vs, gs, fs, cs, tcs, tes, vsLine, gsLine, fsLine, csLine, tcsLine, tesLine, header, extra, NULL, attributeNames, nAttributes, flags);
		delete shaderText;
	}
	return res;
}

int Renderer::getFormatSize(const AttributeFormat format) const 
{
	static int formatSize[] = { sizeof(float), sizeof(half), sizeof(ubyte) };
	return formatSize[format];
}

void Renderer::setShaderConstant1i(const char *name, const int constant)
{
	if(selectedShader != SHADER_NONE)
		setShaderConstantRaw(name, &constant, sizeof(constant));
}

void Renderer::setShaderConstant1f(const char *name, const float constant)
{
	if(selectedShader != SHADER_NONE)
		setShaderConstantRaw(name, &constant, sizeof(constant));
}

void Renderer::setShaderConstant2f(const char *name, const Vec2 &constant)
{
	if(selectedShader != SHADER_NONE)
		setShaderConstantRaw(name, &constant, sizeof(constant));
}

void Renderer::setShaderConstant3f(const char *name, const Vec3 &constant)
{
	if(selectedShader != SHADER_NONE)
		setShaderConstantRaw(name, &constant, sizeof(constant));
}

void Renderer::setShaderConstant4f(const char *name, const Vec4 &constant)
{
	if(selectedShader != SHADER_NONE)
		setShaderConstantRaw(name, &constant, sizeof(constant));
}

void Renderer::setShaderConstant4x4f(const char *name, const Mat4x4 &constant)
{
	if(selectedShader != SHADER_NONE)
		setShaderConstantRaw(name, &constant, sizeof(constant));
}

void Renderer::setShaderConstantStruct(const char *name, const void* constant, const int elementSize)
{
	if (selectedShader != SHADER_NONE)
	{
		setShaderConstantRaw(name, constant, elementSize);
	}
}

void Renderer::setShaderConstantArray1i(const char *name, const int *constant, const UINT count)
{
	if(selectedShader != SHADER_NONE)
		setShaderConstantRaw(name, constant, count * sizeof(int));
}

void Renderer::setShaderConstantArray1f(const char *name, const float *constant, const UINT count)
{
	if(selectedShader != SHADER_NONE)
		setShaderConstantRaw(name, constant, count * sizeof(float));
}

void Renderer::setShaderConstantArray2f(const char *name, const Vec2 *constant, const UINT count)
{
	if(selectedShader != SHADER_NONE)
		setShaderConstantRaw(name, constant, count * sizeof(Vec2));
}

void Renderer::setShaderConstantArray3f(const char *name, const Vec3 *constant, const UINT count)
{
	if(selectedShader != SHADER_NONE)
		setShaderConstantRaw(name, constant, count * sizeof(Vec3));
}

void Renderer::setShaderConstantArray4f(const char *name, const Vec4 *constant, const UINT count)
{
	if(selectedShader != SHADER_NONE)
		setShaderConstantRaw(name, constant, count * sizeof(Vec4));
}

void Renderer::setShaderConstantArray4x4f(const char *name, const Mat4x4 *constant, const UINT count)
{
	if(selectedShader != SHADER_NONE)
		setShaderConstantRaw(name, constant, count * sizeof(Mat4x4));
}

void Renderer::setShaderConstantArrayStruct(const char *name, const void* constant, const int elementSize, const UINT count)
{
	if (selectedShader != SHADER_NONE)
	{
		setShaderConstantRaw(name, constant, elementSize*count);
	}
}

void Renderer::setViewport(const int width, const int height, const int posX, const int posY)
{
	viewportWidth  = width;
	viewportHeight = height;

	viewportPosX = posX;
	viewportPosY = posY;
}

FontID Renderer::addFont(TexFont texFont)
{
	fonts.push_back(texFont);

	return fonts.size() - 1;
}

float Renderer::getTextWidth(const FontID font, const char *str, int length) const 
{
	if (font < 0) return 0;
	if (length < 0) length = (int) strlen(str);

	Character *chars = fonts[font].chars;

	float len = 0;
	for (int i = 0; i < length; i++)
	{
		len += chars[(unsigned char) str[i]].xadvance;
	}

	return len;
}

float Renderer::getTextHeight(const FontID font, const char *str, int length) const 
{
	if (font < 0) return 0;
	if (length < 0) length = (int) strlen(str);

	Character *chars = fonts[font].chars;

	float highest = 0.0f;
	float lowest = 0.0f;

	for (int i = 0; i < length; i++)
	{
		if(highest > chars[(unsigned char) str[i]].yOffset)
			highest = chars[(unsigned char) str[i]].yOffset;

		if(lowest < (chars[(unsigned char) str[i]].yOffset + chars[(unsigned char) str[i]].ratioY))
			lowest = chars[(unsigned char) str[i]].yOffset + chars[(unsigned char) str[i]].ratioY;
	}

	return lowest - highest;
}

UINT Renderer::getTextQuads(const char *str) const 
{
	UINT n = 0;
	while (*str)
	{
		if (*str != '\n') n++;
		str++;
	}
	return n;
}

void Renderer::fillTextBuffer(TexVertex *dest, const char *str, float x, float y, const float charWidth, const float charHeight, const FontID font) const 
{
    float startx = x;

    while (*str)
	{
        if (*str == '\n')
		{
            y += charHeight;
            x = startx;
        } 
		else 
		{
			Character chr = fonts[font].chars[*(unsigned char *) str];
			float cw = charWidth * chr.ratioX;
			float ch = charHeight * chr.ratioY;
			float ox = charWidth * chr.xOffset;
			float oy = charHeight * chr.yOffset;
			
			//drawQuad(x, y, x + cw, y + charHeight, NULL, chr.x0, chr.y0, chr.x1, chr.y1);
			dest[0].position = Vec2(x + cw + ox, y + oy);
			dest[0].texCoord = Vec2(chr.x1, chr.y0);
			dest[1].position = Vec2(x + ox, y + oy);
			dest[1].texCoord = Vec2(chr.x0, chr.y0);			
			dest[2].position = Vec2(x + ox, y + ch + oy);
			dest[2].texCoord = Vec2(chr.x0, chr.y1);

			dest[3].position = Vec2(x + cw + ox, y + oy);
			dest[3].texCoord = Vec2(chr.x1, chr.y0);
			dest[4].position = Vec2(x + ox, y + ch + oy);
			dest[4].texCoord = Vec2(chr.x0, chr.y1);			
			dest[5].position = Vec2(x + cw + ox, y + ch + oy);
			dest[5].texCoord = Vec2(chr.x1, chr.y1);

			dest += 6;
			x += (charWidth * chr.xadvance);
        }
        str++;
    }
}

bool Renderer::drawText(const char *str, float x, float y, const float charWidth, const float charHeight, const FontID font, const SamplerStateID samplerState, const BlendStateID blendState, const DepthStateID depthState, Color* textColor)
{
	if (font == FONT_NONE) return false;

	UINT n = 6 * getTextQuads(str);
	if (n == 0) return true;

	if (n > fontBufferCount)
	{
		fontBuffer = (TexVertex *) realloc(fontBuffer, n * sizeof(TexVertex));
		fontBufferCount = n;
	}

	fillTextBuffer(fontBuffer, str, x, y, charWidth, charHeight, font);

	drawTextured(PRIM_TRIANGLES, fontBuffer, n, fonts[font].texture, samplerState, blendState, depthState, textColor);

	return true;
}

#ifdef PROFILE
void Renderer::profileFrameStart(const float frameTime){
	profileStringIndex = sprintf(profileString, "Frametime: %.2fms\n\n", frameTime * 1000.0f);
	profileFrame = true;

	profileTimeIndex = 0;
	if (profileReset){
		memset(profileTimes, 0, sizeof(profileTimes));
		profileFrameCount = 0;
	}

	profileFrameCount++;
}

void Renderer::profileFrameEnd(){
	profileReset = !profileFrame;
	profileFrame = false;
}

void Renderer::profileBegin(const char *name){
	if (profileFrame){
		finish();

		profileStartTime = getCurrentTime();
		profileStringIndex += sprintf(profileString + profileStringIndex, "%s: ", name);
	}
}

void Renderer::profileNext(const char *name){
	if (profileFrame){
		finish();

		float currTime = getCurrentTime();
		profileTimes[profileTimeIndex] += (currTime - profileStartTime);

		profileStringIndex += sprintf(profileString + profileStringIndex, "%.2fms\n%s: ", (profileTimes[profileTimeIndex] / profileFrameCount) * 1000.0f, name);

		profileStartTime = currTime;
		profileTimeIndex++;
	}
}

void Renderer::profileEnd(){
	if (profileFrame){
		finish();
		float currTime = getCurrentTime();
		profileTimes[profileTimeIndex] += (currTime - profileStartTime);

		profileStringIndex += sprintf(profileString + profileStringIndex, "%.2fms\n", (profileTimes[profileTimeIndex] / profileFrameCount) * 1000.0f);

		profileTimeIndex++;
	}
}
#endif
