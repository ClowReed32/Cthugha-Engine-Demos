#pragma once
//========================================================================
// GUIComponentInterface.h - Interface for GUI components
//
//========================================================================

#include "EntityComponent.h"

class GUIComponentInterface : public EntityComponent
{
public:
	//    virtual ComponentId VGetComponentId(void) const override { return COMPONENT_ID; }

	// RenderComponentInterface
	virtual shared_ptr<UIFunction> VGetUIFunction(void) = 0;
	virtual GameViewId VGetGameViewID(void) = 0;
};
