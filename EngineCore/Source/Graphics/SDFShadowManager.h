//========================================================================
// SDFShadowManager.h - defines the class for manage SDF shadow generation 
//					and shadow scene structure update.
//
//========================================================================

#pragma once

#include "..\Msvc\CthughaStd.h"

#define SDF_MAX_ELEMENTS 50
#define SDF_MAX_CLUSTERS 20
#define CLUSTER_DIM 14.0f

#define MAX_CULLED_OBJECT 1024
#define MAX_SHADOW_POINT_LIGHT 256
#define MAX_SHADOW_DIRECTIONAL_LIGHT 32
#define MAX_OBJECT_PER_LIGHT 128

//---------------------------------------------------------------------------------------------------------------------
// 
//	ShadowCoordResourceLoader class
//
//---------------------------------------------------------------------------------------------------------------------

class ShadowCoordResourceLoader : public IResourceLoader
{
public:
    //Constructor
    ShadowCoordResourceLoader() { }

    virtual bool VUseRawFile() { return false; }
    virtual bool VDiscardRawBufferAfterLoad() { return true; }
    virtual unsigned int VGetLoadedResourceSize(char *rawBuffer, unsigned int rawSize);
    virtual bool VLoadResource(char *rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle);
    virtual std::string VGetPattern() { return "*.pdata"; }
};

//---------------------------------------------------------------------------------------------------------------------
// 
//	SDFResourceLoader class
//
//---------------------------------------------------------------------------------------------------------------------

class SDFResourceLoader : public IResourceLoader
{
public:
    //Constructor
    SDFResourceLoader() { }

    virtual bool VUseRawFile() { return false; }
    virtual bool VDiscardRawBufferAfterLoad() { return true; }
    virtual unsigned int VGetLoadedResourceSize(char *rawBuffer, unsigned int rawSize);
    virtual bool VLoadResource(char *rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle);
    virtual std::string VGetPattern() { return "*.psdf"; }
};

//---------------------------------------------------------------------------------------------------------------------
// 
//	ShadowCoordResourceExtraData class
//
//---------------------------------------------------------------------------------------------------------------------

class ShadowCoordResourceExtraData : public IResourceExtraData
{
    friend class COLLADAMeshResourceLoader;

public:
    ShadowCoordResourceExtraData()
    {
        //Nothing
    }

    virtual ~ShadowCoordResourceExtraData()
    {

    }

    virtual std::string VToString()
    {
        return "ShadowCoordResourceLoader";
    }

    std::map<std::string, std::pair<Vec3, Vec3>> m_SDFTexCoord;
};

//---------------------------------------------------------------------------------------------------------------------
// 
//	SDFResourceResourceExtraData class
//
//---------------------------------------------------------------------------------------------------------------------

class SDFResourceResourceExtraData : public IResourceExtraData
{
    friend class COLLADAMeshResourceLoader;

public:
    SDFResourceResourceExtraData()
    {
        //Nothing
    }

    virtual ~SDFResourceResourceExtraData()
    {

    }

    virtual std::string VToString()
    {
        return "SDFResourceLoader";
    }

    TextureID m_SDFTextureID;
	Vec3 m_vPackDimensions;
};

//---------------------------------------------------------------------------------------------------------------------
// 
//	SDFShadowManager class
//
//---------------------------------------------------------------------------------------------------------------------

class SDFShadowManager
{
    std::map<std::string, std::pair<Vec3, Vec3>> m_SDFTextureCoords;
    TextureID m_texSDFPackTextureID;
	Vec3 m_vSDFShadowPackDim;

	StructuredBufferID m_sbPoint_DirectionalLightBounds;
	StructuredBufferID m_sbCulledObjectBounds;
	StructuredBufferID m_sbCulledObjectSDFParameters;
	StructuredBufferID m_sbPoint_Directional_Light_CulledObject_Intersection;

	ShaderID m_shPointLight_Object_Intersection_ShaderID;
	ShaderID m_shDirectionalLight_Object_Intersection_ShaderID;

	UINT m_uiPointLightCount;
	UINT m_uiDirectionalLightCount;
	UINT m_uiCulledObjectCount;

public:

    SDFShadowManager();
	~SDFShadowManager() { };

	void OnUpdatePointDirectionalLightAndCullingObject(Scene *pScene);
	void OnUpdatePointDirectionalLightAndCullingObject(Scene *pScene, std::list<std::shared_ptr<SceneNode>> LightList);
	void OnUpdateSDFObjectParameter(Scene *pScene);
	void SetShaderSDFBuffersForLight(Scene* pScene);

	bool GetSDFCoords(std::string resourceName, Vec4& coord, Vec4& dim);
	Vec3 GetSDFPackDimensions() { return m_vSDFShadowPackDim; }
};