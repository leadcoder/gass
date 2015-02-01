//
// Copyright (c) 2009-2010 Mikko Mononen memon@inside.org
//
// This software is provided 'as-is', without any express or implied
// warranty.  In no event will the authors be held liable for any damages
// arising from the use of this software.
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
//

#ifndef MESHLOADER_OBJ
#define MESHLOADER_OBJ

class rcMeshLoaderObj
{
public:
	rcMeshLoaderObj();
	~rcMeshLoaderObj();
	
	bool load(const char* fileName);
	bool load(float* verts, float* normals, int* areas, int* tris, int vert_count,int tri_count)
	{
		m_verts  = verts;
		m_normals  = normals;
		m_tris = tris;
		m_vertCount  = vert_count;
		m_triCount = tri_count;
		m_areas = areas;
		return true;
	}

	inline const float* getVerts() const { return m_verts; }
	inline const float* getNormals() const { return m_normals; }
	inline const int* getTris() const { return m_tris; }
	inline const int* getAreas() const { return m_areas; }

	inline int getVertCount() const { return m_vertCount; }
	inline int getTriCount() const { return m_triCount; }
	inline const char* getFileName() const { return m_filename; }
private:
	
	void addVertex(float x, float y, float z, int& cap);
	void addTriangle(int a, int b, int c, int& cap);
	
	char m_filename[260];
	
	float* m_verts;
	int* m_tris;
	int* m_areas;
	float* m_normals;
	int m_vertCount;
	int m_triCount;
};

#endif // MESHLOADER_OBJ
