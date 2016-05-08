#pragma once

//========================================================================
// EntityResource.h - Defines the classes for help us to load entity files.                  
//					Improve the performance of the entity loads.
//========================================================================

#include "..\Msvc\CthughaStd.h"
#include "..\ResourceCache\ResCache.h"

class EntityResourceLoader;

struct Burst
{
	float fTime;
	UINT uParticlesEmitted;
};

// Component Structs ///////////////////////////////////////////////////////

// ComponentResourceData
class ComponentResourceData
{
public:
	ComponentResourceData() { }
};

// MeshRenderComponent
class MeshRenderComponentResourceData : public ComponentResourceData
{
private:
	MeshRenderComponentResourceData() { }

	friend class EntityResourceLoader;

public:
	std::string sMeshResourceName;
	std::string sShaderFillName;
	std::string sMaterialResourceName;
	std::string sGIShaderResource;
	std::string sGIMaterialResource;
	bool bGlobalIllumination;
	bool bCastShadow;
	bool bCastVolumetricShadow;
    bool bParallaxMapping;
	bool bStaticNode;
	bool bShadowGeneratedAsTwoSided;
};

// LightRenderComponent
class LightRenderComponentResourceData : public ComponentResourceData
{
private:
	LightRenderComponentResourceData() { }

	friend class EntityResourceLoader;

public:
	bool bStaticNode;
	std::string sLightType;

	//Common Light Properties
	Vec4 vLightColor;
	float fIntensitive;
	bool bGenerateShadow;
	bool bGenerateVolumetricShadows;

	//Directional Light Properties
	Vec4 vDirection;
	float fMaxRadius;
	float fMaxLength;

	//Point and Spot Light Properties
	float fAttenuationRadius;

	//Spot Light Properties
	float fThetaAngle;
	float fPhiAngle;
};

// DecalRenderComponent
class DecalRenderComponentResourceData : public ComponentResourceData
{
private:
	DecalRenderComponentResourceData() { }

	friend class EntityResourceLoader;

public:
	bool bStaticNode;

	std::string sShaderResource;
	std::string sMaterialResource;

	UINT uMaskId;
};

// PhysicComponent
class PhysicComponentResourceData : public ComponentResourceData
{
private:
	PhysicComponentResourceData();

	friend class EntityResourceLoader;

public:
	std::string sDensity;
	std::string sMaterial;

	//Constrains
	Vec3 vRigidBodyTranslationConstrain;
	Vec3 vRigidBodyRotationConstrain;
	bool bRigidBodyTranslationConstrain;
	bool bRigidBodyRotationConstrain;

	//Offsets
	Vec3 vRigidBodyTranslation;
	Vec3 vRigidBodyRotation;
	Vec3 VRigidBodyScale;
	bool bRigidBodyTranslation;
	bool bRigidBodyRotation;
	bool bRigidBodyScale;

	//Shape
	std::string sShapeType;
	bool bGhostObject;

	//Common Sphere, Box and Capsule
	Vec3 vShapeOffset;

	//Sphere
	float fSphereRadius;

	//Box
	Vec3 vBoxDimensions; //(Width, Height, Depth)

	//Capsule
	float fCapsuleRadius;
	float fCapsuleHeight;

	//Convex Hull and Triangle Mesh
	std::string sShapeResourceName;

	//Terrain
	std::string m_sHeightMapResource;
	int m_iWidth;
	int m_iDepth;
	float m_fMaxHeight;
	float m_fMinHeight;
	float m_fWorldWidth;
	float m_fWorldDepth;
	std::string m_sUpAxis;
	std::string m_sHeightMapValueType;
};

class ParticleSystemComponentResourceData : public ComponentResourceData
{
private:
	ParticleSystemComponentResourceData() { }

	friend class EntityResourceLoader;

public:
	bool bStaticNode;

	//Particle System Properties
	float fDuration;
	float bLooping;
	float fStartDelay;
	float fStartLifetime;
	float fStartSpeed;
	float fStartScale;
	float fStartRotation;
	Vec4 vStartColor;
	float fGravity;
	UINT uMaxParticles;

	//Emission Properties
	bool bEmissionActive;
	float fRate;
	std::vector<Burst> lBursts;

	// Shape Source Properties ////////////////////////////////////
	//Common Properties
	bool bShapeActive;
	std::string sShapeType;
	bool bFromShell;
	bool bRandomDirection;

	//Sphere and Hemisphere
	float fRadius;

	//Cone
	float fConeRadius;
	float fAngle;
	float fLength;
	std::string sEmitFrom;

	//Box
	Vec3 vBoxDimensions;
	///////////////////////////////////////////////////////////////

