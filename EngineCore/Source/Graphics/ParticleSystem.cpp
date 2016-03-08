//========================================================================
// ParticleSystem.cpp : implement ParticleSystem class
//
//========================================================================

#include "CthughaStd.h"

#include "ParticleSystem.h"
#include "Material.h"

//	ParticleSystem class implementation

ParticleSystem::ParticleSystem(ParticleSystemProperties properties, Emission emissionProperties, std::shared_ptr<ParticlesSourceShape> sourceShapeProperties,
	VectorialProperties forceOverLifetimeProperties, VectorialProperties velocityOverLifetimeProperties,
    ScalarProperties sizeOverLifetimeProperties, ScalarProperties rotationOverLifetimeProperties, ParticleSystemRenderProperties renderProperties)
{
	m_Properties = properties;
	m_EmissionProperties = emissionProperties;
	m_SourceShapeProperties = sourceShapeProperties;
	m_ForceOverLifetimeProperties = forceOverLifetimeProperties;
	m_VelocityOverLifetimeProperties = velocityOverLifetimeProperties;
	m_SizeOverLifetimeProperties = sizeOverLifetimeProperties;
	m_RotationOverLifetimeProperties = rotationOverLifetimeProperties;
	m_RenderProperties = renderProperties;

	lBurstShoot.resize(emissionProperties.lBursts.size());

	for (auto burst : lBurstShoot)
		burst = false;

	m_fParticleSystemTime = 0.0f;
	m_bHasInit = false;
	m_uParticleIndex = 0;

	m_ParticleSystemAabb.isVoid = true;

	m_lParticleList.resize(m_Properties.uMaxParticles);
	m_lParticleExtraDataList.resize(m_Properties.uMaxParticles);

	//Load Particle Texture
	Resource ParticleTextureResource(renderProperties.sTextureSource);
	shared_ptr<ResHandle> ParticleTextureResourceHandle = g_pApp->m_ResCache->GetHandle(&ParticleTextureResource);

	if (!ParticleTextureResourceHandle)
	{
		CHG_ERROR("Not found SSAO PostProcessing Shader");
	}

	m_ParticleSystemTextureID = ((TextureResourceExtraData*)ParticleTextureResourceHandle->GetExtra().get())->GetTexture();
}

