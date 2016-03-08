#include "CthughaStd.h"

#include "../CthughaEngine/CthughaApp.h"
#include "../ResourceCache/ResCache.h"
#include "TextHelper.h"

shared_ptr<IResourceLoader> CreateTTFFontResourceLoader()
{
	return shared_ptr<IResourceLoader>(CHG_NEW TTFFontResourceLoader());
}

unsigned int TTFFontResourceLoader::VGetLoadedResourceSize(char *rawBuffer, unsigned int rawSize)
{
	return rawSize;
}

bool TTFFontResourceLoader::VLoadResource(char *rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle)
{
	shared_ptr<TTFFontResourceExtraData> extra = shared_ptr<TTFFontResourceExtraData>(CHG_NEW TTFFontResourceExtraData());
	
	shared_ptr<Renderer> renderer = g_pApp->m_pRenderer;

	if(renderer.get() == NULL) return false;

	extra->m_FontID = renderer->addFont(rawBuffer, rawSize, 14, -1); //fix size

	handle->SetExtra(shared_ptr<TTFFontResourceExtraData>(extra));
	
	return true;
}

CHGTextHelper::CHGTextHelper()
{
	m_textColor = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
	m_textFont = -1;

	m_textSampleState = -1;
	m_textBlendState = -1;
	m_textDepthState = -1;
}

CHGTextHelper::CHGTextHelper(const char* resourceName)
{
	Resource resource(resourceName);
	shared_ptr<ResHandle> handle = g_pApp->m_ResCache->GetHandle(&resource);

	if(handle.get() == NULL)
	{
		CHG_ERROR("Not found resource");
	}

	m_textFont = ((TTFFontResourceExtraData*)handle->GetExtra().get())->m_FontID;

	m_textColor = Vec4(1.0f, 1.0f, 1.0f, 1.0f);

	m_textSampleState = -1;
	//m_textSampleState = g_pApp->m_pRenderer->addSamplerState(LINEAR, CLAMP, CLAMP, CLAMP);
	m_textDepthState = g_pApp->m_pRenderer->addDepthState(false, false);
	m_textBlendState = g_pApp->m_pRenderer->addBlendState(SRC_ALPHA, ONE_MINUS_SRC_ALPHA);
}

void CHGTextHelper::SetColor(Color color)
{
	m_textColor = color;
}

void CHGTextHelper::SetInsertionPos(float x, float y)
{
	m_textPos.x = x;
	m_textPos.y = y;
}

bool CHGTextHelper::DrawTextLine(const char* strmsg)
{
	//return g_pApp->m_pRenderer->drawText(strmsg, m_textPos.x, m_textPos.y, m_textFont, m_textSampleState, m_textDepthState, m_textBlendState, &m_textColor); 
	return true;
}

bool CHGTextHelper::DrawFormattedTextLine(const char* strmsg, ... )
{
	char buffer[250];

	va_list args;
	va_start(args, strmsg);
	int size = sprintf(buffer, strmsg,  args);
	buffer[size] = '\0';
	va_end(args);

	return DrawTextLine(buffer);
}

bool CHGTextHelper::DrawTextLine(Vec4& rc, BYTE flags, const char* strmsg)
{
	//return g_pApp->m_pRenderer->drawText(strmsg, m_textPos.x, m_textPos.y, m_textFont, m_textSampleState, m_textDepthState, m_textBlendState, &m_textColor, &rc, flags); 
	return true;
}

bool CHGTextHelper::DrawFormattedTextLine( Vec4& rc, BYTE flags, const char* strmsg, ... )
{
	char buffer[250];

	va_list args;
	va_start(args, strmsg);
	int size = sprintf(buffer, strmsg,  args);
	buffer[size] = '\0';
	va_end(args);

	return DrawTextLine(rc, flags, buffer);
}