	//Force over Lifetime
	bool bForceOverLifetimeActive;
	std::string sForceValueType;
	Vec3 vForceValue1;
	Vec3 vForceValue2;

	//Velocity over Lifetime
	bool bVeloctiyOverLifetimeActive;
	std::string sVelocityValueType;
	Vec3 vVelocityValue1;
	Vec3 vVelocityValue2;

	//Rotation over Lifetime
	bool bRotationOverLifetimeActive;
	float fRotationValue1;
	float fRotationValue2;

	//Size over Lifetime
	bool bSizeOverLifetimeActive;
	float fSizeValue1;
	float fSizeValue2;

	//Texture
	std::string sParticleTextureResourceName;
};

// TransformComponent
class TransformComponentResourceData : public ComponentResourceData
{
private:
	TransformComponentResourceData() { }
	
	friend class EntityResourceLoader;

public:
	TransformComponentResourceData(Vec3 scale, Vec3 position, Vec3 rotation) { vScale = scale; vPosition = position; vRotation = rotation; }

	Vec3 vScale;
	Vec3 vPosition;
	Vec3 vRotation;
};

// PlacementComponent
class PlacementComponentResourceData : public ComponentResourceData
{
private:
	PlacementComponentResourceData() { }

	friend class EntityResourceLoader;

public:
	float fOverlapLevel;
	float fOverlapRadius;
	Vec2 vScale;
	Vec2 vRotation;
};

////////////////////////////////////////////////////////////////////////////

//
//	Auxiliar classes
/////////////////////////////////////////////////////////////////////////////

// ComponentResource class
class ComponentResource
{
private:
	std::string m_sName;
	std::string m_sXMLComponent;
	std::shared_ptr<ComponentResourceData> m_pComponentResourceData;

	friend class EntityResourceLoader;

public:
	ComponentResource(std::string name) { m_sName = name; }
	~ComponentResource() { }

	void SetComponentResourceData(std::shared_ptr<ComponentResourceData> data) { m_pComponentResourceData = data; }
	std::shared_ptr<ComponentResourceData> GetComponentResourceData() { return m_pComponentResourceData; }
	std::string GetName() { return m_sName; }
};

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
//	EntityResourceExtraData class definition
//
//////////////////////////////////////////////////////////////////////////
class EntityResourceExtraData : public IResourceExtraData
{
private:
	std::string m_sResourceName;
	std::string m_sEntityType;
	std::map<std::string, std::shared_ptr<ComponentResource>> m_lComponentResourceList;

	void AddComponentResource(std::string componentName, std::shared_ptr<ComponentResource> componentResource);

	friend class EntityResourceLoader;

public:
	EntityResourceExtraData(std::string resourceName) { m_sResourceName = resourceName; }
	~EntityResourceExtraData() { }

	std::shared_ptr<ComponentResource> FindComponentResource(std::string name);

	std::string GetResourceName() { return m_sResourceName; }
	std::string GetEntityType() { return m_sEntityType; }
	std::map<std::string, std::shared_ptr<ComponentResource>>::iterator GetFirstComponent() { return m_lComponentResourceList.begin(); }
	std::map<std::string, std::shared_ptr<ComponentResource>>::iterator GetLastComponent() { return m_lComponentResourceList.end(); }

	virtual std::string VToString() { return "EntityResourceExtraData"; }
};

//////////////////////////////////////////////////////////////////////////
//
//	EntityResourceLoader class definition
//
//////////////////////////////////////////////////////////////////////////
class EntityResourceLoader : public IResourceLoader
{
private:
    //Specific Component Loaders
    std::shared_ptr<ComponentResourceData> LoadMeshRenderComponent(TiXmlElement* pData);
	std::shared_ptr<ComponentResourceData> LoadDecalRenderComponent(TiXmlElement* pData);
    std::shared_ptr<ComponentResourceData> LoadLightRenderComponent(TiXmlElement* pData);
    std::shared_ptr<ComponentResourceData> LoadTransformComponent(TiXmlElement* pData);
    std::shared_ptr<ComponentResourceData> LoadPhysicComponent(TiXmlElement* pData);
    std::shared_ptr<ComponentResourceData> LoadPlacementComponent(TiXmlElement* pData);
	std::shared_ptr<ComponentResourceData> LoadParticleSystemComponent(TiXmlElement* pData);

public:
	virtual bool VUseRawFile() { return false; }
	virtual bool VDiscardRawBufferAfterLoad() { return true; }
	virtual unsigned int VGetLoadedResourceSize(char *rawBuffer, unsigned int rawSize) { return rawSize; }
	virtual bool VLoadResource(char *rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle);
	virtual std::string VGetPattern() { return "*.ent"; }
};