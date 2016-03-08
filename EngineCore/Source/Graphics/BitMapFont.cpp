#include "CthughaStd.h"

#include "../CthughaEngine/CthughaApp.h"
#include "../ResourceCache/ResCache.h"
#include "BitMapFont.h"
#include "Material.h"

shared_ptr<IResourceLoader> CreateBitMapFontResourceLoader()
{
	return shared_ptr<IResourceLoader>(CHG_NEW BitMapFontResourceLoader());
}

unsigned int BitMapFontResourceLoader::VGetLoadedResourceSize(char *rawBuffer, unsigned int rawSize)
{
	return rawSize;
}

bool BitMapFontResourceLoader::VLoadResource(char *rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle)
{
	shared_ptr<BitMapFontResourceExtraData> extra = shared_ptr<BitMapFontResourceExtraData>(CHG_NEW BitMapFontResourceExtraData());

	memoryStream stream;

	stream.buffer = rawBuffer;
	stream.initialPtr = rawBuffer;
	stream.size = rawSize;
	stream.offset = 0;

	if(stream.initialPtr == NULL) return false;

	TexFont* font = CHG_NEW TexFont();

	// Determine format by reading the first bytes of the file
	char str[4] = {0};
	memread(str, 3, 1, &stream);
	memseek(&stream, 0, MEM_SEEK_SET);

	if( strcmp(str, "BMF") == 0 )
	{
		char magicString[4];
		memread(magicString, 4, 1, &stream);
		if( strncmp(magicString, "BMF\003", 4) != 0 )
		{
			CHG_ERROR("Unrecognized format BMP font");
			if(font)
			{
				delete font;
			}

			return false;
		}

		// Read each block
		char blockType;
		int blockSize;
		while( memread(&blockType, 1, 1, &stream) )
		{
			// Read the blockSize
			memread(&blockSize, 4, 1, &stream);

			switch( blockType )
			{
			case 1: // info
				ReadInfoBlock(blockSize, stream);
				break;
			case 2: // common
				ReadCommonBlock(blockSize, stream);
				break;
			case 3: // pages
				ReadPagesBlock(blockSize, stream, handle);
				break;
			case 4: // chars
				ReadCharsBlock(blockSize, font, stream);
				break;
			case 5: // kerning pairs
				//Unused	Future Implementation
				ReadKernelBlock(blockSize, stream);
				break;
			default:
				 CHG_ERROR("BMP Font: Unexpected block type (%d)");
				if(font)
				{
					delete font;
				}

				return false;
			}
		}

		extra->m_FontID = g_pApp->m_pRenderer->addFont(*font);
		extra->m_SamplerLinearID = m_vPages[0].sampleFont;

		handle->SetExtra(shared_ptr<BitMapFontResourceExtraData>(extra));

		m_vPages.clear();
		delete font;

		return true;
	}

	if(font)
	{
		delete font;
	}

	return false;
}

void BitMapFontResourceLoader::ReadInfoBlock(int size, memoryStream &stream)
{
	char *buffer = CHG_NEW char[size];
	memread(buffer, size, 1, &stream);
	
	m_infoBlock.outline = ((infoBlock*)buffer)->outline;

	delete buffer;
}

void BitMapFontResourceLoader::ReadCommonBlock(int size, memoryStream &stream)
{
	char *buffer = CHG_NEW char[size];
	memread(buffer, size, 1, &stream);
	
	m_commonBlock.lineHeight = ((commonBlock*)buffer)->lineHeight;
	m_commonBlock.base = ((commonBlock*)buffer)->base;
	m_commonBlock.scaleW = ((commonBlock*)buffer)->scaleW;
	m_commonBlock.scaleH = ((commonBlock*)buffer)->scaleH;
	m_commonBlock.pages = ((commonBlock*)buffer)->pages; 

	delete buffer;
}

void BitMapFontResourceLoader::ReadKernelBlock(int size, memoryStream &stream)
{
	char *buffer = CHG_NEW char[size];
	memread(buffer, size, 1, &stream);
	delete buffer;
}

