//========================================================================
// File: Scene.cpp - implements the container class for 3D Graphics scenes
//
//========================================================================

#include "CthughaStd.h"

#include "..\CthughaEngine\CthughaApp.h"
#include "..\EventManager\EventManager.h"
#include "..\EventManager\Events.h"
#include "..\Utilities\String.h"
#include "..\Graphics\Shader.h"
#include "..\Graphics\SDFShadowManager.h"
#include "..\Graphics\ScreenPostProcessing.h"
#include "..\Graphics\Mesh.h"

#include "Geometry.h"
#include "Light.h"

bool SceneNodeComp(std::shared_ptr<SceneNode> first, std::shared_ptr<SceneNode> second)
{
    return first->VGetOrder() < second->VGetOrder();
}

////////////////////////////////////////////////////
// Scene Implementation
////////////////////////////////////////////////////


//
// Scene::Scene		
//
//    Note: The shared_ptr<IRenderer> was added to allow for both D3D9 and D3D11 renderer implementations.
//          The book only describes D3D11, so to find all the differences, just search for m_Renderer!
//
Scene::Scene(shared_ptr<Renderer> renderer)
{
	m_Root.reset(CHG_NEW OctreeNode());

	m_Root->m_Aabb.isVoid = true;

	m_pRenderer = renderer;

	m_pLightManager.reset(new LightManager());

    //Init screen aligned quad attributes//////////////////////////////////////////////////////////////////

    //Load Ligthing Shader
    Resource LightShaderResource("Shaders\\Deferred_Lighting.shd");
	shared_ptr<ResHandle> LightShaderHandle = g_pApp->m_ResCache->GetHandle(&LightShaderResource);

	if(!LightShaderHandle)
	{
		CHG_ERROR("Not found Lighting Shader");
	}

	m_LightShaderID = ((ShaderResourceExtraData*)LightShaderHandle->GetExtra().get())->m_ShaderID;

	//Load DepthMap Shader
	Resource DepthShaderResource("Shaders\\DepthMapShader.shd");
	shared_ptr<ResHandle> DepthShaderHandle = g_pApp->m_ResCache->GetHandle(&DepthShaderResource);

	if(!DepthShaderHandle)
	{
		CHG_ERROR("Not found DepthMap Shader");
	}

	m_DepthShaderID = ((ShaderResourceExtraData*)DepthShaderHandle->GetExtra().get())->m_ShaderID;

    //Load Test Shader
    Resource TestShaderResource("Shaders\\DebugTargetShader.shd");
	shared_ptr<ResHandle> TestShaderHandle = g_pApp->m_ResCache->GetHandle(&TestShaderResource);

	if(!TestShaderHandle)
	{
		CHG_ERROR("Not found Test Shader");
	}

	m_TestTargetShaderID = ((ShaderResourceExtraData*)TestShaderHandle->GetExtra().get())->m_ShaderID;

	//Load Tile Shader
	Resource TileShaderResource("Shaders\\TiledLighting_Shader.shd");
	shared_ptr<ResHandle> TileShaderHandle = g_pApp->m_ResCache->GetHandle(&TileShaderResource);

	if (!TileShaderHandle)
	{
		CHG_ERROR("Not found Tile Shader");
	}

	m_TileShaderID = ((ShaderResourceExtraData*)TileShaderHandle->GetExtra().get())->m_ShaderID;

    static const float g_quad_vertex_buffer_data[] = 
	{
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		1.0f,  1.0f, 0.0f,
	};
	
	static const float g_unit_cube_vertex_buffer_data[] =
	{
		// Front Face
		-0.5f, 0.5f, -0.5f,
		0.5f, 0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,

		// Right Face
		0.5f, 0.5f, -0.5f,
		0.5f, 0.5f, 0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, 0.5f,

		// Top Face
		-0.5f, 0.5f, 0.5f,
		0.5f, 0.5f, 0.5f,
		-0.5f, 0.5f, -0.5f,
		0.5f, 0.5f, -0.5f,

		// Back Face
		0.5f, 0.5f, 0.5f,
		-0.5f, 0.5f, 0.5f,
		0.5f, -0.5f, 0.5f,
		-0.5f, -0.5f, 0.5f,

		// Left Face
		-0.5f, 0.5f, 0.5f,
		-0.5f, 0.5f, -0.5f,
		-0.5f, -0.5f, 0.5f,
		-0.5f, -0.5f, -0.5f,

		// Bottom Face
		-0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f, 0.5f,
		1.0f, -1.0f, 1.0f
	};

	m_ScreenAlignedQuadVBID = m_pRenderer->addVertexBuffer(18 * sizeof(float), STATIC, g_quad_vertex_buffer_data);
	m_ZeroCenteredUnitCubeVBID = m_pRenderer->addVertexBuffer(72 * sizeof(float), STATIC, g_unit_cube_vertex_buffer_data);

    FormatDesc *vertexDesc = CHG_NEW FormatDesc[5];

	vertexDesc[0].stream = 0;
	vertexDesc[0].format = FORMAT_FLOAT;
	vertexDesc[0].type = TYPE_VERTEX;
	vertexDesc[0].size = 3;

	vertexDesc[1].stream = 1;
	vertexDesc[1].format = FORMAT_FLOAT;
	vertexDesc[1].type = TYPE_NORMAL;
	vertexDesc[1].size = 3;

	vertexDesc[2].stream = 2;
	vertexDesc[2].format = FORMAT_FLOAT;
	vertexDesc[2].type = TYPE_TEXCOORD;
	vertexDesc[2].size = 2;
	
	vertexDesc[3].stream = 3;
	vertexDesc[3].format = FORMAT_FLOAT;
	vertexDesc[3].type = TYPE_TANGENT;
	vertexDesc[3].size = 3;

	vertexDesc[4].stream = 4;
	vertexDesc[4].format = FORMAT_FLOAT;
	vertexDesc[4].type = TYPE_BINORMAL;
	vertexDesc[4].size = 3;

	if (m_DepthShaderID != SHADER_NONE)
		m_DepthShaderVFID = m_pRenderer->addVertexFormat(vertexDesc, 1, m_DepthShaderID);
	if (m_TestTargetShaderID != SHADER_NONE)
		m_TestTargetShaderVFID = m_pRenderer->addVertexFormat(vertexDesc, 1, m_TestTargetShaderID);

    //////////////////////////////////////////////////////////////////////////////////////////////////////////
	
    //Init Target Surfaces////////////////////////////////////////////////////////////////////////////////////
	m_TilePositionRT	= m_pRenderer->addRenderTarget((int)g_pApp->GetScreenSize().x, (int)g_pApp->GetScreenSize().y, 1, 1, 1, FORMAT_R32F, 1, SS_NONE);
    m_PositionRT		= m_pRenderer->addRenderTarget((int)g_pApp->GetScreenSize().x, (int)g_pApp->GetScreenSize().y, 1, 1, 1, FORMAT_R32F, 1, SS_NONE);
	m_ColorRT			= m_pRenderer->addRenderTarget((int)g_pApp->GetScreenSize().x, (int)g_pApp->GetScreenSize().y, 1, 1, 1, FORMAT_RGBA8, 1, SS_NONE);
	m_NormalRT			= m_pRenderer->addRenderTarget((int)g_pApp->GetScreenSize().x, (int)g_pApp->GetScreenSize().y, 1, 1, 1, FORMAT_RGBA16S, 1, SS_NONE);
	m_TangentRT			= m_pRenderer->addRenderTarget((int)g_pApp->GetScreenSize().x, (int)g_pApp->GetScreenSize().y, 1, 1, 1, FORMAT_RGBA8S, 1, SS_NONE);
	m_MREC_RT			= m_pRenderer->addRenderTarget((int)g_pApp->GetScreenSize().x, (int)g_pApp->GetScreenSize().y, 1, 1, 1, FORMAT_RGBA8, 1, SS_NONE);
	m_ForwardRenderSceneRT = m_pRenderer->addRenderTarget((int)g_pApp->GetScreenSize().x, (int)g_pApp->GetScreenSize().y, 1, 1, 1, FORMAT_RGBA16F, 1, SS_NONE);
	m_LightingSceneRT	= m_pRenderer->createStructuredBuffer(sizeof(FramebufferFlatElement), (int)(g_pApp->GetScreenSize().x * g_pApp->GetScreenSize().y), false, true);

	m_DepthRT			= m_pRenderer->addRenderDepth ((int)g_pApp->GetScreenSize().x, (int)g_pApp->GetScreenSize().y, 1,       FORMAT_D24,     1, SS_NONE, SAMPLE_DEPTH);
    //////////////////////////////////////////////////////////////////////////////////////////////////////////

    //Init RenderStates///////////////////////////////////////////////////////////////////////////////////////

    //BlendModes
	m_blendSrcAlpha = m_pRenderer->addBlendState(SRC_ALPHA, ONE_MINUS_SRC_ALPHA);
	m_blendAdd = m_pRenderer->addBlendState(ONE, ONE);

	//RenderStates
	m_backFaceCulling = m_pRenderer->addRasterizerState(CULL_FRONT, SOLID, true, false, 0.0f, 0.0f);
	m_noneFaceCulling = m_pRenderer->addRasterizerState(CULL_NONE, SOLID, true, false, 0.0f, 0.0f);

	// Depth states - use reversed depth (1 to 0) to improve precision
	m_depthTest = m_pRenderer->addDepthState(true, true, LEQUAL);
	m_noDepthTest = m_pRenderer->addDepthState(false, false);
	m_depthTestNoWrite = m_pRenderer->addDepthState(true, false, LEQUAL);

	//SamplerStates
	m_bilinearClamp = m_pRenderer->addSamplerState(BILINEAR, CLAMP, CLAMP, CLAMP);
	m_linearClamp = m_pRenderer->addSamplerState(LINEAR, CLAMP, CLAMP, CLAMP);
	m_pointClamp = m_pRenderer->addSamplerState(NEAREST, CLAMP, CLAMP, CLAMP);
	m_linearWrap = m_pRenderer->addSamplerState(NEAREST, WRAP, WRAP, WRAP);
	m_trilinearWrap = m_pRenderer->addSamplerState(TRILINEAR_ANISO, WRAP, WRAP, WRAP);
	m_trilinearMirror = m_pRenderer->addSamplerState(TRILINEAR_ANISO, MIRROR, MIRROR, MIRROR);
    m_trilinearClamp = m_pRenderer->addSamplerState(TRILINEAR_ANISO, CLAMP, CLAMP, CLAMP);

    m_trilinear[0] = m_trilinearMirror;
    m_trilinear[1] = m_trilinearWrap;
    m_trilinear[2] = m_trilinearClamp;

    //////////////////////////////////////////////////////////////////////////////////////////////////////////

	m_pScreenPostprocessing.reset(CHG_NEW ScreenPostProcessing(this));

	m_tOnCustomPostProcessingEffect = NULL;
	m_pUserContext = NULL;

	m_NumOctreeNodes = 0;
	// event delegates were added post-press
    IEventManager* pEventMgr = IEventManager::Get();
    pEventMgr->VAddListener(MakeDelegate(this, &Scene::NewRenderComponentDelegate), EvtData_New_Render_Component::sk_EventType);
    pEventMgr->VAddListener(MakeDelegate(this, &Scene::DestroyActorDelegate), EvtData_Destroy_Actor::sk_EventType);
    pEventMgr->VAddListener(MakeDelegate(this, &Scene::MoveActorDelegate), EvtData_Move_Actor::sk_EventType);
    pEventMgr->VAddListener(MakeDelegate(this, &Scene::ModifiedRenderComponentDelegate), EvtData_Modified_Render_Component::sk_EventType);
    //pEventMgr->VAddListener(MakeDelegate(this, &Scene::LevelLoadedDelegate), EvtData_Environment_Loaded::sk_EventType);

	delete[] vertexDesc;
}

