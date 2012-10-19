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

/* FIXME Fading doesn't work with multiple grass layers */

#include <boost/bind.hpp>
#include <OgreEntity.h>
#include <OgreSceneManager.h>
#include "GrassLayerComponent.h"
#include "GrassLoaderComponent.h"
#include "PagedGeometry.h"
#include "ImpostorPage.h"
#include "BatchPage.h"
#include "GrassLoader.h"
#include "Sim/Interface/GASSITerrainComponent.h"
#include "Sim/GASSSceneObject.h"

#include "Sim/GASSSimEngine.h"
#include "Sim/GASSSimSystemManager.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/ComponentSystem/GASSIComponent.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/Utils/GASSLogManager.h"

namespace GASS
{
	ITerrainComponent* GrassLayerComponent::m_Terrain = NULL;
	ICollisionSystem * GrassLayerComponent::m_CollisionSystem = NULL;

	GrassLayerComponent::GrassLayerComponent(void) : m_DensityFactor(0.001),
		//m_Bounds(0,0,0,0),
		//m_PageSize(50),
		//m_PagedGeometry(NULL),
		m_GrassLoader (NULL),
		m_MaxSize(1,1),
		m_MinSize(1,1),
		m_EnableSway(false),
		m_SwaySpeed(0),
		m_SwayLength(0),
		m_SwayDistribution(10),
		//m_ViewDist(50),
		m_RenderTechnique("Quad"),
		m_Blend(false),
		m_GrassLayer(NULL)
		//m_LOD0(0)
	{

	}

	GrassLayerComponent::~GrassLayerComponent(void)
	{

	}

