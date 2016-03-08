#pragma once
//========================================================================
// File: Material.h - stores texture and material information for D3D9 and D3D11
//
//========================================================================

//========================================================================
//  
// 
//  class Material				

#include "Geometry.h"
#include "Renderer.h"
#include "../ResourceCache/ResCache.h"
#include "SceneNodes.h"

enum WrapMode {MIRROR_MODE, WRAP_MODE, CLAMP_MODE};
enum BlendMode {OPAQUE_MODE, TRANSLUCENT_MODE, ADITIVE_MODE, SUBSURFACE_MODE};

struct MaterialConstant
{
	std::string sName;
	UINT uSize;
	void* pValue;
};

struct MaterialTexture
{
	std::string sResource;
	std::string sName;
	TextureID id;
};

class MaterialNode;

class Material
{
private:

	friend class MaterialNode;

	// Texture Wrap Mode
	WrapMode m_WrapMode;

	// Material Opacity
	BlendMode m_BlendMode;

	// Shader Textures
	std::vector<MaterialTexture> m_lTextureList;

	// Shader Constants
	std::vector<MaterialConstant> m_lConstantList;

public:
	Material() 
	{
		m_WrapMode = WRAP_MODE;
		m_BlendMode = OPAQUE_MODE;
	}
	~Material() 
	{
		for (auto element : m_lConstantList)
		{
			SAFE_DELETE(element.pValue);
		}
	}

	WrapMode GetWrapMode() { return m_WrapMode; }
	BlendMode GetBlendMode() { return m_BlendMode; }

	void SetWrapMode(WrapMode wrapMode) { m_WrapMode = wrapMode; }
	void SetBlendMode(BlendMode blendMode) { m_BlendMode = blendMode; }

	void AddConstant(std::string name, std::string type, void* pValue);
	void AddTexture(std::string sResource, std::string sName);
	
	void RemoveTextures();
};

class MaterialNode : public SceneNode
{
private:
	std::shared_ptr<Material> m_pMaterial;
	bool m_bActivePOM;
	WeakBaseRenderComponentPtr m_pRenderComponent;

public:
	MaterialNode(std::string sMaterialResourceName, WeakBaseRenderComponentPtr renderComponent);

	virtual bool VOnRestore(Scene *pScene) { return true; }
	virtual bool VOnLostDevice(Scene *pScene) { return true; }
	virtual bool VIsVisible(Scene *pScene) const { return false; }

	virtual SceneNodeType VGetType() { return MATERIAL_NODE; }

	void setActiveMaterial();

	void setActivePOM(bool active) { m_bActivePOM = active;	}
	bool getActivePOM()	{ return m_bActivePOM; }
	bool IsOpaque() { return m_pMaterial->m_BlendMode == OPAQUE_MODE || m_pMaterial->m_BlendMode == SUBSURFACE_MODE; }

	BlendMode GetBlendMode() { return m_pMaterial->m_BlendMode; }
	WrapMode GetWrapMode() { return m_pMaterial->m_WrapMode; }
};

//
// class TextureResourceExtraData
//

class TextureResourceExtraData : public IResourceExtraData
{
	friend class TextureResourceLoader;

public:
	TextureResourceExtraData() {}
	virtual ~TextureResourceExtraData() {}
	virtual std::string VToString() { return "TextureResourceExtraData"; }

	TextureID GetTexture() { return m_TextureID; }
	SamplerStateID GetSampler() { return m_SamplerLinearID; }

protected:
	TextureID m_TextureID;			
	SamplerStateID m_SamplerLinearID;
};



// 
//  class TextureResourceLoader				
//
class TextureResourceLoader : public IResourceLoader
{
public:
	virtual bool VUseRawFile() { return false; }
	virtual bool VDiscardRawBufferAfterLoad() { return true; }
	virtual unsigned int VGetLoadedResourceSize(char *rawBuffer, unsigned int rawSize);
	virtual bool VLoadResource(char *rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle);
};

//
// class MaterialResourceExtraData
//

class MaterialResourceExtraData : public IResourceExtraData
{
	friend class MaterialResourceLoader;

public:
	MaterialResourceExtraData() {}
	virtual ~MaterialResourceExtraData() {}
	virtual std::string VToString() { return "MaterialResourceExtraData"; }

	std::shared_ptr<Material> GetMaterial() { return m_pMaterial; }

protected:
	std::shared_ptr<Material> m_pMaterial;
};



// 
//  class MaterialResourceLoader				
//
class MaterialResourceLoader : public IResourceLoader
{
public:
	virtual bool VUseRawFile() { return false; }
	virtual bool VDiscardRawBufferAfterLoad() { return true; }
	virtual unsigned int VGetLoadedResourceSize(char *rawBuffer, unsigned int rawSize);
	virtual bool VLoadResource(char *rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle);
	virtual std::string VGetPattern() { return "*.mat"; }
};
