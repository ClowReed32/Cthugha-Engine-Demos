#ifndef MESHCOLLIDER_RESOURCE_H_
#define MESHCOLLIDER_RESOURCE_H_

#pragma once
//========================================================================
// File: MeshColliderResource.h - classes to collision with complex object
//
//========================================================================

#include "../Graphics/Geometry.h"
#include "../ResourceCache/ResCache.h"

class MeshColliderResourceLoader;

//
// class MeshColliderResourceExtraData
//
class MeshColliderResourceExtraData : public IResourceExtraData
{
	friend class MeshColliderResourceLoader;

public:
	MeshColliderResourceExtraData() 
	{
		//Nothing
	}

	virtual ~MeshColliderResourceExtraData()
	{

	}

	virtual std::string VToString()
	{
		return "MeshColliderResourceExtraData";
	}

	//Collider Attributes
	std::shared_ptr<Vec3>		m_pColliderVertex;
	std::shared_ptr<UINT>		m_pColliderIndex;

	UINT		m_NumVertex;
	UINT		m_NumIndices;
};

//
// class MeshColliderResourceLoader
//

class MeshColliderResourceLoader : public IResourceLoader
{

public:
	//Constructor
	MeshColliderResourceLoader()
	{

	}

	virtual bool VUseRawFile() { return false; }
	virtual bool VDiscardRawBufferAfterLoad() { return true; }
	virtual unsigned int VGetLoadedResourceSize(char *rawBuffer, unsigned int rawSize);
	virtual bool VLoadResource(char *rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle);
	virtual std::string VGetPattern() { return "*.phyc"; }
};

#endif