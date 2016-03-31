#include "FluidSimulator.h"
#include <Graphics\Shader.h>

#define NUM_CELLS 262144

// Numthreads size for the simulation
const UINT SIMULATION_BLOCK_SIZE = 256;

// Numthreads size for the sort
const UINT BITONIC_BLOCK_SIZE = 512;
const UINT TRANSPOSE_BLOCK_SIZE = 16;

struct Particle
{
	Vec4 vPosition;
	Vec4 vVelocity;
};

struct ParticleDensity
{
	float fDensity;
};

struct ParticleForces
{
	Vec4 vAcceleration;
};

/*
 * FluidSimulator implementation
 */

FluidSimulator::FluidSimulator()
{
	/* Load Computer Shaders */

	//Load BuildGrid_CS
	Resource BuildGridCSResource("Shaders\\Fluid_Simulator\\Build_Grid_CS.shd");
	shared_ptr<ResHandle> BuildGridCSHandle = g_pApp->m_ResCache->GetHandle(&BuildGridCSResource);

	if (!BuildGridCSHandle)
	{
		CHG_ERROR("Not found Build_Grid_CS.shd");
	}

	m_BuildGrid_CS = ((ShaderResourceExtraData*)BuildGridCSHandle->GetExtra().get())->m_ShaderID;

	//Load BitonicSort_CS
	Resource BitonicSortCSResource("Shaders\\Fluid_Simulator\\Build_Grid_CS.shd");
	shared_ptr<ResHandle> BitonicSortCSHandle = g_pApp->m_ResCache->GetHandle(&BitonicSortCSResource);

	if (!BitonicSortCSHandle)
	{
		CHG_ERROR("Not found Build_Grid_CS.shd");
	}

	m_BitonicSort_CS = ((ShaderResourceExtraData*)BitonicSortCSHandle->GetExtra().get())->m_ShaderID;

	//Load MatrixTranspose_CS
	Resource MatrixTransposeCSResource("Shaders\\Fluid_Simulator\\Matrix_Transpose_CS.shd");
	shared_ptr<ResHandle> MatrixTransposeCSHandle = g_pApp->m_ResCache->GetHandle(&MatrixTransposeCSResource);

	if (!MatrixTransposeCSHandle)
	{
		CHG_ERROR("Not found Matrix_Transpose_CS.shd");
	}

	m_MatrixTranspose_CS = ((ShaderResourceExtraData*)MatrixTransposeCSHandle->GetExtra().get())->m_ShaderID;

	//Load ClearGridIndices_CS
	Resource ClearGridIndicesCSResource("Shaders\\Fluid_Simulator\\Clear_Grid_Indices_CS.shd");
	shared_ptr<ResHandle> ClearGridIndicesCSHandle = g_pApp->m_ResCache->GetHandle(&ClearGridIndicesCSResource);

	if (!ClearGridIndicesCSHandle)
	{
		CHG_ERROR("Not found Clear_Grid_Indices_CS");
	}

	m_ClearGridIndices_CS = ((ShaderResourceExtraData*)ClearGridIndicesCSHandle->GetExtra().get())->m_ShaderID;

	//Load BuildGridIndices_CS
	Resource BuildGridIndicesCSResource("Shaders\\Fluid_Simulator\\Build_Grid_Indices_CS.shd");
	shared_ptr<ResHandle> BuildGridIndicesCSHandle = g_pApp->m_ResCache->GetHandle(&BuildGridIndicesCSResource);

	if (!BuildGridIndicesCSHandle)
	{
		CHG_ERROR("Not found Build_Grid_Indices_CS.shd");
	}

	m_BuildGridIndices_CS = ((ShaderResourceExtraData*)BuildGridIndicesCSHandle->GetExtra().get())->m_ShaderID;

	// Init fluid properties
	memset(&m_FluidProperties, 0, sizeof(m_FluidProperties));

	// Init structured buffers indices
	m_ParticlesBuffer = TEXTURE_NONE;
	m_SortedParticlesBuffer = TEXTURE_NONE;
	m_ParticleDensitiesBuffer = TEXTURE_NONE;
	m_ParticleAccelerationsBuffer = TEXTURE_NONE;
	m_GridBuffer = TEXTURE_NONE;
	m_GridIndicesBuffer = TEXTURE_NONE;
}

