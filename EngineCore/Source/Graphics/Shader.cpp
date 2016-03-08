#include "CthughaStd.h"

#include "../CthughaEngine/CthughaApp.h"
#include "../ResourceCache/ResCache.h"
#include "Shader.h"

bool ShaderResourceLoader::VLoadResource(char *rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle)
{
	shared_ptr<ShaderResourceExtraData> extra = shared_ptr<ShaderResourceExtraData>(CHG_NEW ShaderResourceExtraData());

	shared_ptr<Renderer> renderer;
	renderer = g_pApp->m_pRenderer;
	
	if(renderer.get() == NULL) return false;

	extra->m_ShaderID = renderer->createShader(rawBuffer, rawSize);

	if(extra->m_ShaderID == -1)	return false;
	
	handle->SetExtra(shared_ptr<ShaderResourceExtraData>(extra));

	return true;
}

shared_ptr<IResourceLoader> CreateShaderResourceLoader()
{
	return shared_ptr<IResourceLoader>(CHG_NEW ShaderResourceLoader());
}

unsigned int ShaderResourceLoader::VGetLoadedResourceSize(char *rawBuffer, unsigned int rawSize)
{
	return rawSize;
}

bool ShaderLibraryResourceLoader::VLoadResource(char *rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle)
{
	shared_ptr<ShaderLibraryResourceExtraData> extra = shared_ptr<ShaderLibraryResourceExtraData>(CHG_NEW ShaderLibraryResourceExtraData());

	extra->m_pShaderLibraryData = CHG_NEW char[rawSize];

	memcpy(extra->m_pShaderLibraryData, rawBuffer, rawSize);

	handle->SetExtra(shared_ptr<ShaderLibraryResourceExtraData>(extra));

	return true;
}

shared_ptr<IResourceLoader> CreateShaderLibraryResourceLoader()
{
	return shared_ptr<IResourceLoader>(CHG_NEW ShaderLibraryResourceLoader());
}

unsigned int ShaderLibraryResourceLoader::VGetLoadedResourceSize(char *rawBuffer, unsigned int rawSize)
{
	return rawSize;
}

ShaderNode::ShaderNode(std::string shaderFileName, WeakBaseRenderComponentPtr renderComponent)
{
	m_ShaderFileName = shaderFileName;
	m_pRenderComponent = renderComponent;

	Resource resource(shaderFileName);
	shared_ptr<ResHandle> handle = g_pApp->m_ResCache->GetHandle(&resource);

	if(handle.get() == NULL)
	{
		CHG_ERROR("Not found resource" + shaderFileName);
	}

	m_Shader = ((ShaderResourceExtraData*)handle->GetExtra().get())->m_ShaderID;
	m_bhasTessellation = g_pApp->m_pRenderer->hasTessellation(m_Shader);

	m_InQueueFrustum = false;
	m_InQueueLight = false;
	m_InQueueRender = false;
}

bool ShaderNode::setActiveShader()
{
	g_pApp->m_pRenderer->setShader(m_Shader);
	g_pApp->m_pRenderer->setVertexFormat(m_VertexFormatID);

	return true;
}

void ShaderNode::setVertexFormatShaderEntry(const FormatDesc *vertexDesc, const UINT nAttributes)
{
	m_VertexFormatID = g_pApp->m_pRenderer->addVertexFormat(vertexDesc, nAttributes, m_Shader);
}