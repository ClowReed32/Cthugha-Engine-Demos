#pragma once

#include <Msvc\CthughaStd.h>

#ifndef FLUID_SIMULATOR_H_
#define FLUID_SIMULATOR_H_

struct FluidProperties
{
	float fRest_Density;
	float fGas_Stiffness;
	float fViscosity;
	float fParticleMass;
	float fRestitucionCoefficient;
	float fSurfaceTension;
	float fSPH_KernelRadius;
	UINT uNumParticles;
};

/** 
 * This class compute all fluid simulation process and
 * managing fast nns cell structure
 */

class FluidSimulator
{
	static const float fDeltaTime;

	bool m_bActive;

	/*Fluid Properties*/
	FluidProperties m_FluidProperties;

	/*Compute Shaders*/
	// Cell Management
	ShaderID m_BuildGrid_CS;
	ShaderID m_BitonicSort_CS;
	ShaderID m_MatrixTranspose_CS;
	ShaderID m_ClearGridIndices_CS;
	ShaderID m_BuildGridIndices_CS;

	/*Structured Buffers*/
	StructuredBufferID m_ParticlesBuffer;	// Particles position and velocity
	StructuredBufferID m_SortedParticlesBuffer;
	StructuredBufferID m_ParticleDensitiesBuffer;
	StructuredBufferID m_ParticleAccelerationsBuffer;
	StructuredBufferID m_GridBuffer;	// fast nns cells structure
	StructuredBufferID m_GridIndicesBuffer;

	void GPUGridSort();

public:
	FluidSimulator();
	~FluidSimulator() { }

	void Init(FluidProperties fluidProperties, Vec2 vInitialBoxDimension, float fInitialParticleSpacing);
	void Update(unsigned long deltaMs);

	bool IsActive() { m_bActive; }
	bool ChangeActiveState(bool bActive) { m_bActive = bActive; }
};

#endif