void FluidSimulator::Init(FluidProperties fluidProperties, Vec2 vInitialBoxDimension, float fInitialParticleSpacing)
{
	m_FluidProperties = fluidProperties;

	std::shared_ptr<Renderer> pRenderer = g_pApp->m_pRenderer;

	// Init particles position. Initial particles position forms a cuboid with vInitialBoxDimension

	std::vector<Particle> particles;

	UINT uWidthParticles = (UINT)(vInitialBoxDimension.x / fInitialParticleSpacing);
	UINT uHeightParticles = (UINT)(vInitialBoxDimension.y / fInitialParticleSpacing);

	UINT uNumDisposedParticles = 0;
	Vec3 vInitPosition = Vec3(0.0f, 2.0f, 0.0f);

	for (UINT i = 0; uNumDisposedParticles < m_FluidProperties.uNumParticles; ++i)
	{
		for (UINT j = 0; j < uHeightParticles && uNumDisposedParticles < m_FluidProperties.uNumParticles; ++j)
		{
			for (UINT k = 0; k < uWidthParticles && uNumDisposedParticles < m_FluidProperties.uNumParticles; ++k)
			{				
				Particle particle;
				particle.vPosition = Vec4(vInitPosition + Vec3(k*fInitialParticleSpacing, j*fInitialParticleSpacing, i*fInitialParticleSpacing), 1.0f);
				particle.vVelocity = Vec4();
				particles.push_back(particle);

				++uNumDisposedParticles;
			}
		}
	}

	// Create and init structured buffers
	m_ParticlesBuffer = pRenderer->createStructuredBuffer(sizeof(Particle), m_FluidProperties.uNumParticles, false, true, 0, particles.data());
	m_SortedParticlesBuffer = pRenderer->createStructuredBuffer(sizeof(Particle), m_FluidProperties.uNumParticles, false, true);
	m_ParticleDensitiesBuffer = pRenderer->createStructuredBuffer(sizeof(ParticleDensity), m_FluidProperties.uNumParticles, false, true);
	m_ParticleAccelerationsBuffer = pRenderer->createStructuredBuffer(sizeof(ParticleForces), m_FluidProperties.uNumParticles, false, true);
	m_GridBuffer = pRenderer->createStructuredBuffer(2 * sizeof(UINT), m_FluidProperties.uNumParticles, false, true);
	m_GridIndicesBuffer = pRenderer->createStructuredBuffer(2 * sizeof(UINT), NUM_CELLS, false, true);
}

