//========================================================================
// File: ScreenPostProcessing.cpp - implement ScreenPostProcessing classes
//
//========================================================================

#include "CthughaStd.h"

#include "..\Graphics\Shader.h"
#include "ScreenPostProcessing.h"

// ScreenPostProcessing class Implementation

ScreenPostProcessing::ScreenPostProcessing(Scene* pScene)
{
	m_pScene = pScene;
	m_pRenderer = pScene->m_pRenderer;
	m_iFlipTargetIndex = 0;

	m_bSsaoActive = true;
	m_bBloomActive = true;
	m_bSmaaActive = false;

	//Load PostProcessing Shader
	Resource PostProcessingShaderResource("Shaders\\PostProcessingShader.shd");
	shared_ptr<ResHandle> PostProcessingShaderHandle = g_pApp->m_ResCache->GetHandle(&PostProcessingShaderResource);

	if (!PostProcessingShaderHandle)
	{
		CHG_ERROR("Not found Test Shader");
	}

	m_PostProcessingShaderID = ((ShaderResourceExtraData*)PostProcessingShaderHandle->GetExtra().get())->m_ShaderID;

	//Load SSAO Shader
	Resource SSAOPostProcessingShaderResource("Shaders\\SsaoShader.shd");
	shared_ptr<ResHandle> SSAOPostProcessingShaderHandle = g_pApp->m_ResCache->GetHandle(&SSAOPostProcessingShaderResource);

	if (!SSAOPostProcessingShaderHandle)
	{
		CHG_ERROR("Not found SSAO PostProcessing Shader");
	}

	m_SSAOPostProcessingShaderID = ((ShaderResourceExtraData*)SSAOPostProcessingShaderHandle->GetExtra().get())->m_ShaderID;

	//Load Vertical Blur SSAO Shader
	Resource VBlurSsaoShaderResource("Shaders\\VerticalGaussianShader.shd");
	shared_ptr<ResHandle> VBlurSsaoShaderHandle = g_pApp->m_ResCache->GetHandle(&VBlurSsaoShaderResource);

	if (!VBlurSsaoShaderHandle)
	{
		CHG_ERROR("Not found SSAO PostProcessing Shader");
	}

	m_VerticalBlurSsaoShaderID = ((ShaderResourceExtraData*)VBlurSsaoShaderHandle->GetExtra().get())->m_ShaderID;

	//Load Horizontal Blur SSAO Shader
	Resource HBlurSsaoShaderResource("Shaders\\HorizontalGaussianShader.shd");
	shared_ptr<ResHandle> HBlurSsaoShaderHandle = g_pApp->m_ResCache->GetHandle(&HBlurSsaoShaderResource);

	if (!HBlurSsaoShaderHandle)
	{
		CHG_ERROR("Not found SSAO PostProcessing Shader");
	}

	m_HorizontalBlurSsaoShaderID = ((ShaderResourceExtraData*)HBlurSsaoShaderHandle->GetExtra().get())->m_ShaderID;

	//Load Temporal Blur Shader
	Resource TemporalBlurShaderResource("Shaders\\TemporalBlurShader.shd");
	shared_ptr<ResHandle> TemporalBlurShaderHandle = g_pApp->m_ResCache->GetHandle(&TemporalBlurShaderResource);

	if (!TemporalBlurShaderHandle)
	{
		CHG_ERROR("Not found Horizontal Blur Shader");
	}

	m_TemporalBlurSsaoShaderID = ((ShaderResourceExtraData*)TemporalBlurShaderHandle->GetExtra().get())->m_ShaderID;

	FormatDesc *vertexDesc = CHG_NEW FormatDesc[1];

	vertexDesc[0].stream = 0;
	vertexDesc[0].format = FORMAT_FLOAT;
	vertexDesc[0].type = TYPE_VERTEX;
	vertexDesc[0].size = 3;

	if (m_PostProcessingShaderID != SHADER_NONE)
		m_PostProcessingShaderVFID = m_pRenderer->addVertexFormat(vertexDesc, 1, m_PostProcessingShaderID);
	if (m_SSAOPostProcessingShaderID != SHADER_NONE)
		m_SSAOPostProcessingShaderVFID = m_pRenderer->addVertexFormat(vertexDesc, 1, m_SSAOPostProcessingShaderID);
	if (m_VerticalBlurSsaoShaderID != SHADER_NONE)
		m_VerticalBlurSsaoShaderVFID = m_pRenderer->addVertexFormat(vertexDesc, 1, m_VerticalBlurSsaoShaderID);
	if (m_HorizontalBlurSsaoShaderID != SHADER_NONE)
		m_HorizontalBlurSsaoShaderVFID = m_pRenderer->addVertexFormat(vertexDesc, 1, m_HorizontalBlurSsaoShaderID);
	if (m_TemporalBlurSsaoShaderID != SHADER_NONE)
		m_TemporalBlurSsaoShaderVFID = m_pRenderer->addVertexFormat(vertexDesc, 1, m_TemporalBlurSsaoShaderID);

	//Ssao RTs /////////////////////////////////////////////////////////////
	m_SSAOPostProcessingRT[0] = m_pRenderer->addRenderTarget((int)g_pApp->GetScreenSize().x / 2, (int)g_pApp->GetScreenSize().y / 2, 1, 1, 1, FORMAT_R8, 1, SS_NONE);
	m_SSAOPostProcessingRT[1] = m_pRenderer->addRenderTarget((int)g_pApp->GetScreenSize().x / 2, (int)g_pApp->GetScreenSize().y / 2, 1, 1, 1, FORMAT_R8, 1, SS_NONE);
	m_halfAuxiliarRT = m_pRenderer->addRenderTarget((int)g_pApp->GetScreenSize().x / 2, (int)g_pApp->GetScreenSize().y / 2, 1, 1, 1, FORMAT_R8, 1, SS_NONE);
	m_LowResDepth[0] = m_pRenderer->addRenderTarget((int)g_pApp->GetScreenSize().x / 2, (int)g_pApp->GetScreenSize().y / 2, 1, 1, 1, FORMAT_R16F, 1, SS_NONE);
	m_LowResDepth[1] = m_pRenderer->addRenderTarget((int)g_pApp->GetScreenSize().x / 2, (int)g_pApp->GetScreenSize().y / 2, 1, 1, 1, FORMAT_R16F, 1, SS_NONE);
	////////////////////////////////////////////////////////////////////////

	// Bloom /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	m_fSensitiveLuminance = 0.8f;
	m_fBloomIntensitive = 0.075f;

	// Init gaussian kernel values

	for (int i = 0; i < (NUM_HALF_PRECOMPUTED_GAUSSIAN_DISTRIBUTION + 1); i++)
	{
		float fSample = 0.1f * i;
		m_GaussianKernel[i + NUM_HALF_PRECOMPUTED_GAUSSIAN_DISTRIBUTION] = GaussianDistribution(fSample, 2.0f);
	}

	for (int i = 0; i > (-NUM_HALF_PRECOMPUTED_GAUSSIAN_DISTRIBUTION - 1); i--)
	{
		float fSample = 0.1f * i;
		m_GaussianKernel[i + NUM_HALF_PRECOMPUTED_GAUSSIAN_DISTRIBUTION] = GaussianDistribution(fSample, 2.0f);
	}

	// Load Shaders
	Resource BloomSelectAndReduceShaderResource("Shaders\\ScreenPostProcessing\\BloomSelectAndReduceShader.shd");
	shared_ptr<ResHandle> BloomSelectAndReduceShaderHandle = g_pApp->m_ResCache->GetHandle(&BloomSelectAndReduceShaderResource);

	if (!BloomSelectAndReduceShaderHandle)
	{
		CHG_ERROR("Not found Bloom Select and Reduce Shader");
	}

	m_BloomHighSelectAndReduceShader = ((ShaderResourceExtraData*)BloomSelectAndReduceShaderHandle->GetExtra().get())->m_ShaderID;

	Resource BloomReduceShaderResource("Shaders\\ScreenPostProcessing\\BloomReduceShader.shd");
	shared_ptr<ResHandle> BloomReduceShaderHandle = g_pApp->m_ResCache->GetHandle(&BloomReduceShaderResource);

	if (!BloomReduceShaderHandle)
	{
		CHG_ERROR("Not found Bloom Reduce Shader");
	}

	m_BloomReduceShader = ((ShaderResourceExtraData*)BloomReduceShaderHandle->GetExtra().get())->m_ShaderID;

	Resource BloomHorizontalGaussianBlurResource("Shaders\\ScreenPostProcessing\\BloomHorizontalGaussianBlur.shd");
	shared_ptr<ResHandle> BloomHorizontalGaussianBlurShaderHandle = g_pApp->m_ResCache->GetHandle(&BloomHorizontalGaussianBlurResource);

	if (!BloomHorizontalGaussianBlurShaderHandle)
	{
		CHG_ERROR("Not found Bloom Horizontal Gaussian Blur Shader");
	}

	m_BloomHorizontalGaussianShader = ((ShaderResourceExtraData*)BloomHorizontalGaussianBlurShaderHandle->GetExtra().get())->m_ShaderID;

	Resource BloomVerticalGaussianBlurResource("Shaders\\ScreenPostProcessing\\BloomVerticalGaussianBlur.shd");
	shared_ptr<ResHandle> BloomVerticalGaussianBlurShaderHandle = g_pApp->m_ResCache->GetHandle(&BloomVerticalGaussianBlurResource);

	if (!BloomVerticalGaussianBlurShaderHandle)
	{
		CHG_ERROR("Not found Bloom Vertical Gaussian Blur Shader");
	}

	m_BloomVerticalGaussianShader = ((ShaderResourceExtraData*)BloomVerticalGaussianBlurShaderHandle->GetExtra().get())->m_ShaderID;

	// Bloom Vertex Formats
	if (m_BloomHighSelectAndReduceShader != SHADER_NONE)
		m_BloomHighSelectAndReduceShaderVFID = m_pRenderer->addVertexFormat(vertexDesc, 1, m_BloomHighSelectAndReduceShader);
	if (m_BloomReduceShader != SHADER_NONE)
		m_BloomReduceShaderVFID = m_pRenderer->addVertexFormat(vertexDesc, 1, m_BloomReduceShader);
	if (m_BloomHorizontalGaussianShader != SHADER_NONE)
		m_BloomHorizontalGaussianShaderVFID = m_pRenderer->addVertexFormat(vertexDesc, 1, m_BloomHorizontalGaussianShader);
	if (m_BloomVerticalGaussianShader != SHADER_NONE)
		m_BloomVerticalGaussianShaderVFID = m_pRenderer->addVertexFormat(vertexDesc, 1, m_BloomVerticalGaussianShader);

	// Bloom RTs
	for (UINT i = 0; i < 6; i++)
	{
		m_BloomFilterRT_0[i] = m_pRenderer->addRenderTarget((int)g_pApp->GetScreenSize().x / (1 << (i + 1)), (int)g_pApp->GetScreenSize().y / (1 << (i + 1)), 1, 1, 1, FORMAT_RGBA16F, 1, SS_NONE);
		m_BloomFilterRT_1[i] = m_pRenderer->addRenderTarget((int)g_pApp->GetScreenSize().x / (1 << (i + 1)), (int)g_pApp->GetScreenSize().y / (1 << (i + 1)), 1, 1, 1, FORMAT_RGBA16F, 1, SS_NONE);
	}

	m_fFilterSize[0] = 1.0f / 100.0f;
	m_fFilterSize[1] = 4.0f / 100.0f;
	m_fFilterSize[2] = 16.0f / 100.0f;
	m_fFilterSize[3] = 32.0f / 100.0f;
	m_fFilterSize[4] = 64.0f / 100.0f;
	m_fFilterSize[5] = 64.0f / 100.0f;

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Smaa RTs
	m_SceneColorRT = pScene->m_ColorRT;		//Re-use this render target from scene class
	m_EdgeDetectionRT = pScene->m_MREC_RT;

	// Load Shaders
	Resource SmaaEdgeDetectorShaderResource("Shaders\\ScreenPostProcessing\\SMAA_EdgeDetectionShader.shd");
	shared_ptr<ResHandle> SmaaEdgeDetectorShaderHandle = g_pApp->m_ResCache->GetHandle(&SmaaEdgeDetectorShaderResource);

	if (!SmaaEdgeDetectorShaderHandle)
	{
		CHG_ERROR("Not found Smaa Edge Detector");
	}

	m_SmaaEdgeDetectorShader = ((ShaderResourceExtraData*)SmaaEdgeDetectorShaderHandle->GetExtra().get())->m_ShaderID;

	// Bloom Vertex Formats
	if (m_SmaaEdgeDetectorShader != SHADER_NONE)
		m_SmaaEdgeDetectorShaderVFID = m_pRenderer->addVertexFormat(vertexDesc, 1, m_SmaaEdgeDetectorShader);	

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	delete[] vertexDesc;
}