//
// Scene::~Scene
//
Scene::~Scene()
{
	// event delegates were added post-press!
    IEventManager* pEventMgr = IEventManager::Get();
    pEventMgr->VRemoveListener(MakeDelegate(this, &Scene::NewRenderComponentDelegate), EvtData_New_Render_Component::sk_EventType);
    pEventMgr->VRemoveListener(MakeDelegate(this, &Scene::DestroyActorDelegate), EvtData_Destroy_Actor::sk_EventType);
    pEventMgr->VRemoveListener(MakeDelegate(this, &Scene::MoveActorDelegate), EvtData_Move_Actor::sk_EventType);

    pEventMgr->VRemoveListener(MakeDelegate(this, &Scene::ModifiedRenderComponentDelegate), EvtData_Modified_Render_Component::sk_EventType);
}

//
// Scene::OnRender					- Chapter 16, page 539
//
bool Scene::OnRender()
{
// The render passes

    float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    UINT offset[] = {0};
	static int iFlipTargetIndex = 0;

    // Temporal blur //////////////////////
    static float timeCount = 0.0f;
    static float fps = 1.0f;
    static int ticks = 0;

    fps = (float)ticks / timeCount;

    timeCount += g_pApp->getElapsedTime();
    ticks++;
    ////////////////////////////////////////

	if (m_Root && m_Camera)
	{
		m_Camera->SetViewTransform(this);		        
        iFlipTargetIndex = (iFlipTargetIndex + 1) % 2;

        Mat4x4 inverseViewMat = m_Camera->GetInverseView();
        Mat4x4 inverseProjMat = m_Camera->GetInverseProjection();

        //Frustum Culling
        m_Root->TraverseOctree(this);	//Static Object
		TraverseDynamicObject();	//Dynamic Object

        m_RenderQueue.sort(SceneNodeComp);
		m_ForwardRenderQueue.sort(SceneNodeComp);
		m_DecalRenderQueue.sort(SceneNodeComp);

		//printf("Objects in Screen %d\n", m_RenderQueue.size());
		//printf("Lights in Screen %d\n", m_LightQueue.size());

        //Depth Pre Pass////////////////////////////////////////////////////////////////////////////////////////
		m_RenderPass = DEPTH_STATE;

        m_pRenderer->setViewport((int)(g_pApp->GetScreenSize().x), (int)(g_pApp->GetScreenSize().y));

		TextureID DepthBufferRT[] = { m_TilePositionRT };
		m_pRenderer->changeRenderTargets(DepthBufferRT, elementsOf(DepthBufferRT), m_DepthRT);

	    m_pRenderer->clear(true, true, false, ClearColor, 1.0f);
			
			m_pRenderer->reset();

		    m_pRenderer->setRasterizerState(m_backFaceCulling);
		    m_pRenderer->setDepthState(m_depthTest);

			g_pApp->m_pRenderer->setShader(m_DepthShaderID);
			g_pApp->m_pRenderer->setVertexFormat(m_DepthShaderVFID);

			g_pApp->m_pRenderer->setShaderConstant4x4f("View", *((Mat4x4*)&GetCamera()->GetView()));
			g_pApp->m_pRenderer->setShaderConstant4x4f("Projection", *((Mat4x4*)&GetCamera()->GetProjection()));

			for (auto object : m_RenderQueue)
			{
				object->VOnRender(this);
			}

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////

		// Update SDF Object Scene //////////////////////////////////////////////////////////////////////////////////////

		g_pApp->m_pSDFShadowManager->OnUpdateSDFObjectParameter(this);

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		//Update Voxel Texture//////////////////////////////////////////////////////////////////////////////////////////

		g_pApp->m_pVoxelManager->UpdateGICascades(this);

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		
		//fill GBuffers//////////////////////////////////////////////////////////////////////////////////////////////////

        m_RenderPass = FILLBUFFER_STATE;

		m_pRenderer->setViewport((int)g_pApp->GetScreenSize().x, (int)g_pApp->GetScreenSize().y);
		TextureID bufferRTs[] = { m_PositionRT, m_NormalRT, m_ColorRT, m_MREC_RT, m_TangentRT };
	    m_pRenderer->changeRenderTargets(bufferRTs, elementsOf(bufferRTs), m_DepthRT);

	    m_pRenderer->clear(true, false, false, ClearColor, 1.0f);

		    m_pRenderer->reset();

		    m_pRenderer->setRasterizerState(m_backFaceCulling);
		    //m_pRenderer->setShaderConstant4f("cameraPos", m_Camera.getPosition());	

		    m_pRenderer->setDepthState(m_depthTest);

            while(!m_RenderQueue.empty())
			{
				m_RenderQueue.back()->VOnRender(this);
				RemoveLastNodeToRenderQueue();
			}

		TextureID decalsRTs[] = { m_NormalRT, m_ColorRT, m_MREC_RT };
		m_pRenderer->changeRenderTargets(decalsRTs, elementsOf(decalsRTs), m_DepthRT);

			g_pApp->m_pRenderer->setDepthState(m_depthTestNoWrite);
			m_pRenderer->setBlendState(m_blendSrcAlpha);

			while (!m_DecalRenderQueue.empty())
			{
				m_DecalRenderQueue.back()->VOnRender(this);
				RemoveLastNodeToDecalRenderQueue();
			}

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		
		//Forward DepthMap Pass////////////////////////////////////////////////////////////////////////////////////////
		m_RenderPass = DEPTH_STATE;

		m_pRenderer->setViewport((int)(g_pApp->GetScreenSize().x), (int)(g_pApp->GetScreenSize().y));

		m_pRenderer->changeRenderTargets(DepthBufferRT, elementsOf(DepthBufferRT), m_DepthRT);

		//m_pRenderer->clear(false, false, false, ClearColor, 1.0f);

		m_pRenderer->reset();

		m_pRenderer->setRasterizerState(m_backFaceCulling);
		m_pRenderer->setDepthState(m_depthTest);

		g_pApp->m_pRenderer->setShader(m_DepthShaderID);
		g_pApp->m_pRenderer->setVertexFormat(m_DepthShaderVFID);

		g_pApp->m_pRenderer->setShaderConstant4x4f("View", *((Mat4x4*)&GetCamera()->GetView()));
		g_pApp->m_pRenderer->setShaderConstant4x4f("Projection", *((Mat4x4*)&GetCamera()->GetProjection()));

		for (auto object : m_ForwardRenderQueue)
		{
			object->VOnRender(this);
		}

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////

		//Voxel Cone Tracing Pass///////////////////////////////////////////////////////////////////////////////////////////////

		g_pApp->m_pVoxelManager->VoxelConeTracing(this);

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		// Update SDF Shadow Data ///////////////////////////////////////////////////////////////////////////////////////////

		g_pApp->m_pSDFShadowManager->OnUpdatePointDirectionalLightAndCullingObject(this);

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		// Tile Lighting Pass ///////////////////////////////////////////////////////////////////////////////////////////////

		m_RenderPass = LIGHTING_STATE;
		m_pRenderer->changeRenderTargets(NULL, 0, TEXTURE_NONE);

			m_pRenderer->reset();

			m_pRenderer->setShader(m_TileShaderID);

			m_pRenderer->setTexture("TranslucentDepthTarget", m_TilePositionRT);
			m_pRenderer->setTexture("OpaqueDepthTarget", m_PositionRT);

			m_pRenderer->setShaderConstant2f("vLightingBufferDimensions", g_pApp->GetScreenSize());
			m_pRenderer->setShaderConstant2f("vCameraNearFar", Vec2(GetCamera()->GetFrustum().m_Near, GetCamera()->GetFrustum().m_Far));
			m_pRenderer->setShaderConstant4x4f("mProjection", m_Camera->GetProjection());

			m_pLightManager->UpdateLightBuffers(this);
			m_pLightManager->SetLightBuffers(this);

			unsigned int dispatchWidth = m_pLightManager->GetNumWidthTiles();
			unsigned int dispatchHeight = m_pLightManager->GetNumHeightTiles();

			m_pRenderer->setShaderConstant1i("uNum_Groups_X", dispatchWidth);

			m_pRenderer->apply();

			// Dispatch
			m_pRenderer->dispatchCompute(dispatchWidth, dispatchHeight, 1);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		
		// Volume Lighting Pass ////////////////////////////////////////////////////////////////////////////////////////////

		g_pApp->m_pVoxelManager->SolveVolumeLighting(this);

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		//Lighting pass////////////////////////////////////////////////////////////////////////////////////////////////////

            m_pRenderer->reset();

		    m_pRenderer->setShader(m_LightShaderID);

			m_pRenderer->setTexture("depthTarget", m_PositionRT);
			m_pRenderer->setTexture("normalTarget", m_NormalRT);
			m_pRenderer->setTexture("baseTarget", m_ColorRT);
			m_pRenderer->setTexture("MREC_Target", m_MREC_RT);
            m_pRenderer->setTexture("indirectRadianceTarget", g_pApp->m_pVoxelManager->getIndirectDiffuseRT());

			m_pRenderer->setSamplerState("LinearClamp", m_linearClamp);

			m_pRenderer->setShaderConstant4x4f("mView", m_Camera->GetView());
            m_pRenderer->setShaderConstant4x4f("mInverseView", inverseViewMat);
			m_pRenderer->setShaderConstant4x4f("mInverseProjection", inverseProjMat);

			m_pRenderer->setStructuredBuffer("sbLightingBuffer", m_LightingSceneRT);
			m_pRenderer->setShaderConstant2f("vLightingBufferDimensions", g_pApp->GetScreenSize());

			m_pRenderer->setShaderConstant1i("uNum_Groups_X", dispatchWidth);

			g_pApp->m_pRenderer->setShaderConstant1i("iActiveShadows", 1);

			g_pApp->m_pSDFShadowManager->SetShaderSDFBuffersForLight(this);
			m_pLightManager->SetLightBuffers(this);

			m_pRenderer->apply();

			// Dispatch
			m_pRenderer->dispatchCompute(dispatchWidth, dispatchHeight, 1);

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		// FORWARD RENDERING PASS //////////////////////////////////////////////////////////////////////////////////////////////////////

		m_pRenderer->setViewport((int)g_pApp->GetScreenSize().x, (int)g_pApp->GetScreenSize().y);
		TextureID ForwardBufferRTs[] = { m_ForwardRenderSceneRT };
		m_pRenderer->changeRenderTargets(ForwardBufferRTs, elementsOf(ForwardBufferRTs), m_DepthRT);

		m_pRenderer->clear(true, false, false, ClearColor, 1.0f);

			m_pRenderer->reset();

			m_pRenderer->setRasterizerState(m_backFaceCulling);
			//m_pRenderer->setShaderConstant4f("cameraPos", m_Camera.getPosition());	

			m_pRenderer->setDepthState(m_depthTest);

			while (!m_ForwardRenderQueue.empty())
			{
				m_ForwardRenderQueue.back()->VOnRender(this);
				RemoveLastNodeToFordwardRenderQueue();
			}

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		//Custom Postprocessing Pass////////////////////////////////////////////////////////////////////////////////////////////////////
		if (m_tOnCustomPostProcessingEffect)
			m_tOnCustomPostProcessingEffect(this, m_pUserContext);
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		//PostProcessing Pass///////////////////////////////////////////////////////////////////////////////////////////////////////////

		m_pScreenPostprocessing->OnRenderPostProcessing();

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		// Voxel Debug //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		m_pRenderer->setViewport((int)g_pApp->GetScreenSize().x, (int)g_pApp->GetScreenSize().y);
		m_pRenderer->changeToMainFramebuffer();

		g_pApp->m_pVoxelManager->DrawVoxels(0, this);
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        //Test pass
		/*m_pRenderer->setViewport(256, 720);
        m_pRenderer->changeToMainFramebuffer();

        //m_pRenderer->clear(true, true, false, ClearColor, 1.0f);
		
		    m_pRenderer->reset();

		    m_pRenderer->setShader(m_TestTargetShaderID);
		    m_pRenderer->setVertexFormat(m_TestTargetShaderVFID);
		    m_pRenderer->setVertexBuffer(0, 1, &m_ScreenAlignedQuadVBID, offset);
			m_pRenderer->setDepthState(m_noDepthTest);
			m_pRenderer->setRasterizerState(m_noneFaceCulling);

			float depthLevel = (g_pApp->getTick() % 50000)/50000.0f;

			m_pRenderer->setTexture("voxelTexture", g_pApp->m_pVoxelManager->m_rVoxelGridMipMaps[0]);
			m_pRenderer->setSamplerState("baseFilter", m_trilinearClamp);
			m_pRenderer->setShaderConstant1f("depthLevel", depthLevel);
			m_pRenderer->setShaderConstant1f("mipLevel", 0);
			m_pRenderer->setShaderConstant1i("cascadeResolution", 128);

			m_pRenderer->apply();

			m_pRenderer->drawArrays(PRIM_TRIANGLES, 0, 6);*/
	}

	return true;
}	

