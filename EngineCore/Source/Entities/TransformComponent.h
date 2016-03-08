#pragma once
//========================================================================
// TransformComponent.h - Component for managing transforms on actors
//
//========================================================================

#include "EntityComponent.h"

//---------------------------------------------------------------------------------------------------------------------
// This component implementation is a very simple representation of the physical aspect of an actor.  It just defines 
// the transform and doesn't register with the physics system at all.
//---------------------------------------------------------------------------------------------------------------------
class TransformComponent : public EntityComponent
{
    Mat4x4 m_transform;
	Vec3 m_Scale;
	Vec3 m_Rotation;
	Vec3 m_Position;

public:
	static const char* g_Name;
	virtual const char* VGetName() const { return g_Name; }

    TransformComponent(void) : m_transform(Mat4x4::g_Identity) { }
    virtual bool VInit(TiXmlElement* pData) override;
	virtual bool VInit(std::shared_ptr<ComponentResource> pData) override;
    virtual TiXmlElement* VGenerateXml(void) override;

    // transform functions
    Mat4x4 GetTransform(void) const { return m_transform; }
    void SetTransform(const Mat4x4& newTransform) { m_transform = newTransform; }
    Vec3 GetPosition(void) const { return m_Position; }
    Vec3 GetScale(void) const { return m_Scale; }
    void SetPosition(const Vec3& pos) 
	{
		m_Position = pos;

		m_transform.rows[0].w = m_Position.x;
		m_transform.rows[1].w = m_Position.y;
		m_transform.rows[2].w = m_Position.z;
	}
    Vec3 GetRotation(void) const { return m_Rotation; }

	// Transform Methods
	Mat4x4 RotateAxis(Vec3 vAxis, float fAngle);
	Mat4x4 RotateXYZ(float fAngleX, float fAngleY, float fAngleZ);
	Mat4x4 RotateX(float fAngleX);
	Mat4x4 RotateY(float fAngleY);
	Mat4x4 RotateZ(float fAngleZ);
	Mat4x4 Translate(float fPosX, float fPosY, float fPosZ);
	Mat4x4 Scale(float fScaleX, float fScaleY, float fScaleZ);
};
