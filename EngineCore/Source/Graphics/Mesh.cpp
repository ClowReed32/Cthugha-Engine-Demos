//========================================================================
// Mesh.cpp - classes to render meshes in D3D9 and D3D11
//
//========================================================================

#include "CthughaStd.h"

#include "../CthughaEngine/CthughaApp.h"
#include "../ResourceCache/ResCache.h"
#include "..\Entities\RenderComponent.h"
//#include "Lights.h"
#include "SceneNodes.h"
#include "Mesh.h"
//#include "Raycast.h"
#include "SDFShadowManager.h"

#pragma warning (disable : 4244)

#define POS_NORM_UV_TAN 0
#define POS_UV_TAN 1

struct IndexStruct
{
	unsigned int data[6];
};

struct VertexStruct
{
	Vec3 Pos;
	Vec3 Norm;
	Vec2 UV;
	Vec3 Tang;
	Vec3 Bin;
};

shared_ptr<IResourceLoader> CreateCOLLADAMeshResourceLoader()
{
	return shared_ptr<IResourceLoader>(CHG_NEW COLLADAMeshResourceLoader());
}

unsigned int COLLADAMeshResourceLoader::VGetLoadedResourceSize(char *rawBuffer, unsigned int rawSize)
{
	return rawSize;
}

void COLLADAMeshResourceLoader::combineStaticComponents(float* &pVertex, UINT* &pIndex, UINT& numVertices, UINT& numIndex, UINT index)
{
    IndexStruct AuxElementVertex;
	std::vector<VertexStruct> Vertices;
	std::vector<UINT> Indices;
	std::vector<std::vector<IndexStruct>> SortVertexVector;
	unsigned int offset, count = 0;
	SortVertexVector.resize(m_vTempStaticMeshes[index].Positions.size());

	if(m_vTempStaticMeshes[index].TypeIndex == POS_NORM_UV_TAN)
		offset = 4;
	else
		offset = 3;

	for(unsigned int i = 0; i < m_vTempStaticMeshes[index].ComplexIndex.size(); i+=offset)
	{
		if(offset == 4)
		{
			AuxElementVertex.data[0] = m_vTempStaticMeshes[index].ComplexIndex[i];
			AuxElementVertex.data[1] = m_vTempStaticMeshes[index].ComplexIndex[i + 1];
			AuxElementVertex.data[2] = m_vTempStaticMeshes[index].ComplexIndex[i + 2];
			AuxElementVertex.data[3] = m_vTempStaticMeshes[index].ComplexIndex[i + 3];
			AuxElementVertex.data[4] = i;
		}
		else
		{
			AuxElementVertex.data[0] = m_vTempStaticMeshes[index].ComplexIndex[i];
			AuxElementVertex.data[1] = m_vTempStaticMeshes[index].ComplexIndex[i + 1];
			AuxElementVertex.data[2] = m_vTempStaticMeshes[index].ComplexIndex[i + 2];
			AuxElementVertex.data[4] = i;
		}

		SortVertexVector[AuxElementVertex.data[0]].push_back(AuxElementVertex);
	}
	
	for(unsigned int i = 0; i < m_vTempStaticMeshes[index].Positions.size(); i++)
	{
		SortVertexVector[i][0].data[5] = count;
		count++;

		if(offset == 4)
		{
			VertexStruct VertexData = {m_vTempStaticMeshes[index].Positions[SortVertexVector[i][0].data[0]], m_vTempStaticMeshes[index].Normals[SortVertexVector[i][0].data[1]], m_vTempStaticMeshes[index].UVs[SortVertexVector[i][0].data[2]], m_vTempStaticMeshes[index].Tangents[SortVertexVector[i][0].data[3]], m_vTempStaticMeshes[index].BiTangents[SortVertexVector[i][0].data[3]]};
			Vertices.push_back(VertexData);
		}
		else
		{
			VertexStruct VertexData = {m_vTempStaticMeshes[index].Positions[SortVertexVector[i][0].data[0]], m_vTempStaticMeshes[index].Normals[SortVertexVector[i][0].data[0]], m_vTempStaticMeshes[index].UVs[SortVertexVector[i][0].data[1]], m_vTempStaticMeshes[index].Tangents[SortVertexVector[i][0].data[2]], m_vTempStaticMeshes[index].BiTangents[SortVertexVector[i][0].data[2]]};
			Vertices.push_back(VertexData);
		}

		for(unsigned int j = 1; j < SortVertexVector[i].size(); j++)
		{
			bool repeat = false;
			unsigned int ElementRepeat = 0;

			for(unsigned int k = 0; k < j; k++)
			{
				if(SortVertexVector[i][j].data[1] == SortVertexVector[i][k].data[1])
				{
					if(SortVertexVector[i][j].data[2] == SortVertexVector[i][k].data[2])
					{
						if(SortVertexVector[i][j].data[3] == SortVertexVector[i][k].data[3])
						{						
							repeat = true;
							ElementRepeat = k;
							break;
						}
					}
				}
			}

			if(repeat)
			{
				SortVertexVector[i][j].data[5] = SortVertexVector[i][ElementRepeat].data[5];
			}
			else
			{
				SortVertexVector[i][j].data[5] = count;
				count++;
				if(offset == 4)
				{
					VertexStruct VertexData = {m_vTempStaticMeshes[index].Positions[SortVertexVector[i][j].data[0]], m_vTempStaticMeshes[index].Normals[SortVertexVector[i][j].data[1]], m_vTempStaticMeshes[index].UVs[SortVertexVector[i][j].data[2]], m_vTempStaticMeshes[index].Tangents[SortVertexVector[i][j].data[3]], m_vTempStaticMeshes[index].BiTangents[SortVertexVector[i][j].data[3]]};
					Vertices.push_back(VertexData);
				}
				else
				{
					VertexStruct VertexData = {m_vTempStaticMeshes[index].Positions[SortVertexVector[i][j].data[0]], m_vTempStaticMeshes[index].Normals[SortVertexVector[i][j].data[0]], m_vTempStaticMeshes[index].UVs[SortVertexVector[i][j].data[1]], m_vTempStaticMeshes[index].Tangents[SortVertexVector[i][j].data[2]], m_vTempStaticMeshes[index].BiTangents[SortVertexVector[i][j].data[2]]};
					Vertices.push_back(VertexData);
				}
			}
		}
	}

	for(unsigned int i = 0; i < m_vTempStaticMeshes[index].Positions.size(); i++)
	{
		for(unsigned int j = 0; j < SortVertexVector[i].size(); j++)
		{
			m_vTempStaticMeshes[index].ComplexIndex[SortVertexVector[i][j].data[4]] = SortVertexVector[i][j].data[5];
		}
	}

	for(unsigned int i = 0; i < m_vTempStaticMeshes[index].ComplexIndex.size(); i+=offset)
	{
		Indices.push_back(m_vTempStaticMeshes[index].ComplexIndex[i]);
	}

	numIndex = Indices.size();
	numVertices = Vertices.size();

	pIndex = (UINT*)malloc(numIndex*sizeof(UINT));
	pVertex = (float*)malloc(numVertices*sizeof(VertexStruct));

	int aux1 = numVertices*sizeof(VertexStruct);
	int aux2 = 3*numVertices + 3*numVertices + 2*numVertices + 3*numVertices + 3*numVertices;

	for(UINT i = 0; i < numIndex; i++)
	{
		pIndex[i] = Indices[i];
	}

	UINT offsetVertex[4] = {3*numVertices, 3*numVertices + 3*numVertices, 3*numVertices + 3*numVertices + 2*numVertices, 3*numVertices + 3*numVertices + 2*numVertices + 3*numVertices};
	count = 0;

	for(UINT i = 0; i < numVertices; i++)
	{
		pVertex[count] = Vertices[i].Pos.x;
		pVertex[count + 1] = Vertices[i].Pos.y;
		pVertex[count + 2] = Vertices[i].Pos.z;

		pVertex[count + offsetVertex[0]] = Vertices[i].Norm.x;
		pVertex[count + offsetVertex[0] + 1] = Vertices[i].Norm.y;
		pVertex[count + offsetVertex[0] + 2] = Vertices[i].Norm.z;

		//pVertex[count + offsetVertex[1]] = Vertices[i].UV.x;
		//pVertex[count + offsetVertex[1] + 1] = Vertices[i].UV.y;

		pVertex[count + offsetVertex[2]] = Vertices[i].Tang.x;
		pVertex[count + offsetVertex[2] + 1] = Vertices[i].Tang.y;
		pVertex[count + offsetVertex[2] + 2] = Vertices[i].Tang.z;

		pVertex[count + offsetVertex[3]] = Vertices[i].Bin.x;
		pVertex[count + offsetVertex[3] + 1] = Vertices[i].Bin.y;
		pVertex[count + offsetVertex[3] + 2] = Vertices[i].Bin.z;

		count += 3;
	}

	count = 0;

	for(UINT i = 0; i < numVertices; i++)
	{
		pVertex[count + offsetVertex[1]] = Vertices[i].UV.x;
		pVertex[count + offsetVertex[1] + 1] = Vertices[i].UV.y;

		count += 2;
	}	
}

