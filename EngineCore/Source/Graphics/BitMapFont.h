#include "Geometry.h"
#include "../Graphics/Renderer.h"
#include "../ResourceCache/ResCache.h"

//
//	Auxiliar Data Structures
//

struct memoryStream;

struct FontTexture
{
	TextureID textureFont;
	SamplerStateID sampleFont;
};

struct infoBlock
{
    WORD fontSize;
    BYTE reserved:4;
    BYTE bold    :1;
    BYTE italic  :1;
    BYTE unicode :1;
    BYTE smooth  :1;
    BYTE charSet;
    WORD stretchH;
    BYTE aa;
    BYTE paddingUp;
    BYTE paddingRight;
    BYTE paddingDown;
    BYTE paddingLeft;
    BYTE spacingHoriz;
    BYTE spacingVert;
    BYTE outline;         // Added with version 2
    char fontName[1];
};

struct pagesBlock
{
    char pageNames[1];
};

struct charsBlock
{
    struct charInfo
    {
        DWORD id;
        WORD  x;
        WORD  y;
        WORD  width;
        WORD  height;
        short xoffset;
        short yoffset;
        short xadvance;
        BYTE  page;
        BYTE  chnl;
    } chars[1];
};

struct commonBlock
{
    WORD lineHeight;
    WORD base;
    WORD scaleW;
    WORD scaleH;
    WORD pages;
    BYTE packed  :1;
    BYTE reserved:7;
	BYTE alphaChnl;
	BYTE redChnl;
	BYTE greenChnl;
	BYTE blueChnl;
};

struct kerningPairsBlock
{
    struct kerningPair
    {
        DWORD first;
        DWORD second;
        short amount;
    } kerningPairs[1];
};


//
// class BitMapFontResourceExtraData
//

class BitMapFontResourceExtraData : public IResourceExtraData
{
	friend class BitMapFontResourceLoader;

public:
	BitMapFontResourceExtraData() {}
	virtual ~BitMapFontResourceExtraData() {}
	virtual std::string VToString() { return "BitMapFontResourceExtraData"; }

	FontID GetFontID() { return m_FontID; }
	SamplerStateID GetSamplerID() { return m_SamplerLinearID; }

protected:
	FontID m_FontID;			
	SamplerStateID m_SamplerLinearID;
};



// 
//  class BitMapFontResourceLoader				
//
class BitMapFontResourceLoader : public IResourceLoader
{
public:
	virtual bool VUseRawFile() { return false; }
	virtual bool VDiscardRawBufferAfterLoad() { return true; }
	virtual unsigned int VGetLoadedResourceSize(char *rawBuffer, unsigned int rawSize);
	virtual bool VLoadResource(char *rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle);
	virtual std::string VGetPattern() { return "*.fnt"; }

private:
	infoBlock m_infoBlock;
	commonBlock m_commonBlock;
	kerningPairsBlock m_kerningPairsBlock;
	std::vector<FontTexture> m_vPages;

	void ReadInfoBlock(int size, memoryStream &stream);
	void ReadCommonBlock(int size, memoryStream &stream);
	void ReadPagesBlock(int size, memoryStream &stream, shared_ptr<ResHandle> handle);
	void ReadCharsBlock(int size, TexFont* font, memoryStream &stream);
	void ReadKernelBlock(int size, memoryStream &stream);
	void LoadPage(int id, const char *pageFile, shared_ptr<ResHandle> handle);
};