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
#include "DensityMapComponent.h"
#include "PagedGeometry.h"
#include "ImpostorPage.h"
#include "BatchPage.h"
#include "TreeLoader2D.h"
#include "TreeLoader3D.h"
#include "GrassLoader.h"
#include "Sim/Components/Graphics/Geometry/ITerrainComponent.h"
#include "Sim/Scenario/Scene/SceneObject.h"
#include "Sim/Scenario/Scene/SceneObjectManager.h"



//#include "Plugins/Ogre/OgreGraphicsSceneManager.h"
#include "Core/ComponentSystem/ComponentFactory.h"
#include "Core/ComponentSystem/IComponent.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/IMessage.h"


namespace GASS
{
	void DensityMapComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("DensityMapComponent",new Creator<DensityMapComponent, IComponent>);
		RegisterProperty<std::string>("DensityMap", &DensityMapComponent::GetDensityMap, &DensityMapComponent::SetDensityMap);
	}

	void DensityMapComponent::OnCreate()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(DensityMapComponent::OnLoad,LoadGFXComponentsMessage,1));
		GetSceneObject()->RegisterForMessage(REG_TMESS(DensityMapComponent::OnUnload,UnloadComponentsMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(DensityMapComponent::OnPaint,GrassPaintMessage,0));
		
	}


	DensityMapComponent::DensityMapComponent(void) : m_DensityMap(NULL)
	{
		
	}

	DensityMapComponent::~DensityMapComponent(void)
	{
	
	}

	void DensityMapComponent::OnUnload(UnloadComponentsMessagePtr message)
	{
		
	}

	void DensityMapComponent::OnLoad(LoadGFXComponentsMessagePtr message)
	{
		if(m_DensityMapFilename != "")
			LoadDensityMap(m_DensityMapFilename,CHANNEL_COLOR);
	}


	void DensityMapComponent::OnPaint(GrassPaintMessagePtr message)
	{
		const Vec3 world_pos = message->GetPosition();
		if(m_DensityMap)
		{
			Ogre::uchar *data = static_cast<Ogre::uchar*>(m_DensityMap->data);
			int wsize = m_DensityMap->getWidth()-1;

			const Ogre::Real height = m_MapBounds.height();
			const Ogre::Real width = m_MapBounds.width();
			const Ogre::Real x_pos = (world_pos.x - m_MapBounds.left)/width;
			const Ogre::Real y_pos = (world_pos.z - m_MapBounds.top)/height;

			const Ogre::Real brush_size_texture_space_x = message->GetBrushSize()/width;
			const Ogre::Real brush_size_texture_space_y = message->GetBrushSize()/height;
			const Ogre::Real brush_inner_radius = (message->GetBrushInnerSize()*0.5)/height;

			long startx = (x_pos - brush_size_texture_space_x) * (wsize);
			long starty = (y_pos - brush_size_texture_space_y) * (wsize);
			long endx = (x_pos + brush_size_texture_space_x) * (wsize);
			long endy= (y_pos + brush_size_texture_space_y) * (wsize);
			startx = std::max(startx, 0L);
			starty = std::max(starty, 0L);
			endx = std::min(endx, (long)wsize-1);
			endy = std::min(endy, (long)wsize-1);
			for (long y = starty; y <= endy; ++y)
			{
				int tmploc = y * (wsize+1);
				for (long x = startx; x <= endx; ++x)
				{
					Ogre::Real tsXdist = (x / (float)wsize) - x_pos;
					Ogre::Real tsYdist = (y / (float)wsize) - y_pos;

					Ogre::Real dist = Ogre::Math::Sqrt(tsYdist * tsYdist + tsXdist * tsXdist);

					Ogre::Real weight = std::min((Ogre::Real)1.0,((dist - brush_inner_radius )/ Ogre::Real(0.5 * brush_size_texture_space_x - brush_inner_radius)));
					if( weight < 0) weight = 0;
					weight = 1.0 - (weight * weight);
					//weight = 1;

					float val = float(data[tmploc + x])/255.0f;
					val += weight*message->GetIntensity()*3;
					//val = std::min(val, 255.0f);
					//val = std::max(val, 0.0f);
					if(val > 1.0)
						val = 1;
					if(val < 0.0)
						val = 0;
					data[tmploc + x] = val*255;
				}
			}
		}
	}

	void DensityMapComponent::LoadDensityMap(const std::string &mapFile, int channel)
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

	float DensityMapComponent::GetDensityAt(float x, float z)
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
}


