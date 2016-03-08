#ifndef SCREEN_POSTPROCESSING_H_
#define SCREEN_POSTPROCESSING_H_

//========================================================================
// File: ScreenPostProcessing.h - classes to add postprocessing
//
//========================================================================

#include "Scene.h"

#define NUM_PRECOMPUTED_GAUSSIAN_DISTRIBUTION 101
#define NUM_HALF_PRECOMPUTED_GAUSSIAN_DISTRIBUTION 50

class ScreenPostProcessing
{
private:
	bool m_bSsaoActive;
	bool m_bBloomActive;
	bool m_bSmaaActive;

	Scene* m_pScene;
	std::shared_ptr<Renderer> m_pRenderer;

	// Pre Computed Gaussian Kernel;
	float m_GaussianKernel[NUM_PRECOMPUTED_GAUSSIAN_DISTRIBUTION];

	// SSAO
	//Render Targets
	TextureID	   m_LowResDepth[2];
	TextureID      m_SSAOPostProcessingRT[2];
	TextureID	   m_halfAuxiliarRT;
	//Shaders
	ShaderID       m_SSAOPostProcessingShaderID;
	ShaderID	   m_VerticalBlurSsaoShaderID;
	ShaderID	   m_HorizontalBlurSsaoShaderID;
	ShaderID	   m_TemporalBlurSsaoShaderID;
	//Vertex Formats
	VertexFormatID m_SSAOPostProcessingShaderVFID;
	VertexFormatID m_VerticalBlurSsaoShaderVFID;
	VertexFormatID m_HorizontalBlurSsaoShaderVFID;
	VertexFormatID m_TemporalBlurSsaoShaderVFID;
	//Temporal Blur attributes
	Mat4x4 m_prevFrameView;
	int m_iFlipTargetIndex;

	// Bloom
	TextureID m_BloomFilterRT_0[6];
	TextureID m_BloomFilterRT_1[6];
	//Shader
	ShaderID m_BloomHighSelectAndReduceShader;
	ShaderID m_BloomReduceShader;
	ShaderID m_BloomHorizontalGaussianShader;
	ShaderID m_BloomVerticalGaussianShader;
	//Vertex Formats
	VertexFormatID m_BloomHighSelectAndReduceShaderVFID;
	VertexFormatID m_BloomReduceShaderVFID;
	VertexFormatID m_BloomHorizontalGaussianShaderVFID;
	VertexFormatID m_BloomVerticalGaussianShaderVFID;
	//Filters Size
	float m_fFilterSize[6];

	float m_fSensitiveLuminance;
	float m_fBloomIntensitive;

	// Smaa
	TextureID m_SceneColorRT;
	TextureID m_EdgeDetectionRT;
	TextureID m_AreaTexture;
	TextureID m_SearchTexture;
	//Shader
	ShaderID m_SmaaEdgeDetectorShader;
	ShaderID m_SmaaBlendingWeightCalculatorShader;
	//Vertex Formats
	VertexFormatID m_SmaaEdgeDetectorShaderVFID;
	VertexFormatID m_SmaaBlendingWeightCalculatorShaderVFID;

	// Render Postprocessing
	VertexFormatID m_PostProcessingShaderVFID;
	ShaderID       m_PostProcessingShaderID;

	// Private Methods
	bool ComputeSmaa();
	bool ComputeSsao();
	bool ComputeBloom();

public:
	ScreenPostProcessing(Scene* pScene);
	~ScreenPostProcessing() { }

	bool OnRenderPostProcessing();

	TextureID GetSceneColorRT() { return m_SceneColorRT; }
	TextureID GetEdgeDetectorRT() { return m_EdgeDetectionRT; }

	TextureID GetBloomRT(int index0, int index1) { return (index0 == 0) ? m_BloomFilterRT_0[index1] : m_BloomFilterRT_1[index1]; }

	void SsaoActive(bool active);
	void BloomActive(bool active);
};

#endif