bool COLLADAMeshResourceLoader::VLoadResource(char *rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle)
{
	const std::string fileURI("from std::istream");
	shared_ptr<COLLADAMeshResourceExtraData> extra = shared_ptr<COLLADAMeshResourceExtraData>(CHG_NEW COLLADAMeshResourceExtraData());
	
	shared_ptr<Renderer> renderer = g_pApp->m_pRenderer;

	if(renderer.get() == NULL) return false;

	#pragma region //Open the file and get the root node
	dae.Parse(rawBuffer, NULL, TIXML_ENCODING_UTF8);
	root = dae.RootElement();

	//Check if import succeded
	if(!root)
	{
		CHG_ERROR("Document import failed. \n");
		return false;
	}
	#pragma endregion
		
	#pragma region //Get the library_visual_scenes
	library_visual_scenes = root->FirstChildElement("library_visual_scenes");

	//Check if there is a <library_visual_scenes>
	if(!library_visual_scenes)
	{
		CHG_ERROR("<library_visual_scenes> not found.\n");
		return false;
	}
	#pragma endregion

	#pragma region //Get the library_geometries
	library_geometries = root->FirstChildElement("library_geometries");

	//Check if there is a <library_geometries>
	if(!library_geometries)
	{
		CHG_ERROR("<library_geometries> not found.\n");
		return false;
	}
	#pragma endregion

	#pragma region //Get the library_images
	library_images = root->FirstChildElement("library_images");

	//Check if there is a <library_geometries>
	if(!library_images)
	{
		//CHG_ERROR("<library_images> not found.\n");
		//return false;
	}
	#pragma endregion

	#pragma region //Get the library_materials
	library_materials = root->FirstChildElement("library_materials");

	//Check if there is a <library_materials>
	if (!library_materials)
		CHG_WARNING("<library_materials> not found.\n");
	#pragma endregion

	#pragma region //Get the library_effects
	library_effects = root->FirstChildElement("library_effects");

	//Check if there is a <library_effects>
	if (!library_effects)
		CHG_WARNING("<library_effects> not found.\n");
	#pragma endregion

	//Get the <library_animations>
	library_animations = root->FirstChildElement("library_animations");

	//Create map for geometries
	for (TiXmlElement* pNode = library_geometries->FirstChildElement(); pNode; pNode = pNode->NextSiblingElement())
	{
		std::string id = pNode->Attribute("id");
		m_GeometriesList[id] = pNode;
	}

	//Create map for materials
	for (TiXmlElement* pNode = library_materials->FirstChildElement(); pNode; pNode = pNode->NextSiblingElement())
	{
		std::string id = pNode->Attribute("id");
		m_MaterialsList[id] = pNode;
	}

	//Create map for effects
	for (TiXmlElement* pNode = library_effects->FirstChildElement(); pNode; pNode = pNode->NextSiblingElement())
	{
		std::string id = pNode->Attribute("id");
		m_EffectsList[id] = pNode;
	}

	//Process <library_visual_scenes>
	processVisualScenes();

	//processSkinnedMeshes
	//processSkinnedMeshes(meshes);
	
	//Process library_animations if it exists
	/*if(library_animations) 
		processAnimations(meshes);*/

	//Process the <geometry> node for each mesh
	processGeometries();

	//processMaterial();

	//Compile vertex components into one buffer for each Skinned Mesh
	//and also compile Animation Keyframes list
	/*for(unsigned int i = 0; i < meshes->SkinnedMeshes.size(); i++)
	{
		meshes->SkinnedMeshes[i]->combineComponents();
		meshes->SkinnedMeshes[i]->combineJointAnimations();
	}*/

	//Compile vertex components into one buffer

	extra->m_pMesh.reset(CHG_NEW COLLADAMesh());
	extra->m_pMesh->m_MeshAabb.isVoid = true;

	for(unsigned int i = 0; i < m_vTempStaticMeshes.size(); i++)
	{
		float *vertexData = NULL;
		UINT *indexData = NULL;
		UINT numVertex = 0;
		UINT numIndex = 0;

		combineStaticComponents(vertexData, indexData, numVertex, numIndex, i);

		//Create SubMesh
		UINT offsetVertex[4] = {3*numVertex, 3*numVertex + 3*numVertex, 3*numVertex + 3*numVertex + 2*numVertex, 3*numVertex + 3*numVertex + 2*numVertex + 3*numVertex};
		extra->m_pMesh->m_vSubMeshes.push_back(COLLADASubMesh(m_vTempStaticMeshes[i].name));
		extra->m_pMesh->m_vSubMeshes.back().m_pParent = extra->m_pMesh.get();

		//Create Vertex and Index Buffer for this SubMesh
		extra->m_pMesh->m_pRenderer = renderer;
		extra->m_pMesh->m_vSubMeshes.back().m_PosVertexBufferID = renderer->addVertexBuffer(numVertex * sizeof(Vec3), STATIC, vertexData);
		extra->m_pMesh->m_vSubMeshes.back().m_NormVertexBufferID = renderer->addVertexBuffer(numVertex * sizeof(Vec3), STATIC, &vertexData[offsetVertex[0]]);
		extra->m_pMesh->m_vSubMeshes.back().m_UVVertexBufferID = renderer->addVertexBuffer(numVertex * sizeof(Vec2), STATIC, &vertexData[offsetVertex[1]]);
		extra->m_pMesh->m_vSubMeshes.back().m_TangVertexBufferID = renderer->addVertexBuffer(numVertex * sizeof(Vec3), STATIC, &vertexData[offsetVertex[2]]);
		extra->m_pMesh->m_vSubMeshes.back().m_BinormVertexBufferID = renderer->addVertexBuffer(numVertex * sizeof(Vec3), STATIC, &vertexData[offsetVertex[3]]);

		extra->m_pMesh->m_vSubMeshes.back().m_IndexBufferID = renderer->addIndexBuffer(numIndex, sizeof(unsigned int), STATIC, indexData);
		extra->m_pMesh->m_vSubMeshes.back().m_NumVertex = numVertex;
		extra->m_pMesh->m_vSubMeshes.back().m_NumIndex = numIndex;
		extra->m_pMesh->m_vSubMeshes.back().m_LocalTransform = m_vTempStaticMeshes[i].localMatrix;
		
		extra->m_pMesh->m_vSubMeshes[i].m_SubMeshAabb.SetFromVertexSet(m_vTempStaticMeshes[i].Positions.data(), m_vTempStaticMeshes[i].Positions.size());
		extra->m_pMesh->m_MeshAabb = extra->m_pMesh->m_MeshAabb.Union(extra->m_pMesh->m_vSubMeshes[i].m_SubMeshAabb);
	}

	std::string name = handle->GetName();

	std::size_t begin = 0, end = 0, aux = 0;

	while(aux != std::string::npos)
	{
		begin = aux;
		aux = name.find("\\", aux + 1);
	}

	begin++;

	end = name.find(".dae");
	end -= begin;

	name = name.substr(begin, end);

	extra->m_pMesh->setName(name);
	handle->SetExtra(shared_ptr<COLLADAMeshResourceExtraData>(extra));
	
	//Set the pointers back to NULL, safety precaution for Debug build
	m_GeometriesList.clear();
	m_EffectsList.clear();
	m_MaterialsList.clear();

	root = NULL; library_visual_scenes = NULL; library_geometries = NULL;
	dae.Clear();
	m_vTempStaticMeshes.clear();

	return true;
}

