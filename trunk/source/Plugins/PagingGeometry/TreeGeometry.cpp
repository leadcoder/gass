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
#include "TreeGeometry.h"
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
#include "Core/MessageSystem/Message.h"


namespace GASS
{

	void TreeGeometry::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("TreeGeometry",new Creator<TreeGeometry, IComponent>);
	
		RegisterProperty<std::string>("ColorMap", &TreeGeometry::GetColorMap, &TreeGeometry::SetColorMap);
		RegisterProperty<std::string>("DensityMap", &TreeGeometry::GetDensityMap, &TreeGeometry::SetDensityMap);
		RegisterProperty<float>("DensityFactor", &TreeGeometry::GetDensityFactor, &TreeGeometry::SetDensityFactor);
		RegisterProperty<float>("MeshDistance", &TreeGeometry::GetMeshDistance, &TreeGeometry::SetMeshDistance);
		RegisterProperty<float>("MeshFadeDistance", &TreeGeometry::GetMeshFadeDistance, &TreeGeometry::SetMeshFadeDistance);
		RegisterProperty<float>("ImposterDistance", &TreeGeometry::GetImposterDistance, &TreeGeometry::SetImposterDistance);
		RegisterProperty<float>("ImposterFadeDistance", &TreeGeometry::GetImposterFadeDistance, &TreeGeometry::SetImposterFadeDistance);
		RegisterProperty<Vec4>("Bounds", &TreeGeometry::GetBounds, &TreeGeometry::SetBounds);
		RegisterProperty<Vec2>("MaxMinScale", &TreeGeometry::GetMaxMinScale, &TreeGeometry::SetMaxMinScale);
		RegisterProperty<bool>("CastShadows", &TreeGeometry::GetCastShadows, &TreeGeometry::SetCastShadows);
		RegisterProperty<bool>("CreateShadowMap", &TreeGeometry::GetCreateShadowMap, &TreeGeometry::SetCreateShadowMap);
		RegisterProperty<bool>("SetHeightAtStartup", &TreeGeometry::GetPrecalcHeight, &TreeGeometry::SetPrecalcHeight);
		RegisterProperty<float>("PageSize", &GetPageSize, &SetPageSize);
		RegisterProperty<float>("ImposterAlphaRejectionValue", &GetImposterAlphaRejectionValue, &SetImposterAlphaRejectionValue);
		
		//REGISTER_PROP(String,TreeGeometry,m_DensityMapFilename,"DensityMap",CProperty::STREAM|CProperty::READONLY,"");
		/*REGISTER_PROP(Float,TreeGeometry,m_DensityFactor,"DensityFactor",CProperty::STREAM|CProperty::READONLY,"");
		REGISTER_PROP(Float,TreeGeometry,m_MeshDist,"MeshDistance",CProperty::STREAM|CProperty::READONLY,"");
		REGISTER_PROP(Float,TreeGeometry,m_MeshFadeDist,"MeshFadeDistance",CProperty::STREAM|CProperty::READONLY,"");
		REGISTER_PROP(Float,TreeGeometry,m_ImposterDist,"ImposterDistance",CProperty::STREAM|CProperty::READONLY,"");
		REGISTER_PROP(Float,TreeGeometry,m_ImposterFadeDist,"ImposterFadeDistance",CProperty::STREAM|CProperty::READONLY,"");
		REGISTER_PROP(Vect4D,TreeGeometry,m_Bounds,"Bounds",CProperty::STREAM|CProperty::READONLY,"");
		REGISTER_PROP(Vect2D,TreeGeometry,m_MaxMinScale,"MaxMinScale",CProperty::STREAM|CProperty::READONLY,"");
		REGISTER_PROP(Bool,TreeGeometry,m_CastShadows,"CastShadows",CProperty::STREAM|CProperty::READONLY,"");
		REGISTER_PROP(Bool,TreeGeometry,m_CreateShadowMap,"CreateShadowMap",CProperty::STREAM|CProperty::READONLY,"");
		REGISTER_PROP(Bool,TreeGeometry,m_PrecalcHeight,"SetHeightAtStartup",CProperty::STREAM|CProperty::READONLY,"");*/
	}

	
	


	TreeGeometry::TreeGeometry(void)
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

	TreeGeometry::~TreeGeometry(void)
	{
	}


	void TreeGeometry::OnLoad(MessagePtr message)
	{
		Ogre::SceneManager* sm = Ogre::Root::getSingleton().getSceneManagerIterator().getNext();
		//OgreGraphicsSceneManager* ogsm = boost::any_cast<OgreGraphicsSceneManager*>(message->GetData("GraphicsSceneManager"));
		//assert(ogsm);
		//Ogre::SceneManager* sm = ogsm->GetSceneManger();
	
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
			treeLoader2d->setHeightFunction(TreeGeometry::GetTerrainHeight);
			if(m_ColorMapFilename!= "") treeLoader2d->setColorMap(m_ColorMapFilename);
			m_PagedGeometry->setPageLoader(treeLoader2d);
		}
		float volume = m_MapBounds.width() * m_MapBounds.height();
		unsigned int treeCount = m_DensityFactor * volume;
		if(m_DensityMapFilename != "")
			LoadDensityMap(m_DensityMapFilename,CHANNEL_COLOR);

		ITerrainComponent * terrain = NULL;//FindTerrain();
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
						float y = terrain->GetHeight(x,z);//HiFi::Root::Get().GetLevel()->GetTerrainHeight(x,z);
						treeLoader3d->addTree(myTree,  Ogre::Vector3(x, y,z) ,Ogre::Degree(yaw), scale);
					}
					else treeLoader2d->addTree(myTree,  Ogre::Vector2(x, z) ,Ogre::Degree(yaw), scale);
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
		}
	}

	void TreeGeometry::OnCreate()
	{
		GetSceneObject()->RegisterForMessage(SceneObject::OBJECT_MESSAGE_LOAD_GFX_COMPONENTS,  MESSAGE_FUNC(TreeGeometry::OnLoad),1);
	}

	void TreeGeometry::LoadDensityMap(const std::string &mapFile, int channel)
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

	float TreeGeometry::GetDensityAt(float x, float z)
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

	float TreeGeometry::GetTerrainHeight(float x, float z)
	{
		return 0;
	}

}


