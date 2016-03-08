//========================================================================
// SDFShadowManager.cpp : implement SDFShadowManager class
//
//========================================================================

#include "CthughaStd.h"

#include "AditionalShadowDataManager.h"

//	TerrainMesh class implementation

bool AdditionalShadowDataManager::SetAdditionalDataShadows(Scene *pScene)
{
    // Render custom shadow generation
    for (UINT i = 0; i < m_ltOnRenderShadowMethods.size(); i++)
    {
		m_ltOnRenderShadowMethods[i](pScene, m_lpUserContexts[i]);
    }
    
    return true;
}

int AdditionalShadowDataManager::AddAdditionalDataShadows(bool(*pOnRenderShadowMethods)(Scene* pScene, void* pUserContext), void* pUserContext)
{
	m_ltOnRenderShadowMethods.push_back(pOnRenderShadowMethods);
	m_lpUserContexts.push_back(pUserContext);

    return m_ltOnRenderShadowMethods.size() - 1;
}

void AdditionalShadowDataManager::RemoveAdditionalDataShadows(int index)
{
	m_lpUserContexts.erase(m_lpUserContexts.begin() + index);
	m_ltOnRenderShadowMethods.erase(m_ltOnRenderShadowMethods.begin() + index);
}