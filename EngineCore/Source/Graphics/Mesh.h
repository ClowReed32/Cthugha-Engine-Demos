#pragma once
//========================================================================
// File: Mesh.h - classes to render meshes
//
//========================================================================

#include "Geometry.h"
#include "Material.h"
#include "Shader.h"
#include "../ResourceCache/ResCache.h"
#include "Scene.h"

struct TempMesh
{
	std::vector<Vec3> Positions;
	std::vector<Vec2> UVs;
	std::vector<Vec3> Normals;
	std::vector<Vec3> Tangents;
	std::vector<Vec3> BiTangents;
	std::vector<unsigned int> ComplexIndex;
	unsigned int TypeIndex;

	TiXmlElement* geometry;
	TiXmlElement* materialNode;
	Material material;

	std::string m_DiffuseTextureName;
	std::string m_NormalTextureName;
	std::string m_SpecularTextureName;

	std::string name;
	Mat4x4 localMatrix;
};

class MeshNode;
class COLLADAMesh;

//
// class SdkMeshResourceLoader							- Chapter 16, page 561
//
class SdkMeshResourceLoader : public IResourceLoader
{
public:
	virtual bool VUseRawFile() { return false; }
	virtual bool VDiscardRawBufferAfterLoad() { return true; }
	virtual unsigned int VGetLoadedResourceSize(char *rawBuffer, unsigned int rawSize);
	virtual bool VLoadResource(char *rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle);
	virtual std::string VGetPattern() { return "*.sdkmesh"; }
};

//
//class COLLADASubMesh
//

class COLLADASubMesh
{
private:

	friend class COLLADAMeshResourceLoader;
	friend class MeshNode;
	
	Mat4x4 m_LocalTransform;
	Material m_Material;

	VertexBufferID m_PosVertexBufferID;
	VertexBufferID m_NormVertexBufferID;
	VertexBufferID m_UVVertexBufferID;
	VertexBufferID m_TangVertexBufferID;
	VertexBufferID m_BinormVertexBufferID;
	IndexBufferID m_IndexBufferID;

	UINT m_NumIndex;
	UINT m_NumVertex;

	Primitives m_PrimitiveType;
	COLLADAMesh *m_pParent;

	Aabb m_SubMeshAabb;

	std::string m_Name;

public:
	COLLADASubMesh()
	{
		m_LocalTransform = m_LocalTransform.g_Identity;

		m_PosVertexBufferID = VB_NONE;
		m_NormVertexBufferID = VB_NONE;
		m_UVVertexBufferID = VB_NONE;
		m_TangVertexBufferID = VB_NONE;
		m_BinormVertexBufferID = VB_NONE;
		m_IndexBufferID = IB_NONE;

		m_NumVertex = 0;
		m_NumIndex = 0;

		m_PrimitiveType = PRIM_TRIANGLES;

		m_Name = "";		
	}

	COLLADASubMesh(std::string name)
	{
		m_pParent = NULL;
		m_LocalTransform = m_LocalTransform.g_Identity;

		m_PosVertexBufferID = VB_NONE;
		m_NormVertexBufferID = VB_NONE;
		m_UVVertexBufferID = VB_NONE;
		m_TangVertexBufferID = VB_NONE;
		m_BinormVertexBufferID = VB_NONE;
		m_IndexBufferID = IB_NONE;

		m_NumVertex = 0;
		m_NumIndex = 0;

		m_PrimitiveType = PRIM_TRIANGLES;

		m_Name = name;
	}

	std::string getName()
	{
		return m_Name;
	}

	void setName(std::string Name)
	{
		m_Name = Name;
	}

	Mat4x4 getLocalTransform()
	{
		return m_LocalTransform;
	}

	void setLocalTransform(Mat4x4 localTransform)
	{
		m_LocalTransform = localTransform;
	}

	Material GetMaterial() { return m_Material; }

	void Remove();

	void Render();
};

class COLLADAMesh
{
	friend class COLLADAMeshResourceLoader;
	friend class MeshNode;

public:
	shared_ptr<Renderer> m_pRenderer;

	COLLADAMesh()
	{
		m_pRenderer = NULL;
	}

	COLLADAMesh(std::string name)
	{
		m_Name = name;
		m_pRenderer = NULL;
	}

	~COLLADAMesh();

	std::string getName()
	{
		return m_Name;
	}

	void setName(std::string name)
	{
		m_Name = name;
	}

	void Render();

	COLLADASubMesh GetSubMesh(UINT i) {	return m_vSubMeshes[i];	}
	void RemoveSubMeshes();

private:
	Aabb m_MeshAabb;
	std::vector<COLLADASubMesh> m_vSubMeshes;
	std::string m_Name;
	
};

class COLLADAMeshResourceLoader : public IResourceLoader
{
private:
	//Dae file
	TiXmlDocument dae;

	//Root node
	TiXmlElement* root;

	//<library_visual_scenes> node
	TiXmlElement* library_visual_scenes;

	//<library_geometries> node
	TiXmlElement* library_geometries;

	//<library_images> node
	TiXmlElement* library_images;

	//<library_materials> node
	TiXmlElement* library_materials;

	//<library_effects> node
	TiXmlElement* library_effects;

	//<library_animations> node
	TiXmlElement* library_animations;

	//Geometries Map
	std::map<std::string, TiXmlElement*> m_GeometriesList;

	//Materials Map
	std::map<std::string, TiXmlElement*> m_MaterialsList;

	//Effects Map
	std::map<std::string, TiXmlElement*> m_EffectsList;