//
// Scene::AddNodeToRenderQueue	
//
void Scene::AddNodeToRenderQueue(shared_ptr<SceneNode> node)
{ 
	if (node->VGetType() == MESH_NODE)
	{
		MeshNode* pMeshNode = (MeshNode*)node.get();
		
		if (pMeshNode->IsOpaque())
			m_RenderQueue.push_back(node);
		else
			m_ForwardRenderQueue.push_back(node);
	}
	else if (node->VGetType() == DECAL_NODE)
	{
		m_DecalRenderQueue.push_back(node);
	}
	else
	{
		m_RenderQueue.push_back(node);
	}	
	
	node->m_InQueueRender = true;
}

//
// Scene::OnLostDevice						- not in the book
//
//    All Scene nodes implement VOnLostDevice, which is called in the D3D9 renderer.
//
bool Scene::OnLostDevice()
{
	if (m_Root)
	{
		return m_Root->VOnLostDevice(this);
	}
	return true;
}

//
// Scene::OnRestore	
//
bool Scene::OnRestore()
{
	if (!m_Root)
		return true;

	//V_RETURN(m_Renderer->VOnRestore());

	return m_Root->VOnRestore(this);
}

bool Scene::AddChild(EntityId id, shared_ptr<ISceneNode> kid)
{ 
	if (id != INVALID_ENTITY_ID)
	{
		// This allows us to search for this later based on actor id
		m_ActorMap[id] = kid;	
	}

    std::shared_ptr<SceneNode> node = std::static_pointer_cast<SceneNode>(kid);

    m_lStaticSceneNodes.push_back(kid);
    m_Root->m_Aabb = m_Root->m_Aabb.Union(kid->VGetAabb().Transform(node->ToWorld()));

    return true; 
}