void COLLADAMeshResourceLoader::processVisualScenes()
{
	//Get the <visual_scene> node
	TiXmlElement* visual_scene = library_visual_scenes->FirstChildElement("visual_scene");

	//Get all the <node>'s for the <visual_scene>
	std::vector<TiXmlElement*> nodes;

	for (TiXmlElement* pNode = visual_scene->FirstChildElement(); pNode; pNode = pNode->NextSiblingElement())
	{
		nodes.push_back(pNode);
	}

	//For each <node>...
	for (unsigned int i = 0; i < nodes.size(); i++)
	{
		//Get the name and the type, if they exist
		std::string Name = nodes[i]->Attribute("name");
		std::string Type = nodes[i]->Attribute("type");

		//Skip JOINT node's, only meshes
		if (Type == "JOINT") continue;

		TiXmlElement* instance_controller = NULL;
		instance_controller = nodes[i]->FirstChildElement("instance_controller");

		//Get the <instance_geometry> node that corresponds to this <node>
		TiXmlElement* instance_geometry = NULL;
		instance_geometry = nodes[i]->FirstChildElement("instance_geometry");

		//If there is no <instance_geometry>, this isn't a static mesh and we will skip it.
		if (!instance_geometry) continue;

		//Get the <geometry> node that is referenced by the <instance_geometry>
		std::string url = instance_geometry->Attribute("url");
		url = url.substr(1, url.length() - 1);
		TiXmlElement* geometry = m_GeometriesList.find(url)->second;


		//If the referenced node was not found, skip this node
		if (!geometry) continue;

		TiXmlElement* bind_material = NULL;
		TiXmlElement* material = NULL;

		bind_material = instance_geometry->FirstChildElement("bind_material");
		bind_material = bind_material->FirstChildElement("technique_common");
		bind_material = bind_material->FirstChildElement("instance_material");

		if (bind_material)
		{
			std::string target = bind_material->Attribute("target");
			target = target.substr(1, target.length() - 1);
			material = m_MaterialsList.find(target)->second;
		}

		TempMesh AuxMesh;

		AuxMesh.name = Name;
		AuxMesh.localMatrix = processMatrix(nodes[i]->FirstChildElement("matrix"));

		//Now create a new mesh, set it's <geometry> node and get it's World transform.
		m_vTempStaticMeshes.push_back(AuxMesh);
		m_vTempStaticMeshes.back().geometry = geometry;
		m_vTempStaticMeshes.back().materialNode = material;
	}
}