	void GrassLayerComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("GrassLayerComponent",new Creator<GrassLayerComponent, IComponent>);
		RegisterProperty<float>("DensityFactor", &GrassLayerComponent::GetDensityFactor, &GrassLayerComponent::SetDensityFactor);
		RegisterProperty<std::string>("Material", &GrassLayerComponent::GetMaterial, &GrassLayerComponent::SetMaterial);
		RegisterProperty<std::string>("FadeTech", &GrassLayerComponent::GetFadeTech, &GrassLayerComponent::SetFadeTech);
		RegisterProperty<std::string>("RenderTechnique", &GrassLayerComponent::GetRenderTechnique, &GrassLayerComponent::SetRenderTechnique);
		RegisterProperty<bool>("BlendWithGround", &GrassLayerComponent::GetBlendWithGround, &GrassLayerComponent::SetBlendWithGround);
		RegisterProperty<Vec2>("MaxSize", &GrassLayerComponent::GetMaxSize, &GrassLayerComponent::SetMaxSize);
		RegisterProperty<Vec2>("MinSize", &GrassLayerComponent::GetMinSize, &GrassLayerComponent::SetMinSize);
		RegisterProperty<float>("SwaySpeed", &GrassLayerComponent::GetSwaySpeed, &GrassLayerComponent::SetSwaySpeed);
		RegisterProperty<float>("SwayLength", &GrassLayerComponent::GetSwayLength, &GrassLayerComponent::SetSwayLength);
		RegisterProperty<bool>("EnableSway", &GrassLayerComponent::GetEnableSway, &GrassLayerComponent::SetEnableSway);
		RegisterProperty<float>("SwayDistribution", &GrassLayerComponent::GetSwayDistribution, &GrassLayerComponent::SetSwayDistribution);
	}

	void GrassLayerComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(GrassLayerComponent::OnLoad,GrassLoaderComponentLoaded,0));
		//wait for grass loader
	}

	void GrassLayerComponent::OnDelete()
	{

	}

	void GrassLayerComponent::OnLoad(GrassLoaderComponentLoadedPtr message)
	{
		Ogre::SceneManager* sm = Ogre::Root::getSingleton().getSceneManagerIterator().getNext();
		Ogre::Camera* ocam = sm->getCameraIterator().getNext();
		
		GrassLoaderComponentPtr gl_component = GetSceneObject()->GetFirstComponentByClass<GrassLoaderComponent>(true);
		m_GrassLoader = gl_component->GetGrassLoader();
		
		m_GrassLayer = m_GrassLoader->addLayer(m_Material);
		m_GrassLayer->setMaximumSize(m_MaxSize.x,m_MaxSize.y);
		m_GrassLayer->setMinimumSize(m_MinSize.x,m_MinSize.y);
		m_GrassLayer->setDensity(m_DensityFactor);
		m_GrassLayer->setMapBounds(gl_component->GetMapBounds());
		if(m_DensityMapFilename != "")
			m_GrassLayer->setDensityMap(m_DensityMapFilename);
		m_GrassLayer->setDensityMap(gl_component->GetDensityTexture(),CHANNEL_BLUE);
		
		if(gl_component->GetColorMap() != "")
			m_GrassLayer->setColorMap(gl_component->GetColorMap());

		SetFadeTech(m_FadeTech);
		SetRenderTechnique(m_RenderTechnique );
		if(m_GrassLayer)
		{
			UpdateSway();
			m_GrassLayer->setLightingEnabled(false); //do lit in fp
		}
	}

	void GrassLayerComponent::SetDensityFactor(float factor)
	{
		m_DensityFactor = factor;
		if(m_GrassLayer)
			m_GrassLayer->setDensity(m_DensityFactor);
	}

	float GrassLayerComponent::GetDensityFactor() const
	{
		return m_DensityFactor;
	}

	
	void GrassLayerComponent::SetColorMap(const std::string &name)
	{
		if(m_GrassLayer)
			m_GrassLayer->setColorMap(m_ColorMapFilename);
	}

	std::string GrassLayerComponent::GetMaterial() const
	{
		return m_Material;
	}

	void GrassLayerComponent::SetMaterial(const std::string &name)
	{
		m_Material = name;
		if(m_GrassLayer)
		{
			m_GrassLayer->setMaterialName(name);
			GrassLoaderComponentPtr gl_component = GetSceneObject()->GetFirstComponentByClass<GrassLoaderComponent>(true);
			{
				gl_component->ReloadGeometry();
			}
		}
	}

	std::string GrassLayerComponent::GetFadeTech() const
	{
		return m_FadeTech;
	}

	void GrassLayerComponent::SetFadeTech(const std::string &tech)
	{
		m_FadeTech = tech;
		if(m_GrassLayer)
		{
			
			if(m_FadeTech == "AlphaGrow")
			{
				m_GrassLayer->setFadeTechnique(FADETECH_ALPHAGROW);
			}
			else if(m_FadeTech == "Alpha")
			{
				m_GrassLayer->setFadeTechnique(FADETECH_ALPHA);
			}
			else if(m_FadeTech == "Grow")
			{
				m_GrassLayer->setFadeTechnique(FADETECH_GROW);
			}
		}
	}

	std::string GrassLayerComponent::GetRenderTechnique() const
	{
		return m_RenderTechnique;
	}

	void GrassLayerComponent::SetRenderTechnique(const std::string &tech)
	{
		m_RenderTechnique = tech;
		if(m_GrassLayer)
		{
			if(m_RenderTechnique == "Quad")
			{
				m_GrassLayer->setRenderTechnique(GRASSTECH_QUAD,m_Blend);
			}
			else if(m_RenderTechnique == "CrossQuads")
			{
				m_GrassLayer->setRenderTechnique(GRASSTECH_CROSSQUADS,m_Blend);
			}
			else if(m_RenderTechnique == "Sprite")
			{
				m_GrassLayer->setRenderTechnique(GRASSTECH_SPRITE,m_Blend);
			}
		}
	}

	bool GrassLayerComponent::GetBlendWithGround() const
	{
		return m_Blend;

	}

	void GrassLayerComponent::SetBlendWithGround(bool value)
	{
		m_Blend = value;
		SetRenderTechnique(m_RenderTechnique);
	}

	Vec2 GrassLayerComponent::GetMaxSize() const
	{
		return m_MaxSize;
	}

	void GrassLayerComponent::SetMaxSize(const Vec2 &size)
	{
		m_MaxSize = size;

		if(m_GrassLayer)
			m_GrassLayer->setMaximumSize(m_MaxSize.x,m_MaxSize.y);
	}

	Vec2 GrassLayerComponent::GetMinSize() const
	{
		return m_MinSize;
	}

	void GrassLayerComponent::SetMinSize(const Vec2 &size)
	{
		m_MinSize = size;
		if(m_GrassLayer)
			m_GrassLayer->setMinimumSize(m_MinSize.x,m_MinSize.y);
	}

	float GrassLayerComponent::GetSwaySpeed()const
	{
		return m_SwaySpeed;
	}

	void GrassLayerComponent::SetSwaySpeed(float speed)
	{
		m_SwaySpeed = speed;
		if(m_GrassLayer)
		{
			m_GrassLayer->setSwaySpeed(m_SwaySpeed);
		}
	}

	float GrassLayerComponent::GetSwayLength() const
	{
		return m_SwayLength;
	}

	void GrassLayerComponent::SetSwayLength(float length)
	{
		m_SwayLength = length;
		if(m_GrassLayer)
		{
			m_GrassLayer->setSwayLength(m_SwayLength);
		}
	}

	bool GrassLayerComponent::GetEnableSway() const
	{
		return m_EnableSway;
	}

	void GrassLayerComponent::SetEnableSway(bool value)
	{
		m_EnableSway = value;
		if(m_GrassLayer)
		{
			if(m_EnableSway)
			{
				m_GrassLayer->setAnimationEnabled(true);
			}
			else
			{
				m_GrassLayer->setAnimationEnabled(false);
			}
		}
	}

	float GrassLayerComponent::GetSwayDistribution() const
	{
		return m_SwayDistribution;
	}

	void GrassLayerComponent::SetSwayDistribution(float distribution)
	{
		m_SwayDistribution = distribution;
		if(m_GrassLayer)
		{
			m_GrassLayer->setSwayDistribution(m_SwayDistribution);
		}
	}
	
	void GrassLayerComponent::UpdateSway()
	{
		if(m_GrassLayer)
		{
			if(m_EnableSway)
			{
				m_GrassLayer->setAnimationEnabled(true);
				m_GrassLayer->setSwaySpeed(m_SwaySpeed);
				m_GrassLayer->setSwayLength(m_SwayLength);
				m_GrassLayer->setSwayDistribution(m_SwayDistribution);
				//FileLog::Print("speed:%f length:%f dist:%f",m_SwaySpeed,m_SwayLength,m_SwayDistribution);
			}
			else
			{
				m_GrassLayer->setAnimationEnabled(false);
			}
		}
	}

	void GrassLayerComponent::OnRoadMessage(RoadMessagePtr message)
	{
		Ogre::Real brush_size = message->GetPaintWidth();
		std::vector<Vec3> rwps = message->GetRoadWaypoints();
		for(size_t i = 0; i < rwps.size()-1; i++)
		{
			const Vec3 line = rwps[i+1] - rwps[i];
			const Float  length = line.Length();
			const Vec3 dir = line*(1.0/length);
			const Float  step_size = 0.1;
			Float dist = 0;
			while(dist < length)
			{
				Vec3 pos = rwps[i] + dir*dist;
				dist += step_size;
				if(message->GetPaintWidth())
					Paint(pos, message->GetPaintWidth(),message->GetPaintWidth(), -10);
			}
		}
	}

	void GrassLayerComponent::OnPaint(GrassPaintMessagePtr message)
	{
			Paint(message->GetPosition(), message->GetBrushSize(), message->GetBrushInnerSize(), message->GetIntensity());
	}

	void GrassLayerComponent::Paint(const Vec3 &world_pos, float brush_size, float brush_inner_size , float intensity)
	{
		GrassLoaderComponentPtr gl_component = GetSceneObject()->GetFirstComponentByClass<GrassLoaderComponent>();
		Forests::DensityMap *dmap = m_GrassLayer->getDensityMap();
		if(dmap)
		{
			TBounds map_bounds = gl_component->GetMapBounds();
			Ogre::PixelBox pbox = dmap->getPixelBox();
			Ogre::uchar *data = static_cast<Ogre::uchar*>(dmap->getPixelBox().data);
			
			int wsize = static_cast<int>(dmap->getPixelBox().getWidth()-1);

			unsigned char rgbaShift[4];
			int layerID = 0;
			int pos = rgbaShift[layerID] / 8;

			const Ogre::Real height = map_bounds.height();
			const Ogre::Real width = map_bounds.width();
			const Ogre::Real x_pos = (world_pos.x - map_bounds.left)/width;
			const Ogre::Real y_pos = (world_pos.z - map_bounds.top)/height;

			const Ogre::Real brush_size_texture_space_x = brush_size/width;
			const Ogre::Real brush_size_texture_space_y = brush_size/height;
			const Ogre::Real brush_inner_radius = (brush_inner_size*0.5)/height;

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
					val += weight*intensity;
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
}


