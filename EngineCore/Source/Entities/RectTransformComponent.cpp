//============================================================================
// RectTransformComponent.cpp - Component for managing transforms on GUI elements
//
//============================================================================

#pragma warning(disable : 4244)

#include "CthughaStd.h"
#include "RectTransformComponent.h"

const char* RectTransformComponent::g_Name = "RectTransformComponent";

bool RectTransformComponent::VInit(TiXmlElement* pData)
{
	CHG_ASSERT(pData);

	Vec2 position, scaleV, rectDimensions;

	TiXmlElement* pScaleElement = pData->FirstChildElement("Scale");
	if (pScaleElement)
	{
		double x = 0;
		double y = 0;
		pScaleElement->Attribute("x", &x);
		pScaleElement->Attribute("y", &y);
		scaleV = Vec2(x, y);
	}

	TiXmlElement* pPositionElement = pData->FirstChildElement("Position");
	if (pPositionElement)
	{
		double x = 0;
		double y = 0;
		pPositionElement->Attribute("x", &x);
		pPositionElement->Attribute("y", &y);
		position = Vec2(x, y);
	}

	TiXmlElement* pDimensionElement = pData->FirstChildElement("Dimension");
	if (pDimensionElement)
	{
		double x = 0;
		double y = 0;
		pDimensionElement->Attribute("width", &x);
		pDimensionElement->Attribute("height", &y);
		m_RectDimensions = Vec2(x, y);
	}

	Mat4x4 translation;
	translation = translate(position.x, position.y, 0.0f);

	Mat4x4 scaleM;
	scaleM = scale(scaleV.x*m_RectDimensions.x, scaleV.y*m_RectDimensions.y, 1.0f);

	m_Position = position;
	m_Scale = scaleV;

	m_transform = translation * scaleM;

	return true;
}

TiXmlElement* RectTransformComponent::VGenerateXml(void)
{
	TiXmlElement* pBaseElement = CHG_NEW TiXmlElement(VGetName());

	// initial transform -> position
	TiXmlElement* pPosition = CHG_NEW TiXmlElement("Position");
	Vec2 pos(GetPosition());
	pPosition->SetAttribute("x", ToStr(pos.x).c_str());
	pPosition->SetAttribute("y", ToStr(pos.y).c_str());
	pBaseElement->LinkEndChild(pPosition);

	/***
	// This is not supported yet
	// initial transform -> position
	TiXmlElement* pScale = GCC_NEW TiXmlElement("Scale");
	pPosition->SetAttribute("x", ToStr(m_scale.x).c_str());
	pPosition->SetAttribute("y", ToStr(m_scale.y).c_str());
	pPosition->SetAttribute("z", ToStr(m_scale.z).c_str());
	pBaseElement->LinkEndChild(pScale);
	**/

	return pBaseElement;
}