//Process a <geometry> node for each mesh
void COLLADAMeshResourceLoader::processGeometries()
{
	//Foreach mesh...

	//static Mesh
	for (unsigned int i = 0; i < m_vTempStaticMeshes.size(); i++)
	{
		//Get the <mesh> node
		TiXmlElement* mesh = m_vTempStaticMeshes[i].geometry->FirstChildElement("mesh");

		//Get the <source> nodes
		std::vector<TiXmlElement*> sources;

		for (TiXmlElement* pNode = mesh->FirstChildElement("source"); pNode; pNode = pNode->NextSiblingElement("source"))
		{
			sources.push_back(pNode);
		}

		//Get the <triangles> node (yes it will be in the sources array above if you wish to find it that way)
		TiXmlElement* triangles = mesh->FirstChildElement("triangles");

		//Process each <source> child
		processSources(&m_vTempStaticMeshes[i], sources);

		//Process the <triangles> child
		processTriangles(&m_vTempStaticMeshes[i], triangles);
	}

	//skinned Mesh
	/*for(unsigned int i = 0; i < meshes->SkinnedMeshes.size(); i++)
	{
		//Get the <mesh> node
		daeElement* mesh = meshes->SkinnedMeshes[i]->geometry->getDescendant("mesh");

		//Get the <source> nodes
		daeTArray<daeElementRef> sources = mesh->getChildren();

		//Get the <triangles> node (yes it will be in the sources array above if you wish to find it that way)
		daeElement* triangles = mesh->getDescendant("triangles");

		//Process each <source> child
		for(unsigned int z = 0; z < sources.getCount(); z++) processSource(meshes->SkinnedMeshes[i], sources[z]);

		//Process the <triangles> child
		processTriangles(meshes->SkinnedMeshes[i], triangles);
	}*/
}

void COLLADAMeshResourceLoader::processMaterial()
{

	/*for(unsigned int i = 0; i < m_vTempStaticMeshes.size(); i++)
	{
		//meshes->StaticMeshes[i]->stdMaterial = meshes->StaticMeshes[i]->material->getAttribute("name");

		TiXmlElement* instance_effect = NULL;
		instance_effect = m_vTempStaticMeshes[i].materialNode->FirstChildElement("instance_effect");

		TiXmlElement* effect = NULL;

		if(instance_effect)
		{
			std::string url = instance_effect->Attribute("url");
			url = url.substr(1, url.length() - 1);
			effect = m_EffectsList.find(url)->second;

			Material MaterialAux;

			effect = effect->FirstChildElement("profile_COMMON");
			effect = effect->FirstChildElement("technique");

            TiXmlElement* extra = effect->FirstChildElement("extra");

            if (extra)
                extra = extra->FirstChildElement("technique");

			TiXmlElement* effectAux = effect->FirstChildElement("lambert");

			if (!effectAux)
				effectAux = effect->FirstChildElement("blinn");

			effect = effectAux;

			TiXmlElement* emissive = NULL;
			emissive = effect->FirstChildElement("emission");
			if(emissive)
			{
				TiXmlElement* emissiveColor = NULL;
				emissiveColor = emissive->FirstChildElement("color");

				if(emissiveColor)
				{
					Vec4 kE;
					std::string pArray = emissiveColor->GetText();
					std::stringstream stm(pArray);

					stm >> kE.x;
					stm >> kE.y;
					stm >> kE.z;
					stm >> kE.w;

					MaterialAux.setkE(kE);
				}
			}

            TiXmlElement* tesselationLevel = NULL;
            tesselationLevel = effect->FirstChildElement("tesselationLevel");
            if (tesselationLevel)
            {
                TiXmlElement* tesselationLevelValue = NULL;
                tesselationLevelValue = tesselationLevel->FirstChildElement("float");

                if (tesselationLevelValue)
                {
                    float fTesselationLevel;
                    std::string pArray = tesselationLevelValue->GetText();
                    std::stringstream stm(pArray);

                    stm >> fTesselationLevel;

					MaterialAux.setDisplacementDepth(fTesselationLevel);
                }
            }

            TiXmlElement* tesselationSubdivision = NULL;
            tesselationSubdivision = effect->FirstChildElement("tesselationSubdivision");
            if (tesselationSubdivision)
            {
                TiXmlElement* tesselationSubdivisionValue = NULL;
                tesselationSubdivisionValue = tesselationSubdivision->FirstChildElement("float");

                if (tesselationSubdivisionValue)
                {
                    float ftesselationSubdivision;
                    std::string pArray = tesselationSubdivisionValue->GetText();
                    std::stringstream stm(pArray);

                    stm >> ftesselationSubdivision;

                    MaterialAux.setTesselationSubdivision(ftesselationSubdivision);
                }
            }

			TiXmlElement* ambient = NULL;
			ambient = effect->FirstChildElement("ambient");
			if(ambient)
			{
				TiXmlElement* ambientColor = NULL;
				ambientColor = ambient->FirstChildElement("color");

				if(ambientColor)
				{
					Vec4 kA;
					std::string pArray = ambientColor->GetText();
					std::stringstream stm(pArray);

					stm >> kA.x;
					stm >> kA.y;
					stm >> kA.z;
					stm >> kA.w;

					MaterialAux.setAmbientColor(kA);
					MaterialAux.setkA(1.0f);
				}
			}

			TiXmlElement* specular = NULL;
			specular = effect->FirstChildElement("specular");
			if(specular)
			{
				TiXmlElement* color = NULL;
				color = specular->FirstChildElement("color");

				if(color)
				{
					Vec4 specularColor;
					std::string pArray = color->GetText();
					std::stringstream stm(pArray);

					stm >> specularColor.x;
					stm >> specularColor.y;
					stm >> specularColor.z;
					stm >> specularColor.w;
								
					MaterialAux.setSpecularColor(specularColor);
					MaterialAux.setkS(specularColor.x);
				}

				TiXmlElement* SpecularTexture = NULL;
				SpecularTexture = specular->FirstChildElement("texture");

				if(SpecularTexture)
				{
					std::string SpecularFile = SpecularTexture->Attribute("texture");
					SpecularFile.resize(SpecularFile.length() - 8);	

                    SpecularFile.replace(SpecularFile.find_last_of("_"), 1, ".");

					m_vTempStaticMeshes[i].m_SpecularTextureName = SpecularFile;
				}
			}
			
			TiXmlElement* shininess = NULL;
			shininess = effect->FirstChildElement("shininess");
			if(shininess)
			{
				float Exponent;
				TiXmlElement* value = shininess->FirstChildElement("float");
				std::string pArray = value->GetText();
				std::stringstream stm(pArray);

				stm >> Exponent;
				MaterialAux.setExponect(Exponent);
			}

			TiXmlElement* effect_texture = NULL;
			effect_texture = effect->FirstChildElement("texture");

			TiXmlElement* diffuse = NULL;
			diffuse = effect->FirstChildElement("diffuse");
			
			if(diffuse)
			{
				TiXmlElement* DiffuseTexture = NULL;
				DiffuseTexture = diffuse->FirstChildElement("texture");

				if(DiffuseTexture)
				{
                    std::string DiffuseTextureFile = DiffuseTexture->Attribute("texture");
					DiffuseTextureFile.resize(DiffuseTextureFile.length() - 8);

                    DiffuseTextureFile.replace(DiffuseTextureFile.find_last_of("_"), 1, ".");

					m_vTempStaticMeshes[i].m_DiffuseTextureName = DiffuseTextureFile;

					TiXmlElement* Extra = NULL;
					Extra = DiffuseTexture->FirstChildElement("extra");

					if (Extra)
					{
						Extra = Extra->FirstChildElement("technique");

						if (Extra)
						{
							Extra = Extra->FirstChildElement("mirrorU");
							
							if (Extra)
							{
								std::string mirrored = Extra->GetText();

								MaterialAux.setMirroredTexture(mirrored == "1" ? true : false);
							}
						}
					}
				}

				TiXmlElement* DiffuseColor = NULL;
				DiffuseColor = diffuse->FirstChildElement("color");

				if(DiffuseColor)
				{
					Vec4 diffuseColor;
					std::string pArray = DiffuseColor->GetText();
					std::stringstream stm(pArray);

					stm >> diffuseColor.x;
					stm >> diffuseColor.y;
					stm >> diffuseColor.z;
					stm >> diffuseColor.w;
								
					MaterialAux.setDiffuseColor(diffuseColor);
					MaterialAux.setkD(1.0f);
				}
				
			}
			
            if (extra)
            {
                TiXmlElement* bump = NULL;
                bump = extra->FirstChildElement("bump");

                if (bump)
                {
                    TiXmlElement* BumpTexture = NULL;
                    BumpTexture = bump->FirstChildElement("texture");

                    if (BumpTexture)
                    {
                        std::string BumpFile = BumpTexture->Attribute("texture");
                        BumpFile.resize(BumpFile.length() - 8);

                        BumpFile.replace(BumpFile.find_last_of("_"), 1, ".");

                        m_vTempStaticMeshes[i].m_NormalTextureName = BumpFile;
                    }
                }
            }

			m_vTempStaticMeshes[i].material = MaterialAux;
		}
	}*/

	/*for(unsigned int i = 0; i < meshes->SkinnedMeshes.size(); i++)
	{
		meshes->SkinnedMeshes[i]->stdMaterial = meshes->SkinnedMeshes[i]->material->getAttribute("name");
	}*/
}

