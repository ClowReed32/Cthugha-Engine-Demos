//========================================================================
// PhysicsComponent.cpp - Component for physical movement and collision detection
//
//========================================================================

#include "CthughaStd.h"
#include "PhysicsComponent.h"
#include "TransformComponent.h"
#include "../Physics/Physics.h"
#include "../Utilities/String.h"
#include "../Physics/MeshColliderResource.h"

// all times are units-per-second
const float DEFAULT_MAX_VELOCITY = 1.5f;
const float DEFAULT_MAX_ANGULAR_VELOCITY = 1.2f;

//---------------------------------------------------------------------------------------------------------------------
//ConvexHull implementation
//---------------------------------------------------------------------------------------------------------------------

ConvexHullProperties::ConvexHullProperties(std::string collisionMeshFilename)
{
	m_ShapeType = "ConvexHull";

	//Load MeshCollider Source
	Resource MeshColliderResource(collisionMeshFilename);
	shared_ptr<ResHandle> MeshColliderSourceHandle = g_pApp->m_ResCache->GetHandle(&MeshColliderResource);

	if(MeshColliderSourceHandle.get() == NULL)
	{
		CHG_ERROR("Not found MeshCollider Source");
	}

	m_pVertex = ((MeshColliderResourceExtraData*)MeshColliderSourceHandle->GetExtra().get())->m_pColliderVertex;
	m_NumVertex = ((MeshColliderResourceExtraData*)MeshColliderSourceHandle->GetExtra().get())->m_NumVertex;
}

ConvexHullProperties::~ConvexHullProperties()
{

}

TriangleMeshProperties::TriangleMeshProperties(std::string collisionMeshFilename)
{
	m_ShapeType = "TriangleMesh";

	//Load MeshCollider Source
	Resource MeshColliderResource(collisionMeshFilename);
	shared_ptr<ResHandle> MeshColliderSourceHandle = g_pApp->m_ResCache->GetHandle(&MeshColliderResource);

	if (MeshColliderSourceHandle.get() == NULL)
	{
		CHG_ERROR("Not found MeshCollider Source");
	}

	m_pVertex = ((MeshColliderResourceExtraData*)MeshColliderSourceHandle->GetExtra().get())->m_pColliderVertex;
	m_NumVertex = ((MeshColliderResourceExtraData*)MeshColliderSourceHandle->GetExtra().get())->m_NumVertex;
	m_pIndex = ((MeshColliderResourceExtraData*)MeshColliderSourceHandle->GetExtra().get())->m_pColliderIndex;
	m_NumIndex = ((MeshColliderResourceExtraData*)MeshColliderSourceHandle->GetExtra().get())->m_NumIndices;
}

TriangleMeshProperties::~TriangleMeshProperties()
{

}

HeightFieldProperties::HeightFieldProperties(std::string sHeightmapResource, int iWidth, int iDepth, float fMaxHeight, float fMinHeight, float fWorldWidth, float fWorldDepth, std::string sUpAxis, std::string sHeightMapValueType)
{
	/*void* pHeightMapRaw;
	Resource resource(sHeightmapResource);
	
	pHeightMapRaw = std::static_pointer_cast<CartographerResourceExtraData>(g_pApp->m_ResCache->GetHandle(&resource)->GetExtra())->GetHeightmapRawPointer();
	float fOffset = 0.9f;

	m_pHeightMapRaw = CHG_NEW float[iWidth*iDepth];
	float fHeightScale;

	if (sHeightMapValueType == "SHORT")
	{
		fHeightScale = abs(fMinHeight - fMaxHeight) / 65535;

		for (int i = 0; i < iWidth*iDepth; i++)
		{
			USHORT depth = *((unsigned short*)pHeightMapRaw + i);
			m_pHeightMapRaw[i] = fMinHeight + (depth * fHeightScale) + fOffset;
		}
	}
	else if (sHeightMapValueType == "UCHAR")
	{
		fHeightScale = abs(fMinHeight - fMaxHeight) / 255;

		for (int i = 0; i < iWidth*iDepth; i++)
		{
			UCHAR depth = *((unsigned short*)pHeightMapRaw + i);
			m_pHeightMapRaw[i] = fMinHeight + (depth * fHeightScale);
		}
	}
	else
	{
		fHeightScale = 1.0f;

		for (int i = 0; i < iWidth*iDepth; i++)
		{
			float depth = *((float*)pHeightMapRaw + i);
			m_pHeightMapRaw[i] = depth;
		}
	}
	
	m_iWidth = iWidth;
	m_iDepth = iDepth;
	m_fMaxHeight = fMaxHeight;
	m_fMinHeight = fMinHeight;
	m_fWorldWidth = fWorldWidth;
	m_fWorldDepth = fWorldDepth;
	m_sUpAxis = sUpAxis;
	m_sHeightMapValueType = sHeightMapValueType;

	m_ShapeType = "HeightField";*/
}

