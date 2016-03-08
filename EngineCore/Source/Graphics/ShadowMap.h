#ifndef _SHADOW_MAP_H_
#define _SHADOW_MAP_H_

#include "CthughaStd.h"
#include "QueryQueue.h"

struct stackBuffer
{
    char* data;
    int size;
    int top;
};

//////////////////////////////////////////////////////////////////
//
// class AtlasTreeNode						
//
// A hierarchical container for help to managing atlas
//
///////////////////////////////////////////////////////////////////

class AtlasTreeNode
{
public:
	AtlasTreeNode(int maxSize, int minSize, stackBuffer* data);
	~AtlasTreeNode();

    void Reset();
    bool addTextureToAtlas(int tam, Vec2 &pos);
	
private:
    int m_Size;
    int m_level;
    int m_NumFrees;
    Vec2 m_AtlasPos;
    int m_MinSize;
    int m_MaxSize;

    bool m_Empty;
    bool m_Full;

    AtlasTreeNode* m_pRoot;
    AtlasTreeNode* m_pParent;
    AtlasTreeNode* m_vpChildren[4];

    //data
    stackBuffer* m_pTreeData;

    //private methods
    void initChilds();

	friend class ShadowMapManager;
};

//////////////////////////////////////////////////////////////////
//
// class ShadowMapManager						
//
// A hierarchical container for help to managing atlas
//
///////////////////////////////////////////////////////////////////

class ShadowMapManager
{
private:
	TextureID m_OmniDirectionalAtlasID;
	TextureID m_DirectionalAtlasID;

	//////////////////////////////////////////////////////////////
	//Shadow Test Attributes
	//////////////////////////////////////////////////////////////
public:
	TextureID m_ShadowTestCoverageRT;
	ShaderID m_ShadowTestCoverageShaderID;
	
	//Cascaded Frustum on Large Directional Light
	ShaderID m_ClearZBoundsShaderID;
	ShaderID m_ReduceZBoundsShaderID;
	ShaderID m_LogPartitionShaderID;

	UINT m_TestQuery;
private:
	//////////////////////////////////////////////////////////////

    ShaderID m_ShadowShaderID;
    VertexFormatID m_ShadowVertexFormatID;
	VertexFormatID m_ShadowTestCoverageVFID;

    //QueryQueue
    std::shared_ptr<QueryQueue> m_pQueryQueue;

	int m_DirectionalAtlasRes;

	int m_MaxLevel, m_MinLevel; //Max Resolution Atlas, Min Resolution Texture
	float m_InverseMaxLevel;
	int m_TotalMemory, m_UsedMemory; //Memory in KB

	AtlasTreeNode* m_pRoot; //Manager texture Atlas space

	stackBuffer* m_pAtlasTreeData; //Secuential region of memory for treeAtlas

	void reset();

public:
	ShadowMapManager(int maxLevel, int minLevel, int directionalAtlasRes);
	~ShadowMapManager();

	bool UpdateShadows(Scene *pScene);
    bool UpdateShadowsV_2(Scene *pScene);

	////////////////////////////////////////////////////////////
	int getMaxLevel() { return m_MaxLevel; }
	int getMinLevel() { return m_MinLevel; }
	float getInverseMaxLevel() { return m_InverseMaxLevel; }

	int getTotalMemory() { return m_TotalMemory; }
	int getUsedMemory() { return m_UsedMemory; }

	TextureID getOmniDirectionalAtlasID() { return m_OmniDirectionalAtlasID; }
};

#endif