//Process a <source> node
void COLLADAMeshResourceLoader::processSources(TempMesh *mesh, std::vector<TiXmlElement*> sources)
{
	for (unsigned int z = 0; z < sources.size(); z++)
	{
		TiXmlElement* source = sources[z];

		//Get Positions
		if (std::string(source->Attribute("id")).find("position") != std::string::npos)
		{
			//Get the <float_array> node
			TiXmlElement* float_array = source->FirstChildElement("float_array");

			//Get the number of raw float's contained
			unsigned int count = atoi(float_array->Attribute("count"));

			//Get the raw string representation
			std::string positionArray = float_array->GetText();

			//Read each float, in groups of three
			char* positionArrayAux = CHG_NEW char[positionArray.size() + 1];
			strcpy(positionArrayAux, positionArray.c_str());

			char *tok = strtok(positionArrayAux, " ");

			for (unsigned int i = 0; i < (count / 3); i++)
			{
				float x, y, z;

				x = atof(tok);
				tok = strtok(NULL, " ");
				y = atof(tok);
				tok = strtok(NULL, " ");
				z = atof(tok);
				tok = strtok(NULL, " ");

				//Push this back as another Position component
				mesh->Positions.push_back(Vec3(x, y, z));
			}

			delete[] positionArrayAux;
		}

		//Get Normals
		if (std::string(source->Attribute("id")).find("normal") != std::string::npos)
		{
			//Get the <float_array> node
			TiXmlElement* float_array = source->FirstChildElement("float_array");

			//Get the number of raw float's contained
			unsigned int count = atoi(float_array->Attribute("count"));

			//Get the raw string representation
			std::string normalsArray = float_array->GetText();

			//Read each float, in groups of three
			char* normalsArrayAux = CHG_NEW char[normalsArray.size() + 1];
			strcpy(normalsArrayAux, normalsArray.c_str());

			char *tok = strtok(normalsArrayAux, " ");

			for (unsigned int i = 0; i < (count / 3); i++)
			{
				float x, y, z;

				x = atof(tok);
				tok = strtok(NULL, " ");
				y = atof(tok);
				tok = strtok(NULL, " ");
				z = atof(tok);
				tok = strtok(NULL, " ");

				//Push this back as another Position component
				mesh->Normals.push_back(Vec3(x, y, z));
			}

			delete[] normalsArrayAux;
		}

		//Get UVs at layer0
		if (std::string(source->Attribute("id")).find("map1") != std::string::npos || std::string(source->Attribute("id")).find("uvSet1") != std::string::npos)
		{
			//Get the <float_array> node
			TiXmlElement* float_array = source->FirstChildElement("float_array");

			//Get the number of raw float's contained
			unsigned int count = atoi(float_array->Attribute("count"));

			//Get the raw string representation
			std::string uvArray = float_array->GetText();

			//Read each float, in groups of three
            char* uvsArrayAux = CHG_NEW char[uvArray.size() + 1];
			strcpy(uvsArrayAux, uvArray.c_str());

			char *tok = strtok(uvsArrayAux, " ");

			for (unsigned int i = 0; i < (count / 2); i++)
			{
				float x, y;

				x = atof(tok);
				tok = strtok(NULL, " ");
				y = atof(tok);
				tok = strtok(NULL, " ");

				//Push this back as another Position component
				mesh->UVs.push_back(Vec2(x, y));
			}

			delete[] uvsArrayAux;
		}

		//Get Tangents at layer0, the reason there are different naming schemes, this covers the ones I've come into contact with
		if (std::string(source->Attribute("id")).find("map1-tangents") != std::string::npos || std::string(source->Attribute("id")).find("textangents") != std::string::npos)
		{
			//Get the <float_array> node
			TiXmlElement* float_array = source->FirstChildElement("float_array");

			//Get the number of raw float's contained
			unsigned int count = atoi(float_array->Attribute("count"));

			//Get the raw string representation
			std::string tangentsArray = float_array->GetText();

			//Read each float, in groups of three
            char* tangentsArrayAux = CHG_NEW char[tangentsArray.size() + 1];
			strcpy(tangentsArrayAux, tangentsArray.c_str());

			char *tok = strtok(tangentsArrayAux, " ");

			for (unsigned int i = 0; i < (count / 3); i++)
			{
				float x, y, z;

				x = atof(tok);
				tok = strtok(NULL, " ");
				y = atof(tok);
				tok = strtok(NULL, " ");
				z = atof(tok);
				tok = strtok(NULL, " ");

				//Push this back as another Position component
				mesh->Tangents.push_back(Vec3(x, y, z));
			}

			delete[] tangentsArrayAux;
		}

		//Get BiTangents at layer0, read above about the different names
		if (std::string(source->Attribute("id")).find("map1-binormals") != std::string::npos || std::string(source->Attribute("id")).find("texbinormals") != std::string::npos)
		{
			//Get the <float_array> node
			TiXmlElement* float_array = source->FirstChildElement("float_array");

			//Get the number of raw float's contained
			unsigned int count = atoi(float_array->Attribute("count"));

			//Get the raw string representation
			std::string biTangentsArray = float_array->GetText();

			//Read each float, in groups of three
            char* biTangentsArrayAux = CHG_NEW char[biTangentsArray.size() + 1];
			strcpy(biTangentsArrayAux, biTangentsArray.c_str());

			char *tok = strtok(biTangentsArrayAux, " ");

			for (unsigned int i = 0; i < (count / 3); i++)
			{
				float x, y, z;

				x = atof(tok);
				tok = strtok(NULL, " ");
				y = atof(tok);
				tok = strtok(NULL, " ");
				z = atof(tok);
				tok = strtok(NULL, " ");

				//Push this back as another Position component
				mesh->BiTangents.push_back(Vec3(x, y, z));
			}

			delete[] biTangentsArrayAux;
		}

	}
}

