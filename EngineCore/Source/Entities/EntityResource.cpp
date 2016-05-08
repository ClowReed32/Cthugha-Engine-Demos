//========================================================================
// EntityResource.cpp : implement Entity resource classes
//
//========================================================================

#include "EntityResource.h"

// PhysicComponentResourceData class implementation

PhysicComponentResourceData::PhysicComponentResourceData()
{
	sDensity = "";
	sMaterial = "";

	//Constrains
	vRigidBodyTranslationConstrain = Vec3();
	vRigidBodyRotationConstrain = Vec3();
	bRigidBodyTranslationConstrain = false;
	bRigidBodyRotationConstrain = false;

	//Offsets
	vRigidBodyTranslation = Vec3();
	vRigidBodyRotation = Vec3();
	VRigidBodyScale = Vec3();
	bRigidBodyTranslation = false;
	bRigidBodyRotation = false;
	bRigidBodyScale = false;

	//Shape
	sShapeType = "";
	bGhostObject = false;

	//Common Sphere, Box and Capsule
	vShapeOffset = Vec3();

	//Sphere
	fSphereRadius = 0.0f;

	//Box
	vBoxDimensions = Vec3();

	//Capsule
	fCapsuleRadius = 0.0f;
	fCapsuleHeight = 0.0f;

	//Convex Hull and Triangle Mesh
	sShapeResourceName = "";

	//Terrain
	m_sHeightMapResource = "";
	m_iWidth = 0;
	m_iDepth = 0;
	m_fMaxHeight = 0.0f;
	m_fMinHeight = 0.0f;
	m_fWorldWidth = 0.0f;
	m_fWorldDepth = 0.0f;
	m_sUpAxis = "";
	m_sHeightMapValueType = "";
}

// EntityResourceExtraData class implementation

void EntityResourceExtraData::AddComponentResource(std::string componentName, std::shared_ptr<ComponentResource> componentResource)
{
	m_lComponentResourceList[componentName] = componentResource;
}

std::shared_ptr<ComponentResource> EntityResourceExtraData::FindComponentResource(std::string name)
{
	std::map<std::string, std::shared_ptr<ComponentResource>>::iterator it = m_lComponentResourceList.find(name);

	return (it != m_lComponentResourceList.end()) ? it->second : std::shared_ptr<ComponentResource>();
}

// EntityResourceLoader class implementation

std::shared_ptr<ComponentResourceData> EntityResourceLoader::LoadMeshRenderComponent(TiXmlElement* pData)
{
	CHG_ASSERT(pData);

    std::shared_ptr<MeshRenderComponentResourceData> pComponentData(CHG_NEW MeshRenderComponentResourceData);

    std::string staticNode = pData->Attribute("Static");
    pComponentData->bStaticNode = staticNode.compare("true") == 0 ? true : false;

    TiXmlElement* pMeshResourceElement = pData->FirstChildElement("MeshResource");
    if (pMeshResourceElement)
    {
        pComponentData->sMeshResourceName = pMeshResourceElement->Attribute("Path");
    }

    TiXmlElement* pShaderElement = pData->FirstChildElement("Shaders");
    if (pShaderElement)
    {
        pComponentData->sShaderFillName = pShaderElement->Attribute("ShaderFill");
		pComponentData->sMaterialResourceName = pShaderElement->Attribute("Material");
    }

	TiXmlElement* pGlobalIlluminationElement = pData->FirstChildElement("GlobalIlluminationProperties");
	if (pGlobalIlluminationElement)
	{
		pComponentData->sGIShaderResource = pGlobalIlluminationElement->Attribute("Shader");
		pComponentData->sGIMaterialResource = pGlobalIlluminationElement->Attribute("Material");
		pComponentData->bGlobalIllumination = true;
	}
	else
		pComponentData->bGlobalIllumination = false;

    TiXmlElement* pRenderPropertiesElement = pData->FirstChildElement("RenderProperties");
    if (pRenderPropertiesElement)
    {
        std::string castShadows = pRenderPropertiesElement->Attribute("CastShadows");
        pComponentData->bCastShadow = castShadows.compare("true") == 0 ? true : false;
        std::string activePOM = pRenderPropertiesElement->Attribute("ActiveParallax");
        pComponentData->bParallaxMapping = activePOM.compare("true") == 0 ? true : false;
		pComponentData->bCastVolumetricShadow = std::string(pRenderPropertiesElement->Attribute("CastVolumetricShadow")) == "true";
		pComponentData->bShadowGeneratedAsTwoSided = std::string(pRenderPropertiesElement->Attribute("ShadowGeneratedAsTwoSided")) == "true";
    }

    return pComponentData;
}