bool ScreenPostProcessing::ComputeSmaa()
{
	UINT offset[] = { 0 };
	
	Vec2 vPixelSize = 1.0f / g_pApp->GetScreenSize();

	// Edge Detector //////////////////////////////////////////////////////////////////////////////////

	m_pRenderer->setViewport((int)g_pApp->GetScreenSize().x, (int)g_pApp->GetScreenSize().y);
	TextureID EdgeDetectorRTs[] = { m_EdgeDetectionRT };
	m_pRenderer->changeRenderTargets(EdgeDetectorRTs, elementsOf(EdgeDetectorRTs), TEXTURE_NONE);

	m_pRenderer->reset();

	m_pRenderer->setShader(m_SmaaEdgeDetectorShader);
	m_pRenderer->setVertexFormat(m_SmaaEdgeDetectorShaderVFID);
	m_pRenderer->setVertexBuffer(0, 1, &m_pScene->m_ScreenAlignedQuadVBID, offset);
	m_pRenderer->setDepthState(m_pScene->m_noDepthTest);
	m_pRenderer->setRasterizerState(m_pScene->m_noneFaceCulling);

	m_pRenderer->setTexture("SceneTarget", m_SceneColorRT);

	m_pRenderer->setSamplerState("PointClampSampler", m_pScene->m_pointClamp);
	m_pRenderer->setShaderConstant2f("fPixelSize", vPixelSize);

	m_pRenderer->apply();

	m_pRenderer->drawArrays(PRIM_TRIANGLES, 0, 6);

	/////////////////////////////////////////////////////////////////////////////////////////////////////

	return true;
}