//Process a <triangles> node
void COLLADAMeshResourceLoader::processTriangles(TempMesh *mesh, TiXmlElement* triangles)
{
	//Get the <p> node
	TiXmlElement* p = triangles->FirstChildElement("p");
	unsigned int elementPerVertex = 0;

	//Get Semantic
	std::vector<TiXmlElement*> semantic;

	for (TiXmlElement* pNode = triangles->FirstChildElement("input"); pNode; pNode = pNode->NextSiblingElement("input"))
	{
		semantic.push_back(pNode);
	}

	if (semantic.size() == 5)
	{
		mesh->TypeIndex = POS_NORM_UV_TAN;
		elementPerVertex = 4;
	}
	else
	{
		mesh->TypeIndex = POS_UV_TAN;
		elementPerVertex = 3;
	}

	//Get the number of faces, multiply by 3 to get number of indices
	unsigned int count = atoi(triangles->Attribute("count")) * 3;

	//Get the raw string representation
	std::string pArray = p->GetText();

	//Read each unsigned int
    char* pArrayAux = CHG_NEW char[pArray.size() + 1];
	strcpy(pArrayAux, pArray.c_str());

	char *tok = strtok(pArrayAux, " ");

	for (unsigned int i = 0; i < elementPerVertex*count; i++)
	{
		unsigned int p = 0;

		p = atoi(tok);
		tok = strtok(NULL, " ");

		//Push this back as another Position component
		mesh->ComplexIndex.push_back(p);
	}

	delete[] pArrayAux;

}

//Read a <matrix> node, better to have this as a generalized function, will read into OpenGL style, conversion to DirectX later...
Mat4x4 COLLADAMeshResourceLoader::processMatrix(TiXmlElement* node)
{
	Mat4x4 out;
	std::string world = node->GetText();
	std::stringstream stm(world);

#pragma region Read the matrix
	stm >> out.rows[0].x;
	stm >> out.rows[0].y;
	stm >> out.rows[0].z;
	stm >> out.rows[0].w;

	stm >> out.rows[1].x;
	stm >> out.rows[1].y;
	stm >> out.rows[1].z;
	stm >> out.rows[1].w;

	stm >> out.rows[2].x;
	stm >> out.rows[2].y;
	stm >> out.rows[2].z;
	stm >> out.rows[2].w;

	stm >> out.rows[3].x;
	stm >> out.rows[3].y;
	stm >> out.rows[3].z;
	stm >> out.rows[3].w;
#pragma endregion

	return out;
}

////////////////////////////////////////////////////
// COLLADAMesh Implementation
////////////////////////////////////////////////////

COLLADAMesh::~COLLADAMesh()
{
	for(UINT i=0; i < m_vSubMeshes.size(); i++)
	{
		m_vSubMeshes.pop_back();
	}
}

void COLLADAMesh::Render()
{
	for(UINT i=0; i < m_vSubMeshes.size(); i++)
	{
		m_vSubMeshes[i].Render();
	}
}

void COLLADAMesh::RenderInstances(const UINT uNumInstances)
{
	for (UINT i = 0; i < m_vSubMeshes.size(); i++)
	{
		m_vSubMeshes[i].RenderInstances(uNumInstances);
	}
}

void COLLADAMesh::RemoveSubMeshes()
{
	for (UINT i = 0; i < m_vSubMeshes.size(); i++)
	{
		m_vSubMeshes[i].Remove();
	}
}

