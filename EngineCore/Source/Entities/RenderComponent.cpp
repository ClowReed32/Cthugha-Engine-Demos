#pragma once
//========================================================================
// RenderComponent.cpp : classes that define renderable components of actors like Meshes, Skyboxes, Lights, etc.
//
//========================================================================

#include "CthughaStd.h"

#include "..\Graphics\ParticleSystem.h"
#include "..\Graphics\Mesh.h"
#include "..\Graphics\Decal.h"
#include "..\Graphics\Shader.h"
#include "..\Graphics\Material.h"

#include "..\Utilities\String.h"

#include "..\EventManager\Events.h"

#include "RenderComponent.h"
#include "TransformComponent.h"

const char* MeshRenderComponent::g_Name = "MeshRenderComponent";
const char* LightRenderComponent::g_Name = "LightRenderComponent";
const char* ParticleSystemComponent::g_Name = "ParticleSystemComponent";
const char* DecalRenderComponent::g_Name = "DecalRenderComponent";
/*const char* SphereRenderComponent::g_Name = "SphereRenderComponent";
const char* TeapotRenderComponent::g_Name = "TeapotRenderComponent";
const char* GridRenderComponent::g_Name = "GridRenderComponent";
const char* SkyRenderComponent::g_Name = "SkyRenderComponent";*/

//---------------------------------------------------------------------------------------------------------------------
// RenderComponent
//---------------------------------------------------------------------------------------------------------------------
bool BaseRenderComponent::VInit(TiXmlElement* pData)
{
    // color
    TiXmlElement* pColorNode = pData->FirstChildElement("Color");
    if (pColorNode)
        m_color = LoadColor(pColorNode);

    return VDelegateInit(pData);
}

void BaseRenderComponent::VPostInit(void)
{
    shared_ptr<SceneNode> pSceneNode(VGetSceneNode());
    shared_ptr<EvtData_New_Render_Component> pEvent(CHG_NEW EvtData_New_Render_Component(m_pOwner->GetId(), pSceneNode, m_bStaticNode));
    IEventManager::Get()->VTriggerEvent(pEvent);
}


void BaseRenderComponent::VOnChanged(void)
{
    shared_ptr<EvtData_Modified_Render_Component> pEvent(CHG_NEW EvtData_Modified_Render_Component(m_pOwner->GetId()));
    IEventManager::Get()->VTriggerEvent(pEvent);
}


TiXmlElement* BaseRenderComponent::VGenerateXml(void)
{
    TiXmlElement* pBaseElement = VCreateBaseElement();

    // color
    TiXmlElement* pColor = CHG_NEW TiXmlElement("Color");
    pColor->SetAttribute("r", ToStr(m_color.x).c_str());
    pColor->SetAttribute("g", ToStr(m_color.y).c_str());
    pColor->SetAttribute("b", ToStr(m_color.z).c_str());
    pColor->SetAttribute("a", ToStr(m_color.w).c_str());
    pBaseElement->LinkEndChild(pColor);

    // create XML for inherited classes
    VCreateInheritedXmlElements(pBaseElement);

    return pBaseElement;
}

shared_ptr<SceneNode> BaseRenderComponent::VGetSceneNode(void)
{
    if (!m_pSceneNode)
        m_pSceneNode = VCreateSceneNode();
    return m_pSceneNode;
}

Color BaseRenderComponent::LoadColor(TiXmlElement* pData)
{
	Color color;

    double r = 1.0;
    double g = 1.0;
    double b = 1.0;
    double a = 1.0;

    pData->Attribute("r", &r);
    pData->Attribute("g", &g);
    pData->Attribute("b", &b);
    pData->Attribute("a", &a);

    color.x = (float)r;
    color.y = (float)g;
    color.z = (float)b;
    color.w = (float)a;

	return color;
}


//---------------------------------------------------------------------------------------------------------------------
// MeshRenderComponent
//---------------------------------------------------------------------------------------------------------------------
bool MeshRenderComponent::VInit(TiXmlElement* pData)
{
	std::string staticNode = pData->Attribute("Static");
	m_bStaticNode = staticNode.compare("true") == 0 ? true : false;

	TiXmlElement* pMeshResourceElement = pData->FirstChildElement("MeshResource");
    if (pMeshResourceElement)
    {
		m_MeshResourceName = pMeshResourceElement->Attribute("Path");
	}

	TiXmlElement* pShaderElement = pData->FirstChildElement("Shaders");
    if (pShaderElement)
    {
        m_FillShaderName = pShaderElement->Attribute("ShaderFill");
		m_MaterialResourceName = pShaderElement->Attribute("Material");
	}

	TiXmlElement* pGlobalIlluminationElement = pData->FirstChildElement("GlobalIlluminationProperties");
	if (pGlobalIlluminationElement)
	{
		m_GIShaderResource = pGlobalIlluminationElement->Attribute("Shader");
		m_GIMaterialResource = pGlobalIlluminationElement->Attribute("Material");
		m_RenderProperties.bGlobalIllumination = true;
	}
	else
		m_RenderProperties.bGlobalIllumination = false;

	TiXmlElement* pRenderPropertiesElement = pData->FirstChildElement("RenderProperties");
    if (pRenderPropertiesElement)
    {
		std::string castShadows = pRenderPropertiesElement->Attribute("CastShadows");
		m_RenderProperties.CastShadows = castShadows.compare("true") == 0 ? true : false;
		std::string activePOM = pRenderPropertiesElement->Attribute("ActiveParallax");
		m_RenderProperties.ActivePOM = activePOM.compare("true") == 0 ? true : false;
		m_RenderProperties.bCastVolumetricShadows = std::string(pRenderPropertiesElement->Attribute("CastVolumetricShadow")) == "true";
		m_RenderProperties.bShadowGeneratedAsTwoSided = std::string(pRenderPropertiesElement->Attribute("ShadowGeneratedAsTwoSided")) == "true";
	}

    return true;
}