bool ScreenPostProcessing::ComputeBloom()
{
	UINT offset[] = { 0 };

	// Select and Reduce 1/2 Pass ///////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	m_pRenderer->setViewport((int)(g_pApp->GetScreenSize().x*0.5f), (int)(g_pApp->GetScreenSize().y*0.5f));
	TextureID ReduceBufferRTs[] = { m_BloomFilterRT_0[0] };
	m_pRenderer->changeRenderTargets(ReduceBufferRTs, elementsOf(ReduceBufferRTs), TEXTURE_NONE);

	m_pRenderer->reset();

	m_pRenderer->setShader(m_BloomHighSelectAndReduceShader);
	m_pRenderer->setVertexFormat(m_BloomHighSelectAndReduceShaderVFID);
	m_pRenderer->setVertexBuffer(0, 1, &m_pScene->m_ScreenAlignedQuadVBID, offset);
	m_pRenderer->setDepthState(m_pScene->m_noDepthTest);
	m_pRenderer->setRasterizerState(m_pScene->m_noneFaceCulling);

	m_pRenderer->setStructuredBuffer("tex_DeferredRenderSource", m_pScene->m_LightingSceneRT);
	m_pRenderer->setTexture("tex_ForwardRenderSource", m_pScene->m_ForwardRenderSceneRT);

	m_pRenderer->setSamplerState("PointClampSampler", m_pScene->m_pointClamp);
	m_pRenderer->setShaderConstant2f("vScreenResolution", g_pApp->GetScreenSize());
	m_pRenderer->setShaderConstant2f("vInverseScreenResolution", 1.0f / g_pApp->GetScreenSize());
	m_pRenderer->setShaderConstant1f("fSensitiveLuminance", m_fSensitiveLuminance);

	m_pRenderer->apply();

	m_pRenderer->drawArrays(PRIM_TRIANGLES, 0, 6);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Reduce Pass ///////////////////////////////////////////////////////////////////////////////////////////////////////////

	m_pRenderer->reset();

	m_pRenderer->setShader(m_BloomReduceShader);
	m_pRenderer->setVertexFormat(m_BloomReduceShaderVFID);
	m_pRenderer->setVertexBuffer(0, 1, &m_pScene->m_ScreenAlignedQuadVBID, offset);
	m_pRenderer->setDepthState(m_pScene->m_noDepthTest);
	m_pRenderer->setRasterizerState(m_pScene->m_noneFaceCulling);
	
	for(UINT i = 1; i < 6; i++)
	{
		m_pRenderer->setViewport((int)(g_pApp->GetScreenSize().x/(1 << (i + 1))), (int)(g_pApp->GetScreenSize().y/(1 << (i + 1))));
		ReduceBufferRTs[0] = m_BloomFilterRT_0[i];
		m_pRenderer->changeRenderTargets(ReduceBufferRTs, elementsOf(ReduceBufferRTs), TEXTURE_NONE);

		m_pRenderer->setTexture("tex_RenderSource", m_BloomFilterRT_0[i - 1]);
		m_pRenderer->setSamplerState("PointClampSampler", m_pScene->m_pointClamp);

		m_pRenderer->setShaderConstant2f("vInverseScreenResolution", 1.0f / (g_pApp->GetScreenSize()/(float)(1 << i)));

		m_pRenderer->apply();

		m_pRenderer->drawArrays(PRIM_TRIANGLES, 0, 6);
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Gaussin Filter ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	float fGaussianKernel[100];

	// Horizontal Blur

	m_pRenderer->reset();

	m_pRenderer->setShader(m_BloomHorizontalGaussianShader);
	m_pRenderer->setVertexFormat(m_BloomHorizontalGaussianShaderVFID);
	m_pRenderer->setVertexBuffer(0, 1, &m_pScene->m_ScreenAlignedQuadVBID, offset);
	m_pRenderer->setDepthState(m_pScene->m_noDepthTest);
	m_pRenderer->setRasterizerState(m_pScene->m_noneFaceCulling);

	for (UINT i = 0; i < 6; i++)
	{
		Vec2 vRenderTargetSize = g_pApp->GetScreenSize() / (float)(1 << (i + 1));
		int iGaussianKernelSize = ((int)(vRenderTargetSize.x*m_fFilterSize[i]) / 2) * 2 + 1;
		int iGaussianKernelRadius = (iGaussianKernelSize - 1) / 2;

		UINT uKernelInit = NUM_HALF_PRECOMPUTED_GAUSSIAN_DISTRIBUTION - ((NUM_HALF_PRECOMPUTED_GAUSSIAN_DISTRIBUTION / iGaussianKernelRadius)*iGaussianKernelRadius);
		UINT uSampleOffset = NUM_HALF_PRECOMPUTED_GAUSSIAN_DISTRIBUTION / iGaussianKernelRadius;
		UINT uCount = 0;

		for (UINT sample = uKernelInit; sample < NUM_PRECOMPUTED_GAUSSIAN_DISTRIBUTION; sample += uSampleOffset)
		{
			fGaussianKernel[uCount++] = m_GaussianKernel[sample];
		}

		m_pRenderer->setViewport((int)vRenderTargetSize.x, (int)vRenderTargetSize.y);
		ReduceBufferRTs[0] = m_BloomFilterRT_1[i];
		m_pRenderer->changeRenderTargets(ReduceBufferRTs, elementsOf(ReduceBufferRTs), TEXTURE_NONE);

		m_pRenderer->setTexture("tex_RenderSource", m_BloomFilterRT_0[i]);
		m_pRenderer->setSamplerState("PointClampSampler", m_pScene->m_pointClamp);

		m_pRenderer->setShaderConstant2f("vInverseScreenResolution", 1.0f / vRenderTargetSize);
		m_pRenderer->setShaderConstant1i("iKernelSize", iGaussianKernelSize - 1);
		m_pRenderer->setShaderConstantArray1f("vGaussianKernel", fGaussianKernel, uCount);

		m_pRenderer->apply();

		m_pRenderer->drawArrays(PRIM_TRIANGLES, 0, 6);
	}

	// Vertical Blur

	m_pRenderer->reset();

	m_pRenderer->setShader(m_BloomVerticalGaussianShader);
	m_pRenderer->setVertexFormat(m_BloomVerticalGaussianShaderVFID);
	m_pRenderer->setVertexBuffer(0, 1, &m_pScene->m_ScreenAlignedQuadVBID, offset);
	m_pRenderer->setDepthState(m_pScene->m_noDepthTest);
	m_pRenderer->setRasterizerState(m_pScene->m_noneFaceCulling);

	for (UINT i = 0; i < 6; i++)
	{
		Vec2 vRenderTargetSize = g_pApp->GetScreenSize() / (float)(1 << (i + 1));
		int iGaussianKernelSize = ((int)(vRenderTargetSize.x*m_fFilterSize[i]) / 2) * 2 + 1;
		int iGaussianKernelRadius = (iGaussianKernelSize - 1) / 2;

		UINT uKernelInit = NUM_HALF_PRECOMPUTED_GAUSSIAN_DISTRIBUTION - ((NUM_HALF_PRECOMPUTED_GAUSSIAN_DISTRIBUTION / iGaussianKernelRadius)*iGaussianKernelRadius);
		UINT uSampleOffset = NUM_HALF_PRECOMPUTED_GAUSSIAN_DISTRIBUTION / iGaussianKernelRadius;
		UINT uCount = 0;

		for (UINT sample = uKernelInit; sample < NUM_PRECOMPUTED_GAUSSIAN_DISTRIBUTION; sample += uSampleOffset)
		{
			fGaussianKernel[uCount++] = m_GaussianKernel[sample];
		}

		m_pRenderer->setViewport((int)vRenderTargetSize.x, (int)vRenderTargetSize.y);
		ReduceBufferRTs[0] = m_BloomFilterRT_0[i];
		m_pRenderer->changeRenderTargets(ReduceBufferRTs, elementsOf(ReduceBufferRTs), TEXTURE_NONE);

		m_pRenderer->setTexture("tex_RenderSource", m_BloomFilterRT_1[i]);
		m_pRenderer->setSamplerState("PointClampSampler", m_pScene->m_pointClamp);

		m_pRenderer->setShaderConstant2f("vInverseScreenResolution", 1.0f / vRenderTargetSize);
		m_pRenderer->setShaderConstant1i("iKernelSize", iGaussianKernelSize - 1);
		m_pRenderer->setShaderConstantArray1f("vGaussianKernel", fGaussianKernel, uCount);

		m_pRenderer->apply();

		m_pRenderer->drawArrays(PRIM_TRIANGLES, 0, 6);
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	return true;
}

bool ScreenPostProcessing::ComputeSsao()
{
	float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	UINT offset[] = { 0 };

	// Temporal blur //////////////////////
	static float timeCount = 0.0f;
	static float fps = 1.0f;
	static int ticks = 0;

	fps = (float)ticks / timeCount;

	timeCount += g_pApp->getElapsedTime();
	ticks++;
	////////////////////////////////////////

	Mat4x4 inverseViewMat = m_pScene->m_Camera->GetInverseView();
	Mat4x4 inverseProjMat = m_pScene->m_Camera->GetInverseProjection();

	// SSAO ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	m_pRenderer->setViewport((int)g_pApp->GetScreenSize().x, (int)g_pApp->GetScreenSize().y);
	TextureID SSAOPostProcessingbufferRTs[] = { m_halfAuxiliarRT, m_LowResDepth[m_iFlipTargetIndex] };
	m_pRenderer->changeRenderTargets(SSAOPostProcessingbufferRTs, elementsOf(SSAOPostProcessingbufferRTs), TEXTURE_NONE);

	m_pRenderer->reset();

	m_pRenderer->setShader(m_SSAOPostProcessingShaderID);
	m_pRenderer->setVertexFormat(m_SSAOPostProcessingShaderVFID);
	m_pRenderer->setVertexBuffer(0, 1, &m_pScene->m_ScreenAlignedQuadVBID, offset);
	m_pRenderer->setDepthState(m_pScene->m_noDepthTest);
	m_pRenderer->setRasterizerState(m_pScene->m_noneFaceCulling);

	m_pRenderer->setTexture("depthTexture", m_pScene->m_PositionRT);
	m_pRenderer->setTexture("normalTexture", m_pScene->m_NormalRT);
	m_pRenderer->setSamplerState("targetFilter", m_pScene->m_linearClamp);
	m_pRenderer->setShaderConstant4x4f("InverseProjection", inverseProjMat);
	m_pRenderer->setShaderConstant4x4f("View", m_pScene->m_Camera->GetView());

	m_pRenderer->apply();

	m_pRenderer->drawArrays(PRIM_TRIANGLES, 0, 6);

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	Vec2 vHalfScreenSize = g_pApp->GetScreenSize()*0.5f;

	//Vertical Gaussian Blur Pass//////////////////////////////////////////////////////////////////////////////////////////////////
	m_pRenderer->setViewport((int)vHalfScreenSize.x, (int)vHalfScreenSize.y);
	TextureID VerticalBlurBufferRTs[] = { m_SSAOPostProcessingRT[m_iFlipTargetIndex] };
	m_pRenderer->changeRenderTargets(VerticalBlurBufferRTs, elementsOf(VerticalBlurBufferRTs), TEXTURE_NONE);

	m_pRenderer->setShader(m_VerticalBlurSsaoShaderID);
	m_pRenderer->setVertexFormat(m_VerticalBlurSsaoShaderVFID);

	m_pRenderer->setTexture("ssaoRT", m_halfAuxiliarRT);
	m_pRenderer->setTexture("depthTexture", m_LowResDepth[m_iFlipTargetIndex]);
	m_pRenderer->setSamplerState("PostProcessingSampler", m_pScene->m_pointClamp);
	m_pRenderer->setShaderConstant2f("screenResolution", g_pApp->GetScreenSize());

	m_pRenderer->apply();

	m_pRenderer->drawArrays(PRIM_TRIANGLES, 0, 6);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//Horizontal Gaussian Blur Pass//////////////////////////////////////////////////////////////////////////////////////////////////
	m_pRenderer->setViewport((int)vHalfScreenSize.x, (int)vHalfScreenSize.y);
	TextureID HorizontalBlurBufferRTs[] = { m_halfAuxiliarRT };
	m_pRenderer->changeRenderTargets(HorizontalBlurBufferRTs, elementsOf(HorizontalBlurBufferRTs), TEXTURE_NONE);

	m_pRenderer->setShader(m_HorizontalBlurSsaoShaderID);
	m_pRenderer->setVertexFormat(m_HorizontalBlurSsaoShaderVFID);

	m_pRenderer->setTexture("ssaoRT", m_SSAOPostProcessingRT[m_iFlipTargetIndex]);
	m_pRenderer->setTexture("depthTexture", m_LowResDepth[m_iFlipTargetIndex]);
	m_pRenderer->setSamplerState("PostProcessingSampler", m_pScene->m_pointClamp);
	m_pRenderer->setShaderConstant2f("screenResolution", g_pApp->GetScreenSize());

	m_pRenderer->apply();

	m_pRenderer->drawArrays(PRIM_TRIANGLES, 0, 6);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//Temporal Blur Pass//////////////////////////////////////////////////////////////////////////////////////////////////
	m_pRenderer->setViewport((int)vHalfScreenSize.x, (int)vHalfScreenSize.y);
	TextureID TemporalBlurBufferRTs[] = { m_SSAOPostProcessingRT[m_iFlipTargetIndex] };
	m_pRenderer->changeRenderTargets(TemporalBlurBufferRTs, elementsOf(TemporalBlurBufferRTs), TEXTURE_NONE);

	m_pRenderer->setShader(m_TemporalBlurSsaoShaderID);
	m_pRenderer->setVertexFormat(m_TemporalBlurSsaoShaderVFID);

	m_pRenderer->setTexture("ssaoRT", m_halfAuxiliarRT);
	m_pRenderer->setTexture("depthRT", m_LowResDepth[m_iFlipTargetIndex]);
	m_pRenderer->setTexture("prevSsaoRT", m_SSAOPostProcessingRT[(m_iFlipTargetIndex + 1) % 2]);
	m_pRenderer->setTexture("prevDepthRT", m_LowResDepth[(m_iFlipTargetIndex + 1) % 2]);
	m_pRenderer->setSamplerState("PostProcessingSampler", m_pScene->m_pointClamp);

	m_pRenderer->setShaderConstant4x4f("InverseProjection", inverseProjMat);
	m_pRenderer->setShaderConstant4x4f("InverseView", inverseViewMat);
	m_pRenderer->setShaderConstant4x4f("Projection", m_pScene->m_Camera->GetProjection());
	m_pRenderer->setShaderConstant4x4f("PrevFrameView", m_prevFrameView);

	m_pRenderer->setShaderConstant1f("fFrameTime", 1.0f / fps);

	m_pRenderer->apply();

	m_pRenderer->drawArrays(PRIM_TRIANGLES, 0, 6);

	m_prevFrameView = m_pScene->m_Camera->GetView();

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//Vertical Gaussian Blur Pass//////////////////////////////////////////////////////////////////////////////////////////////////
	m_pRenderer->setViewport((int)vHalfScreenSize.x, (int)vHalfScreenSize.y);
	VerticalBlurBufferRTs[0] = m_halfAuxiliarRT;
	m_pRenderer->changeRenderTargets(VerticalBlurBufferRTs, elementsOf(VerticalBlurBufferRTs), TEXTURE_NONE);

	m_pRenderer->setShader(m_VerticalBlurSsaoShaderID);
	m_pRenderer->setVertexFormat(m_VerticalBlurSsaoShaderVFID);

	m_pRenderer->setTexture("ssaoRT", m_SSAOPostProcessingRT[m_iFlipTargetIndex]);
	m_pRenderer->setTexture("depthTexture", m_LowResDepth[m_iFlipTargetIndex]);
	m_pRenderer->setSamplerState("PostProcessingSampler", m_pScene->m_pointClamp);
	m_pRenderer->setShaderConstant2f("screenResolution", g_pApp->GetScreenSize());

	m_pRenderer->apply();

	m_pRenderer->drawArrays(PRIM_TRIANGLES, 0, 6);

	//Horizontal Gaussian Blur Pass//////////////////////////////////////////////////////////////////////////////////////////////////
	m_pRenderer->setViewport((int)vHalfScreenSize.x, (int)vHalfScreenSize.y);
	HorizontalBlurBufferRTs[0] = m_SSAOPostProcessingRT[m_iFlipTargetIndex];
	m_pRenderer->changeRenderTargets(HorizontalBlurBufferRTs, elementsOf(HorizontalBlurBufferRTs), TEXTURE_NONE);

	m_pRenderer->setShader(m_HorizontalBlurSsaoShaderID);
	m_pRenderer->setVertexFormat(m_HorizontalBlurSsaoShaderVFID);

	m_pRenderer->setTexture("ssaoRT", m_halfAuxiliarRT);
	m_pRenderer->setTexture("depthTexture", m_LowResDepth[m_iFlipTargetIndex]);
	m_pRenderer->setSamplerState("PostProcessingSampler", m_pScene->m_pointClamp);
	m_pRenderer->setShaderConstant2f("screenResolution", g_pApp->GetScreenSize());

	m_pRenderer->apply();

	m_pRenderer->drawArrays(PRIM_TRIANGLES, 0, 6);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	return true;
}

bool ScreenPostProcessing::OnRenderPostProcessing()
{
	//SSAO
	if (m_bSsaoActive)
		ComputeSsao();

	if (m_bBloomActive)
		ComputeBloom();

	//PostProcessing Pass///////////////////////////////////////////////////////////////////////////////////////////////////////////
	UINT offset[] = { 0 };
	
	m_pRenderer->setViewport((int)g_pApp->GetScreenSize().x, (int)g_pApp->GetScreenSize().y);

	if (m_bSmaaActive)
	{
		TextureID SceneColorRTs[] = { m_SceneColorRT };
		m_pRenderer->changeRenderTargets(SceneColorRTs, elementsOf(SceneColorRTs), TEXTURE_NONE);
	}
	else
		m_pRenderer->changeToMainFramebuffer();

	m_pRenderer->reset();

	m_pRenderer->setShader(m_PostProcessingShaderID);
	m_pRenderer->setVertexFormat(m_PostProcessingShaderVFID);
	m_pRenderer->setVertexBuffer(0, 1, &m_pScene->m_ScreenAlignedQuadVBID, offset);
	m_pRenderer->setDepthState(m_pScene->m_noDepthTest);
	m_pRenderer->setRasterizerState(m_pScene->m_noneFaceCulling);

	/*if (m_tOnCustomPostProcessingEffect)
	m_pRenderer->setTexture("postProcessingTarget", m_PostProcessingRT);
	else*/
	m_pRenderer->setStructuredBuffer("postProcessingTarget", m_pScene->m_LightingSceneRT);
	m_pRenderer->setTexture("ForwardRenderTarget", m_pScene->m_ForwardRenderSceneRT);	
	
	m_pRenderer->setSamplerState("PostProcessingSampler", m_pScene->m_pointClamp);
	m_pRenderer->setSamplerState("LinearClampSampler", m_pScene->m_linearClamp);

	m_pRenderer->setTextureArray("tex_BloomTexture", m_BloomFilterRT_0, 6);
	m_pRenderer->setTexture("halfDepthTexture", m_LowResDepth[m_iFlipTargetIndex]);
	m_pRenderer->setTexture("depthTexture", m_pScene->m_PositionRT);
	m_pRenderer->setTexture("ssaoRT", m_SSAOPostProcessingRT[m_iFlipTargetIndex]);
	m_pRenderer->setTexture("volumeLightingTarget", g_pApp->m_pVoxelManager->getVolumeLightingRT());

	m_pRenderer->setShaderConstant2f("screenResolution", g_pApp->GetScreenSize());
	m_pRenderer->setShaderConstant2f("fBloomIntensitive", m_fBloomIntensitive);

	m_pRenderer->apply();

	m_pRenderer->drawArrays(PRIM_TRIANGLES, 0, 6);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	if (m_bSmaaActive)
		ComputeSmaa();


	return true;
}

void ScreenPostProcessing::SsaoActive(bool active)
{ 
	if (!active)
	{
		float ClearColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		
		TextureID ClearSSAOPostProcessingbufferRTs[] = { m_SSAOPostProcessingRT[0], m_SSAOPostProcessingRT[1] };
		m_pRenderer->changeRenderTargets(ClearSSAOPostProcessingbufferRTs, elementsOf(ClearSSAOPostProcessingbufferRTs), TEXTURE_NONE);

		m_pRenderer->clear(true, false, false, ClearColor, 1.0f);
	}

	m_bSsaoActive = active; 
}

void ScreenPostProcessing::BloomActive(bool active)
{ 
	if (!active)
	{
		float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

		TextureID ClearBloomPostProcessingbufferRTs[] = { m_BloomFilterRT_0[0], m_BloomFilterRT_0[1], m_BloomFilterRT_0[2], m_BloomFilterRT_0[3], m_BloomFilterRT_0[4], m_BloomFilterRT_0[5] };
		m_pRenderer->changeRenderTargets(ClearBloomPostProcessingbufferRTs, elementsOf(ClearBloomPostProcessingbufferRTs), TEXTURE_NONE);

		m_pRenderer->clear(true, false, false, ClearColor, 1.0f);
	}

	m_bBloomActive = active; 
}