void COLLADASubMesh::RenderInstances(const UINT uNumInstances)
{
	//Set separate vertexBuffers

	VertexBufferID vertexBuffer[5] = { m_PosVertexBufferID, m_NormVertexBufferID, m_UVVertexBufferID, m_TangVertexBufferID, m_BinormVertexBufferID };
	UINT offset[5] = { 0, 0, 0, 0, 0 };

	m_pParent->m_pRenderer->setVertexBuffer(0, 5, vertexBuffer, offset);
	m_pParent->m_pRenderer->setIndexBuffer(m_IndexBufferID);

	m_pParent->m_pRenderer->apply();

	if (m_pParent->m_pRenderer->activeShaderHasTessellation())
		m_pParent->m_pRenderer->drawElementsInstanced(PRIM_PATCHES, 0, m_NumIndex, 0, m_NumVertex, uNumInstances, 0);
	else
		m_pParent->m_pRenderer->drawElementsInstanced(m_PrimitiveType, 0, m_NumIndex, 0, m_NumVertex, uNumInstances, 0);
}

void COLLADASubMesh::Render()
{
	//Set separate vertexBuffers
	
	VertexBufferID vertexBuffer[5] = {m_PosVertexBufferID, m_NormVertexBufferID, m_UVVertexBufferID, m_TangVertexBufferID, m_BinormVertexBufferID};
	UINT offset[5] = {0, 0, 0, 0, 0};

	m_pParent->m_pRenderer->setVertexBuffer(0, 5, vertexBuffer, offset);
	m_pParent->m_pRenderer->setIndexBuffer(m_IndexBufferID);

	m_pParent->m_pRenderer->apply();

    if(m_pParent->m_pRenderer->activeShaderHasTessellation())
	    m_pParent->m_pRenderer->drawElements(PRIM_PATCHES, 0, m_NumIndex, 0, m_NumVertex);
    else
        m_pParent->m_pRenderer->drawElements(m_PrimitiveType, 0, m_NumIndex, 0, m_NumVertex);
}

void COLLADASubMesh::Remove()
{
	m_pParent->m_pRenderer->removeIndexBuffer(m_IndexBufferID);
	m_pParent->m_pRenderer->removeVertexBuffer(m_PosVertexBufferID);
	m_pParent->m_pRenderer->removeVertexBuffer(m_NormVertexBufferID);
	m_pParent->m_pRenderer->removeVertexBuffer(m_UVVertexBufferID);
	m_pParent->m_pRenderer->removeVertexBuffer(m_TangVertexBufferID);
	m_pParent->m_pRenderer->removeVertexBuffer(m_BinormVertexBufferID);

	m_Material.RemoveTextures();
}

////////////////////////////////////////////////////
// MeshNode Implementation
////////////////////////////////////////////////////

MeshNode::MeshNode(const EntityId entityId, 
		WeakBaseRenderComponentPtr renderComponent,
		std::string meshFileName, 
		const Mat4x4 *world, const Vec3 vScale, const std::string sMaterialResource, const std::string *shaderFileNames, const std::string sGIMaterialResource, const std::string sGIShaderResource, RenderProperties renderProperties)
{
	//Load Mesh from file if it`s necessary
	Resource resource(meshFileName);
	shared_ptr<ResHandle> handle = g_pApp->m_ResCache->GetHandle(&resource);

	BaseRenderComponent* pRenderComponent = renderComponent;

	if(handle.get() == NULL)
	{
		CHG_ERROR("Not found mesh resource: " + meshFileName);
	}

	m_pMesh = ((COLLADAMeshResourceExtraData*)handle->GetExtra().get())->m_pMesh;

    m_pMaterialNode.reset(CHG_NEW MaterialNode(sMaterialResource, renderComponent));
	m_pMaterialNode->setActivePOM(renderProperties.ActivePOM);

	// Vertex Format Shader Description //////////////////////////////////////////

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

	////////////////////////////////////////////////////////////////////////////////

	if(shaderFileNames != NULL)
	{
		if(shaderFileNames->size() != 0)
		{
			m_vShaderNode = std::shared_ptr<ShaderNode>(CHG_NEW ShaderNode(*shaderFileNames, renderComponent));
			m_vShaderNode->setVertexFormatShaderEntry(vertexDesc, 5);
		}
		else //Default Shader
		{
			//Generacion automatica de shader por defecto segun el tipo de material usado
			CHG_WARNING("Not found default shader");
		}
	}
	else //default shaders
	{
		//Generacion automatica de shader por defecto segun el tipo de material usado
				CHG_ERROR("Not found default shader");
	}

	m_bHasGIShader = renderProperties.bGlobalIllumination;

	if (renderProperties.bGlobalIllumination)
	{
		if (sGIShaderResource != "")
		{
			m_pGIShaderNode = std::shared_ptr<ShaderNode>(CHG_NEW ShaderNode(sGIShaderResource, renderComponent));
			m_pGIShaderNode->setVertexFormatShaderEntry(vertexDesc, 3);
		}
		else
		{
			CHG_ERROR("Not Found GI Shader");
		}

		if (sGIMaterialResource != "")
		{
			m_pGIMaterialNode.reset(CHG_NEW MaterialNode(sGIMaterialResource, renderComponent));
			m_pGIMaterialNode->setActivePOM(false);
		}
		else
		{
			CHG_ERROR("Not Found GI Material");
		}
	}

	/*if(m_vShaderNodes[1]->hasTessellation())
	{
		for(UINT i=0; i < m_pMesh->m_vSubMeshes.size(); i++)
		{
			m_pMesh->m_vSubMeshes[i].m_PrimitiveType = PRIM_PATCHES;
		}
	}*/

	delete[] vertexDesc;

	m_bCastShadow = renderProperties.CastShadows;
	m_EntityID = entityId;
	m_IsVisible = true;
	m_pRenderComponent = renderComponent;
	m_Aabb = m_pMesh->m_MeshAabb;
	m_MeshFileName = meshFileName;
	m_bCastSDFShadows = false;
	m_bCastVolumetricShadows = false;

	m_InQueueFrustum = false;
	m_InQueueLight = false;
	m_InQueueRender = false;

	m_ToWorld = *world;
	m_FromWorld = m_ToWorld.inverse();

	m_bHasShadowShader = false;
	m_bShadowGeneratedAsTwoSided = false;

	m_bIsDrawable = true;
	m_bStatic = pRenderComponent->IsStatic();

	// Set SDF Shadow Attributes
	Vec4 SDFCoords, SDFDim;

	if (g_pApp->m_pSDFShadowManager->GetSDFCoords(m_pMesh->getName(), SDFCoords, SDFDim))
	{
		Vec3 vSDFPackDimensions = g_pApp->m_pSDFShadowManager->GetSDFPackDimensions();

		m_bCastSDFShadows = renderProperties.CastShadows;
		m_bCastVolumetricShadows = renderProperties.bCastVolumetricShadows;
		m_bShadowGeneratedAsTwoSided = renderProperties.bShadowGeneratedAsTwoSided;

		m_vLocalPositionExtent = Vec4((m_Aabb.vcMax + 0.2f) - m_Aabb.vcCenter, 1.0f);
		m_vSDFUVOffset = Vec4(0.5f*SDFDim.xyz() + SDFCoords.xyz(), 1.0f);
		m_vSDFUVScaleAndVolumeScale = Vec4((1.0f / (m_Aabb.vcMax - m_Aabb.vcMin + 0.4f)) * SDFDim.xyz(), vScale.x);
		m_mWorldToVolume = transpose(translate(-m_Aabb.vcCenter)*(!m_ToWorld));

		Vec3 vSphereBoundCenter = m_Aabb.vcCenter;
		m_vSphereBoundCenterAndRadius.w = length((m_Aabb.vcMax + 0.2f) - vSphereBoundCenter);

		if (pRenderComponent->IsStatic())
		{
			vSphereBoundCenter = (m_ToWorld * Vec4(vSphereBoundCenter, 1.0f)).xyz();
		}

		m_vSphereBoundCenterAndRadius = Vec4(vSphereBoundCenter, m_vSphereBoundCenterAndRadius.w);
	}	
}

