#include "CthughaStd.h"

#include "../CthughaEngine/CthughaApp.h"
#include "../ResourceCache/ResCache.h"
#include "MeshColliderResource.h"

shared_ptr<IResourceLoader> CreateMeshColliderResourceLoader()
{
	return shared_ptr<IResourceLoader>(CHG_NEW MeshColliderResourceLoader());
}

unsigned int MeshColliderResourceLoader::VGetLoadedResourceSize(char *rawBuffer, unsigned int rawSize)
{
	return rawSize;
}

bool MeshColliderResourceLoader::VLoadResource(char *rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle)
{
	shared_ptr<MeshColliderResourceExtraData> extra = shared_ptr<MeshColliderResourceExtraData>(CHG_NEW MeshColliderResourceExtraData());

	memoryStream cpPtr;
	cpPtr.buffer = rawBuffer;
	cpPtr.initialPtr = rawBuffer;
	cpPtr.size = rawSize;
	cpPtr.offset = 0;

	char fileType[5];
	memread ( fileType, sizeof(char), 4, &cpPtr );
	fileType[4] = '\0';

	if(strcmp(fileType, "PHYC") != 0)
	{
		CHG_ERROR("Unknow File Error");
		return false;
	}

	//Read numVertex and numIndices
	memread(&extra->m_NumVertex, sizeof(UINT), 1, &cpPtr);
	memread(&extra->m_NumIndices, sizeof(UINT), 1, &cpPtr);

	//Load vertex data and index data
	extra->m_pColliderVertex = CHG_NEW Vec3[extra->m_NumVertex];
	extra->m_pColliderIndex = CHG_NEW UINT[extra->m_NumIndices];

	memread(extra->m_pColliderVertex, sizeof(Vec3), extra->m_NumVertex, &cpPtr);
	memread(extra->m_pColliderIndex, sizeof(UINT), extra->m_NumIndices, &cpPtr);

	handle->SetExtra(shared_ptr<MeshColliderResourceExtraData>(extra));

	return true;
}