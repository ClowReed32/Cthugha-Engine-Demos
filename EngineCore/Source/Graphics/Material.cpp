//========================================================================
// File: Material.cpp - stores texture and material information for D3D9 and D3D11
//
//========================================================================

#include "CthughaStd.h"

#include "../CthughaEngine/CthughaApp.h"
#include "../ResourceCache/ResCache.h"
#include "Material.h"


void Material::AddTexture(std::string sResource, std::string sName)
{
	if (sResource == "")
	{
		CHG_WARNING("A Texture have not resource file. Texture wont be loaded.");
		return;
	}

	if (sName == "")
	{
		CHG_WARNING("Texture must have a name.");
		return;
	}

	MaterialTexture texture;

	texture.sResource = sResource;
	texture.sName = sName;

	Resource resource(sResource);
	shared_ptr<ResHandle> handle = g_pApp->m_ResCache->GetHandle(&resource);

	if (handle.get() == NULL)
	{
		CHG_ERROR("Not found texture resource: " + sResource);
	}

	texture.id = ((TextureResourceExtraData*)handle->GetExtra().get())->GetTexture();

	m_lTextureList.push_back(texture);
}

void Material::AddConstant(std::string name, std::string type, void* pValue)
{
	MaterialConstant newProperty;

	if (type == "VEC4")
	{
		newProperty.uSize = 16;
	}
	else if (type == "VEC3")
	{
		newProperty.uSize = 12;
	}
	else if (type == "VEC2")
	{
		newProperty.uSize = 8;
	}
	else if (type == "INTEGER" || type == "FLOAT")
	{
		newProperty.uSize = 4;
	}

	newProperty.sName = name;
	newProperty.pValue = pValue;

	m_lConstantList.push_back(newProperty);
}

void Material::RemoveTextures()
{
	std::shared_ptr<Renderer> pRenderer = g_pApp->m_pRenderer;

	for (auto texture : m_lTextureList)
	{
		if (texture.id != TEXTURE_NONE)
			pRenderer->removeTexture(texture.id);

		if (texture.sResource != "")
			g_pApp->m_ResCache->FreeResource(texture.sResource);
	}
}

MaterialNode::MaterialNode(std::string sMaterialResourceName, WeakBaseRenderComponentPtr renderComponent)
{
	Resource resource(sMaterialResourceName);
	shared_ptr<ResHandle> handle = g_pApp->m_ResCache->GetHandle(&resource);

	if (handle.get() == NULL)
	{
		CHG_ERROR("Not found material resource: " + sMaterialResourceName);
	}

	m_pMaterial = ((MaterialResourceExtraData*)handle->GetExtra().get())->GetMaterial();

	m_pRenderComponent = renderComponent;
	m_bActivePOM = false;
}

void MaterialNode::setActiveMaterial()
{
	shared_ptr<Renderer> pRenderer = g_pApp->m_pRenderer;

	for (auto texture : m_pMaterial->m_lTextureList)
	{
		pRenderer->setTexture(texture.sName.c_str(), texture.id);
	}

	for (auto element : m_pMaterial->m_lConstantList)
	{
		pRenderer->setShaderConstantStruct(element.sName.c_str(), element.pValue, element.uSize);
	}
}

//
// class DdsResourceLoader					- creates an interface with the Resource cache to load DDS files
//
class DdsResourceLoader : public TextureResourceLoader
{
public:
	virtual std::string VGetPattern() { return "*.dds"; }
};

shared_ptr<IResourceLoader> CreateDDSResourceLoader()
{
	return shared_ptr<IResourceLoader>(CHG_NEW DdsResourceLoader());
}

//
// class JpgResourceLoader					- creates an interface with the Resource cache to load JPG files
//
class JpgResourceLoader : public TextureResourceLoader
{
public:
	virtual std::string VGetPattern() { return "*.jpg"; }
};

shared_ptr<IResourceLoader> CreateJPGResourceLoader()
{
	return shared_ptr<IResourceLoader>(CHG_NEW JpgResourceLoader());
}

//
// class PngResourceLoader					- creates an interface with the Resource cache to load Png files
//
class PngResourceLoader : public TextureResourceLoader
{
public:
	virtual std::string VGetPattern() { return "*.png"; }
};

shared_ptr<IResourceLoader> CreatePNGResourceLoader()
{
	return shared_ptr<IResourceLoader>(CHG_NEW PngResourceLoader());
}

//
// class RawResourceLoader					- creates an interface with the Resource cache to load Raw files
//
class RawResourceLoader : public TextureResourceLoader
{
public:
    virtual std::string VGetPattern() { return "*.raw"; }
};

shared_ptr<IResourceLoader> CreateRAWResourceLoader()
{
    return shared_ptr<IResourceLoader>(CHG_NEW RawResourceLoader());
}

unsigned int TextureResourceLoader::VGetLoadedResourceSize(char *rawBuffer, unsigned int rawSize)
{
	// This will keep the resource cache from allocating memory for the texture, so DirectX can manage it on it's own.
	return 0;
}

