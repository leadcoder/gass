/****************************************************************************
* This file is part of GASS.                                                *
* See http://sourceforge.net/projects/gass/                                 *
*                                                                           *
* Copyright (c) 2008-2009 GASS team. See Contributors.txt for details.      *
*                                                                           *
* GASS is free software: you can redistribute it and/or modify              *
* it under the terms of the GNU Lesser General Public License as published  *
* by the Free Software Foundation, either version 3 of the License, or      *
* (at your option) any later version.                                       *
*                                                                           *
* GASS is distributed in the hope that it will be useful,                   *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU Lesser General Public License for more details.                       *
*                                                                           *
* You should have received a copy of the GNU Lesser General Public License  *
* along with GASS. If not, see <http://www.gnu.org/licenses/>.              *
*****************************************************************************/

#include <boost/bind.hpp>
#include <OgreEntity.h>
#include <Ogre.h>
#include <OgreSceneManager.h>
#include <OgreHardwarePixelBuffer.h>
#include "TreeGeometryComponent.h"
#include "PagedGeometry.h"
#include "ImpostorPage.h"
#include "BatchPage.h"
#include "TreeLoader2D.h"
#include "TreeLoader3D.h"
#include "GrassLoader.h"
#include "Sim/Components/Graphics/Geometry/ITerrainComponent.h"
#include "Sim/Scenario/Scene/SceneObject.h"


//#include "Plugins/Ogre/OgreGraphicsSceneManager.h"
#include "Core/ComponentSystem/ComponentFactory.h"
#include "Core/ComponentSystem/IComponent.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/IMessage.h"


namespace GASS
{
	ITerrainComponent* TreeGeometryComponent::m_Terrain = NULL;

	void TreeGeometryComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("TreeGeometryComponent",new Creator<TreeGeometryComponent, IComponent>);
		