HeightFieldProperties::~HeightFieldProperties()
{
	SAFE_DELETE_ARRAY(m_pHeightMapRaw);
	m_pHeightMapRaw = NULL;
}

//---------------------------------------------------------------------------------------------------------------------
// PhysicsComponent implementation
//---------------------------------------------------------------------------------------------------------------------

const char *PhysicsComponent::g_Name = "PhysicsComponent";

PhysicsComponent::PhysicsComponent(void)
{
    m_RigidBodyLocation = Vec3(0.f,0.f,0.f);
	m_RigidBodyOrientation = Vec3(0.f,0.f,0.f);
	m_RigidBodyScale = Vec3(1.f,1.f,1.f);

	m_RigidBodyRotateConstrains = Vec3(1.0f, 1.0f, 1.0f);
	m_RigidBodyTranslateConstrains = Vec3(1.0f, 1.0f, 1.0f);

    m_acceleration = Vec3(0.0f, 0.0f, 0.0f);
    m_angularAcceleration = 0;
    m_maxVelocity = DEFAULT_MAX_VELOCITY;
    m_maxAngularVelocity = DEFAULT_MAX_ANGULAR_VELOCITY;
}

PhysicsComponent::~PhysicsComponent(void)
{
    m_pGamePhysics->VRemoveEntity(m_pOwner->GetId());
}

bool PhysicsComponent::VInit(TiXmlElement* pData)
{
    // no point in having a physics component with no game physics
    m_pGamePhysics = g_pApp->m_pGame->VGetGamePhysics();
    if (!m_pGamePhysics)
        return false;

    // shape
    TiXmlElement* pShape = pData->FirstChildElement("Shape");
    if (pShape)
    {
		BuildRigidBodyShape(pShape);
    }

    // density
    TiXmlElement* pDensity = pData->FirstChildElement("Density");
    if (pDensity)
        m_density = pDensity->FirstChild()->Value();

    // material
    TiXmlElement* pMaterial = pData->FirstChildElement("PhysicsMaterial");
    if (pMaterial)
        m_material = pMaterial->FirstChild()->Value();

    // initial transform
    TiXmlElement* pRigidBodyTransform = pData->FirstChildElement("RigidBodyTransform");
    if (pRigidBodyTransform)
        BuildRigidBodyTransform(pRigidBodyTransform);

	// constrains
	TiXmlElement* pRigidBodyConstrains = pData->FirstChildElement("RigidBodyConstrains");
	if (pRigidBodyConstrains)
		BuildRigidBodyConstrains(pRigidBodyConstrains);


    return true;
}

bool PhysicsComponent::VInit(std::shared_ptr<ComponentResource> pData)
{
	// no point in having a physics component with no game physics
	m_pGamePhysics = g_pApp->m_pGame->VGetGamePhysics();
	if (!m_pGamePhysics)
		return false;

    std::shared_ptr<PhysicComponentResourceData> pPhysicComponentData = std::static_pointer_cast<PhysicComponentResourceData>(pData->GetComponentResourceData());

	// shape
    BuildRigidBodyShape(pPhysicComponentData);
	
    // density
	m_density = pPhysicComponentData->sDensity;

	// material
    m_material = pPhysicComponentData->sMaterial;

	// initial transform
    BuildRigidBodyTransform(pPhysicComponentData);

	// constrains
    BuildRigidBodyConstrains(pPhysicComponentData);

	return true;
}

