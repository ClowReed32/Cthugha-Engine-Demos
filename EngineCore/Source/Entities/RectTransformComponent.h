#pragma once
//==============================================================================
// RectTransformComponent.h - Component for managing 2D transforms on GUI Elements
//
//==============================================================================

#include "EntityComponent.h"

class RectTransformComponent : public EntityComponent
{
	Mat4x4 m_transform;
	Vec2 m_Scale;
	Vec2 m_Position;
	Vec2 m_RectDimensions;

public:
	static const char* g_Name;
	virtual const char* VGetName() const { return g_Name; }

	RectTransformComponent(void) : m_transform(Mat4x4::g_Identity) { }
	virtual bool VInit(TiXmlElement* pData) override;
    virtual bool VInit(std::shared_ptr<ComponentResource> pComponentResource) override { return true; }
	virtual TiXmlElement* VGenerateXml(void) override;

	// transform functions
	Mat4x4 GetTransform(void) const { return m_transform; }
	void SetTransform(const Mat4x4& newTransform) { m_transform = newTransform; }
	Vec2 GetPosition(void) const { return m_Position; }
	Vec2 GetScale(void) const { return m_Scale; }
	void SetPosition(const Vec2& pos)
	{
		m_Position = pos;

		m_transform.rows[0].w = m_Position.x;
		m_transform.rows[1].w = m_Position.y;
		m_transform.rows[2].w = 0.0f;
	}
	Vec2 GetRectDimension() { return m_RectDimensions; }
};