void ParticleSystem::GenerateParticles(float fDelta)
{
	CHGRandom random = g_pApp->m_pGame->GetRNG();

    if (m_fParticleSystemTime < m_Properties.fStartDelay && !m_bHasInit)
        return;

    m_bHasInit = true;

    UINT uNumParticles = (UINT)(fDelta*m_EmissionProperties.fRate);

	for (UINT i = 0; i < m_EmissionProperties.lBursts.size(); i++)
    {
		if (m_EmissionProperties.lBursts[i].fTime < m_fParticleSystemTime && !lBurstShoot[i])
		{
			uNumParticles += m_EmissionProperties.lBursts[i].uParticlesEmitted;
			lBurstShoot[i] = true;
		}            
    }

    for (UINT i = 0; i < uNumParticles; i++)
    {
		ParticleExtraData particleExtraData;
		Particle particle;

		//Position and velocity
		
		Vec3 vExtraVelocity;

		if (m_VelocityOverLifetimeProperties.valueType == RANDOM_BETWEN_TWO_CONSTANTS)
			vExtraVelocity = m_VelocityOverLifetimeProperties.vValue1 + (m_VelocityOverLifetimeProperties.vValue2 - m_VelocityOverLifetimeProperties.vValue1)*random.Random();
		else
			vExtraVelocity = m_VelocityOverLifetimeProperties.vValue1;

		if (m_SourceShapeProperties->GetType() == SPHERE_SOURCE)
		{
			std::shared_ptr<ParticlesSphereSource> sphereSource = std::static_pointer_cast<ParticlesSphereSource>(m_SourceShapeProperties);			

			float fAlpha = random.Random()*CHG_2PI;
			float fBeta = random.Random()*CHG_2PI;

			if (!sphereSource->EmitInRandomDirection())
				particleExtraData.vVelocity = Vec3(cos(fAlpha)*sin(fBeta), cos(fBeta), sin(fAlpha)*sin(fAlpha))*m_Properties.fStartSpeed + vExtraVelocity;				
			else
				particleExtraData.vVelocity = normalize(Vec3(random.Random(), random.Random(), random.Random()))*m_Properties.fStartSpeed + vExtraVelocity;

			if (m_SourceShapeProperties->EmitFromShell())
				particleExtraData.vPosition = Vec3(cos(fAlpha)*sin(fBeta), cos(fBeta), sin(fAlpha)*sin(fAlpha))*sphereSource->GetRadius();
			else
				particleExtraData.vPosition = Vec3(cos(fAlpha)*sin(fBeta), cos(fBeta), sin(fAlpha)*sin(fAlpha))*sphereSource->GetRadius()*random.Random();
		}
		else if (m_SourceShapeProperties->GetType() == HEMISPHERE_SOURCE)
		{
			ParticlesHemisphereSource hemisphereSource = *(ParticlesHemisphereSource*)&m_SourceShapeProperties;

			float fAlpha = random.Random()*CHG_2PI;
			float fBeta = random.Random()*CHG_PI;

			if (!hemisphereSource.EmitInRandomDirection())
				particleExtraData.vVelocity = Vec3(cos(fAlpha)*sin(fBeta), cos(fBeta), sin(fAlpha)*sin(fAlpha))*m_Properties.fStartSpeed + vExtraVelocity;
			else
				particleExtraData.vVelocity = normalize(Vec3(random.Random(), random.Random(), random.Random()))*m_Properties.fStartSpeed + vExtraVelocity;

			if (m_SourceShapeProperties->EmitFromShell())
				particleExtraData.vPosition = Vec3(cos(fAlpha)*sin(fBeta), cos(fBeta), sin(fAlpha)*sin(fAlpha))*hemisphereSource.GetRadius();
			else
				particleExtraData.vPosition = Vec3(cos(fAlpha)*sin(fBeta), cos(fBeta), sin(fAlpha)*sin(fAlpha))*hemisphereSource.GetRadius()*random.Random();
		}
		else if (m_SourceShapeProperties->GetType() == CONE_SOURCE)
		{
			ParticlesConeSource coneSource = *(ParticlesConeSource*)&m_SourceShapeProperties;

			float fHeight = coneSource.GetRadius()/tan(coneSource.GetAngle());
            
			float fAlpha = random.Random()*CHG_2PI;
			float fBeta = random.Random()*CHG_PI;

			if (coneSource.GetSourceConeType() == BASE_SOURCE)
			{
				particleExtraData.vPosition = Vec3(sin(fAlpha), 0.0f, cos(fBeta))*coneSource.GetRadius();
				
				if (!m_SourceShapeProperties->EmitFromShell())
					particleExtraData.vPosition *= random.Random();
			}
			else
			{
                float pendient = coneSource.GetRadius() / fHeight;

                particleExtraData.vPosition = Vec3(sin(fAlpha), 0.0f, cos(fBeta))*pendient*(fHeight + random.Random()*coneSource.GetLength());

                if (!m_SourceShapeProperties->EmitFromShell())
                {
                    float fRandom = random.Random();
                    particleExtraData.vPosition.x *= fRandom;
                    particleExtraData.vPosition.z *= fRandom;
                }					
			}

            Vec3 vWorldCenter = Vec3(0.0f, -fHeight, 0.0f);

            if (!coneSource.EmitInRandomDirection())
                particleExtraData.vVelocity = normalize(particleExtraData.vPosition - vWorldCenter)*m_Properties.fStartSpeed + vExtraVelocity;
            else
                particleExtraData.vVelocity = normalize(Vec3(random.Random(), random.Random(), random.Random()))*m_Properties.fStartSpeed + vExtraVelocity;
		}
		else if (m_SourceShapeProperties->GetType() == BOX_SOURCE)
		{
			ParticlesBoxSource boxSource = *(ParticlesBoxSource*)&m_SourceShapeProperties;

            particleExtraData.vPosition = (2.0f*Vec3(random.Random(), random.Random(), random.Random()) - 1.0f)*boxSource.GetBoxDimensions();

            if (!boxSource.EmitInRandomDirection())
                particleExtraData.vVelocity = Vec3(0.0f, 1.0f, 0.0f)*m_Properties.fStartSpeed + vExtraVelocity;
            else
                particleExtraData.vVelocity = normalize(Vec3(random.Random(), random.Random(), random.Random()))*m_Properties.fStartSpeed + vExtraVelocity;
		}

        particle.iDead = false;
        
        if (m_ForceOverLifetimeProperties.valueType == RANDOM_BETWEN_TWO_CONSTANTS)
            particleExtraData.vForce = m_ForceOverLifetimeProperties.vValue1 + (m_ForceOverLifetimeProperties.vValue2 - m_ForceOverLifetimeProperties.vValue1)*random.Random() + Vec3(0.0f, m_Properties.fGravity, 0.0f);
        else
            particleExtraData.vForce = m_ForceOverLifetimeProperties.vValue1 + Vec3(0.0f, m_Properties.fGravity, 0.0f);

		particleExtraData.fScale = m_Properties.fStartScale * m_SizeOverLifetimeProperties.fValue1 + (m_SizeOverLifetimeProperties.fValue2 - m_SizeOverLifetimeProperties.fValue1)*random.Random();
        particleExtraData.fAngularSpeed = m_RotationOverLifetimeProperties.fValue1 + (m_RotationOverLifetimeProperties.fValue2 - m_RotationOverLifetimeProperties.fValue1)*random.Random();

        particleExtraData.fLifetime = 0.0f;
        particleExtraData.fRotation = m_Properties.fStartRotation;

        m_lParticleList[(m_uParticleIndex++) % m_Properties.uMaxParticles] = particle;
        m_lParticleExtraDataList[(m_uParticleIndex++) % m_Properties.uMaxParticles] = particleExtraData;
    }
}

