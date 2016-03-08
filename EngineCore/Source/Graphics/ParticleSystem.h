#pragma once

//========================================================================
// ParticleSystem.h - defines the class for render particles in
//							the scene.                  
//
//========================================================================

#include "Geometry.h"
#include "..\ResourceCache\ResCache.h"
#include "..\Entities\RenderComponent.h"

struct Particle
{
    Mat4x4 mParticleWorld;
    int iDead;
    int iPadding[3];
};

struct ParticleExtraData
{
    Vec3 vForce;
    Vec3 vVelocity;
	Vec3 vPosition;
    float fScale;
    float fRotation;
    float fAngularSpeed;
    float fLifetime;
};

class ParticleSystemNode;

class ParticleSystem
{
private:
	std::vector<bool> lBurstShoot;

    ParticleSystemProperties m_Properties;
    Emission m_EmissionProperties;
    std::shared_ptr<ParticlesSourceShape> m_SourceShapeProperties;
    VectorialProperties m_ForceOverLifetimeProperties;
    VectorialProperties m_VelocityOverLifetimeProperties;
    ScalarProperties m_SizeOverLifetimeProperties;
    ScalarProperties m_RotationOverLifetimeProperties;
    ParticleSystemRenderProperties m_RenderProperties;

    std::vector<Particle> m_lParticleList;
    std::vector<ParticleExtraData> m_lParticleExtraDataList;

    float m_fParticleSystemTime;
	Aabb m_ParticleSystemAabb;

    bool m_bHasInit;
    UINT m_uParticleIndex;

	TextureID m_ParticleSystemTextureID;

	friend class ParticleSystemNode;

public:
	ParticleSystem(ParticleSystemProperties properties, Emission emissionProperties, std::shared_ptr<ParticlesSourceShape> sourceShapeProperties,
        VectorialProperties forceOverLifetimeProperties, VectorialProperties velocityOverLifetimeProperties,
        ScalarProperties sizeOverLifetimeProperties, ScalarProperties rotationOverLifetimeProperties, ParticleSystemRenderProperties renderProperties);

    void GenerateParticles(float fDelta);
    void UpdateParticles(float fDelta);
	void Update(float fDelta);
};

class ParticleSystemNode : public DrawableNode
{
private:
	WeakBaseRenderComponentPtr m_pRenderComponent;
	
	std::shared_ptr<ParticleSystem> m_pParticleSystem;

public:
	ParticleSystemNode(const EntityId entityId,
		WeakBaseRenderComponentPtr renderComponent, const Mat4x4 *world,
		ParticleSystemProperties properties, Emission emissionProperties, std::shared_ptr<ParticlesSourceShape> sourceShapeProperties,
		VectorialProperties forceOverLifetimeProperties, VectorialProperties velocityOverLifetimeProperties,
        ScalarProperties sizeOverLifetimeProperties, ScalarProperties rotationOverLifetimeProperties, ParticleSystemRenderProperties renderProperties);

	~ParticleSystemNode();

	virtual bool VOnRestore(Scene *pScene);
	virtual bool VOnLostDevice(Scene *pScene) { return true; }
	virtual bool VIsVisible(Scene *pScene) const { return this->m_IsVisible; }
	virtual bool VOnRender(Scene *pScene);
	virtual bool VOnUpdate(Scene *pScene, DWORD const elapsedMs);

	virtual SceneNodeType VGetType() { return MESH_NODE; }
};