#ifndef SHADER_H_
#define SHADER_H_

#include "../ResourceCache/ResCache.h"
#include "SceneNodes.h"

class ShaderResourceLoader : public IResourceLoader
{
public:
	//Constructor
	ShaderResourceLoader()
	{
		//NOTHING
	}

	virtual bool VUseRawFile() { return false; }
	virtual bool VDiscardRawBufferAfterLoad() { return true; }
	virtual unsigned int VGetLoadedResourceSize(char *rawBuffer, unsigned int rawSize);
	virtual bool VLoadResource(char *rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle);
	virtual std::string VGetPattern() { return "*.shd"; }
};

//
// class ShaderResourceExtraData
//
class ShaderResourceExtraData : public IResourceExtraData
{
	friend class COLLADAMeshResourceLoader;

public:
	ShaderResourceExtraData() 
	{
		m_ShaderID = -1;
	}

	virtual ~ShaderResourceExtraData()
	{
		//Nothing
	}

	virtual std::string VToString()
	{
		return "ShaderResourceLoader";
	}

	ShaderID m_ShaderID;
};

class ShaderLibraryResourceLoader : public IResourceLoader
{
public:
	//Constructor
	ShaderLibraryResourceLoader()
	{
		//NOTHING
	}

	virtual bool VUseRawFile() { return false; }
	virtual bool VDiscardRawBufferAfterLoad() { return true; }
	virtual unsigned int VGetLoadedResourceSize(char *rawBuffer, unsigned int rawSize);
	virtual bool VLoadResource(char *rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle);
	virtual std::string VGetPattern() { return "*.shdl"; }
};

//
// class ShaderLibraryResourceExtraData
//
class ShaderLibraryResourceExtraData : public IResourceExtraData
{
	void* m_pShaderLibraryData;

	friend class ShaderLibraryResourceLoader;

public:
	ShaderLibraryResourceExtraData()
	{
		m_pShaderLibraryData = NULL;
	}

	virtual ~ShaderLibraryResourceExtraData()
	{
		SAFE_DELETE_ARRAY(m_pShaderLibraryData);
	}

	virtual std::string VToString()
	{
		return "ShaderLibraryResourceExtraData";
	}

	void* GetShaderLibraryData() { return m_pShaderLibraryData; }
};


class ShaderNode : public SceneNode
{
private:
	ShaderID m_Shader;
	VertexFormatID m_VertexFormatID;
	std::string m_ShaderFileName;
	WeakBaseRenderComponentPtr m_pRenderComponent;

	bool m_bhasTessellation; //help for tessellation mesh

    friend class MeshNode;
	friend class DecalNode;

public:
	ShaderNode(std::string shaderFileName, WeakBaseRenderComponentPtr renderComponent);
	
	virtual bool VOnRestore(Scene *pScene)  { return true; }
	virtual bool VOnLostDevice(Scene *pScene) { return true; }
	virtual bool VIsVisible(Scene *pScene) const { return false; }

	virtual SceneNodeType VGetType() { return SHADER_NODE; }

	bool setActiveShader();
	bool hasTessellation() { return m_bhasTessellation; }
	void setVertexFormatShaderEntry(const FormatDesc *vertexDesc, const UINT nAttributes);
};

#endif
