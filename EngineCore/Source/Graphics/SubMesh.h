#ifndef SUBMESH_H_
#define SUBMESH_H_

class Mesh;
class Material;
class VertexBuffer;
class Mat4x4;

class SubMesh
{
	friend class Mesh;

private:

	Material* m_pMaterial;

	VertexBuffer* m_pVertexBuffer;
	UINT m_NumVertex;
	UINT m_NumPrimitives;
	UINT m_VertexBase;
	UINT m_MinIndex;
	UINT m_IndexBase;

	Mat4x4 mLocalTransform;
	string m_Name;
	Mesh* m_pParent;

};

#endif