void BitMapFontResourceLoader::ReadPagesBlock(int size, memoryStream &stream, shared_ptr<ResHandle> handle)
{
	char *buffer = CHG_NEW char[size];
	memread(buffer, size, 1, &stream);

	pagesBlock *blk = (pagesBlock*)buffer;

	for( int id = 0, pos = 0; pos < size; id++ )
	{
		LoadPage(id, &blk->pageNames[pos], handle);
		pos += 1 + (int)strlen(&blk->pageNames[pos]);
	}

	delete buffer;
}

void BitMapFontResourceLoader::LoadPage(int id, const char *pageFile, shared_ptr<ResHandle> handle)
{
	std::string str;
	shared_ptr<ResHandle> Texturehandle;

	// Load the texture from the same directory as the font descriptor file

	// Find the directory
	str = handle->GetName();

	for( size_t n = 0; (n = str.find('/', n)) != std::string::npos; ) str.replace(n, 1, "\\");
	size_t i = str.rfind('\\');
	if( i != std::string::npos )
		str = str.substr(0, i+1);
	else
		str = "";

	// Load the font textures
	str += pageFile;

	Texturehandle = g_pApp->m_ResCache->GetHandle(&Resource(str));
	FontTexture fontTexture = {((TextureResourceExtraData*)(Texturehandle->GetExtra().get()))->GetTexture(), ((TextureResourceExtraData*)(Texturehandle->GetExtra().get()))->GetSampler()};

	m_vPages.push_back(fontTexture);
}

void BitMapFontResourceLoader::ReadCharsBlock(int size, TexFont* font, memoryStream &stream)
{
	char *buffer = CHG_NEW char[size];
	memread(buffer, size, 1, &stream);
	font->texture = m_vPages[0].textureFont;

	charsBlock *blk = (charsBlock*)buffer;
	float maxHeight = 0.0f, maxWidth = 0.0f;

	for( int n = 0; int(n*sizeof(charsBlock::charInfo)) < size; n++ )
	{
		font->chars[blk->chars[n].id].x0 = (float)blk->chars[n].x / (float)m_commonBlock.scaleW;
		font->chars[blk->chars[n].id].y0 = (float)blk->chars[n].y / (float)m_commonBlock.scaleH;
		font->chars[blk->chars[n].id].x1 = (float)(blk->chars[n].x + blk->chars[n].width) / (float)m_commonBlock.scaleW;
		font->chars[blk->chars[n].id].y1 = (float)(blk->chars[n].y + blk->chars[n].height) / (float)m_commonBlock.scaleH;
		font->chars[blk->chars[n].id].page = blk->chars[n].page;
		font->chars[blk->chars[n].id].ratioX = 1.0f;
		font->chars[blk->chars[n].id].ratioY = 1.0f;
		font->chars[blk->chars[n].id].xOffset = blk->chars[n].xoffset;
		font->chars[blk->chars[n].id].yOffset = blk->chars[n].yoffset;
		font->chars[blk->chars[n].id].xadvance = blk->chars[n].xadvance;

		if(maxHeight < blk->chars[n].height)
		{
			maxHeight = blk->chars[n].height;
		}

		if(maxWidth < blk->chars[n].width)
		{
			maxWidth = blk->chars[n].width;
		}
	}

	//Calculate ratio
	for(int i = 0; i < 256; i++)
	{
		font->chars[i].ratioX = ((font->chars[i].x1 - font->chars[i].x0)*(float)m_commonBlock.scaleW) / maxWidth;
		font->chars[i].ratioY = ((font->chars[i].y1 - font->chars[i].y0)*(float)m_commonBlock.scaleW) / maxHeight;
		font->chars[i].xOffset /= maxWidth;
		font->chars[i].yOffset /= maxHeight;
		font->chars[i].xadvance /= maxWidth;
	}

	delete buffer;

}