bool TextureResourceLoader::VLoadResource(char *rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle)
{
	shared_ptr<TextureResourceExtraData> extra = shared_ptr<TextureResourceExtraData>(CHG_NEW TextureResourceExtraData());

	shared_ptr<Renderer> renderer;
	renderer = g_pApp->m_pRenderer;
	
	if(renderer.get() == NULL) return false;

	extra->m_SamplerLinearID = -1;
	extra->m_TextureID = -1;

	if((extra->m_SamplerLinearID = renderer->addSamplerState(TRILINEAR_ANISO, WRAP, WRAP, WRAP)) == -1)	return false;
	if((extra->m_TextureID = renderer->createTexture(rawBuffer, rawSize, VGetPattern(), true, extra->m_SamplerLinearID)) == -1) return false;

	handle->SetExtra(shared_ptr<TextureResourceExtraData>(extra));

	return true;
}

shared_ptr<IResourceLoader> CreateMaterialResourceLoader()
{
	return shared_ptr<IResourceLoader>(CHG_NEW MaterialResourceLoader());
}

unsigned int MaterialResourceLoader::VGetLoadedResourceSize(char *rawBuffer, unsigned int rawSize)
{
	// This will keep the resource cache from allocating memory for the texture, so DirectX can manage it on it's own.
	return 0;
}

bool MaterialResourceLoader::VLoadResource(char *rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle)
{
	shared_ptr<MaterialResourceExtraData> extra = shared_ptr<MaterialResourceExtraData>(CHG_NEW MaterialResourceExtraData());
	std::shared_ptr<Material> pMaterial = std::shared_ptr<Material>(CHG_NEW Material());

	const std::string fileURI("from std::istream");

	TiXmlDocument materialResource;
	TiXmlElement* root;
	std::string sTextureFile;

	materialResource.Parse(rawBuffer, NULL, TIXML_ENCODING_UTF8);
	root = materialResource.RootElement();

	TiXmlElement* pPropertiesElement = root->FirstChildElement("Properties");

    std::string sBlendMode = pPropertiesElement->Attribute("BlendMode");

    if (sBlendMode == "OPAQUE")
    {
        pMaterial->SetBlendMode(OPAQUE_MODE);
    }
    else if (sBlendMode == "TRANSLUCENT")
    {
        pMaterial->SetBlendMode(TRANSLUCENT_MODE);
    }
    else if (sBlendMode == "ADITIVE")
    {
        pMaterial->SetBlendMode(ADITIVE_MODE);
    }
	else if (sBlendMode == "SUBSURFACE")
	{
		pMaterial->SetBlendMode(SUBSURFACE_MODE);
	}

    std::string sWrap = pPropertiesElement->Attribute("Wrap");

    if (sWrap == "MIRROR")
    {
        pMaterial->SetWrapMode(MIRROR_MODE);
    }
    else if (sWrap == "CLAMP")
    {
        pMaterial->SetWrapMode(CLAMP_MODE);
    }
    else if (sWrap == "WRAP")
    {
        pMaterial->SetWrapMode(WRAP_MODE);
    }

    TiXmlElement* pTexturesElement = root->FirstChildElement("Textures");

    for (TiXmlElement* pNode = pTexturesElement->FirstChildElement(); pNode; pNode = pNode->NextSiblingElement())
    {
        std::string sName = pNode->Attribute("Name");
        std::string sFile = pNode->Attribute("File");

        pMaterial->AddTexture(sFile, sName);
    }

    TiXmlElement* pConstantsElement = root->FirstChildElement("Constants");

    for (TiXmlElement* pNode = pConstantsElement->FirstChildElement(); pNode; pNode = pNode->NextSiblingElement())
    {
        std::string sName = pNode->Attribute("Name");
        std::string sType = pNode->Attribute("Type");
        void* pValue;

        if (sType == "VEC4")
        {
            Vec4 vVec = Vec4(pNode->Attribute("Value"));
            pValue = CHG_NEW Vec4();
            memccpy(pValue, &vVec, 1, 16);
        }
        else if (sType == "VEC3")
        {
            Vec3 vVec = Vec3(pNode->Attribute("Value"));
            pValue = CHG_NEW Vec3();
            memccpy(pValue, &vVec, 1, 12);
        }
        else if (sType == "VEC2")
        {
            Vec2 vVec = Vec2(pNode->Attribute("Value"));
            pValue = CHG_NEW Vec2();
            memccpy(pValue, &vVec, 1, 8);
        }
        else if (sType == "INTEGER")
        {
            int iVal = atoi(pNode->Attribute("Value"));
            pValue = CHG_NEW int();
            memccpy(pValue, &iVal, 1, 4);
        }
        else if (sType == "FLOAT")
        {
            float fVal = (float)atof(pNode->Attribute("Value"));
            pValue = CHG_NEW float();
            memccpy(pValue, &fVal, 1, 4);
        }

        pMaterial->AddConstant(sName, sType, pValue);
    }

    extra->m_pMaterial = pMaterial;

	handle->SetExtra(shared_ptr<MaterialResourceExtraData>(extra));

	return true;
}