//========================================================================
// TransformComponent.cpp - Component for managing transforms on actors
//
//========================================================================

#pragma warning(disable : 4244)

#include "CthughaStd.h"
#include "TransformComponent.h"
#include "Math.h"
#include "../Utilities/String.h"

const char* TransformComponent::g_Name = "TransformComponent";

bool TransformComponent::VInit(TiXmlElement* pData)
{
    CHG_ASSERT(pData);

	Vec3 position, rotate, scaleV;

	TiXmlElement* pScaleElement = pData->FirstChildElement("Scale");
    if (pScaleElement)
    {
        double x = 0;
        double y = 0;
        double z = 0;
        pScaleElement->Attribute("x", &x);
        pScaleElement->Attribute("y", &y);
        pScaleElement->Attribute("z", &z);
		scaleV = Vec3(x, y, z);
	}

	TiXmlElement* pOrientationElement = pData->FirstChildElement("Rotation");
    if (pOrientationElement)
    {
        double yaw = 0;
        double pitch = 0;
        double roll = 0;
        pOrientationElement->Attribute("x", &yaw);
        pOrientationElement->Attribute("y", &pitch);
        pOrientationElement->Attribute("z", &roll);
		rotate = Vec3(yaw, pitch, roll);
	}

	TiXmlElement* pPositionElement = pData->FirstChildElement("Position");
    if (pPositionElement)
    {
        double x = 0;
        double y = 0;
        double z = 0;
        pPositionElement->Attribute("x", &x);
        pPositionElement->Attribute("y", &y);
        pPositionElement->Attribute("z", &z);
        position = Vec3(x, y, z);
    }

	Mat4x4 translation;
	translation = translate(position);
	

	Mat4x4 rotation;
	rotation = rotateXYZ((float)DEGREES_TO_RADIANS(rotate.x), (float)DEGREES_TO_RADIANS(rotate.y), (float)DEGREES_TO_RADIANS(rotate.z));

	Mat4x4 scaleM;
	scaleM = scale(scaleV.x, scaleV.y, scaleV.z);

    m_Position = position;
    m_Scale = scaleV;
    m_Rotation = rotate;

    m_transform = translation * rotation * scaleM;
    
    return true;
}

bool TransformComponent::VInit(std::shared_ptr<ComponentResource> pData)
{
	CHG_ASSERT(pData);

    std::shared_ptr<TransformComponentResourceData> pTransformComponentData = std::static_pointer_cast<TransformComponentResourceData>(pData->GetComponentResourceData());

	Vec3 position, rotate, scaleV;

    scaleV = pTransformComponentData->vScale;
    rotate = pTransformComponentData->vRotation;
    position = pTransformComponentData->vPosition;

	Mat4x4 translation;
	translation = translate(position);


	Mat4x4 rotation;
	rotation = rotateXYZ((float)DEGREES_TO_RADIANS(rotate.x), (float)DEGREES_TO_RADIANS(rotate.y), (float)DEGREES_TO_RADIANS(rotate.z));

	Mat4x4 scaleM;
	scaleM = scale(scaleV.x, scaleV.y, scaleV.z);

	m_Position = position;
	m_Scale = scaleV;
	m_Rotation = rotate;

	m_transform = translation * rotation * scaleM;

	return true;
}

Mat4x4 TransformComponent::RotateAxis(Vec3 vAxis, float fAngle)
{
	Mat4x4 mat = rotateAxis(vAxis, fAngle);
	m_Position = (mat * Vec4(m_Position, 1.0f)).xyz();
	m_transform = mat * m_transform;

	return m_transform;
}

Mat4x4 TransformComponent::RotateXYZ(float fAngleX, float fAngleY, float fAngleZ)
{
	m_Rotation += Vec3(fAngleX, fAngleY, fAngleZ);

	Mat4x4 mat = RotateXYZ(fAngleX, fAngleY, fAngleZ);
	m_Position = (mat * Vec4(m_Position, 1.0f)).xyz();
	m_transform = mat * m_transform;

	return m_transform;
}

Mat4x4 TransformComponent::RotateX(float fAngleX)
{
	m_Rotation.x += fAngleX;

	Mat4x4 mat = rotateX(fAngleX);
	m_Position = (mat * Vec4(m_Position, 1.0f)).xyz();
	m_transform = mat * m_transform;

	return m_transform;
}

Mat4x4 TransformComponent::RotateY(float fAngleY)
{
	m_Rotation.y += fAngleY;

	Mat4x4 mat = rotateY(fAngleY);
	m_Position = (mat * Vec4(m_Position, 1.0f)).xyz();
	m_transform = mat * m_transform;

	return m_transform;
}

Mat4x4 TransformComponent::RotateZ(float fAngleZ)
{
	m_Rotation.z += fAngleZ;

	Mat4x4 mat = rotateZ(fAngleZ);
	m_Position = (mat * Vec4(m_Position, 1.0f)).xyz();
	m_transform = mat * m_transform;

	return m_transform;
}

Mat4x4 TransformComponent::Translate(float fPosX, float fPosY, float fPosZ)
{
	m_Position += Vec3(fPosX, fPosY, fPosZ);

	m_transform = translate(fPosX, fPosY, fPosZ) * m_transform;

	return m_transform;
}

Mat4x4 TransformComponent::Scale(float fScaleX, float fScaleY, float fScaleZ)
{
	m_Scale += Vec3(fScaleX, fScaleY, fScaleZ);

	m_transform = scale(fScaleX, fScaleY, fScaleZ) * m_transform;

	return m_transform;
}

TiXmlElement* TransformComponent::VGenerateXml(void)
{
    TiXmlElement* pBaseElement = CHG_NEW TiXmlElement(VGetName());

    // initial transform -> position
    TiXmlElement* pPosition = CHG_NEW TiXmlElement("Position");
    Vec3 pos(GetPosition());
    pPosition->SetAttribute("x", ToStr(pos.x).c_str());
    pPosition->SetAttribute("y", ToStr(pos.y).c_str());
    pPosition->SetAttribute("z", ToStr(pos.z).c_str());
    pBaseElement->LinkEndChild(pPosition);

    // initial transform -> LookAt
    TiXmlElement* pDirection = CHG_NEW TiXmlElement("YawPitchRoll");
	Vec3 orient(GetRotation());
	orient.x = RADIANS_TO_DEGREES(orient.x);
	orient.y = RADIANS_TO_DEGREES(orient.y);
	orient.z = RADIANS_TO_DEGREES(orient.z);
    pDirection->SetAttribute("x", ToStr(orient.x).c_str());
    pDirection->SetAttribute("y", ToStr(orient.y).c_str());
    pDirection->SetAttribute("z", ToStr(orient.z).c_str());
    pBaseElement->LinkEndChild(pDirection);

    return pBaseElement;
}