bool MeshRenderComponent::VInit(std::shared_ptr<ComponentResource> pData)
{
    std::shared_ptr<MeshRenderComponentResourceData> pMeshRenderComponentData = std::static_pointer_cast<MeshRenderComponentResourceData>(pData->GetComponentResourceData());

    m_bStaticNode = pMeshRenderComponentData->bStaticNode;
	m_MeshResourceName = pMeshRenderComponentData->sMeshResourceName;
	m_FillShaderName = pMeshRenderComponentData->sShaderFillName;
	m_MaterialResourceName = pMeshRenderComponentData->sMaterialResourceName;
	m_GIShaderResource = pMeshRenderComponentData->sGIShaderResource;
	m_GIMaterialResource = pMeshRenderComponentData->sGIMaterialResource;
	m_RenderProperties.bGlobalIllumination = pMeshRenderComponentData->bGlobalIllumination;
	m_RenderProperties.CastShadows = pMeshRenderComponentData->bCastShadow;
	m_RenderProperties.ActivePOM = pMeshRenderComponentData->bParallaxMapping;
	m_RenderProperties.bCastVolumetricShadows = pMeshRenderComponentData->bCastVolumetricShadow;
	m_RenderProperties.bShadowGeneratedAsTwoSided = pMeshRenderComponentData->bShadowGeneratedAsTwoSided;

	return true;
}

shared_ptr<SceneNode> MeshRenderComponent::VCreateSceneNode(void)
{
    // get the transform component
    shared_ptr<TransformComponent> pTransformComponent = MakeStrongPtr(m_pOwner->GetComponent<TransformComponent>(TransformComponent::g_Name));
    if (pTransformComponent)
    {
		WeakBaseRenderComponentPtr weakThis(this);
		std::string vShaderArray[1] = {m_FillShaderName};

		shared_ptr<SceneNode> mesh(CHG_NEW MeshNode(m_pOwner->GetId(), 
													weakThis,
													m_MeshResourceName, 
													&pTransformComponent->GetTransform(), pTransformComponent->GetScale(), m_MaterialResourceName, vShaderArray, m_GIMaterialResource, m_GIShaderResource, m_RenderProperties));

		return mesh;
    }

    return shared_ptr<SceneNode>();
}

void MeshRenderComponent::VCreateInheritedXmlElements(TiXmlElement *)
{
}

//---------------------------------------------------------------------------------------------------------------------
// LightRenderComponent
//---------------------------------------------------------------------------------------------------------------------
LightRenderComponent::LightRenderComponent(void)
{
}