TiXmlElement* PhysicsComponent::VGenerateXml(void)
{
    TiXmlElement* pBaseElement = CHG_NEW TiXmlElement(VGetName());

    // shape
	/*TiXmlElement* pShape = CHG_NEW TiXmlElement("Shape");
    TiXmlText* pShapeText = CHG_NEW TiXmlText(m_shape.c_str());
    pShape->LinkEndChild(pShapeText);
	pBaseElement->LinkEndChild(pShape);*/

    // density
    TiXmlElement* pDensity = CHG_NEW TiXmlElement("Density");
    TiXmlText* pDensityText = CHG_NEW TiXmlText(m_density.c_str());
    pDensity->LinkEndChild(pDensityText);
    pBaseElement->LinkEndChild(pDensity);

    // material
    TiXmlElement* pMaterial = CHG_NEW TiXmlElement("Material");
    TiXmlText* pMaterialText = CHG_NEW TiXmlText(m_material.c_str());
    pMaterial->LinkEndChild(pMaterialText);
    pBaseElement->LinkEndChild(pMaterial);

    // rigid body transform
    TiXmlElement* pInitialTransform = CHG_NEW TiXmlElement("RigidBodyTransform");

    // initial transform -> position
    TiXmlElement* pPosition = CHG_NEW TiXmlElement("Position");
    pPosition->SetAttribute("x", ToStr(m_RigidBodyLocation.x).c_str());
    pPosition->SetAttribute("y", ToStr(m_RigidBodyLocation.y).c_str());
    pPosition->SetAttribute("z", ToStr(m_RigidBodyLocation.z).c_str());
    pInitialTransform->LinkEndChild(pPosition);

    // initial transform -> orientation
    TiXmlElement* pOrientation = CHG_NEW TiXmlElement("Orientation");
    pOrientation->SetAttribute("yaw", ToStr(m_RigidBodyOrientation.x).c_str());
    pOrientation->SetAttribute("pitch", ToStr(m_RigidBodyOrientation.y).c_str());
    pOrientation->SetAttribute("roll", ToStr(m_RigidBodyOrientation.z).c_str());
    pInitialTransform->LinkEndChild(pOrientation);

	// initial transform -> scale 
    TiXmlElement* pScale = CHG_NEW TiXmlElement("Scale");
    pScale->SetAttribute("x", ToStr(m_RigidBodyScale.x).c_str());
    pScale->SetAttribute("y", ToStr(m_RigidBodyScale.y).c_str());
    pScale->SetAttribute("z", ToStr(m_RigidBodyScale.z).c_str());
    pInitialTransform->LinkEndChild(pScale);

    pBaseElement->LinkEndChild(pInitialTransform);

    return pBaseElement;
}