void ParticleSystem::UpdateParticles(float fDelta)
{
    for (UINT i = 0; i < m_lParticleExtraDataList.size(); i++)
    {
        if (m_lParticleExtraDataList[i].fLifetime < m_Properties.fStartLifetime || m_lParticleList[i].iDead)
        {
            m_lParticleExtraDataList[i].vVelocity += m_lParticleExtraDataList[i].vForce*fDelta;
            m_lParticleExtraDataList[i].vPosition += m_lParticleExtraDataList[i].vVelocity*fDelta;
            m_lParticleExtraDataList[i].fRotation += m_lParticleExtraDataList[i].fAngularSpeed*fDelta;
            m_lParticleExtraDataList[i].fLifetime += fDelta;

			m_ParticleSystemAabb.Union(m_lParticleExtraDataList[i].vPosition);

            m_lParticleList[i].mParticleWorld = translate(m_lParticleExtraDataList[i].vPosition) * rotateZ(m_lParticleExtraDataList[i].fRotation) * scale(m_lParticleExtraDataList[i].fScale, m_lParticleExtraDataList[i].fScale, m_lParticleExtraDataList[i].fScale);
        }
        else
        {
            m_lParticleList[i].iDead = true;
        }
    }
}

void ParticleSystem::Update(float fDelta)
{
	if (m_Properties.bLooping && m_fParticleSystemTime <= m_Properties.fDuration)
		GenerateParticles(fDelta);

	UpdateParticles(fDelta);

	m_fParticleSystemTime += fDelta;

	if (m_fParticleSystemTime > m_Properties.fDuration)
	{
		m_fParticleSystemTime = m_fParticleSystemTime - m_Properties.fDuration;

		for (auto burst : lBurstShoot)
			burst = false;
	}
}

////////////////////////////////////////////////////
// ParticleSystemNode Implementation
////////////////////////////////////////////////////

ParticleSystemNode::ParticleSystemNode(const EntityId entityId,
	WeakBaseRenderComponentPtr renderComponent, const Mat4x4 *world,
	ParticleSystemProperties properties, Emission emissionProperties, std::shared_ptr<ParticlesSourceShape> sourceShapeProperties,
	VectorialProperties forceOverLifetimeProperties, VectorialProperties velocityOverLifetimeProperties,
    ScalarProperties sizeOverLifetimeProperties, ScalarProperties rotationOverLifetimeProperties, ParticleSystemRenderProperties renderProperties)
{
	m_pParticleSystem.reset(CHG_NEW ParticleSystem(properties, emissionProperties, sourceShapeProperties,
		forceOverLifetimeProperties, velocityOverLifetimeProperties,
		sizeOverLifetimeProperties, rotationOverLifetimeProperties, renderProperties));

	m_EntityID = entityId;
	m_IsVisible = true;
	m_pRenderComponent = renderComponent;
	m_Aabb = m_pParticleSystem->m_ParticleSystemAabb;

	m_InQueueFrustum = false;
	m_InQueueLight = false;
	m_InQueueRender = false;

	m_ToWorld = *world;
	m_FromWorld = m_ToWorld.inverse();

	m_bHasShadowShader = false;
	m_bHasGIShader = false;

	m_bIsDrawable = true;
}

ParticleSystemNode::~ParticleSystemNode()
{
}

bool ParticleSystemNode::VOnRestore(Scene *pScene)
{
	SceneNode::VOnRestore(pScene);

	return true;
}

bool ParticleSystemNode::VOnRender(Scene *pScene)
{
	return true;
}

bool ParticleSystemNode::VOnUpdate(Scene* pScene, DWORD elapsedTimeMs)
{
	float fDelta = elapsedTimeMs / 1000.0f;

	m_pParticleSystem->Update(fDelta);

	return true;
}