bool LightRenderComponent::VInit(TiXmlElement* pData)
{
	std::string staticNode = pData->Attribute("Static");
	m_bStaticNode = staticNode.compare("true") == 0 ? true : false;

	TiXmlElement* pLightTypeElement = pData->FirstChildElement("LightType");
    if (pLightTypeElement)
    {
		std::string type = pLightTypeElement->Attribute("type");

        LightType ligthType;

        if(type.compare("DIRECTIONAL_LIGHT") == 0)
        {
            ligthType = DIRECTIONAL_LIGHT;

            shared_ptr<DirectionalLight> light(new DirectionalLight());

            TiXmlElement* pLightPropertiesElement = pData->FirstChildElement("LightProperties");
            if (pLightPropertiesElement)
            {
		        light->m_LightColor = Vec4(pLightPropertiesElement->Attribute("Color"));
                light->m_LightDirection = Vec4(pLightPropertiesElement->Attribute("Direction"));

				light->m_fMaxRadius = (float)atof(pLightPropertiesElement->Attribute("MaxRadius"));
				light->m_fMaxLength = (float)atof(pLightPropertiesElement->Attribute("MaxLength"));

                double intensitive;
                pLightPropertiesElement->Attribute("Intensitive", &intensitive);
                light->m_LightIntensitive = (float)intensitive;

                std::string generateShadow = pLightPropertiesElement->Attribute("GenerateShadow");
                light->m_GenerateShadow = generateShadow.compare("true") == 0 ? true : false;

				light->m_bGenerateVolumetricShadow = std::string(pLightPropertiesElement->Attribute("GenerateVolumetricShadow")) == "true";

				Vec3 vUp = -light->m_LightDirection.xyz();
				Vec3 vLook = normalize(cross((abs(dot(vUp, Vec3(0.0f, 0.0f, 1.0f)) == 1.0f)) ? Vec3(0.0f, 0.0f, 1.0f) : Vec3(1.0f, 0.0f, 0.0f), vUp));
				Vec3 vRight = cross(vUp, vLook);

				light->m_mLightDirMat = Mat4x4(vRight.x, vUp.x, vLook.x, 0.0f,
											   vRight.y, vUp.y, vLook.y, 0.0f, 
											   vRight.z, vUp.z, vLook.z, 0.0f, 
												   0.0f,  0.0f,    0.0f, 1.0f);
	        }

            m_pLight = light;
        }
        else if(type.compare("POINT_LIGHT") == 0)
        {
            ligthType = POINT_LIGHT;

            shared_ptr<PointLight> light(new PointLight());

            TiXmlElement* pLightPropertiesElement = pData->FirstChildElement("LightProperties");
            if (pLightPropertiesElement)
            {
                double intensitive, range;

		        light->m_LightColor = Vec4(pLightPropertiesElement->Attribute("Color"));

                pLightPropertiesElement->Attribute("Intensitive", &intensitive);
                light->m_LightIntensitive = (float)intensitive;

                std::string generateShadow = pLightPropertiesElement->Attribute("GenerateShadow", &intensitive);
                light->m_GenerateShadow = generateShadow.compare("true") == 0 ? true : false;

				light->m_bGenerateVolumetricShadow = std::string(pLightPropertiesElement->Attribute("GenerateVolumetricShadow")) == "true";

                pLightPropertiesElement->Attribute("LightRadius", &range);
                light->m_LightRadius = (float)range;
	        }

            m_pLight = light;
        }
        else if(type.compare("SPOT_LIGHT") == 0)
        {
            ligthType = SPOT_LIGHT;

            shared_ptr<SpotLight> light(new SpotLight());

            TiXmlElement* pLightPropertiesElement = pData->FirstChildElement("LightProperties");
            if (pLightPropertiesElement)
            {
                double intensitive, range, thetaAngle, phiAngle;

		        light->m_LightColor = Vec4(pLightPropertiesElement->Attribute("Color"));
                light->m_LightDirection = Vec4(pLightPropertiesElement->Attribute("Direction"));

                pLightPropertiesElement->Attribute("Intensitive", &intensitive);
                light->m_LightIntensitive = (float)intensitive;

                std::string generateShadow = pLightPropertiesElement->Attribute("GenerateShadow");
                light->m_GenerateShadow = generateShadow.compare("true") == 0 ? true : false;

				light->m_bGenerateVolumetricShadow = std::string(pLightPropertiesElement->Attribute("GenerateVolumetricShadow")) == "true";

                pLightPropertiesElement->Attribute("LightRadius", &range);
				light->m_LightRadius = (float)range;

                pLightPropertiesElement->Attribute("thetaAngle", &thetaAngle);
                light->m_thetaAngle = (float)DEGREES_TO_RADIANS(thetaAngle);

                pLightPropertiesElement->Attribute("phiAngle", &phiAngle);
                light->m_phiAngle = (float)DEGREES_TO_RADIANS(phiAngle);
	        }

            m_pLight = light;
        }
	}

    return true;
}

bool LightRenderComponent::VInit(std::shared_ptr<ComponentResource> pData)
{
    std::shared_ptr<LightRenderComponentResourceData> pLightRenderComponentData = std::static_pointer_cast<LightRenderComponentResourceData>(pData->GetComponentResourceData());

    m_bStaticNode = pLightRenderComponentData->bStaticNode;

	std::string type = pLightRenderComponentData->sLightType;

	LightType ligthType;

	if (type.compare("DIRECTIONAL_LIGHT") == 0)
	{
		ligthType = DIRECTIONAL_LIGHT;

		shared_ptr<DirectionalLight> light(new DirectionalLight());

		light->m_LightColor = pLightRenderComponentData->vLightColor;
		light->m_LightDirection = pLightRenderComponentData->vDirection;
        light->m_LightIntensitive = pLightRenderComponentData->fIntensitive;

		light->m_GenerateShadow = pLightRenderComponentData->bGenerateShadow;
		light->m_bGenerateVolumetricShadow = pLightRenderComponentData->bGenerateVolumetricShadows;

		light->m_fMaxRadius = pLightRenderComponentData->fMaxRadius;
		light->m_fMaxLength = pLightRenderComponentData->fMaxLength;

		Vec3 vUp = -light->m_LightDirection.xyz();
		Vec3 vLook = normalize(cross(vUp, (abs(dot(vUp, Vec3(0.0f, 0.0f, 1.0f)) == 1.0f)) ? Vec3(0.0f, 0.0f, 1.0f) : Vec3(1.0f, 0.0f, 0.0f)));
		Vec3 vRight = cross(vLook, vRight);

		light->m_mLightDirMat = Mat4x4(vRight.x, vUp.x, vLook.x, 0.0f,
			vRight.y, vUp.y, vLook.y, 0.0f,
			vRight.z, vUp.z, vLook.z, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);

		m_pLight = light;
	}
	else if (type.compare("POINT_LIGHT") == 0)
	{
		ligthType = POINT_LIGHT;

		shared_ptr<PointLight> light(new PointLight());

        light->m_LightColor = pLightRenderComponentData->vLightColor;

        light->m_LightIntensitive = pLightRenderComponentData->fIntensitive;
				
        light->m_GenerateShadow = pLightRenderComponentData->bGenerateShadow;

		light->m_bGenerateVolumetricShadow = pLightRenderComponentData->bGenerateVolumetricShadows;

		light->m_LightRadius = pLightRenderComponentData->fAttenuationRadius;

		m_pLight = light;
	}
	else if (type.compare("SPOT_LIGHT") == 0)
	{
		ligthType = SPOT_LIGHT;

		shared_ptr<SpotLight> light(new SpotLight());

        light->m_LightColor = pLightRenderComponentData->vLightColor;
        light->m_LightDirection = pLightRenderComponentData->vDirection;

        light->m_LightIntensitive = pLightRenderComponentData->fIntensitive;

        light->m_GenerateShadow = pLightRenderComponentData->bGenerateShadow;

		light->m_bGenerateVolumetricShadow = pLightRenderComponentData->bGenerateVolumetricShadows;

        light->m_LightRadius = pLightRenderComponentData->fAttenuationRadius;

		light->m_thetaAngle = DEGREES_TO_RADIANS(pLightRenderComponentData->fThetaAngle);
		light->m_phiAngle = DEGREES_TO_RADIANS(pLightRenderComponentData->fPhiAngle);

		m_pLight = light;
	}

	return true;
}