bool Scene::AddDynamicSceneNode(EntityId id, shared_ptr<ISceneNode> object)
{
	if (id != INVALID_ENTITY_ID)
	{
		m_ActorMap[id] = object;
	}

	m_DynamicObjectList[id] = object;

	return true;
}

bool Scene::BuildOctree()
{
    m_Root->AddChildrenToNode(this, m_lStaticSceneNodes);

    return true;
}

bool Scene::RemoveChild(EntityId id)
{
	if (id == INVALID_ENTITY_ID)
		return false;
	
	shared_ptr<SceneNode> kid = std::static_pointer_cast<SceneNode>(FindEntity(id));

	if (!kid)
		return false;

	bool bIsStatic = kid->m_bIsStatic;
	m_ActorMap.erase(id);

	if (bIsStatic)
		return m_Root->VRemoveChild(id);
	else
	{
		m_DynamicObjectList.erase(id);
		return true;
	}
}

void Scene::TraverseDynamicObject()
{
	for (std::map<EntityId, shared_ptr<ISceneNode>>::iterator it = m_DynamicObjectList.begin(); it != m_DynamicObjectList.end(); ++it)
	{
		std::shared_ptr<SceneNode> object = static_pointer_cast<SceneNode>(it->second);

		if(object->IsVisibleFromCamera(this))
		{
			if(!object->VIsInRenderQueue())
			{
				AddNodeToRenderQueue(object);
				
				if(object->VGetType() == LIGHT_NODE)
					AddNodeToLightQueue(object);
			}
		}
	}
}