std::shared_ptr<ComponentResourceData> EntityResourceLoader::LoadLightRenderComponent(TiXmlElement* pData)
{
	CHG_ASSERT(pData);

    std::shared_ptr<LightRenderComponentResourceData> pComponentData(CHG_NEW LightRenderComponentResourceData);

    std::string staticNode = pData->Attribute("Static");
    pComponentData->bStaticNode = staticNode.compare("true") == 0 ? true : false;

    TiXmlElement* pLightTypeElement = pData->FirstChildElement("LightType");
    if (pLightTypeElement)
    {
        std::string type = pLightTypeElement->Attribute("type");

        if (type.compare("DIRECTIONAL_LIGHT") == 0)
        {
            TiXmlElement* pLightPropertiesElement = pData->FirstChildElement("LightProperties");
            if (pLightPropertiesElement)
            {
                pComponentData->sLightType = "DIRECTIONAL_LIGHT";
                pComponentData->vLightColor = Vec4(pLightPropertiesElement->Attribute("Color"));
                pComponentData->vDirection = Vec4(pLightPropertiesElement->Attribute("Direction"));

                pComponentData->fIntensitive = (float)std::atof(pLightPropertiesElement->Attribute("Intensitive"));

                std::string generateShadow = pLightPropertiesElement->Attribute("GenerateShadow");
                pComponentData->bGenerateShadow = generateShadow.compare("true") == 0 ? true : false;
				pComponentData->bGenerateVolumetricShadows = std::string(pLightPropertiesElement->Attribute("GenerateVolumetricShadow")) == "true";

				pComponentData->fMaxRadius = (float)atof(pLightPropertiesElement->Attribute("MaxRadius"));
				pComponentData->fMaxLength = (float)atof(pLightPropertiesElement->Attribute("MaxLength"));
            }
        }
        else if (type.compare("POINT_LIGHT") == 0)
        {
            TiXmlElement* pLightPropertiesElement = pData->FirstChildElement("LightProperties");
            if (pLightPropertiesElement)
            {
                pComponentData->sLightType = "POINT_LIGHT";
                pComponentData->vLightColor = Vec4(pLightPropertiesElement->Attribute("Color"));

				pComponentData->fIntensitive = (float)atof(pLightPropertiesElement->Attribute("Intensitive"));

                std::string generateShadow = pLightPropertiesElement->Attribute("GenerateShadow");
                pComponentData->bGenerateShadow = generateShadow.compare("true") == 0 ? true : false;
				pComponentData->bGenerateVolumetricShadows = std::string(pLightPropertiesElement->Attribute("GenerateVolumetricShadow")) == "true";
                
				pComponentData->fAttenuationRadius = (float)std::atof(pLightPropertiesElement->Attribute("LightRadius"));
            }
        }
        else if (type.compare("SPOT_LIGHT") == 0)
        {
            TiXmlElement* pLightPropertiesElement = pData->FirstChildElement("LightProperties");
            if (pLightPropertiesElement)
            {
                pComponentData->sLightType = "SPOT_LIGHT";
                pComponentData->vLightColor = Vec4(pLightPropertiesElement->Attribute("Color"));
                pComponentData->vDirection = Vec4(pLightPropertiesElement->Attribute("Direction"));

				pComponentData->fIntensitive = (float)std::atof(pLightPropertiesElement->Attribute("Intensitive"));

                std::string generateShadow = pLightPropertiesElement->Attribute("GenerateShadow");
                pComponentData->bGenerateShadow = generateShadow.compare("true") == 0 ? true : false;
				pComponentData->bGenerateVolumetricShadows = std::string(pLightPropertiesElement->Attribute("GenerateVolumetricShadow")) == "true";

				pComponentData->fAttenuationRadius = (float)std::atof(pLightPropertiesElement->Attribute("LightRadius"));

                pComponentData->fThetaAngle = (float)std::atof(pLightPropertiesElement->Attribute("thetaAngle"));
                pComponentData->fPhiAngle = (float)std::atof(pLightPropertiesElement->Attribute("phiAngle"));
            }
        }
    }

    return pComponentData;
}