		RegisterProperty<std::string>("Mesh", &TreeGeometryComponent::GetMesh, &TreeGeometryComponent::SetMesh);
		RegisterProperty<std::string>("ColorMap", &TreeGeometryComponent::GetColorMap, &TreeGeometryComponent::SetColorMap);
		RegisterProperty<std::string>("DensityMap", &TreeGeometryComponent::GetDensityMap, &TreeGeometryComponent::SetDensityMap);
		RegisterProperty<float>("DensityFactor", &TreeGeometryComponent::GetDensityFactor, &TreeGeometryComponent::SetDensityFactor);
		RegisterProperty<float>("MeshDistance", &TreeGeometryComponent::GetMeshDistance, &TreeGeometryComponent::SetMeshDistance);
		RegisterProperty<float>("MeshFadeDistance", &TreeGeometryComponent::GetMeshFadeDistance, &TreeGeometryComponent::SetMeshFadeDistance);
		RegisterProperty<float>("ImposterDistance", &TreeGeometryComponent::GetImposterDistance, &TreeGeometryComponent::SetImposterDistance);
		RegisterProperty<float>("ImposterFadeDistance", &TreeGeometryComponent::GetImposterFadeDistance, &TreeGeometryComponent::SetImposterFadeDistance);
		RegisterProperty<Vec4>("Bounds", &TreeGeometryComponent::GetBounds, &TreeGeometryComponent::SetBounds);
		RegisterProperty<Vec2>("MaxMinScale", &TreeGeometryComponent::GetMaxMinScale, &TreeGeometryComponent::SetMaxMinScale);
		RegisterProperty<bool>("CastShadows", &TreeGeometryComponent::GetCastShadows, &TreeGeometryComponent::SetCastShadows);
		RegisterProperty<bool>("CreateShadowMap", &TreeGeometryComponent::GetCreateShadowMap, &TreeGeometryComponent::SetCreateShadowMap);
		RegisterProperty<bool>("SetHeightAtStartup", &TreeGeometryComponent::GetPrecalcHeight, &TreeGeometryComponent::SetPrecalcHeight);
		RegisterProperty<float>("PageSize", &GetPageSize, &SetPageSize);
		RegisterProperty<float>("ImposterAlphaRejectionValue", &GetImposterAlphaRejectionValue, &SetImposterAlphaRejectionValue);
	}


	TreeGeometryComponent::TreeGeometryComponent(void) : m_Bounds(0,0,0,0)
	{
		m_DensityFactor = 0.001;
		m_MaxMinScale.x = 1.1;
		m_MaxMinScale.y = 0.9;
		m_CastShadows = true;
		m_MeshDist = 100;
		m_ImposterDist = 500;
		m_PrecalcHeight = true;
		m_PageSize = 80;
		m_MeshFadeDist = 0;
		m_ImposterFadeDist = 0;
		m_ImposterAlphaRejectionValue = 50;
		m_CreateShadowMap = false;

	}

	TreeGeometryComponent::~TreeGeometryComponent(void)
	{
	}


	void TreeGeometryComponent::OnUnload(UnloadComponentsMessagePtr message)
	{
		if(m_PagedGeometry)
		{
			if(m_PagedGeometry->getPageLoader())
				delete m_PagedGeometry->getPageLoader();
			delete m_PagedGeometry;
		}

		Ogre::SceneManager* sm = Ogre::Root::getSingleton().getSceneManagerIterator().getNext();
		Ogre::Camera* ocam = sm->getCameraIterator().getNext();
		ocam->getViewport()->getTarget()->removeListener(this);
	}

	void TreeGeometryComponent::OnLoad(LoadGFXComponentsMessagePtr message)
	{
		
		//OgreGraphicsSceneManager* ogsm = boost::any_cast<OgreGraphicsSceneManager*>(message->GetData("GraphicsSceneManager"));
		//assert(ogsm);
		//Ogre::SceneManager* sm = ogsm->GetSceneManger();
		Ogre::SceneManager* sm = Ogre::Root::getSingleton().getSceneManagerIterator().getNext();
		Ogre::Camera* ocam = sm->getCameraIterator().getNext();
		ocam->getViewport()->getTarget()->addListener(this);

		bool user_bounds = true;
		if(m_Bounds.x == 0 && m_Bounds.y == 0 && m_Bounds.z == 0 && m_Bounds.w == 0)
		{
			user_bounds = false;
		}

		if(!user_bounds)
		{
			TerrainComponentPtr terrain = GetSceneObject()->GetFirstComponent<ITerrainComponent>();
			if(terrain)
			{
				Vec3 bmin,bmax;
				terrain->GetBounds(bmin,bmax);

				m_Bounds.x = bmin.x;
				m_Bounds.y = bmin.z;

				m_Bounds.z = bmax.x;
				m_Bounds.w = bmax.z;
				//for speed we save the raw pointer , we will access this for each height callback 
				m_Terrain = terrain.get();
			}
			else
			{
				m_Bounds.Set(0,0,2000,2000);
			}
			m_MapBounds = TBounds(m_Bounds.x, m_Bounds.y, m_Bounds.z, m_Bounds.w);
		}
		else m_MapBounds = TBounds(m_Bounds.x, m_Bounds.y, m_Bounds.z, m_Bounds.w);


		m_PagedGeometry = new PagedGeometry(ocam, m_PageSize);
	
		ImpostorPage::setImpostorColor(Ogre::ColourValue(0.5,0.5,0.5,1));
		assert(m_PagedGeometry);
		m_PagedGeometry->setImposterAlphaRejectValue(m_ImposterAlphaRejectionValue);
		if(m_MeshDist > 0)
			m_PagedGeometry->addDetailLevel<BatchPage>(m_MeshDist,m_MeshFadeDist);
		if(m_ImposterDist >  0)
		{
			m_PagedGeometry->addDetailLevel<ImpostorPage>(m_ImposterDist,m_ImposterFadeDist);

		}
		//Set up a TreeLoader for easy use
		TreeLoader2D *treeLoader2d = NULL;
		TreeLoader3D *treeLoader3d = NULL;

		if(m_PrecalcHeight)
		{
			treeLoader3d = new TreeLoader3D(m_PagedGeometry, m_MapBounds);
			if(m_ColorMapFilename!= "") treeLoader2d->setColorMap(m_ColorMapFilename);
			m_PagedGeometry->setPageLoader(treeLoader3d);
		}
		else
		{
			treeLoader2d = new TreeLoader2D(m_PagedGeometry, m_MapBounds);
			treeLoader2d->setHeightFunction(TreeGeometryComponent::GetTerrainHeight);
			if(m_ColorMapFilename!= "") treeLoader2d->setColorMap(m_ColorMapFilename);
			m_PagedGeometry->setPageLoader(treeLoader2d);
		}
		float volume = m_MapBounds.width() * m_MapBounds.height();
		unsigned int treeCount = m_DensityFactor * volume;
		if(m_DensityMapFilename != "")
			LoadDensityMap(m_DensityMapFilename,CHANNEL_COLOR);

		//TerrainComponentPtr terrain = GetSceneObject()->GetFirstComponent<ITerrainComponent>();
		//m_Terrain = terrain.get();
		/*Image shadowMap;
		int shadow_size = 4096;
		if(m_CreateShadowMap)
		{
			shadowMap.Allocate(shadow_size, shadow_size, 24);
		}*/
		Ogre::Entity *myTree = sm->createEntity(m_Name, m_MeshFileName);
		myTree->setCastShadows(m_CastShadows);
		if (m_DensityMap != NULL)
		{
			for (int i = 0; i < treeCount; i++)
			{

				//Determine whether this grass will be added based on the local density.
				//For example, if localDensity is .32, grasses will be added 32% of the time.
				float x, z, yaw, scale;
				x = Ogre::Math::RangeRandom(m_MapBounds.left, m_MapBounds.right);
				z = Ogre::Math::RangeRandom(m_MapBounds.top, m_MapBounds.bottom);
				if (Ogre::Math::UnitRandom() <= GetDensityAt(x, z))
				{
					yaw = Ogre::Math::RangeRandom(0, 360);
					scale = Ogre::Math::RangeRandom(m_MaxMinScale.x, m_MaxMinScale.y);
					if(m_PrecalcHeight)
					{
						float y = m_Terrain->GetHeight(x,z);//HiFi::Root::Get().GetLevel()->GetTerrainHeight(x,z);
						treeLoader3d->addTree(myTree,  Ogre::Vector3(x, y,z) ,Ogre::Degree(yaw), scale);
					}
					else 
						treeLoader2d->addTree(myTree,  Ogre::Vector2(x, z) ,Ogre::Degree(yaw), scale);
					//treeLoader->addTree(myTree, x, z,yaw, scale);
				/*	if(m_CreateShadowMap)
					{
						float u = (x -  m_MapBounds.left) / fabs(m_MapBounds.left - m_MapBounds.right);
						float v = (z - m_MapBounds.top) / fabs(m_MapBounds.top - m_MapBounds.bottom);
						float pixel_x = u*shadow_size;
						float pixel_y = v*shadow_size;
						int c_pix_x = ((int)pixel_x);
						int c_pix_y = ((int)pixel_y);
						float delta_x = pixel_x - c_pix_x;
						float delta_y = pixel_y - c_pix_y;


						if(delta_x > 0.5)
						{
							c_pix_x++;
						}
						if(delta_y > 0.5)
						{
							c_pix_y++;
						}
						if(c_pix_x > 1 && c_pix_x < shadow_size-1 && c_pix_y > 1 && c_pix_y < shadow_size-1)
						{
						    Vec3 tmpVec = Vec3(1,1,1);
							shadowMap.SetPixel(c_pix_x,c_pix_y, tmpVec);
							tmpVec.Set(0.5,0.5,0.5);
							shadowMap.SetPixel(c_pix_x+1,c_pix_y, tmpVec);
							shadowMap.SetPixel(c_pix_x,c_pix_y+1, tmpVec);
							shadowMap.SetPixel(c_pix_x-1,c_pix_y, tmpVec);
							shadowMap.SetPixel(c_pix_x,c_pix_y-1, tmpVec);
						}
					}*/
				}
			}

			/*if(m_CreateShadowMap)
			{
				shadowMap.SaveTGA("ShadowMap" + m_FileName + ".tga");
			}*/
			delete[] m_DensityMap->data;
			delete m_DensityMap;
			m_DensityMap = NULL;
			//make update to create imposters
			m_PagedGeometry->update();
		}
	}