void Scene::TraverseDynamicObjectFromSpecificFrustum(std::vector<std::shared_ptr<SceneNode>> &frustumQueue, Frustum frustum, Mat4x4 frustumMat)
{
	for (std::map<EntityId, shared_ptr<ISceneNode>>::iterator it = m_DynamicObjectList.begin(); it != m_DynamicObjectList.end(); ++it)
	{
		std::shared_ptr<SceneNode> object = static_pointer_cast<SceneNode>(it->second);

		if(object->IsVisibleFromFrustum(frustum, frustumMat, this))
		{
			if(!object->IsInFrustumQueue())
			{
				if (object->VGetType() == MESH_NODE)
				{
					frustumQueue.push_back(object);
					object->SetInFrustumQueue(true);
				}
			}
		}
	}
}

void Scene::TraverseDynamicObjectFromAabb(std::list<std::shared_ptr<SceneNode>> &AabbQueue, std::list<std::shared_ptr<SceneNode>> &AabbLightQueue, Aabb aabb)
{
	for (std::map<EntityId, shared_ptr<ISceneNode>>::iterator it = m_DynamicObjectList.begin(); it != m_DynamicObjectList.end(); ++it)
	{
		std::shared_ptr<SceneNode> object = static_pointer_cast<SceneNode>(it->second);

		if(object->IntersectAabb(aabb, this))
		{
			if(!object->IsInFrustumQueue())
			{
				AabbQueue.push_back(object);

				if(object->VGetType() == LIGHT_NODE)
					AabbLightQueue.push_back(object);

				object->SetInFrustumQueue(true);
			}
		}
	}
}