std::shared_ptr<ComponentResourceData> EntityResourceLoader::LoadDecalRenderComponent(TiXmlElement* pData)
{
	CHG_ASSERT(pData);

	std::shared_ptr<DecalRenderComponentResourceData> pComponentData(CHG_NEW DecalRenderComponentResourceData);

	std::string staticNode = pData->Attribute("Static");
	pComponentData->bStaticNode = staticNode.compare("true") == 0 ? true : false;

	TiXmlElement* pMaterialElement = pData->FirstChildElement("Material");
	if (pMaterialElement)
	{
		pComponentData->sShaderResource = pMaterialElement->Attribute("ShaderResource");
		pComponentData->sMaterialResource = pMaterialElement->Attribute("MaterialResource");
	}

	TiXmlElement* pDecalMaskElement = pData->FirstChildElement("DecalMask");
	if (pDecalMaskElement)
	{
		pComponentData->uMaskId = atoi(pDecalMaskElement->Attribute("MaskId"));
	}

	return pComponentData;
}

std::shared_ptr<ComponentResourceData> EntityResourceLoader::LoadTransformComponent(TiXmlElement* pData)
{
    CHG_ASSERT(pData);

    std::shared_ptr<TransformComponentResourceData> pComponentData(CHG_NEW TransformComponentResourceData);

    Vec3 position, rotate, scaleV;

    TiXmlElement* pScaleElement = pData->FirstChildElement("Scale");
    if (pScaleElement)
    {
		float x = 0, y = 0, z = 0;
		x = (float)std::atof(pScaleElement->Attribute("x"));
		y = (float)std::atof(pScaleElement->Attribute("y"));
		z = (float)std::atof(pScaleElement->Attribute("z"));
        pComponentData->vScale = Vec3(x, y, z);
    }

    TiXmlElement* pOrientationElement = pData->FirstChildElement("Rotation");
    if (pOrientationElement)
    {
        float yaw = 0, pitch = 0, roll = 0;
        yaw = (float)std::atof(pOrientationElement->Attribute("x"));
		pitch = (float)std::atof(pOrientationElement->Attribute("y"));
		roll = (float)std::atof(pOrientationElement->Attribute("z"));
        pComponentData->vRotation = Vec3(yaw, pitch, roll);
    }

    TiXmlElement* pPositionElement = pData->FirstChildElement("Position");
    if (pPositionElement)
    {
		float x = 0, y = 0, z = 0;
		x = (float)std::atof(pPositionElement->Attribute("x"));
		y = (float)std::atof(pPositionElement->Attribute("y"));
		z = (float)std::atof(pPositionElement->Attribute("z"));
        pComponentData->vPosition = Vec3(x, y, z);
    }

    return pComponentData;
}