void PhysicsComponent::VPostInit(void)
{
    if (m_pOwner)
    {
		if (m_pShapeProperties->GetShapeType() == "Sphere")
		{
			std::shared_ptr<SphereProperties> pSphereProperties = std::static_pointer_cast<SphereProperties>(m_pShapeProperties);

			if(!m_pShapeProperties->IsGhost())
			{
				m_pGamePhysics->VAddSphere(pSphereProperties->GetRadius(), m_pOwner, m_density, m_material, m_RigidBodyRotateConstrains, m_RigidBodyTranslateConstrains, pSphereProperties->GetOffset());
			}
			else
			{
				m_pGamePhysics->VAddGhostSphere(pSphereProperties->GetRadius(), m_pOwner);
			}
		}
		else if (m_pShapeProperties->GetShapeType() == "Box")
		{
			std::shared_ptr<BoxProperties> pBoxProperties = std::static_pointer_cast<BoxProperties>(m_pShapeProperties);

			if(!m_pShapeProperties->IsGhost())
			{
				m_pGamePhysics->VAddBox(pBoxProperties->GetBoxDimension(), m_pOwner, m_density, m_material, m_RigidBodyRotateConstrains, m_RigidBodyTranslateConstrains, pBoxProperties->GetOffset());
			}
			else
			{
				m_pGamePhysics->VAddGhostBox(pBoxProperties->GetBoxDimension(), m_pOwner);
			}
		}
		else if (m_pShapeProperties->GetShapeType() == "Capsule")
		{
			std::shared_ptr<CapsuleProperties> pCapsuleProperties = std::static_pointer_cast<CapsuleProperties>(m_pShapeProperties);

			if(!m_pShapeProperties->IsGhost())
			{
				m_pGamePhysics->VAddCapsule(pCapsuleProperties->GetRadius(), pCapsuleProperties->GetHeight(), m_pOwner, m_density, m_material, m_RigidBodyRotateConstrains, m_RigidBodyTranslateConstrains, pCapsuleProperties->GetOffset());
			}
			else
			{
				m_pGamePhysics->VAddGhostCapsule(pCapsuleProperties->GetRadius(), pCapsuleProperties->GetHeight(), m_pOwner);
			}
		}
		else if (m_pShapeProperties->GetShapeType() == "ConvexHull")
		{
			std::shared_ptr<ConvexHullProperties> pConvexHullProperties = std::static_pointer_cast<ConvexHullProperties>(m_pShapeProperties);

			if(!m_pShapeProperties->IsGhost())
			{
				m_pGamePhysics->VAddConvexHull(pConvexHullProperties->GetVertexList().get(), pConvexHullProperties->GetNumVertex(), m_pOwner, m_density, m_material, m_RigidBodyRotateConstrains, m_RigidBodyTranslateConstrains);
			}
			else
			{
				m_pGamePhysics->VAddGhostConvexHull(pConvexHullProperties->GetVertexList().get(), pConvexHullProperties->GetNumVertex(), m_pOwner);
			}
		}
		else if (m_pShapeProperties->GetShapeType() == "TriangleMesh")
		{
			std::shared_ptr<TriangleMeshProperties> pTriangleMeshProperties = std::static_pointer_cast<TriangleMeshProperties>(m_pShapeProperties);
			m_pGamePhysics->VAddTriangleMesh((float*)pTriangleMeshProperties->GetVertexList().get(), pTriangleMeshProperties->GetNumVertex(), pTriangleMeshProperties->GetIndexList().get(), pTriangleMeshProperties->GetNumIndex()/3, m_pOwner, m_density, m_material, m_RigidBodyRotateConstrains, m_RigidBodyTranslateConstrains);
		}
		else if (m_pShapeProperties->GetShapeType() == "HeightField")
		{
			std::shared_ptr<HeightFieldProperties> pHeightFieldProperties = std::static_pointer_cast<HeightFieldProperties>(m_pShapeProperties);
			m_pGamePhysics->VAddHeightField(pHeightFieldProperties->GetHeightmapRaw(), pHeightFieldProperties->GetWidth(), pHeightFieldProperties->GetDepth(), pHeightFieldProperties->GetMaxHeight(), pHeightFieldProperties->GetMinHeight(), pHeightFieldProperties->GetUpAxis(), Vec2(pHeightFieldProperties->GetWorldWidth(), pHeightFieldProperties->GetWorldDepth()), m_pOwner, m_density, m_material, m_RigidBodyRotateConstrains, m_RigidBodyTranslateConstrains);
		}
	}
}

void PhysicsComponent::VUpdate(int deltaMs)
{
    // get the transform component
    shared_ptr<TransformComponent> pTransformComponent = MakeStrongPtr(m_pOwner->GetComponent<TransformComponent>(TransformComponent::g_Name));
    if (!pTransformComponent)
    {
        CHG_ERROR("No transform component!");
        return;
    }

    // get the direction the object is facing
    Mat4x4 transform = pTransformComponent->GetTransform();

	float accelerationModule = length(m_acceleration);

	if (accelerationModule != 0)
    {
        // calculate the acceleration this frame

        float accelerationToApplyThisFrame = accelerationModule / 1000.f * (float)deltaMs;

		m_pGamePhysics->VApplyForce(m_acceleration/=accelerationModule, accelerationModule, m_pOwner->GetId());

        // logging
		/*
        CHG_LOG("Actor", "Acceleration: " + ToStr(accelerationToApplyThisFrame) + "; velocityScalar: " + ToStr(velocityScalar) + \
                "; direction: " + ToStr(direction) + "; direction.Length(): " + ToStr(length(direction)) + \
                "; velocity: " + ToStr(velocity) + "; velocity.Length(): " + ToStr(length(velocity)));
		*/
    }

    if (m_angularAcceleration != 0)
    {
        // calculate the acceleration this frame
        float angularAccelerationToApplyThisFrame = m_angularAcceleration / 1000.f * (float)deltaMs;
		m_pGamePhysics->VApplyTorque(transform.GetUp(), angularAccelerationToApplyThisFrame, m_pOwner->GetId());

        // logging
        //GCC_LOG("Actor", "Angular Acceleration: " + ToStr(angularAccelerationToApplyThisFrame) );
    }
}

