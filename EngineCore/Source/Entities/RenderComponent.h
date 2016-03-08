#pragma once
//========================================================================
// RenderComponent.h : classes that define renderable components of actors like Meshes, Skyboxes, Lights, etc.
//
// Part of the GameCode4 Application
//========================================================================

#include "..\Graphics\SceneNodes.h"
#include "..\Graphics\Light.h"
#include "RenderComponentInterface.h"

//---------------------------------------------------------------------------------------------------------------------
// RenderComponent base class.  This class does most of the work except actually creating the scene, which is 
// delegated to the subclass through a factory method:
// http://en.wikipedia.org/wiki/Factory_method_pattern
//---------------------------------------------------------------------------------------------------------------------
class BaseRenderComponent : public RenderComponentInterface
{
protected:
    Color m_color;
	bool m_bStaticNode;
    shared_ptr<SceneNode> m_pSceneNode;

public:
    virtual bool VInit(TiXmlElement* pData) override;
	virtual bool VInit(std::shared_ptr<ComponentResource> pData) override { return true; }
    virtual void VPostInit(void) override;
	virtual void VOnChanged(void) override;
    virtual TiXmlElement* VGenerateXml(void) override;
	const Color GetColor() const { return m_color; }
	bool IsStatic() { return m_bStaticNode; }

	shared_ptr<SceneNode> GetSceneNode()
	{
		if (m_pSceneNode)
			return m_pSceneNode;

		return std::shared_ptr<SceneNode>();
	}

protected:
    // loads the SceneNode specific data (represented in the <SceneNode> tag)
    virtual bool VDelegateInit(TiXmlElement* pData) { return true; }
    virtual shared_ptr<SceneNode> VCreateSceneNode(void) = 0;  // factory method to create the appropriate scene node
    Color LoadColor(TiXmlElement* pData);

    // editor stuff
	virtual TiXmlElement* VCreateBaseElement(void) { return CHG_NEW TiXmlElement(VGetName()); }
    virtual void VCreateInheritedXmlElements(TiXmlElement* pBaseElement) = 0;

private:
    virtual shared_ptr<SceneNode> VGetSceneNode(void) override;
};


//---------------------------------------------------------------------------------------------------------------------
// This class represents a render component built from a Mesh.  In a real game, this is the one you'll use 99% of the 
// time towards the end of the project.  The other classes are important for testing since programming tends to move 
// a lot faster than art in the early stages of development.
//---------------------------------------------------------------------------------------------------------------------
class MeshRenderComponent : public BaseRenderComponent
{
public:
	static const char *g_Name;

	virtual const char *VGetName() const { return g_Name; }
	virtual bool VInit(TiXmlElement* pData) override;
	virtual bool VInit(std::shared_ptr<ComponentResource> pData) override;

	MeshRenderComponent(void) 
	{
		m_MeshResourceName = "";
		m_FillShaderName = "";
		m_MaterialResourceName = "";
		m_GIShaderResource = "";
		m_GIMaterialResource = "";
	};

protected:
	std::string m_MeshResourceName;

	std::string m_FillShaderName;
	std::string m_MaterialResourceName;

	std::string m_GIShaderResource;
	std::string m_GIMaterialResource;

	RenderProperties m_RenderProperties;

    virtual shared_ptr<SceneNode> VCreateSceneNode(void) override;  // factory method to create the appropriate scene node
    
	// editor stuff
	virtual void VCreateInheritedXmlElements(TiXmlElement* pBaseElement);
};


//---------------------------------------------------------------------------------------------------------------------
// Lights
//---------------------------------------------------------------------------------------------------------------------
class LightRenderComponent : public BaseRenderComponent
{
	shared_ptr<Light> m_pLight;

public:
	static const char *g_Name;
	virtual const char *VGetName() const { return g_Name; }

    LightRenderComponent(void);

protected:
    virtual bool VInit(TiXmlElement* pData) override;
	virtual bool VInit(std::shared_ptr<ComponentResource> pData) override;
    virtual shared_ptr<SceneNode> VCreateSceneNode(void) override;  // factory method to create the appropriate scene node

    // editor stuff
    virtual void VCreateInheritedXmlElements(TiXmlElement* pBaseElement);
};

//---------------------------------------------------------------------------------------------------------------------
// Decals
//---------------------------------------------------------------------------------------------------------------------

class DecalRenderComponent : public BaseRenderComponent
{
	std::string m_sShaderResource;
	std::string m_sMaterialResource;
	UINT m_uMaskId;

public:
	static const char *g_Name;
	virtual const char *VGetName() const { return g_Name; }

	DecalRenderComponent(void);

protected:
	virtual bool VInit(TiXmlElement* pData) override;
	virtual bool VInit(std::shared_ptr<ComponentResource> pData) override;
	virtual shared_ptr<SceneNode> VCreateSceneNode(void) override;  // factory method to create the appropriate scene node

	// editor stuff
	virtual void VCreateInheritedXmlElements(TiXmlElement* pBaseElement);
};

//---------------------------------------------------------------------------------------------------------------------
// Particle System
//---------------------------------------------------------------------------------------------------------------------

enum SourceShapeType { SPHERE_SOURCE, HEMISPHERE_SOURCE, CONE_SOURCE, BOX_SOURCE };
enum SourceConeType { BASE_SOURCE, VOLUME_SOURCE };
enum ValueType { CONSTANT, RANDOM_BETWEN_TWO_CONSTANTS };

struct ParticleSystemProperties
{
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
};

