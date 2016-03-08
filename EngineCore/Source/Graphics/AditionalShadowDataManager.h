#ifndef SDFSHADOW_H_
#define SDFSHADOW_H_

//========================================================================
// AdditionalShadowDataManager.h - Help to set additional data for custom
//                  shadow objects.                  
//
//========================================================================

#include "CthughaStd.h"

//////////////////////////////////////////////////////////////////////////
//
//	AdditionalShadowDataManager class definition
//
//////////////////////////////////////////////////////////////////////////

class AdditionalShadowDataManager
{
private:
	std::vector<bool(*)(Scene* pScene, void* pUserContext)> m_ltOnRenderShadowMethods;
	std::vector<void*> m_lpUserContexts;

public:
	AdditionalShadowDataManager() {}

	bool SetAdditionalDataShadows(Scene *pScene);

	int AddAdditionalDataShadows(bool(*pOnRenderShadowMethods)(Scene* pScene, void* pUserContext), void* pUserContext);
	void RemoveAdditionalDataShadows(int index);
};

#endif