void Scene::TraverseDynamicObjectFromAabb(std::vector<std::shared_ptr<SceneNode>> &AabbLightQueue, Aabb aabb)
{
	for (std::map<EntityId, shared_ptr<ISceneNode>>::iterator it = m_DynamicObjectList.begin(); it != m_DynamicObjectList.end(); ++it)
	{
		std::shared_ptr<SceneNode> object = static_pointer_cast<SceneNode>(it->second);

		if (object->IntersectAabb(aabb, this))
		{
			if (!object->IsInFrustumQueue())
			{
				if (object->VGetType() == LIGHT_NODE)
				{
					AabbLightQueue.push_back(object);
					object->SetInFrustumQueue(true);
				}			
			}
		}
	}
}

std::list<std::shared_ptr<SceneNode>> Scene::TraverseDynamicObjectFromAabb(const Aabb aabb)
{
	std::list<std::shared_ptr<SceneNode>> nodes;

	for (std::map<EntityId, shared_ptr<ISceneNode>>::iterator it = m_DynamicObjectList.begin(); it != m_DynamicObjectList.end(); ++it)
	{
		std::shared_ptr<SceneNode> object = static_pointer_cast<SceneNode>(it->second);

		if (object->IntersectAabb(aabb, this))
		{
			if (!object->IsInFrustumQueue())
			{
				if (object->VGetType() == MESH_NODE)
				{
					nodes.push_back(object);
				}
			}
		}
	}

	return nodes;
}