void PhysicsComponent::BuildRigidBodyTransform(std::shared_ptr<PhysicComponentResourceData> pTransformElement)
{
    // FUTURE WORK Mrmike - this should be exactly the same as the TransformComponent - maybe factor into a helper method?
    CHG_ASSERT(pTransformElement);

    if (pTransformElement->bRigidBodyTranslation)
    {
        m_RigidBodyLocation = pTransformElement->vRigidBodyTranslation;
    }

    if (pTransformElement->bRigidBodyRotation)
    {
        m_RigidBodyOrientation = Vec3((float)DEGREES_TO_RADIANS(pTransformElement->vRigidBodyRotation.x), (float)DEGREES_TO_RADIANS(pTransformElement->vRigidBodyRotation.y), (float)DEGREES_TO_RADIANS(pTransformElement->vRigidBodyRotation.z));
    }

    if (pTransformElement->bRigidBodyScale)
    {
        m_RigidBodyScale = pTransformElement->vRigidBodyRotation;
    }
}

void PhysicsComponent::BuildRigidBodyConstrains(std::shared_ptr<PhysicComponentResourceData> pConstrainElement)
{
    CHG_ASSERT(pConstrainElement);

    if (pConstrainElement->bRigidBodyRotationConstrain)
    {
        m_RigidBodyRotateConstrains = pConstrainElement->vRigidBodyRotationConstrain;
    }

    if (pConstrainElement->bRigidBodyTranslationConstrain)
    {
        m_RigidBodyTranslateConstrains = pConstrainElement->vRigidBodyTranslationConstrain;
    }
}

void PhysicsComponent::BuildRigidBodyShape(std::shared_ptr<PhysicComponentResourceData> pShapeElement)
{
    std::string type = pShapeElement->sShapeType;
    bool ghostObject = pShapeElement->bGhostObject;

    if (type == "Sphere")
    {
        m_pShapeProperties.reset(CHG_NEW SphereProperties(pShapeElement->fSphereRadius));
        m_pShapeProperties->SetGhostObject(ghostObject);
        m_pShapeProperties->SetOffset(pShapeElement->vShapeOffset);
    }
    else if (type == "Box")
    {
		m_pShapeProperties.reset(CHG_NEW BoxProperties(pShapeElement->vBoxDimensions.x, pShapeElement->vBoxDimensions.y, pShapeElement->vBoxDimensions.z));
        m_pShapeProperties->SetGhostObject(ghostObject);
        m_pShapeProperties->SetOffset(pShapeElement->vShapeOffset);
    }
    else if (type == "Capsule")
    {
		m_pShapeProperties.reset(CHG_NEW CapsuleProperties(pShapeElement->fCapsuleRadius, pShapeElement->fCapsuleHeight));
        m_pShapeProperties->SetGhostObject(ghostObject);
        m_pShapeProperties->SetOffset(pShapeElement->vShapeOffset);
    }
    else if (type == "ConvexHull")
    {
		m_pShapeProperties.reset(CHG_NEW ConvexHullProperties(pShapeElement->sShapeResourceName));
        m_pShapeProperties->SetGhostObject(ghostObject);
    }
    else if (type == "TriangleMesh")
    {
		m_pShapeProperties.reset(CHG_NEW TriangleMeshProperties(pShapeElement->sShapeResourceName));
        m_pShapeProperties->SetGhostObject(false);
    }
	else if (type == "HeightField")
	{
		m_pShapeProperties.reset(CHG_NEW HeightFieldProperties(pShapeElement->m_sHeightMapResource, pShapeElement->m_iWidth, pShapeElement->m_iDepth, pShapeElement->m_fMaxHeight, pShapeElement->m_fMinHeight, pShapeElement->m_fWorldWidth, pShapeElement->m_fWorldDepth, pShapeElement->m_sUpAxis, pShapeElement->m_sHeightMapValueType));
	}
    else
    {
        CHG_ERROR("Unknow Shape Type");
    }
}