shared_ptr<SceneNode> LightRenderComponent::VCreateSceneNode(void)
{
    shared_ptr<TransformComponent> pTransformComponent = MakeStrongPtr(m_pOwner->GetComponent<TransformComponent>(TransformComponent::g_Name));
    if (pTransformComponent)
    {
		WeakBaseRenderComponentPtr weakThis(this);

		return shared_ptr<SceneNode>(CHG_NEW LightNode(m_pOwner->GetId(), weakThis, m_pLight, &(pTransformComponent->GetTransform())));  
	}
    return shared_ptr<SceneNode>();
}

void LightRenderComponent::VCreateInheritedXmlElements(TiXmlElement *pBaseElement)
{
    /*TiXmlElement* pSceneNode = GCC_NEW TiXmlElement("Light");

    // attenuation
    TiXmlElement* pAttenuation = GCC_NEW TiXmlElement("Attenuation");
    pAttenuation->SetAttribute("const", ToStr(m_Props.m_Attenuation[0]).c_str());
    pAttenuation->SetAttribute("linear", ToStr(m_Props.m_Attenuation[1]).c_str());
    pAttenuation->SetAttribute("exp", ToStr(m_Props.m_Attenuation[2]).c_str());
    pSceneNode->LinkEndChild(pAttenuation);

    // shape
    TiXmlElement* pShape = GCC_NEW TiXmlElement("Shape");
    pShape->SetAttribute("range", ToStr(m_Props.m_Range).c_str());
    pShape->SetAttribute("falloff", ToStr(m_Props.m_Falloff).c_str());
    pShape->SetAttribute("theta", ToStr(m_Props.m_Theta).c_str());
    pShape->SetAttribute("phi", ToStr(m_Props.m_Phi).c_str());
    pSceneNode->LinkEndChild(pShape);

    pBaseElement->LinkEndChild(pSceneNode);*/
}

//---------------------------------------------------------------------------------------------------------------------
// Decals
//---------------------------------------------------------------------------------------------------------------------

DecalRenderComponent::DecalRenderComponent(void)
{
	m_sShaderResource = "";
	m_sMaterialResource = "";
	m_uMaskId = 0;
}

bool DecalRenderComponent::VInit(TiXmlElement* pData)
{
	std::string staticNode = pData->Attribute("Static");
	m_bStaticNode = staticNode.compare("true") == 0 ? true : false;

	TiXmlElement* pMaterialElement = pData->FirstChildElement("Material");
	if (pMaterialElement)
	{
		m_sShaderResource = pMaterialElement->Attribute("ShaderResource");
		m_sMaterialResource = pMaterialElement->Attribute("MaterialResource");
	}
	
	TiXmlElement* pDecalMaskElement = pData->FirstChildElement("DecalMask");
	if (pDecalMaskElement)
	{
		m_uMaskId = atoi(pDecalMaskElement->Attribute("MaskId"));
	}

	return true;
}

bool DecalRenderComponent::VInit(std::shared_ptr<ComponentResource> pData)
{
	std::shared_ptr<DecalRenderComponentResourceData> pDecalRenderComponentData = std::static_pointer_cast<DecalRenderComponentResourceData>(pData->GetComponentResourceData());

	m_bStaticNode = pDecalRenderComponentData->bStaticNode;
	
	m_sShaderResource = pDecalRenderComponentData->sShaderResource;
	m_sMaterialResource = pDecalRenderComponentData->sMaterialResource;
	m_uMaskId = pDecalRenderComponentData->uMaskId;

	return true;
}

shared_ptr<SceneNode> DecalRenderComponent::VCreateSceneNode(void)
{
	shared_ptr<TransformComponent> pTransformComponent = MakeStrongPtr(m_pOwner->GetComponent<TransformComponent>(TransformComponent::g_Name));
	if (pTransformComponent)
	{
		WeakBaseRenderComponentPtr weakThis(this);

		return shared_ptr<SceneNode>(CHG_NEW DecalNode(m_pOwner->GetId(), weakThis, m_sShaderResource, m_sMaterialResource, m_uMaskId, &(pTransformComponent->GetTransform())));
	}
	return shared_ptr<SceneNode>();
}