void Scene::NewRenderComponentDelegate(IEventDataPtr pEventData)
{
    shared_ptr<EvtData_New_Render_Component> pCastEventData = static_pointer_cast<EvtData_New_Render_Component>(pEventData);

    EntityId actorId = pCastEventData->GetActorId();
    shared_ptr<SceneNode> pSceneNode(pCastEventData->GetSceneNode());
	bool bIsStatic = pCastEventData->IsStaticNode();

    // FUTURE WORK: Add better error handling here.		
    if (FAILED(pSceneNode->VOnRestore(this)))
    {
		std::string error = "Failed to restore scene node to the scene for actorid " + ToStr(actorId);
        CHG_ERROR(error);
        return;
    }

	pSceneNode->m_bIsStatic = bIsStatic;

	if(bIsStatic)
		AddChild(actorId, pSceneNode);
	else
		AddDynamicSceneNode(actorId, pSceneNode);
}

void Scene::ModifiedRenderComponentDelegate(IEventDataPtr pEventData)
{
    shared_ptr<EvtData_Modified_Render_Component> pCastEventData = static_pointer_cast<EvtData_Modified_Render_Component>(pEventData);

    EntityId actorId = pCastEventData->GetActorId();
	if (actorId == INVALID_ENTITY_ID)
	{
		CHG_ERROR("Scene::ModifiedRenderComponentDelegate - unknown actor id!");
		return;
	}

	if (g_pApp->GetGameLogic()->GetState()==BGS_LoadingGameEnvironment)
		return;

	shared_ptr<ISceneNode> pSceneNode = FindEntity(actorId);
    // FUTURE WORK: Add better error handling here.		
    if (!pSceneNode  || FAILED(pSceneNode->VOnRestore(this)))
    {
        CHG_ERROR("Failed to restore scene node to the scene for actorid " + ToStr(actorId));
    }
}