struct ParticleSystemRenderProperties
{
	std::string sTextureSource;
	bool bCastShadows;
	bool bReceiveShadows;
};

struct Emission
{
	bool bActive;
	float fRate;

	std::vector<Burst> lBursts;
};

struct ScalarProperties
{
	bool bActive;
	float fValue1;
	float fValue2;
};

struct VectorialProperties
{
	bool bActive;
	ValueType valueType;
	Vec3 vValue1;
	Vec3 vValue2;
};

// Source Types //////////////////////////////////////////////////////////////////////

class ParticlesSourceShape
{
protected:
	SourceShapeType m_SourceType;
	bool m_bFromShell;
	bool m_bRandomDirection;
	bool m_bActive;

public:
	ParticlesSourceShape() { m_SourceType = CONE_SOURCE; m_bFromShell = false; }

	SourceShapeType GetType() { return m_SourceType; }
	bool EmitFromShell() { return m_bFromShell; }
	bool EmitInRandomDirection() { return m_bRandomDirection; }
	bool IsActive() { return m_bActive; }
	void SetActive(bool bActive) { m_bActive = bActive; }
};

class ParticlesSphereSource : public ParticlesSourceShape
{
	float m_fRadius;

public:
	ParticlesSphereSource()
	{
		m_fRadius = 1.0f;
		m_bFromShell = false;
		m_bRandomDirection = false;

		m_SourceType = SPHERE_SOURCE;
	}

	ParticlesSphereSource(float fRadius, bool bFromShell, bool bRandomDirection)
	{
		m_fRadius = fRadius;
		m_bFromShell = bFromShell;
		m_bRandomDirection = bRandomDirection;

		m_SourceType = SPHERE_SOURCE;
	}

	float GetRadius() { return m_fRadius; }
};

class ParticlesHemisphereSource : public ParticlesSourceShape
{
	float m_fRadius;

public:
	ParticlesHemisphereSource()
	{
		m_fRadius = 1.0f;
		m_bFromShell = false;
		m_bRandomDirection = false;

		m_SourceType = HEMISPHERE_SOURCE;
	}

	ParticlesHemisphereSource(float fRadius, bool bFromShell, bool bRandomDirection)
	{
		m_fRadius = fRadius;
		m_bFromShell = bFromShell;
		m_bRandomDirection = bRandomDirection;

		m_SourceType = HEMISPHERE_SOURCE;
	}

	float GetRadius() { return m_fRadius; }
};

class ParticlesConeSource : public ParticlesSourceShape
{
	float m_fRadius;
	float m_fAngle;
	float m_fLength;
	SourceConeType m_SourceConeType;

public:
	ParticlesConeSource()
	{
		m_fRadius = 0.0f;
		m_fAngle = CHG_PI/4.0f;
		m_fLength = 1.0f;
		m_SourceConeType = BASE_SOURCE;
		m_bFromShell = false;
		m_bRandomDirection = false;

		m_SourceType = CONE_SOURCE;
	}

	ParticlesConeSource(float fRadius, float fAngle, float fLength, SourceConeType sourceConeType, bool bFromShell, bool bRandomDirection)
	{
		m_fRadius = fRadius;
		m_fAngle = fAngle;
		m_fLength = fLength;
		m_SourceConeType = sourceConeType;
		m_bFromShell = bFromShell;
		m_bRandomDirection = bRandomDirection;

		m_SourceType = CONE_SOURCE;
	}

	float GetRadius() { return m_fRadius; }
	float GetAngle() { return m_fAngle; }
	float GetLength() { return m_fLength; }
	SourceConeType GetSourceConeType() { return m_SourceConeType; }
};

class ParticlesBoxSource : public ParticlesSourceShape
{
	Vec3 m_vBoxDimensions;

public:
	ParticlesBoxSource()
	{
		m_vBoxDimensions = Vec3(1.0f, 1.0f, 1.0f);
		m_bFromShell = false;
		m_bRandomDirection = false;

		m_SourceType = BOX_SOURCE;
	}

	ParticlesBoxSource(Vec3 vBoxDimensions, bool bFromShell, bool bRandomDirection)
	{
		m_vBoxDimensions = vBoxDimensions;
		m_bFromShell = bFromShell;
		m_bRandomDirection = bRandomDirection;

		m_SourceType = BOX_SOURCE;
	}

	Vec3 GetBoxDimensions() { return m_vBoxDimensions; }
};

///////////////////////////////////////////////////////////////////////////////////////

class ParticleSystemComponent : public BaseRenderComponent
{
	ParticleSystemProperties m_Properties;
	Emission m_EmissionProperties;
	std::shared_ptr<ParticlesSourceShape> m_SourceShapeProperties;
	VectorialProperties m_ForceOverLifetimeProperties;
	VectorialProperties m_VelocityOverLifetimeProperties;
	ScalarProperties m_SizeOverLifetimeProperties;
	ScalarProperties m_RotationOverLifetimeProperties;
    ParticleSystemRenderProperties m_RenderProperties;

public:
	static const char *g_Name;
	virtual const char *VGetName() const { return g_Name; }

	ParticleSystemComponent(void) { }

protected:
	virtual bool VInit(TiXmlElement* pData) override;
	virtual bool VInit(std::shared_ptr<ComponentResource> pData) override;
	virtual shared_ptr<SceneNode> VCreateSceneNode(void) override;  // factory method to create the appropriate scene node

	// editor stuff
    virtual void VCreateInheritedXmlElements(TiXmlElement* pBaseElement){}
};