void PhysicsComponent::BuildRigidBodyConstrains(TiXmlElement* pConstrainElement)
{
	CHG_ASSERT(pConstrainElement);

	TiXmlElement* pRotateConstrainsElement = pConstrainElement->FirstChildElement("Rotate");
	if(pRotateConstrainsElement)
	{
		float x=0, y=0, z=0;

		x = (float)atof(pRotateConstrainsElement->Attribute("x"));
		y = (float)atof(pRotateConstrainsElement->Attribute("y"));
		z = (float)atof(pRotateConstrainsElement->Attribute("z"));

		m_RigidBodyRotateConstrains = Vec3(x, y, z);
	}

	TiXmlElement* pTranslateConstrainsElement = pConstrainElement->FirstChildElement("Translate");
	if(pTranslateConstrainsElement)
	{
		float x=0, y=0, z=0;

		x = (float)atof(pTranslateConstrainsElement->Attribute("x"));
		y = (float)atof(pTranslateConstrainsElement->Attribute("y"));
		z = (float)atof(pTranslateConstrainsElement->Attribute("z"));

		m_RigidBodyTranslateConstrains = Vec3(x, y, z);
	}
}

void PhysicsComponent::BuildRigidBodyShape(TiXmlElement* pShapeElement)
{
	std::string type = pShapeElement->Attribute("Type");
	std::string ghostObject = pShapeElement->Attribute("GhostObject");

	if(type == "Sphere")
	{
		TiXmlElement* pSpherePropertiesElement = pShapeElement->FirstChildElement("SphereProperties");
		if(pSpherePropertiesElement)
		{
			float radius;
			Vec3 offset = Vec3();

			radius = (float)atof(pSpherePropertiesElement->Attribute("Radius"));
			if (pSpherePropertiesElement->Attribute("Offset"))
				offset = Vec3(pSpherePropertiesElement->Attribute("Offset"));

			m_pShapeProperties.reset(new SphereProperties(radius));
			m_pShapeProperties->SetGhostObject(ghostObject == "true" ? true : false);
			m_pShapeProperties->SetOffset(offset);
		}
	}
	else if(type == "Box")
	{
		TiXmlElement* pBoxPropertiesElement = pShapeElement->FirstChildElement("BoxProperties");
		if(pBoxPropertiesElement)
		{
			float width, height, depth;
			Vec3 offset = Vec3();

			width = (float)atof(pBoxPropertiesElement->Attribute("Width"));
			height = (float)atof(pBoxPropertiesElement->Attribute("Height"));
			depth = (float)atof(pBoxPropertiesElement->Attribute("Depth"));
			if (pBoxPropertiesElement->Attribute("Offset"))
				offset = Vec3(pBoxPropertiesElement->Attribute("Offset"));

			m_pShapeProperties.reset(new BoxProperties(width, height, depth));
			m_pShapeProperties->SetGhostObject(ghostObject == "true" ? true : false);
			m_pShapeProperties->SetOffset(offset);
		}
	}
	else if(type == "Capsule")
	{
		TiXmlElement* pCapsulePropertiesElement = pShapeElement->FirstChildElement("CapsuleProperties");
		if(pCapsulePropertiesElement)
		{
			float radius, height;
			Vec3 offset = Vec3();

			radius = (float)atof(pCapsulePropertiesElement->Attribute("Radius"));
			height = (float)atof(pCapsulePropertiesElement->Attribute("Height"));
			if (pCapsulePropertiesElement->Attribute("Offset"))
				offset = Vec3(pCapsulePropertiesElement->Attribute("Offset"));

			m_pShapeProperties.reset(new CapsuleProperties(radius, height));
			m_pShapeProperties->SetGhostObject(ghostObject == "true" ? true : false);
			m_pShapeProperties->SetOffset(offset);
		}
	}
	else if(type == "ConvexHull")
	{
		TiXmlElement* pConvexHullPropertiesElement = pShapeElement->FirstChildElement("ConvexHullProperties");
		if(pConvexHullPropertiesElement)
		{
			std::string meshColliderFilename;
			meshColliderFilename = pConvexHullPropertiesElement->Attribute("MeshColliderSource");

			m_pShapeProperties.reset(new ConvexHullProperties(meshColliderFilename));
			m_pShapeProperties->SetGhostObject(ghostObject == "true" ? true : false);
		}
	}
	else if (type == "TriangleMesh")
	{
		TiXmlElement* pTriangleMeshPropertiesElement = pShapeElement->FirstChildElement("TriangleMeshProperties");
		if (pTriangleMeshPropertiesElement)
		{
			std::string meshColliderFilename;
			meshColliderFilename = pTriangleMeshPropertiesElement->Attribute("MeshColliderSource");

			m_pShapeProperties.reset(new TriangleMeshProperties(meshColliderFilename));
			m_pShapeProperties->SetGhostObject(false);
		}
	}
	else if (type == "HeightField")
	{
		TiXmlElement* pHeightFieldPropertiesElement = pShapeElement->FirstChildElement("HeightFieldProperties");
		if (pHeightFieldPropertiesElement)
		{
			std::string sHeightMapResource = pHeightFieldPropertiesElement->Attribute("HeightmapResource");
			int iWidth = atoi(pHeightFieldPropertiesElement->Attribute("Width"));
			int iDepth = atoi(pHeightFieldPropertiesElement->Attribute("Depth"));
			float fMaxHeight = (float)atof(pHeightFieldPropertiesElement->Attribute("MaxHeight"));
			float fMinHeight = (float)atof(pHeightFieldPropertiesElement->Attribute("MinHeight"));
			float fWorldWidth = (float)atof(pHeightFieldPropertiesElement->Attribute("WorldWidth"));
			float fWorldDepth = (float)atof(pHeightFieldPropertiesElement->Attribute("WorldDepth"));
			std::string sUpAxis = pHeightFieldPropertiesElement->Attribute("UpAxis");
			std::string sHeightMapValueType = pHeightFieldPropertiesElement->Attribute("HeightMapValueType");

			m_pShapeProperties.reset(new HeightFieldProperties(sHeightMapResource, iWidth, iDepth, fMaxHeight, fMinHeight, fWorldWidth, fWorldDepth, sUpAxis, sHeightMapValueType));
			m_pShapeProperties->SetGhostObject(false);
		}
	}
	else
	{
		CHG_ERROR("Unknow Shape Type");
	}
}

