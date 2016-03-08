#ifndef _DECAL_H_
#define _DECAL_H_

#include "Material.h"
#include "Shader.h"
#include "Scene.h"

////////////////////////////////////////////
//
//	class DecalNode
//
////////////////////////////////////////////

class DecalNode : public DrawableNode
{
private:
	WeakBaseRenderComponentPtr m_pRenderComponent;

	shared_ptr<MaterialNode> m_pMaterialNode;
	std::shared_ptr<ShaderNode> m_vShaderNode;

	UINT m_uMaskId;

public:
	DecalNode(const EntityId entityId,
		WeakBaseRenderComponentPtr renderComponent,
		std::string sShaderResource,
		std::string sMaterialResource,
		UINT uMaskId,
		const Mat4x4 *world);

	~DecalNode();

	virtual bool VOnRestore(Scene *pScene);
	virtual bool VOnLostDevice(Scene *pScene) { return true; }
	virtual bool VIsVisible(Scene *pScene) const { return this->m_IsVisible; }
	virtual bool VOnRender(Scene *pScene);
	virtual bool VOnSimpleRender(Scene *pScene) override;
	virtual bool OnRenderWithExternalShader(Scene *pScene) override;
	virtual int VGetOrder() override { return m_vShaderNode->m_Shader; };

	bool IsOpaque() { return m_pMaterialNode->IsOpaque(); }

	virtual SceneNodeType VGetType() { return DECAL_NODE; }
};

#endif