void DecalRenderComponent::VCreateInheritedXmlElements(TiXmlElement *pBaseElement)
{
}

//---------------------------------------------------------------------------------------------------------------------
// Particle System
//---------------------------------------------------------------------------------------------------------------------

bool ParticleSystemComponent::VInit(TiXmlElement* pData)
{
	std::string staticNode = pData->Attribute("Static");
	m_bStaticNode = staticNode.compare("true") == 0 ? true : false;

	TiXmlElement* pParticleSystemPropertiesElement = pData->FirstChildElement("Properties");
	if (pParticleSystemPropertiesElement)
	{
		TiXmlElement* pPropertyElement;

		pPropertyElement = pParticleSystemPropertiesElement->FirstChildElement("Duration");
		if (pPropertyElement)
			m_Properties.fDuration = (float)std::atof(pPropertyElement->GetText());

		pPropertyElement = pParticleSystemPropertiesElement->FirstChildElement("Looping");
		if (pPropertyElement)
			m_Properties.bLooping = std::string(pPropertyElement->GetText()) == "true";

		pPropertyElement = pParticleSystemPropertiesElement->FirstChildElement("StartDelay");
		if (pPropertyElement)
			m_Properties.fStartDelay = (float)std::atof(pPropertyElement->GetText());

		pPropertyElement = pParticleSystemPropertiesElement->FirstChildElement("StartLifetime");
		if (pPropertyElement)
			m_Properties.fStartLifetime = (float)std::atof(pPropertyElement->GetText());

		pPropertyElement = pParticleSystemPropertiesElement->FirstChildElement("StartSpeed");
		if (pPropertyElement)
			m_Properties.fStartSpeed = (float)std::atof(pPropertyElement->GetText());

		pPropertyElement = pParticleSystemPropertiesElement->FirstChildElement("StartScale");
		if (pPropertyElement)
			m_Properties.fStartScale = (float)std::atof(pPropertyElement->GetText());

		pPropertyElement = pParticleSystemPropertiesElement->FirstChildElement("StartRotation");
		if (pPropertyElement)
			m_Properties.fStartRotation = (float)std::atof(pPropertyElement->GetText());

		pPropertyElement = pParticleSystemPropertiesElement->FirstChildElement("StartColor");
		if (pPropertyElement)
			m_Properties.vStartColor = Vec4(pPropertyElement->GetText());

		pPropertyElement = pParticleSystemPropertiesElement->FirstChildElement("Gravity");
		if (pPropertyElement)
			m_Properties.fGravity = (float)std::atof(pPropertyElement->GetText());

		pPropertyElement = pParticleSystemPropertiesElement->FirstChildElement("MaxParticles");
		if (pPropertyElement)
			m_Properties.uMaxParticles = std::atoi(pPropertyElement->GetText());
	}

	m_EmissionProperties.bActive = false;
	TiXmlElement* pEmissionPropertiesElement = pData->FirstChildElement("Emission");
	if (pEmissionPropertiesElement)
	{
		m_EmissionProperties.bActive = std::string(pEmissionPropertiesElement->Attribute("Active")) == "true";

		if (m_EmissionProperties.bActive)
		{
			TiXmlElement* pEmissivePropertyElement;

			pEmissivePropertyElement = pEmissionPropertiesElement->FirstChildElement("Rate");
			if (pEmissivePropertyElement)
				m_EmissionProperties.fRate = (float)std::atof(pEmissivePropertyElement->GetText());

			pEmissivePropertyElement = pEmissionPropertiesElement->FirstChildElement("Bursts");
			if (pEmissivePropertyElement)
			{
				for (TiXmlElement* pNode = pEmissivePropertyElement->FirstChildElement(); pNode; pNode = pNode->NextSiblingElement())
				{
					Burst burst;

					burst.fTime = (float)std::atof(pNode->Attribute("Time"));
					burst.uParticlesEmitted = std::atoi(pNode->Attribute("Particles"));
					m_EmissionProperties.lBursts.push_back(burst);
				}
			}
		}			
	}

	TiXmlElement* pShapePropertiesElement = pData->FirstChildElement("Shape");
	if (pShapePropertiesElement)
	{		
		std::string type = pShapePropertiesElement->Attribute("Type");

		if (type == "Sphere")
		{
			TiXmlElement* pSphereShapePropertiesElement = pShapePropertiesElement->FirstChildElement("SphereShapeProperties");
			if (pSphereShapePropertiesElement)
			{
				float fRadius = (float)std::atof(pSphereShapePropertiesElement->Attribute("Radius"));
				bool bFromShell = std::string(pSphereShapePropertiesElement->Attribute("FromShell")) == "true";
				bool bRandomDirection = std::string(pSphereShapePropertiesElement->Attribute("RandomDirection")) == "true";

				m_SourceShapeProperties = std::shared_ptr<ParticlesSphereSource>(CHG_NEW ParticlesSphereSource(fRadius, bFromShell, bRandomDirection));
				m_SourceShapeProperties->SetActive(true);
			}
		}
		else if (type == "Hemisphere")
		{
			TiXmlElement* pHemisphereShapePropertiesElement = pShapePropertiesElement->FirstChildElement("HemisphereShapeProperties");
			if (pHemisphereShapePropertiesElement)
			{
				float fRadius = (float)std::atof(pHemisphereShapePropertiesElement->Attribute("Radius"));
				bool bFromShell = std::string(pHemisphereShapePropertiesElement->Attribute("FromShell")) == "true";
				bool bRandomDirection = std::string(pHemisphereShapePropertiesElement->Attribute("RandomDirection")) == "true";
				
				m_SourceShapeProperties = std::shared_ptr<ParticlesHemisphereSource>(CHG_NEW ParticlesHemisphereSource(fRadius, bFromShell, bRandomDirection));
				m_SourceShapeProperties->SetActive(true);
			}
		}
		else if (type == "Cone")
		{
			TiXmlElement* pConeShapePropertiesElement = pShapePropertiesElement->FirstChildElement("ConeShapeProperties");
			if (pConeShapePropertiesElement)
			{
				float fRadius = (float)std::atof(pConeShapePropertiesElement->Attribute("Radius"));
				float fAngle = (float)std::atof(pConeShapePropertiesElement->Attribute("Angle"));
				float fLength = (float)std::atof(pConeShapePropertiesElement->Attribute("Length"));
				SourceConeType sourceType = std::string(pConeShapePropertiesElement->Attribute("EmitFrom")) == "Base" ? BASE_SOURCE : VOLUME_SOURCE;
				bool bFromShell = std::string(pConeShapePropertiesElement->Attribute("FromShell")) == "true";
				bool bRandomDirection = std::string(pConeShapePropertiesElement->Attribute("RandomDirection")) == "true";

				m_SourceShapeProperties = std::shared_ptr<ParticlesConeSource>(CHG_NEW ParticlesConeSource(fRadius, fAngle, fLength, sourceType, bFromShell, bRandomDirection));
				m_SourceShapeProperties->SetActive(true);
			}
		}
		else if (type == "Box")
		{
			TiXmlElement* pBoxShapePropertiesElement = pShapePropertiesElement->FirstChildElement("BoxShapeProperties");
			if (pBoxShapePropertiesElement)
			{
				Vec3 vDimensions = Vec3(pBoxShapePropertiesElement->Attribute("Dimensions"));
				bool bFromShell = std::string(pBoxShapePropertiesElement->Attribute("FromShell")) == "true";
				bool bRandomDirection = std::string(pBoxShapePropertiesElement->Attribute("RandomDirection")) == "true";

				m_SourceShapeProperties = std::shared_ptr<ParticlesBoxSource>(CHG_NEW ParticlesBoxSource(vDimensions, bFromShell, bRandomDirection));
				m_SourceShapeProperties->SetActive(true);
			}
		}
		else
		{
			CHG_ERROR("Particle System Component Error: Init. Unknown Shape.");
		}	
	}

	m_ForceOverLifetimeProperties.bActive = false;
	TiXmlElement* pForcePropertiesElement = pData->FirstChildElement("ForceOverLifetime");
	if (pForcePropertiesElement)
	{
		m_ForceOverLifetimeProperties.bActive = std::string(pForcePropertiesElement->Attribute("Active")) == "true";

		if (m_ForceOverLifetimeProperties.bActive)
		{
			TiXmlElement* pPropertiesElement;

			pPropertiesElement = pForcePropertiesElement->FirstChildElement("Properties");
			if (pPropertiesElement)
			{
				m_ForceOverLifetimeProperties.valueType = std::string(pPropertiesElement->Attribute("type")) == "Constant" ? CONSTANT : RANDOM_BETWEN_TWO_CONSTANTS;

				if (m_ForceOverLifetimeProperties.valueType == CONSTANT)
				{
					m_ForceOverLifetimeProperties.vValue1 = Vec3(pPropertiesElement->Attribute("Force"));
				}
				else
				{
					m_ForceOverLifetimeProperties.vValue1 = Vec3(pPropertiesElement->Attribute("Force1"));
					m_ForceOverLifetimeProperties.vValue2 = Vec3(pPropertiesElement->Attribute("Force2"));
				}
			}			
		}
	}

	m_VelocityOverLifetimeProperties.bActive = false;
	TiXmlElement* pVelocityPropertiesElement = pData->FirstChildElement("VelocityOverLifetime");
	if (pVelocityPropertiesElement)
	{
		m_VelocityOverLifetimeProperties.bActive = std::string(pVelocityPropertiesElement->Attribute("Active")) == "true";

		if (m_VelocityOverLifetimeProperties.bActive)
		{
			TiXmlElement* pPropertiesElement;

			pPropertiesElement = pVelocityPropertiesElement->FirstChildElement("Properties");
			if (pPropertiesElement)
			{
				m_VelocityOverLifetimeProperties.valueType = std::string(pPropertiesElement->Attribute("type")) == "Constant" ? CONSTANT : RANDOM_BETWEN_TWO_CONSTANTS;

				if (m_VelocityOverLifetimeProperties.valueType == CONSTANT)
				{
					m_VelocityOverLifetimeProperties.vValue1 = Vec3(pPropertiesElement->Attribute("Velocity"));
				}
				else
				{
					m_VelocityOverLifetimeProperties.vValue1 = Vec3(pPropertiesElement->Attribute("Velocity1"));
					m_VelocityOverLifetimeProperties.vValue2 = Vec3(pPropertiesElement->Attribute("Velocity2"));
				}
			}
		}
	}

	m_SizeOverLifetimeProperties.bActive = false;
	TiXmlElement* pSizePropertiesElement = pData->FirstChildElement("SizeOverLifetime");
	if (pSizePropertiesElement)
	{
		m_SizeOverLifetimeProperties.bActive = std::string(pSizePropertiesElement->Attribute("Active")) == "true";

		if (m_SizeOverLifetimeProperties.bActive)
		{
			TiXmlElement* pPropertiesElement;

			pPropertiesElement = pSizePropertiesElement->FirstChildElement("Properties");
			if (pPropertiesElement)
			{
				m_SizeOverLifetimeProperties.fValue1 = (float)std::atof(pPropertiesElement->Attribute("Scale1"));
				m_SizeOverLifetimeProperties.fValue2 = (float)std::atof(pPropertiesElement->Attribute("Scale2"));
			}
		}
	}

	m_RotationOverLifetimeProperties.bActive = false;
	TiXmlElement* pRotationPropertiesElement = pData->FirstChildElement("RotationOverLifetime");
	if (pRotationPropertiesElement)
	{
		m_RotationOverLifetimeProperties.bActive = std::string(pRotationPropertiesElement->Attribute("Active")) == "true";

		if (m_RotationOverLifetimeProperties.bActive)
		{
			TiXmlElement* pPropertiesElement;

			pPropertiesElement = pRotationPropertiesElement->FirstChildElement("Properties");
			if (pPropertiesElement)
			{
				m_RotationOverLifetimeProperties.fValue1 = (float)std::atof(pPropertiesElement->Attribute("Rotate1"));
				m_RotationOverLifetimeProperties.fValue2 = (float)std::atof(pPropertiesElement->Attribute("Rotate2"));
			}
		}
	}

	TiXmlElement* pRenderPropertiesElement = pData->FirstChildElement("Render");
	if (pRenderPropertiesElement)
	{
		TiXmlElement* pPropertiesElement;

		pPropertiesElement = pRenderPropertiesElement->FirstChildElement("Properties");
		if (pPropertiesElement)
		{
			m_RenderProperties.sTextureSource = pPropertiesElement->Attribute("TextureSource");
			m_RenderProperties.bCastShadows = std::string(pPropertiesElement->Attribute("CastShadows")) == "true";
			m_RenderProperties.bReceiveShadows = std::string(pPropertiesElement->Attribute("ReceiveShadows")) == "true";
		}
	}

	return true;
}