void PhysicsComponent::BuildRigidBodyTransform(TiXmlElement* pTransformElement)
{
	// FUTURE WORK Mrmike - this should be exactly the same as the TransformComponent - maybe factor into a helper method?
    CHG_ASSERT(pTransformElement);

    TiXmlElement* pPositionElement = pTransformElement->FirstChildElement("Position");
    if (pPositionElement)
    {
        float x = 0;
        float y = 0;
        float z = 0;
		x = (float)atof(pPositionElement->Attribute("x"));
		y = (float)atof(pPositionElement->Attribute("y"));
		z = (float)atof(pPositionElement->Attribute("z"));
        m_RigidBodyLocation = Vec3(x, y, z);
    }

    TiXmlElement* pOrientationElement = pTransformElement->FirstChildElement("Orientation");
    if (pOrientationElement)
    {
        double yaw = 0;
        double pitch = 0;
        double roll = 0;
        pPositionElement->Attribute("yaw", &yaw);
        pPositionElement->Attribute("pitch", &pitch);
        pPositionElement->Attribute("roll", &roll);
        m_RigidBodyOrientation = Vec3((float)DEGREES_TO_RADIANS(yaw), (float)DEGREES_TO_RADIANS(pitch), (float)DEGREES_TO_RADIANS(roll));
    }

    TiXmlElement* pScaleElement = pTransformElement->FirstChildElement("Scale");
    if (pScaleElement)
    {
        double x = 0;
        double y = 0;
        double z = 0;
        pScaleElement->Attribute("x", &x);
        pScaleElement->Attribute("y", &y);
        pScaleElement->Attribute("z", &z);
        m_RigidBodyScale = Vec3((float)x, (float)y, (float)z);
    }
}