std::shared_ptr<ComponentResourceData> EntityResourceLoader::LoadPhysicComponent(TiXmlElement* pData)
{
	CHG_ASSERT(pData);

    std::shared_ptr<PhysicComponentResourceData> pComponentData(CHG_NEW PhysicComponentResourceData);
	//memset((void*)pComponentData.get(), 0, sizeof(PhysicComponentResourceData));

    // shape
    TiXmlElement* pShapeElement = pData->FirstChildElement("Shape");
    if (pShapeElement)
    {
        std::string type = pShapeElement->Attribute("Type");
        std::string ghostObject = pShapeElement->Attribute("GhostObject");

		pComponentData->sShapeType = type;

        if (type == "Sphere")
        {
            TiXmlElement* pSpherePropertiesElement = pShapeElement->FirstChildElement("SphereProperties");
            if (pSpherePropertiesElement)
            {
                Vec3 offset = Vec3();
                
                if (pSpherePropertiesElement->Attribute("Offset"))
                    offset = Vec3(pSpherePropertiesElement->Attribute("Offset"));

				pComponentData->fSphereRadius = (float)std::atof(pSpherePropertiesElement->Attribute("Radius"));
                pComponentData->bGhostObject = ghostObject == "true" ? true : false;
                pComponentData->vShapeOffset = offset;
            }
        }
        else if (type == "Box")
        {
            TiXmlElement* pBoxPropertiesElement = pShapeElement->FirstChildElement("BoxProperties");
            if (pBoxPropertiesElement)
            {
                Vec3 offset = Vec3();

                if (pBoxPropertiesElement->Attribute("Offset"))
                    offset = Vec3(pBoxPropertiesElement->Attribute("Offset"));

				pComponentData->vBoxDimensions = Vec3((float)std::atof(pBoxPropertiesElement->Attribute("Width")), (float)std::atof(pBoxPropertiesElement->Attribute("Height")), (float)std::atof(pBoxPropertiesElement->Attribute("Depth")));
                pComponentData->bGhostObject = ghostObject == "true" ? true : false;
                pComponentData->vShapeOffset = offset;
            }
        }
        else if (type == "Capsule")
        {
            TiXmlElement* pCapsulePropertiesElement = pShapeElement->FirstChildElement("CapsuleProperties");
            if (pCapsulePropertiesElement)
            {
                Vec3 offset = Vec3();                
                
                if (pCapsulePropertiesElement->Attribute("Offset"))
                    offset = Vec3(pCapsulePropertiesElement->Attribute("Offset"));

				pComponentData->fCapsuleHeight = (float)std::atof(pCapsulePropertiesElement->Attribute("Height"));
				pComponentData->fCapsuleRadius = (float)std::atof(pCapsulePropertiesElement->Attribute("Radius"));
                pComponentData->bGhostObject = ghostObject == "true" ? true : false;
                pComponentData->vShapeOffset = offset;
            }
        }
        else if (type == "ConvexHull")
        {
            TiXmlElement* pConvexHullPropertiesElement = pShapeElement->FirstChildElement("ConvexHullProperties");
            if (pConvexHullPropertiesElement)
            {
                std::string meshColliderFilename;
                meshColliderFilename = pConvexHullPropertiesElement->Attribute("MeshColliderSource");

                pComponentData->sShapeResourceName = meshColliderFilename;
                pComponentData->bGhostObject = ghostObject == "true" ? true : false;
            }
        }
        else if (type == "TriangleMesh")
        {
            TiXmlElement* pTriangleMeshPropertiesElement = pShapeElement->FirstChildElement("TriangleMeshProperties");
            if (pTriangleMeshPropertiesElement)
            {
                std::string meshColliderFilename;
                meshColliderFilename = pTriangleMeshPropertiesElement->Attribute("MeshColliderSource");

                pComponentData->sShapeResourceName = meshColliderFilename;
                pComponentData->bGhostObject = false;
            }
        }
		else if (type == "HeightField")
		{
			TiXmlElement* pHeightFieldPropertiesElement = pShapeElement->FirstChildElement("HeightFieldProperties");
			if (pHeightFieldPropertiesElement)
			{
				pComponentData->m_sHeightMapResource = pHeightFieldPropertiesElement->Attribute("HeightmapResource");
				pComponentData->m_iWidth = atoi(pHeightFieldPropertiesElement->Attribute("Width"));
				pComponentData->m_iDepth = atoi(pHeightFieldPropertiesElement->Attribute("Depth"));
				pComponentData->m_fWorldWidth = (float)atof(pHeightFieldPropertiesElement->Attribute("WorldWidth"));
				pComponentData->m_fWorldDepth = (float)atof(pHeightFieldPropertiesElement->Attribute("WorldDepth"));
				pComponentData->m_fMaxHeight = (float)atof(pHeightFieldPropertiesElement->Attribute("MaxHeight"));
				pComponentData->m_fMinHeight = (float)atof(pHeightFieldPropertiesElement->Attribute("MinHeight"));
				pComponentData->m_sUpAxis = pHeightFieldPropertiesElement->Attribute("UpAxis");
				pComponentData->m_sHeightMapValueType = pHeightFieldPropertiesElement->Attribute("HeightMapValueType");
				
				pComponentData->bGhostObject = false;
			}
		}
        else
        {
            CHG_ERROR("Unknow Shape Type");
        }        
    }

    // density
    TiXmlElement* pDensity = pData->FirstChildElement("Density");
    if (pDensity)
        pComponentData->sDensity = pDensity->FirstChild()->Value();

    // material
    TiXmlElement* pMaterial = pData->FirstChildElement("PhysicsMaterial");
    if (pMaterial)
        pComponentData->sMaterial = pMaterial->FirstChild()->Value();

    // initial transform
    TiXmlElement* pRigidBodyTransform = pData->FirstChildElement("RigidBodyTransform");
    if (pRigidBodyTransform)
    {
        CHG_ASSERT(pRigidBodyTransform);

        pComponentData->bRigidBodyRotation = false;
        pComponentData->bRigidBodyScale = false;
        pComponentData->bRigidBodyTranslation = false;

        TiXmlElement* pPositionElement = pRigidBodyTransform->FirstChildElement("Position");
        if (pPositionElement)
        {
			float x = 0, y = 0, z = 0;
			x = (float)std::atof(pPositionElement->Attribute("x"));
			y = (float)std::atof(pPositionElement->Attribute("y"));
			z = (float)std::atof(pPositionElement->Attribute("z"));
            pComponentData->vRigidBodyTranslation = Vec3(x, y, z);

            pComponentData->bRigidBodyTranslation = true;
        }

        TiXmlElement* pOrientationElement = pRigidBodyTransform->FirstChildElement("Orientation");
        if (pOrientationElement)
        {
			float yaw = 0, pitch = 0, roll = 0;
			yaw = (float)std::atof(pOrientationElement->Attribute("x"));
			pitch = (float)std::atof(pOrientationElement->Attribute("y"));
			roll = (float)std::atof(pOrientationElement->Attribute("z"));
            pComponentData->vRigidBodyRotation = Vec3(yaw, pitch, roll);

            pComponentData->bRigidBodyRotation = true;
        }

        TiXmlElement* pScaleElement = pRigidBodyTransform->FirstChildElement("Scale");
        if (pScaleElement)
        {
			float x = 0, y = 0, z = 0;
			x = (float)std::atof(pScaleElement->Attribute("x"));
			y = (float)std::atof(pScaleElement->Attribute("y"));
			z = (float)std::atof(pScaleElement->Attribute("z"));
            pComponentData->VRigidBodyScale = Vec3(x, y, z);

            pComponentData->bRigidBodyScale = true;
        }
    }        

    // constrains
    TiXmlElement* pRigidBodyConstrains = pData->FirstChildElement("RigidBodyConstrains");
    if (pRigidBodyConstrains)
    {
        CHG_ASSERT(pRigidBodyConstrains);

        pComponentData->bRigidBodyRotationConstrain = false;
        pComponentData->bRigidBodyTranslationConstrain = false;

        TiXmlElement* pRotateConstrainsElement = pRigidBodyConstrains->FirstChildElement("Rotate");
        if (pRotateConstrainsElement)
        {
			float x = 0, y = 0, z = 0;
			x = (float)std::atof(pRotateConstrainsElement->Attribute("x"));
			y = (float)std::atof(pRotateConstrainsElement->Attribute("y"));
			z = (float)std::atof(pRotateConstrainsElement->Attribute("z"));
            pComponentData->vRigidBodyRotationConstrain = Vec3(x, y, z);

            pComponentData->bRigidBodyRotationConstrain = true;
        }

        TiXmlElement* pTranslateConstrainsElement = pRigidBodyConstrains->FirstChildElement("Translate");
        if (pTranslateConstrainsElement)
        {
			float x = 0, y = 0, z = 0;
			x = (float)std::atof(pTranslateConstrainsElement->Attribute("x"));
			y = (float)std::atof(pTranslateConstrainsElement->Attribute("y"));
			z = (float)std::atof(pTranslateConstrainsElement->Attribute("z"));
            pComponentData->vRigidBodyTranslationConstrain = Vec3(x, y, z);

            pComponentData->bRigidBodyTranslationConstrain = true;
        }
    }        

    return pComponentData;
}