void FluidSimulator::GPUGridSort()
{
	std::shared_ptr<Renderer> pRenderer = g_pApp->m_pRenderer;

	const UINT NUM_ELEMENTS = m_FluidProperties.uNumParticles;
	const UINT MATRIX_WIDTH = BITONIC_BLOCK_SIZE;
	const UINT MATRIX_HEIGHT = NUM_ELEMENTS / BITONIC_BLOCK_SIZE;

	pRenderer->reset();
	pRenderer->setShader(m_BitonicSort_CS);

	// Sort the data
	// First sort the rows for the levels <= to the block size
	for (UINT level = 2; level <= BITONIC_BLOCK_SIZE; level <<= 1)
	{
		pRenderer->setShaderConstant4f("g_iLevel", level);
		pRenderer->setShaderConstant4f("g_iLevelMask", level);

		pRenderer->setStructuredBuffer("Data", m_GridBuffer);

		// Sort the row data
		pRenderer->dispatchCompute(NUM_ELEMENTS / BITONIC_BLOCK_SIZE, 1, 1);
	}

	pRenderer->reset();
	pRenderer->setShader(m_MatrixTranspose_CS);

	// Then sort the rows and columns for the levels > than the block size
	// Transpose. Sort the Columns. Transpose. Sort the Rows.
	//for (UINT level = (BITONIC_BLOCK_SIZE << 1); level <= NUM_ELEMENTS; level <<= 1)
	//{
	//	SortCB constants1 = { (level / BITONIC_BLOCK_SIZE), (level & ~NUM_ELEMENTS) / BITONIC_BLOCK_SIZE, MATRIX_WIDTH, MATRIX_HEIGHT };
	//	pd3dImmediateContext->UpdateSubresource(g_pSortCB, 0, NULL, &constants1, 0, 0);

	//	// Transpose the data from buffer 1 into buffer 2
	//	ID3D11ShaderResourceView* pViewNULL = NULL;
	//	UINT UAVInitialCounts = 0;
	//	pd3dImmediateContext->CSSetShaderResources(0, 1, &pViewNULL);
	//	pd3dImmediateContext->CSSetUnorderedAccessViews(0, 1, &tempUAV, &UAVInitialCounts);
	//	pd3dImmediateContext->CSSetShaderResources(0, 1, &inSRV);
	//	pd3dImmediateContext->CSSetShader(g_pSortTranspose, NULL, 0);
	//	pd3dImmediateContext->Dispatch(MATRIX_WIDTH / TRANSPOSE_BLOCK_SIZE, MATRIX_HEIGHT / TRANSPOSE_BLOCK_SIZE, 1);

	//	// Sort the transposed column data
	//	pd3dImmediateContext->CSSetShader(g_pSortBitonic, NULL, 0);
	//	pd3dImmediateContext->Dispatch(NUM_ELEMENTS / BITONIC_BLOCK_SIZE, 1, 1);

	//	SortCB constants2 = { BITONIC_BLOCK_SIZE, level, MATRIX_HEIGHT, MATRIX_WIDTH };
	//	pd3dImmediateContext->UpdateSubresource(g_pSortCB, 0, NULL, &constants2, 0, 0);

	//	// Transpose the data from buffer 2 back into buffer 1
	//	pd3dImmediateContext->CSSetShaderResources(0, 1, &pViewNULL);
	//	pd3dImmediateContext->CSSetUnorderedAccessViews(0, 1, &inUAV, &UAVInitialCounts);
	//	pd3dImmediateContext->CSSetShaderResources(0, 1, &tempSRV);
	//	pd3dImmediateContext->CSSetShader(g_pSortTranspose, NULL, 0);
	//	pd3dImmediateContext->Dispatch(MATRIX_HEIGHT / TRANSPOSE_BLOCK_SIZE, MATRIX_WIDTH / TRANSPOSE_BLOCK_SIZE, 1);

	//	// Sort the row data
	//	pd3dImmediateContext->CSSetShader(g_pSortBitonic, NULL, 0);
	//	pd3dImmediateContext->Dispatch(NUM_ELEMENTS / BITONIC_BLOCK_SIZE, 1, 1);
	//}
}

void FluidSimulator::Update(unsigned long deltaMs)
{
	std::shared_ptr<Renderer> pRenderer = g_pApp->m_pRenderer;

	float fCellLimit = 1000.0f; // Avoid negative keys
	float fInverseKernelRadius = 1.0f / m_FluidProperties.fSPH_KernelRadius;

	if (m_bActive)
	{
		// Build grid Cells ///////////////////////////////////////////////////////////////////////
		pRenderer->reset();

		pRenderer->setShader(m_BuildGrid_CS);

		// Setup Constant Buffer
		pRenderer->setShaderConstant4f("g_fInverseCellDim", fInverseKernelRadius);
		pRenderer->setShaderConstant4f("g_fGridDim", fCellLimit * fInverseKernelRadius);

		// Setup Structured buffer
		pRenderer->setStructuredBuffer("ParticlesRO", m_ParticlesBuffer);
		pRenderer->setStructuredBuffer("GridRW", m_GridBuffer);

		pRenderer->apply();

		// Dispatch
		pRenderer->dispatchCompute(m_FluidProperties.uNumParticles / SIMULATION_BLOCK_SIZE, 1, 1);

		//////////////////////////////////////////////////////////////////////////////////////////////
	}
}