void Scene::DestroyActorDelegate(IEventDataPtr pEventData)
{
    shared_ptr<EvtData_Destroy_Actor> pCastEventData = static_pointer_cast<EvtData_Destroy_Actor>(pEventData);
    RemoveChild(pCastEventData->GetId());
}

void Scene::MoveActorDelegate(IEventDataPtr pEventData)
{
    shared_ptr<EvtData_Move_Actor> pCastEventData = static_pointer_cast<EvtData_Move_Actor>(pEventData);

    EntityId id = pCastEventData->GetId();
    Mat4x4 transform = pCastEventData->GetMatrix();

    shared_ptr<ISceneNode> pNode = FindEntity(id);
    if (pNode)
    {
        pNode->VSetTransform(&transform);

		if (pNode->VGetType() == MESH_NODE)
		{

		}
    }
}

//void Scene::LevelLoadedDelegate(IEventDataPtr pEventData)
//{
//    BuildOctree();
//}

//
// Scene::OnUpdate					
//
bool Scene::OnUpdate(const int deltaMilliseconds)
{
	if (!m_Root)
		return true;

    /*static DWORD lastTime = g_pApp->getTick();
	DWORD elapsedTime = 0;
	DWORD now = g_pApp->getTick();

	elapsedTime = now - lastTime;
	lastTime = now;*/

	m_Root->VOnUpdate(this, deltaMilliseconds);
	
	for (auto sceneNode : m_DynamicObjectList)
	{
		sceneNode.second->VOnUpdate(this, deltaMilliseconds);
	}

	return true;
}

//
// Scene::FindEntity					
//
shared_ptr<ISceneNode> Scene::FindEntity(EntityId id)
{
	SceneActorMap::iterator i = m_ActorMap.find(id);
	if (i==m_ActorMap.end())
	{
		return shared_ptr<ISceneNode>();
	}

	return i->second;
}