std::shared_ptr<ComponentResourceData> EntityResourceLoader::LoadParticleSystemComponent(TiXmlElement* pData)
{
	CHG_ASSERT(pData);

	std::shared_ptr<ParticleSystemComponentResourceData> pComponentData(CHG_NEW ParticleSystemComponentResourceData);

	pComponentData->bStaticNode = std::string(pData->Attribute("Static")) == "true";

	TiXmlElement* pParticleSystemPropertiesElement = pData->FirstChildElement("Properties");
	if (pParticleSystemPropertiesElement)
	{
		TiXmlElement* pPropertyElement;

		pPropertyElement = pParticleSystemPropertiesElement->FirstChildElement("Duration");
		if (pPropertyElement)
			pComponentData->fDuration = (float)std::atof(pPropertyElement->GetText());

		pPropertyElement = pParticleSystemPropertiesElement->FirstChildElement("Looping");
		if (pPropertyElement)
			pComponentData->bLooping = std::string(pPropertyElement->GetText()) == "true";

		pPropertyElement = pParticleSystemPropertiesElement->FirstChildElement("StartDelay");
		if (pPropertyElement)
			pComponentData->fStartDelay = (float)std::atof(pPropertyElement->GetText());

		pPropertyElement = pParticleSystemPropertiesElement->FirstChildElement("StartLifetime");
		if (pPropertyElement)
			pComponentData->fStartLifetime = (float)std::atof(pPropertyElement->GetText());

		pPropertyElement = pParticleSystemPropertiesElement->FirstChildElement("StartSpeed");
		if (pPropertyElement)
			pComponentData->fStartSpeed = (float)std::atof(pPropertyElement->GetText());

		pPropertyElement = pParticleSystemPropertiesElement->FirstChildElement("StartScale");
		if (pPropertyElement)
			pComponentData->fStartScale = (float)std::atof(pPropertyElement->GetText());

		pPropertyElement = pParticleSystemPropertiesElement->FirstChildElement("StartRotation");
		if (pPropertyElement)
			pComponentData->fStartRotation = (float)std::atof(pPropertyElement->GetText());

		pPropertyElement = pParticleSystemPropertiesElement->FirstChildElement("StartColor");
		if (pPropertyElement)
			pComponentData->vStartColor = Vec4(pPropertyElement->GetText());

		pPropertyElement = pParticleSystemPropertiesElement->FirstChildElement("Gravity");
		if (pPropertyElement)
			pComponentData->fGravity = (float)std::atof(pPropertyElement->GetText());

		pPropertyElement = pParticleSystemPropertiesElement->FirstChildElement("MaxParticles");
		if (pPropertyElement)
			pComponentData->uMaxParticles = std::atoi(pPropertyElement->GetText());
	}

	pComponentData->bEmissionActive = false;
	TiXmlElement* pEmissionPropertiesElement = pData->FirstChildElement("Emission");
	if (pEmissionPropertiesElement)
	{
		pComponentData->bEmissionActive = std::string(pEmissionPropertiesElement->Attribute("Active")) == "true";

		if (pComponentData->bEmissionActive)
		{
			TiXmlElement* pEmissivePropertyElement;

			pEmissivePropertyElement = pEmissionPropertiesElement->FirstChildElement("Rate");
			if (pEmissivePropertyElement)
				pComponentData->fRate = (float)std::atof(pEmissivePropertyElement->GetText());

			pEmissivePropertyElement = pEmissionPropertiesElement->FirstChildElement("Bursts");
			if (pEmissivePropertyElement)
			{
				pComponentData->fRate = (float)std::atof(pEmissivePropertyElement->GetText());

				for (TiXmlElement* pNode = pEmissivePropertyElement->FirstChildElement(); pNode; pNode = pNode->NextSiblingElement())
				{
					Burst burst;

					burst.fTime = (float)std::atof(pNode->Attribute("Time"));
					burst.uParticlesEmitted = std::atoi(pNode->Attribute("Particles"));
					pComponentData->lBursts.push_back(burst);
				}
			}
		}
	}

	pComponentData->bShapeActive = false;
	TiXmlElement* pShapePropertiesElement = pData->FirstChildElement("Shape");
	if (pShapePropertiesElement)
	{
		pComponentData->bShapeActive = std::string(pShapePropertiesElement->Attribute("Active")) == "true";

		if (pComponentData->bShapeActive)
		{
			std::string type = pShapePropertiesElement->Attribute("Type");
			pComponentData->sShapeType = type;

			if (type == "Sphere")
			{
				TiXmlElement* pSphereShapePropertiesElement = pShapePropertiesElement->FirstChildElement("SphereShapeProperties");
				if (pSphereShapePropertiesElement)
				{
					pComponentData->fRadius = (float)std::atof(pSphereShapePropertiesElement->Attribute("Radius"));
					pComponentData->bFromShell = std::string(pSphereShapePropertiesElement->Attribute("FromShell")) == "true";
					pComponentData->bRandomDirection = std::string(pSphereShapePropertiesElement->Attribute("RandomDirection")) == "true";
				}
			}
			else if (type == "Hemisphere")
			{
				TiXmlElement* pHemisphereShapePropertiesElement = pShapePropertiesElement->FirstChildElement("HemisphereShapeProperties");
				if (pHemisphereShapePropertiesElement)
				{
					pComponentData->fRadius = (float)std::atof(pHemisphereShapePropertiesElement->Attribute("Radius"));
					pComponentData->bFromShell = std::string(pHemisphereShapePropertiesElement->Attribute("FromShell")) == "true";
					pComponentData->bRandomDirection = std::string(pHemisphereShapePropertiesElement->Attribute("RandomDirection")) == "true";
				}
			}
			else if (type == "Cone")
			{
				TiXmlElement* pConeShapePropertiesElement = pShapePropertiesElement->FirstChildElement("ConeShapeProperties");
				if (pConeShapePropertiesElement)
				{
					pComponentData->fRadius = (float)std::atof(pConeShapePropertiesElement->Attribute("Radius"));
					pComponentData->fAngle = (float)std::atof(pConeShapePropertiesElement->Attribute("Angle"));
					pComponentData->fLength = (float)std::atof(pConeShapePropertiesElement->Attribute("Length"));
					pComponentData->sEmitFrom = pConeShapePropertiesElement->Attribute("EmitFrom");
					pComponentData->bFromShell = std::string(pConeShapePropertiesElement->Attribute("FromShell")) == "true";
					pComponentData->bRandomDirection = std::string(pConeShapePropertiesElement->Attribute("RandomDirection")) == "true";
				}
			}
			else if (type == "Box")
			{
				TiXmlElement* pBoxShapePropertiesElement = pShapePropertiesElement->FirstChildElement("BoxShapeProperties");
				if (pBoxShapePropertiesElement)
				{
					pComponentData->vBoxDimensions = Vec3(pBoxShapePropertiesElement->Attribute("Dimensions"));
					pComponentData->bFromShell = std::string(pBoxShapePropertiesElement->Attribute("FromShell")) == "true";
					pComponentData->bRandomDirection = std::string(pBoxShapePropertiesElement->Attribute("RandomDirection")) == "true";
				}
			}
			else
			{
				CHG_ERROR("Particle System Component Error: Init. Unknown Shape.");
			}
		}
	}

	pComponentData->bForceOverLifetimeActive = false;
	TiXmlElement* pForcePropertiesElement = pData->FirstChildElement("ForceOverLifetime");
	if (pForcePropertiesElement)
	{
		pComponentData->bForceOverLifetimeActive = std::string(pForcePropertiesElement->Attribute("Active")) == "true";

		if (pComponentData->bForceOverLifetimeActive)
		{
			TiXmlElement* pPropertiesElement;

			pPropertiesElement = pForcePropertiesElement->FirstChildElement("Properties");
			if (pPropertiesElement)
			{
				pComponentData->sForceValueType = pPropertiesElement->Attribute("type");

				if (pComponentData->sForceValueType == "Constant")
				{
					pComponentData->vForceValue1 = Vec3(pPropertiesElement->Attribute("Force"));
				}
				else
				{
					pComponentData->vForceValue1 = Vec3(pPropertiesElement->Attribute("Force1"));
					pComponentData->vForceValue2 = Vec3(pPropertiesElement->Attribute("Force2"));
				}
			}
		}
	}

	pComponentData->bVeloctiyOverLifetimeActive = false;
	TiXmlElement* pVelocityPropertiesElement = pData->FirstChildElement("VelocityOverLifetime");
	if (pVelocityPropertiesElement)
	{
		pComponentData->bVeloctiyOverLifetimeActive = std::string(pVelocityPropertiesElement->Attribute("Active")) == "true";

		if (pComponentData->bVeloctiyOverLifetimeActive)
		{
			TiXmlElement* pPropertiesElement;

			pPropertiesElement = pVelocityPropertiesElement->FirstChildElement("Properties");
			if (pPropertiesElement)
			{
				pComponentData->sVelocityValueType = pPropertiesElement->Attribute("type");

				if (pComponentData->sVelocityValueType == "Constant")
				{
					pComponentData->vVelocityValue1 = Vec3(pPropertiesElement->Attribute("Velocity"));
				}
				else
				{
					pComponentData->vVelocityValue1 = Vec3(pPropertiesElement->Attribute("Velocity1"));
					pComponentData->vVelocityValue2 = Vec3(pPropertiesElement->Attribute("Velocity2"));
				}
			}
		}
	}

	pComponentData->bSizeOverLifetimeActive = false;
	TiXmlElement* pSizePropertiesElement = pData->FirstChildElement("SizeOverLifetime");
	if (pSizePropertiesElement)
	{
		pComponentData->bSizeOverLifetimeActive = std::string(pSizePropertiesElement->Attribute("Active")) == "true";

		if (pComponentData->bSizeOverLifetimeActive)
		{
			TiXmlElement* pPropertiesElement;

			pPropertiesElement = pSizePropertiesElement->FirstChildElement("Properties");
			if (pPropertiesElement)
			{
				pComponentData->fSizeValue1 = (float)std::atof(pPropertiesElement->Attribute("Scale1"));
				pComponentData->fSizeValue2 = (float)std::atof(pPropertiesElement->Attribute("Scale2"));
			}
		}
	}

	pComponentData->bRotationOverLifetimeActive = false;
	TiXmlElement* pRotationPropertiesElement = pData->FirstChildElement("SizeOverLifetime");
	if (pRotationPropertiesElement)
	{
		pComponentData->bRotationOverLifetimeActive = std::string(pRotationPropertiesElement->Attribute("Active")) == "true";

		if (pComponentData->bRotationOverLifetimeActive)
		{
			TiXmlElement* pPropertiesElement;

			pPropertiesElement = pRotationPropertiesElement->FirstChildElement("Properties");
			if (pPropertiesElement)
			{
				pComponentData->fRotationValue1 = (float)std::atof(pPropertiesElement->Attribute("Rotate1"));
				pComponentData->fRotationValue2 = (float)std::atof(pPropertiesElement->Attribute("Rotate2"));
			}
		}
	}

	TiXmlElement* pTextureResourceNameElement = pData->FirstChildElement("Texture");
	if (pTextureResourceNameElement)
	{
		pComponentData->sParticleTextureResourceName = pTextureResourceNameElement->FirstChildElement("File")->GetText();
	}

	return pComponentData;
}