/*	void TreeGeometryComponent::CreateMeshData(MeshDataPtr mesh_data, Ogre::MeshPtr mesh)
	{
		mesh_data->NumVertex = 0;
		mesh_data->VertexVector = NULL;
		mesh_data->NumFaces = 0;
		mesh_data->FaceVector = NULL;

		if(mesh->sharedVertexData)
		{
			AddVertexData(mesh->sharedVertexData,mesh_data);
		}

		for(unsigned int i = 0;i < mesh->getNumSubMeshes();++i)
		{
			SubMesh *sub_mesh = mesh->getSubMesh(i);

			if (!sub_mesh->useSharedVertices)
			{
				AddIndexData(sub_mesh->indexData,mesh_data->NumVertex,mesh_data);
				AddVertexData(sub_mesh->vertexData,mesh_data);
			}
			else
			{
				AddIndexData(sub_mesh->indexData,0,mesh_data);
			}
		}
		mesh_data->NumFaces = mesh_data->NumFaces/3.0;
	}

	void TreeGeometryComponent::AddVertexData(const Ogre::VertexData *vertex_data,MeshDataPtr mesh)
	{
		if (!vertex_data)
			return;

		const VertexData *data = vertex_data;

		const unsigned int prev_size = mesh->NumVertex;
		mesh->NumVertex += (unsigned int)data->vertexCount;

		Vec3* tmp_vert = new Vec3[mesh->NumVertex];
		if (mesh->VertexVector)
		{
			memcpy(tmp_vert,mesh->VertexVector,sizeof(Vec3) * prev_size);
			delete[] mesh->VertexVector;
		}
		mesh->VertexVector = tmp_vert;

		// Get the positional buffer element
		{
			const Ogre::VertexElement* posElem = data->vertexDeclaration->findElementBySemantic(Ogre::VES_POSITION);
			Ogre::HardwareVertexBufferSharedPtr vbuf = data->vertexBufferBinding->getBuffer(posElem->getSource());
			const unsigned int vSize = (unsigned int)vbuf->getVertexSize();

			unsigned char* vertex = static_cast<unsigned char*>(vbuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
			float* pReal;
			Vec3 * curVertices = &mesh->VertexVector[prev_size];
			const unsigned int vertexCount = (unsigned int)data->vertexCount;
			for(unsigned int j = 0; j < vertexCount; ++j)
			{
				posElem->baseVertexPointerToElement(vertex, &pReal);
				vertex += vSize;

				curVertices->x = (*pReal++);
				curVertices->y = (*pReal++);
				curVertices->z = (*pReal++);

				//*curVertices = _transform * (*curVertices);

				curVertices++;
			}
			vbuf->unlock();
		}
	}

	void TreeGeometryComponent::AddIndexData(Ogre::IndexData *data, const unsigned int offset,MeshDataPtr mesh)
	{
		const unsigned int prev_size = mesh->NumFaces;
		mesh->NumFaces += (unsigned int)data->indexCount;

		unsigned int* tmp_ind = new unsigned int[mesh->NumFaces];
		if (mesh->FaceVector)
		{
			memcpy (tmp_ind, mesh->FaceVector, sizeof(unsigned int) * prev_size);
			delete[] mesh->FaceVector;
		}
		mesh->FaceVector = tmp_ind;

		const unsigned int numTris = (unsigned int) data->indexCount / 3;
		HardwareIndexBufferSharedPtr ibuf = data->indexBuffer;
		const bool use32bitindexes = (ibuf->getType() == Ogre::HardwareIndexBuffer::IT_32BIT);
		unsigned int index_offset = prev_size;

		if (use32bitindexes)
		{
			const unsigned int* pInt = static_cast<unsigned int*>(ibuf->lock(HardwareBuffer::HBL_READ_ONLY));
			for(unsigned int k = 0; k < numTris; ++k)
			{
				mesh->FaceVector[index_offset ++] = offset + *pInt++;
				mesh->FaceVector[index_offset ++] = offset + *pInt++;
				mesh->FaceVector[index_offset ++] = offset + *pInt++;
			}
			ibuf->unlock();
		}
		else
		{
			const unsigned short* pShort = static_cast<unsigned short*>(ibuf->lock(HardwareBuffer::HBL_READ_ONLY));
			for(unsigned int k = 0; k < numTris; ++k)
			{
				mesh->FaceVector[index_offset ++] = offset + static_cast<unsigned int> (*pShort++);
				mesh->FaceVector[index_offset ++] = offset + static_cast<unsigned int> (*pShort++);
				mesh->FaceVector[index_offset ++] = offset + static_cast<unsigned int> (*pShort++);
			}
			ibuf->unlock();
		}
	}*/

	void TreeGeometryComponent::OnCreate()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(TreeGeometryComponent::OnLoad,LoadGFXComponentsMessage,1));
		GetSceneObject()->RegisterForMessage(REG_TMESS(TreeGeometryComponent::OnUnload,UnloadComponentsMessage,0));
	}

	void TreeGeometryComponent::LoadDensityMap(const std::string &mapFile, int channel)
	{
		//Load image
		Ogre::TexturePtr map = Ogre::TextureManager::getSingleton().load(mapFile, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
		//Copy image to pixelbox
		if (!map.isNull())
		{
			//Get the texture buffer
			Ogre::HardwarePixelBufferSharedPtr buff = map->getBuffer();

			//Prepare a PixelBox (8-bit greyscale) to receive the density values
			m_DensityMap = new Ogre::PixelBox(Ogre::Box(0, 0, buff->getWidth(), buff->getHeight()), Ogre::PF_BYTE_L);
			m_DensityMap->data = new Ogre::uint8[m_DensityMap->getConsecutiveSize()];

			if (channel == CHANNEL_COLOR)
			{
				//Copy to the greyscale density map directly if no channel extraction is necessary
				buff->blitToMemory(*m_DensityMap);
			}
			else
			{
				//If channel extraction is necessary, first convert to a PF_R8G8B8A8 format PixelBox
				//This is necessary for the code below to properly extract the desired channel
				Ogre::PixelBox pixels(Ogre::Box(0, 0, buff->getWidth(), buff->getHeight()), Ogre::PF_R8G8B8A8);
				pixels.data = new Ogre::uint8[pixels.getConsecutiveSize()];
				buff->blitToMemory(pixels);

				//Pick out a channel from the pixel buffer
				size_t channelOffset;
				switch (channel){
				case CHANNEL_RED: channelOffset = 3; break;
				case CHANNEL_GREEN: channelOffset = 2; break;
				case CHANNEL_BLUE: channelOffset = 1; break;
				case CHANNEL_ALPHA: channelOffset = 0; break;
				default: OGRE_EXCEPT(0, "Invalid channel", "GrassLayer::setDensityMap()"); break;
				}

				//And copy that channel into the density map
				Ogre::uint8 *inputPtr = (Ogre::uint8*)pixels.data + channelOffset;
				Ogre::uint8 *outputPtr = (Ogre::uint8*)m_DensityMap->data;
				Ogre::uint8 *outputEndPtr = outputPtr + m_DensityMap->getConsecutiveSize();
				while (outputPtr != outputEndPtr){
					*outputPtr++ = *inputPtr;
					inputPtr += 4;
				}

				//Finally, delete the temporary PF_R8G8B8A8 pixel buffer
				delete[] pixels.data;
			}
		}
	}

	void TreeGeometryComponent::preViewportUpdate(const Ogre::RenderTargetViewportEvent& evt)
	{
		Ogre::Viewport *vp = evt.source;
		m_PagedGeometry->update();
		if(vp) 
			m_PagedGeometry->setCamera(vp->getCamera());
	}


	float TreeGeometryComponent::GetDensityAt(float x, float z)
	{
		assert(m_DensityMap);

		unsigned int mapWidth = (unsigned int)m_DensityMap->getWidth();
		unsigned int mapHeight = (unsigned int)m_DensityMap->getHeight();
		float boundsWidth = m_MapBounds.width();
		float boundsHeight = m_MapBounds.height();

		unsigned int xindex = mapWidth * (x - m_MapBounds.left) / boundsWidth;
		unsigned int zindex = mapHeight * (z - m_MapBounds.top) / boundsHeight;
		if (xindex < 0 || zindex < 0 || xindex >= mapWidth || zindex >= mapHeight)
			return 0.0f;

		Ogre::uint8 *data = (Ogre::uint8*)m_DensityMap->data;
		float val = data[mapWidth * zindex + xindex] / 255.0f;
		return val;
	}

	float TreeGeometryComponent::GetTerrainHeight(float x, float z)
	{
		if(m_Terrain)
			return m_Terrain->GetHeight(x,z);
		else 
			return 0;
	}

}


