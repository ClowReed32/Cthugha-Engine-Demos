#ifndef TEXTHELPER_H_
#define TEXTHELPER_H_

#include "Renderer.h"
#include "../ResourceCache/ResCache.h"

class TTFFontResourceLoader;

class TTFFontResourceExtraData : public IResourceExtraData
{
	friend class TTFFontResourceLoader;

public:
	TTFFontResourceExtraData() 
	{
		//Nothing
	}

	virtual ~TTFFontResourceExtraData()
	{
		//Nothing
	}

	virtual std::string VToString()
	{
		return "TTFFontResourceLoader";
	}

	FontID m_FontID;
};

class TTFFontResourceLoader : public IResourceLoader
{
private:


public:
	//Constructor
	TTFFontResourceLoader()
	{
		//NOTHING
	}

	virtual bool VUseRawFile() { return false; }
	virtual bool VDiscardRawBufferAfterLoad() { return false; }
	virtual unsigned int VGetLoadedResourceSize(char *rawBuffer, unsigned int rawSize);
	virtual bool VLoadResource(char *rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle);
	virtual std::string VGetPattern() { return "*.ttf"; }
};

class CHGTextHelper
{
public:
	CHGTextHelper();
	CHGTextHelper(const char* resourceName);

	void SetInsertionPos(float x, float y);
	void SetColor(Color color);

	bool DrawFormattedTextLine(const char* strMsg, ... );
	bool DrawTextLine(const char* strMsg);
	bool DrawFormattedTextLine( Vec4& rc, BYTE Flags, const char* strMsg, ... );
    bool DrawTextLine( Vec4& rc, BYTE flags, const char* strMsg );

private:
	Vec2 m_textPos;
	Color m_textColor;
	FontID m_textFont;

	SamplerStateID m_textSampleState;
	BlendStateID m_textBlendState;
	DepthStateID m_textDepthState;
};

#endif