bool ParticleSystemComponent::VInit(std::shared_ptr<ComponentResource> pData)
{
	std::shared_ptr<ParticleSystemComponentResourceData> pParticleSystemComponentData = std::static_pointer_cast<ParticleSystemComponentResourceData>(pData->GetComponentResourceData());

	m_bStaticNode = pParticleSystemComponentData->bStaticNode;

	m_Properties.fDuration = pParticleSystemComponentData->fDuration;
	m_Properties.bLooping = pParticleSystemComponentData->bLooping;
	m_Properties.fStartDelay = pParticleSystemComponentData->fStartDelay;
	m_Properties.fStartLifetime = pParticleSystemComponentData->fStartLifetime;
	m_Properties.fStartSpeed = pParticleSystemComponentData->fStartSpeed;
	m_Properties.fStartScale = pParticleSystemComponentData->fStartScale;
	m_Properties.fStartRotation = pParticleSystemComponentData->fStartRotation;
	m_Properties.vStartColor = pParticleSystemComponentData->vStartColor;
	m_Properties.fGravity = pParticleSystemComponentData->fGravity;
	m_Properties.uMaxParticles = pParticleSystemComponentData->uMaxParticles;

	m_EmissionProperties.bActive = pParticleSystemComponentData->bEmissionActive;

	if (m_EmissionProperties.bActive)
	{
		m_EmissionProperties.fRate = pParticleSystemComponentData->fRate;

		for (auto burst : pParticleSystemComponentData->lBursts)
		{
			m_EmissionProperties.lBursts.push_back(*((Burst*)&burst));
		}
	}

	std::string type = pParticleSystemComponentData->sShapeType;

	if (type == "Sphere")
	{
		float fRadius = pParticleSystemComponentData->fRadius;
		bool bFromShell = pParticleSystemComponentData->bFromShell;
		bool bRandomDirection = pParticleSystemComponentData->bRandomDirection;

		m_SourceShapeProperties = std::shared_ptr<ParticlesSphereSource>(CHG_NEW ParticlesSphereSource(fRadius, bFromShell, bRandomDirection));
		m_SourceShapeProperties->SetActive(true);
	}
	else if (type == "Hemisphere")
	{
		float fRadius = pParticleSystemComponentData->fRadius;
		bool bFromShell = pParticleSystemComponentData->bFromShell;
		bool bRandomDirection = pParticleSystemComponentData->bRandomDirection;

		m_SourceShapeProperties = std::shared_ptr<ParticlesHemisphereSource>(CHG_NEW ParticlesHemisphereSource(fRadius, bFromShell, bRandomDirection));
		m_SourceShapeProperties->SetActive(true);
	}
	else if (type == "Cone")
	{
		float fRadius = pParticleSystemComponentData->fRadius;
		float fAngle = pParticleSystemComponentData->fAngle;
		float fLength = pParticleSystemComponentData->fLength;
		SourceConeType emitFrom = pParticleSystemComponentData->sEmitFrom == "Base" ? BASE_SOURCE : VOLUME_SOURCE;
		bool bFromShell = pParticleSystemComponentData->bFromShell;
		bool bRandomDirection = pParticleSystemComponentData->bRandomDirection;

		m_SourceShapeProperties = std::shared_ptr<ParticlesConeSource>(CHG_NEW ParticlesConeSource(fRadius, fAngle, fLength, emitFrom, bFromShell, bRandomDirection));
		m_SourceShapeProperties->SetActive(true);
	}
	else if (type == "Box")
	{
		Vec3 vDimensions = pParticleSystemComponentData->vBoxDimensions;
		bool bFromShell = pParticleSystemComponentData->bFromShell;
		bool bRandomDirection = pParticleSystemComponentData->bRandomDirection;

		m_SourceShapeProperties = std::shared_ptr<ParticlesBoxSource>(CHG_NEW ParticlesBoxSource(vDimensions, bFromShell, bRandomDirection));
		m_SourceShapeProperties->SetActive(true);
	}
	else
	{
		CHG_ERROR("Particle System Component Error: Init. Unknown Shape.");
	}

	m_ForceOverLifetimeProperties.bActive = pParticleSystemComponentData->bForceOverLifetimeActive;

	if (m_ForceOverLifetimeProperties.bActive)
	{
		m_ForceOverLifetimeProperties.valueType = pParticleSystemComponentData->sForceValueType == "Constant" ? CONSTANT : RANDOM_BETWEN_TWO_CONSTANTS;

		if (m_ForceOverLifetimeProperties.valueType == CONSTANT)
		{
			m_ForceOverLifetimeProperties.vValue1 = pParticleSystemComponentData->vForceValue1;
		}
		else
		{
			m_ForceOverLifetimeProperties.vValue1 = pParticleSystemComponentData->vForceValue1;
			m_ForceOverLifetimeProperties.vValue2 = pParticleSystemComponentData->vForceValue2;
		}
	}

	m_VelocityOverLifetimeProperties.bActive = pParticleSystemComponentData->bVeloctiyOverLifetimeActive;

	if (m_VelocityOverLifetimeProperties.bActive)
	{
		m_VelocityOverLifetimeProperties.valueType = pParticleSystemComponentData->sVelocityValueType == "Constant" ? CONSTANT : RANDOM_BETWEN_TWO_CONSTANTS;

		if (m_VelocityOverLifetimeProperties.valueType == CONSTANT)
		{
			m_VelocityOverLifetimeProperties.vValue1 = pParticleSystemComponentData->vVelocityValue1;
		}
		else
		{
			m_VelocityOverLifetimeProperties.vValue1 = pParticleSystemComponentData->vVelocityValue1;
			m_VelocityOverLifetimeProperties.vValue2 = pParticleSystemComponentData->vVelocityValue2;
		}
	}

	m_SizeOverLifetimeProperties.bActive = pParticleSystemComponentData->bSizeOverLifetimeActive;

	if (m_SizeOverLifetimeProperties.bActive)
	{
		m_SizeOverLifetimeProperties.fValue1 = pParticleSystemComponentData->fSizeValue1;
		m_SizeOverLifetimeProperties.fValue2 = pParticleSystemComponentData->fSizeValue2;
	}

	m_RotationOverLifetimeProperties.bActive = pParticleSystemComponentData->bRotationOverLifetimeActive;

	if (m_RotationOverLifetimeProperties.bActive)
	{
		m_RotationOverLifetimeProperties.fValue1 = pParticleSystemComponentData->fRotationValue1;
		m_RotationOverLifetimeProperties.fValue2 = pParticleSystemComponentData->fRotationValue2;
	}

	return true;
}

shared_ptr<SceneNode> ParticleSystemComponent::VCreateSceneNode(void)
{
	shared_ptr<TransformComponent> pTransformComponent = MakeStrongPtr(m_pOwner->GetComponent<TransformComponent>(TransformComponent::g_Name));
	if (pTransformComponent)
	{
		WeakBaseRenderComponentPtr weakThis(this);

		return shared_ptr<SceneNode>(CHG_NEW ParticleSystemNode(m_pOwner->GetId(), weakThis, &pTransformComponent->GetTransform(),
			m_Properties, m_EmissionProperties, m_SourceShapeProperties,
			m_ForceOverLifetimeProperties, m_VelocityOverLifetimeProperties,
			m_SizeOverLifetimeProperties, m_RotationOverLifetimeProperties, m_RenderProperties));
	}
	return shared_ptr<SceneNode>();
}



