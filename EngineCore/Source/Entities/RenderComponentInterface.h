#pragma once
//========================================================================
// RenderComponentInterface.h - Interface for render components
//
//========================================================================

#include "EntityComponent.h"

class RenderComponentInterface : public EntityComponent
{
public:
//    virtual ComponentId VGetComponentId(void) const override { return COMPONENT_ID; }

    // RenderComponentInterface
    virtual shared_ptr<SceneNode> VGetSceneNode(void) = 0;
};