MeshNode::~MeshNode()
{
}

//
// MeshNode::VOnRestore
//
bool MeshNode::VOnRestore(Scene *pScene)
{
	SceneNode::VOnRestore(pScene);

	//Load Mesh from file if it`s necessary
	Resource resource(m_MeshFileName);
	shared_ptr<ResHandle> handle = g_pApp->m_ResCache->GetHandle(&resource);

	if(handle.get() == NULL)
	{
		CHG_ERROR("Not found mesh resource: " + m_MeshFileName);
	}

	m_pMesh = ((COLLADAMeshResourceExtraData*)handle->GetExtra().get())->m_pMesh;

	//Generacion automatica de shader por defecto segun el tipo de material usado

	return true;
}

//
// MeshNode::VRender
//
bool MeshNode::VOnRender(Scene *pScene)
{
	if(pScene->m_RenderPass != 3)
	{
		m_vShaderNode->setActiveShader();
		
		m_pMaterialNode->setActiveMaterial();

		g_pApp->m_pRenderer->setShaderConstant2f("screenSize", g_pApp->GetScreenSize());
		g_pApp->m_pRenderer->setShaderConstant4f("vCameraPosition", Vec4(pScene->GetCamera()->ToWorld().GetPosition(), 1.0f));

		g_pApp->m_pRenderer->setShaderConstant4x4f("View", *((Mat4x4*)&pScene->GetCamera()->GetView()));
		g_pApp->m_pRenderer->setShaderConstant4x4f("Projection", *((Mat4x4*)&pScene->GetCamera()->GetProjection()));

		g_pApp->m_pRenderer->setSamplerState("BaseFilter", pScene->m_trilinear[m_pMaterialNode->GetWrapMode()]);

		//g_pApp->m_pRenderer->setSamplerState("VoxelFilter", pScene->m_linearClamp);
		g_pApp->m_pRenderer->setSamplerState("TargetSampler", pScene->m_linearWrap);

		if (!m_pMaterialNode->IsOpaque())
		{
			std::shared_ptr<LightManager> pLightManager = pScene->GetLightManager();
			
			pLightManager->SetLightBuffers(pScene);

			g_pApp->m_pRenderer->setShaderConstant1i("iActiveShadows", 0);
			g_pApp->m_pRenderer->setShaderConstant2f("vNum_Tiles", Vec2(pLightManager->GetNumWidthTiles(), pLightManager->GetNumHeightTiles()));
			g_pApp->m_pRenderer->setShaderConstant2f("vScreenDimensions", g_pApp->GetScreenSize());
		}

		if (m_pMaterialNode->GetBlendMode() == SUBSURFACE_MODE)
			g_pApp->m_pRenderer->setShaderConstant1i("iGPUIndex", m_uGPUIndex);
	}

    g_pApp->m_pRenderer->setShaderConstant4x4f("World", *((Mat4x4*)&transpose(m_ToWorld)));

	m_pMesh->Render();

	return true;
}

//
// MeshNode::OnRenderWithoutShader
//
bool MeshNode::VOnSimpleRender(Scene *pScene)
{
    g_pApp->m_pRenderer->setShaderConstant4x4f("World", *((Mat4x4*)&transpose(m_ToWorld)));

	m_pMesh->Render();

	return true;
}

//
// MeshNode::OnRenderWithExternalShader
//
bool MeshNode::OnRenderWithExternalShader(Scene *pScene)
{	
	m_pMaterialNode->setActiveMaterial();

	g_pApp->m_pRenderer->setShaderConstant2f("screenSize", g_pApp->GetScreenSize());

	g_pApp->m_pRenderer->setShaderConstant4x4f("View", *((Mat4x4*)&pScene->GetCamera()->GetView()));
	g_pApp->m_pRenderer->setShaderConstant4x4f("Projection", *((Mat4x4*)&pScene->GetCamera()->GetProjection()));

    g_pApp->m_pRenderer->setShaderConstant4x4f("World", *((Mat4x4*)&transpose(m_ToWorld)));

	m_pMesh->Render();

	return true;
}

Vec4 MeshNode::GetSphereBoundCenterAndRadius()
{
	Vec4 vSphereBoundCenter = Vec4(m_vSphereBoundCenterAndRadius.xyz(), 1.0f);

	if (!m_bStatic)
		vSphereBoundCenter = m_ToWorld * vSphereBoundCenter;		

	return Vec4(vSphereBoundCenter.xyz(), m_vSphereBoundCenterAndRadius.w);
}

Mat4x4 MeshNode::GetWorldToVolumeMatrix()
{
	if (!m_bStatic)
		m_mWorldToVolume = transpose(translate(-m_Aabb.vcCenter)*(!m_ToWorld));

	return m_mWorldToVolume; 
}