std::shared_ptr<ComponentResourceData> EntityResourceLoader::LoadPlacementComponent(TiXmlElement* pData)
{
    std::shared_ptr<PlacementComponentResourceData> pComponentData(CHG_NEW PlacementComponentResourceData);

    TiXmlElement* pOverlapElement = pData->FirstChildElement("Overlap");
    if (pOverlapElement)
    {
		pComponentData->fOverlapLevel = (float)std::atof(pOverlapElement->Attribute("Level"));
		pComponentData->fOverlapRadius = (float)std::atof(pOverlapElement->Attribute("Radius"));
    }

    TiXmlElement* pScaleElement = pData->FirstChildElement("Scale");
    if (pScaleElement)
    {
        float min = 0, max = 0;

		min = (float)std::atof(pScaleElement->Attribute("min"));
		max = (float)std::atof(pScaleElement->Attribute("max"));
        pComponentData->vScale = Vec2(min, max);
    }

    TiXmlElement* pRotationElement = pData->FirstChildElement("Rotation");
    if (pRotationElement)
    {
        float min = 0, max = 0;

		min = (float)std::atof(pRotationElement->Attribute("min"));
		max = (float)std::atof(pRotationElement->Attribute("max"));
        pComponentData->vRotation = Vec2(min, max);
    }

    return pComponentData;
}