	//Auxiliar mesh values
	std::vector<TempMesh> m_vTempStaticMeshes;

public:
	//Constructor
	COLLADAMeshResourceLoader()
	{
		root = NULL;
		library_visual_scenes = NULL;
		library_geometries = NULL;
	}

	virtual bool VUseRawFile() { return false; }
	virtual bool VDiscardRawBufferAfterLoad() { return true; }
	virtual unsigned int VGetLoadedResourceSize(char *rawBuffer, unsigned int rawSize);
	virtual bool VLoadResource(char *rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle);
	virtual std::string VGetPattern() { return "*.dae"; }

private:
	//Process a <library_visual_scenes> node
	void processVisualScenes();
	
	//Process each Skinned Mesh's <controller> and build a skeleton
	//for them
	//void processSkinnedMeshes(MeshManager* Meshes);

	//Build a Skeleton from a SkinnedMesh's rootJoint <node>
	//void buildSkeleton(SkinnedMesh* Mesh);

	//Process a Joint <node> and it's children recursively
	//void processJoint(SkinnedMesh* Mesh, daeElement* joint);

	//Process a <controller> node
	//void processController(SkinnedMesh* Mesh);

	//Process an element that contains raw inverse bind pose data
	//void processBindPoseArray(vector<Joint> &Joints, daeElement* source);

	//Process an element that contains raw weights data
	//std::vector<float> processWeightsArray(daeElement* source);

	//Process a <library_animations> node
	//void processAnimations(MeshManager* Meshes);

	//Process an <animation> node
	//void processAnimation(Joint* joint, int jointIndex, daeElement* animation);

	//Process a <geometry> node for each mesh
	void processGeometries();

	void processMaterial();

	//Process a <source> node
	void processSources(TempMesh *mesh, std::vector<TiXmlElement*> sources);

	//Process a <source> node for Skinned Mesh
	//void processSource(SkinnedMesh* mesh, daeElement* source);

	//Process a <triangles> node
	void processTriangles(TempMesh *mesh, TiXmlElement* triangles);

	//Process a <triangles> node for Skinned Mesh
	//void processTriangles(SkinnedMesh* mesh, daeElement* triangles);

	//Read a <matrix> node, better to have this as a generalized function, will read into OpenGL style, conversion to DirectX later...
	Mat4x4 processMatrix(TiXmlElement* node);

	void combineStaticComponents(float* &pVertex, UINT* &pIndex, UINT& numVertices, UINT& numIndex, UINT index);
};

//
// class COLLADAMeshResourceExtraData
//
class COLLADAMeshResourceExtraData : public IResourceExtraData
{
	friend class COLLADAMeshResourceLoader;

public:
	COLLADAMeshResourceExtraData() 
	{
		//Nothing
	}

	virtual ~COLLADAMeshResourceExtraData()
	{
		int ref = m_pMesh.use_count();
	}

	virtual std::string VToString()
	{
		return "COLLADAMeshResourceLoader";
	}

	shared_ptr<COLLADAMesh> m_pMesh;
};

struct SDFObjectElement;

class MeshNode : public DrawableNode
{
private:
	WeakBaseRenderComponentPtr m_pRenderComponent;
	std::string m_MeshFileName;
	std::shared_ptr<COLLADAMesh> m_pMesh;

	// Fill Shader
	std::shared_ptr<MaterialNode> m_pMaterialNode;
	std::shared_ptr<ShaderNode> m_vShaderNode;

	//SDF Shadows Attributes
	Vec4 m_vSDFUVOffset;
	Vec4 m_vSDFUVScaleAndVolumeScale;
	Vec4 m_vLocalPositionExtent;
	Mat4x4 m_mWorldToVolume;

	Vec4 m_vSphereBoundCenterAndRadius;

	bool m_bCastSDFShadows;
	bool m_bCastVolumetricShadows;
	bool m_bShadowGeneratedAsTwoSided;
	UINT m_uGPUIndex;

public:
	MeshNode(const EntityId entityId, 
		WeakBaseRenderComponentPtr renderComponent,
		std::string meshFileName, 
		const Mat4x4 *world, const Vec3 vScale, const std::string sMaterialResource, const std::string *shaderFileNames, const std::string sGIMaterialResource, const std::string sGIShaderResource, RenderProperties renderProperties);

	~MeshNode();

	virtual bool VOnRestore(Scene *pScene);
	virtual bool VOnLostDevice(Scene *pScene) { return true; }
	virtual bool VIsVisible(Scene *pScene) const { return this->m_IsVisible; }
    virtual bool VOnRender(Scene *pScene);
    virtual bool VOnSimpleRender(Scene *pScene) override;
	virtual bool OnRenderWithExternalShader(Scene *pScene) override;
    virtual int VGetOrder() override { return m_vShaderNode->m_Shader; };
	
	Vec4 GetSphereBoundCenterAndRadius();
	Vec4 GetSDFUVOffset() { return m_vSDFUVOffset; }
	Vec4 GetSDFUVScaleAndVolumeScale() { return m_vSDFUVScaleAndVolumeScale; }
	Vec4 GetLocalPositionExtent() { return m_vLocalPositionExtent; }
	Mat4x4 GetWorldToVolumeMatrix();
	std::string GetMeshFileName() { return m_MeshFileName; }
	bool CastSDFShadows() { return m_bCastSDFShadows; }
	bool ShadowGeneratedAsTwoSided() { return m_bShadowGeneratedAsTwoSided; }
	bool IsOpaque() { return m_pMaterialNode->IsOpaque(); }

	void SetGPUIndex(UINT index) { m_uGPUIndex = index; }

	virtual SceneNodeType VGetType() { return MESH_NODE; }
};