bool PhysicsComponent::CollisionConvexTest(const Mat4x4& start, const Mat4x4& end, const Vec3& dir, float minSlopeDot, TestCollisionAnswer& testCollisionAnswer)
{
	return m_pGamePhysics->VCollisionConvexTest(m_pOwner->GetId(), start, end, dir, minSlopeDot, testCollisionAnswer);
}

void PhysicsComponent::ApplyForce(const Vec3& direction, float forceNewtons)
{
    m_pGamePhysics->VApplyForce(direction, forceNewtons, m_pOwner->GetId());
}

void PhysicsComponent::ApplyTorque(const Vec3& direction, float forceNewtons)
{
    m_pGamePhysics->VApplyTorque(direction, forceNewtons, m_pOwner->GetId());
}

bool PhysicsComponent::KinematicMove(const Mat4x4 &transform)
{
	return m_pGamePhysics->VKinematicMove(transform, m_pOwner->GetId());
}

void PhysicsComponent::ApplyAcceleration(const Vec3& acceleration)
{
	m_acceleration = acceleration;
}

void PhysicsComponent::RemoveAcceleration(void)
{
    m_acceleration = Vec3(0.0f, 0.0f, 0.0f);
}

void PhysicsComponent::ApplyAngularAcceleration(float acceleration)
{
    m_angularAcceleration = acceleration;
}

void PhysicsComponent::RemoveAngularAcceleration(void)
{
    m_angularAcceleration = 0;
}

Vec3 PhysicsComponent::GetVelocity(void)
{
    return m_pGamePhysics->VGetVelocity(m_pOwner->GetId());
}

void PhysicsComponent::SetVelocity(const Vec3& velocity)
{
    m_pGamePhysics->VSetVelocity(m_pOwner->GetId(), velocity);
}

void PhysicsComponent::SetVelocityFactor(const Vec3& velocityFactor)
{
	m_pGamePhysics->VSetVelocityFactor(m_pOwner->GetId(), velocityFactor);
}

Vec3 PhysicsComponent::GetAngularVelocity(void)
{
	return m_pGamePhysics->VGetAngularVelocity(m_pOwner->GetId());
}
void PhysicsComponent::SetAngularVelocity(const Vec3& velocity)
{
	m_pGamePhysics->VSetAngularVelocity(m_pOwner->GetId(), velocity);
}

void PhysicsComponent::RotateY(float angleRadians)
{
    shared_ptr<TransformComponent> pTransformComponent = MakeStrongPtr(m_pOwner->GetComponent<TransformComponent>(TransformComponent::g_Name));
    if (pTransformComponent)
    {
        Mat4x4 transform = pTransformComponent->GetTransform();
        Vec3 position = transform.GetPosition();

        Mat4x4 rotateYMat;
        rotateYMat = rotateY(angleRadians);
        rotateYMat.SetPosition(position);

        KinematicMove(rotateYMat);
    }
    else
        CHG_ERROR("Attempting to call RotateY() on actor with no transform component");
}

void PhysicsComponent::SetPosition(float x, float y, float z)
{
    shared_ptr<TransformComponent> pTransformComponent = MakeStrongPtr(m_pOwner->GetComponent<TransformComponent>(TransformComponent::g_Name));
    if (pTransformComponent)
    {
        Mat4x4 transform = pTransformComponent->GetTransform();
        Vec3 position = Vec3(x, y, z);
        transform.SetPosition(position);

        KinematicMove(transform);
    }
    else
        CHG_ERROR("Attempting to call RotateY() on actor with no trnasform component");
}

void PhysicsComponent::Stop(void)
{
    return m_pGamePhysics->VStopEntity(m_pOwner->GetId());
}