bool EntityResourceLoader::VLoadResource(char *rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle)
{
	// Grab the root XML node
	TiXmlDocument doc;
	doc.Parse(rawBuffer, NULL, TIXML_ENCODING_UTF8);
	TiXmlElement* pRoot = doc.RootElement();

	if (!pRoot)
	{
		CHG_ERROR("Failed to find entity resource file.");
		return false;
	}

	std::string resourceName = pRoot->Attribute("resource");
	std::string type = pRoot->Attribute("type");

    shared_ptr<EntityResourceExtraData> extra = shared_ptr<EntityResourceExtraData>(CHG_NEW EntityResourceExtraData(resourceName));
    extra->m_sEntityType = type;

	for (TiXmlElement* pNode = pRoot->FirstChildElement(); pNode; pNode = pNode->NextSiblingElement())
	{
		std::string sName = pNode->Value();
		std::shared_ptr<ComponentResource> pComponentResource(CHG_NEW ComponentResource(sName));

		if (sName == "TransformComponent")
        {
            pComponentResource->m_pComponentResourceData = LoadTransformComponent(pNode);
        }
		else if (sName == "PhysicsComponent")
        {
            pComponentResource->m_pComponentResourceData = LoadPhysicComponent(pNode);
        }
		else if (sName == "LightRenderComponent")
        {
            pComponentResource->m_pComponentResourceData = LoadLightRenderComponent(pNode);
        }
		else if (sName == "MeshRenderComponent")
        {
            pComponentResource->m_pComponentResourceData = LoadMeshRenderComponent(pNode);
        }
		else if (sName == "PlacementComponent")
        {
            pComponentResource->m_pComponentResourceData = LoadPlacementComponent(pNode);
        }
        else
        {
            CHG_ERROR("Unknow component error");
            return false;
        }

        extra->AddComponentResource(pComponentResource->m_sName, pComponentResource);
	}

	handle->SetExtra(extra);

    return true;
}

shared_ptr<IResourceLoader> CreateEntityResourceLoader()
{
	return shared_ptr<IResourceLoader>(CHG_NEW EntityResourceLoader());
}