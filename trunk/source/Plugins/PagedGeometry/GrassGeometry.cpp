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
#include "GrassGeometry.h"
#include "PagedGeometry.h"
#include "ImpostorPage.h"
#include "BatchPage.h"
#include "GrassLoader.h"
#include "Sim/Components/Graphics/Geometry/ITerrainComponent.h"
#include "Sim/Scenario/Scene/SceneObject.h"
#include "Core/ComponentSystem/ComponentFactory.h"
#include "Core/ComponentSystem/IComponent.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/Utils/Log.h"

namespace GASS
{
	ITerrainComponent* GrassGeometry::m_Terrain = NULL;
	GrassGeometry::GrassGeometry(void) : m_DensityFactor(0.001),
		m_Bounds(0,0,0,0),
		m_PageSize(50),
		m_PagedGeometry(NULL),
		m_GrassLoader (NULL),
		m_MaxSize(1,1),
		m_MinSize(1,1),
		m_EnableSway(false),
		m_SwaySpeed(0),
		m_SwayLength(0),
		m_SwayDistribution(10),
		m_ViewDist(50),
		m_RenderTechnique("Quad"),
		m_Blend(false),
		m_GrassLayer(NULL)
//		m_Terrain(NULL)
	{

	}	

	GrassGeometry::~GrassGeometry(void)
	{
		if(m_PagedGeometry)
		{
			if(m_PagedGeometry->getPageLoader())
				delete m_PagedGeometry->getPageLoader();
			delete m_PagedGeometry;
		}
	}


	void GrassGeometry::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("GrassGeometry",new Creator<GrassGeometry, IComponent>);
		RegisterProperty<std::string>("DensityMap", &GetDensityMap, &SetDensityMap);
		RegisterProperty<float>("DensityFactor", &GetDensityFactor, &SetDensityFactor);
		RegisterProperty<float>("PageSize", &GetPageSize, &SetPageSize);
		RegisterProperty<float>("ImposterAlphaRejectionValue", &GetImposterAlphaRejectionValue, &SetImposterAlphaRejectionValue);
		RegisterProperty<Vec4>("Bounds", &GetBounds, &SetBounds);
		RegisterProperty<std::string>("ColorMap", &GetColorMap, &SetColorMap);
		RegisterProperty<std::string>("Material", &GetMaterial, &SetMaterial);
		RegisterProperty<std::string>("FadeTech", &GetFadeTech, &SetFadeTech);
		RegisterProperty<std::string>("RenderTechnique", &GetRenderTechnique, &SetRenderTechnique);
		RegisterProperty<bool>("BlendWithGround", &GetBlendWithGround, &SetBlendWithGround);
		RegisterProperty<Vec2>("MaxSize", &GetMaxSize, &SetMaxSize);
		RegisterProperty<Vec2>("MinSize", &GetMinSize, &SetMinSize);
		RegisterProperty<float>("SwaySpeed", &GetSwaySpeed, &SetSwaySpeed);
		RegisterProperty<float>("SwayLength", &GetSwayLength, &SetSwayLength);
		RegisterProperty<bool>("EnableSway", &GetEnableSway, &SetEnableSway);
		RegisterProperty<float>("SwayDistribution", &GetSwayDistribution, &SetSwayDistribution);
		RegisterProperty<float>("ViewDistance", &GetViewDistance, &SetViewDistance);

	}

	void GrassGeometry::OnCreate()
	{
		REGISTER_OBJECT_MESSAGE_CLASS(GrassGeometry::OnLoad,LoadGFXComponentsMessage,999);
	}

	std::string GrassGeometry::GetDensityMap() const
	{

		return m_DensityMapFilename;


	}
	void GrassGeometry::SetDensityMap(const std::string &dm)
	{
		m_DensityMapFilename = dm;
		if(m_GrassLayer)
			m_GrassLayer->setDensityMap(m_DensityMapFilename);
	}

	float GrassGeometry::GetDensityFactor() const
	{
		return m_DensityFactor;
	}

	void GrassGeometry::SetDensityFactor(float factor)
	{
		m_DensityFactor = factor;
		if(m_GrassLayer)
			m_GrassLayer->setDensity(m_DensityFactor);
	}

	float GrassGeometry::GetPageSize() const
	{
		return m_PageSize;
	}

	void GrassGeometry::SetPageSize(float size)
	{
		m_PageSize = size;

	}
	float GrassGeometry::GetImposterAlphaRejectionValue() const
	{
		return m_ImposterAlphaRejectionValue;
	}

	void GrassGeometry::SetImposterAlphaRejectionValue(float value)
	{
		m_ImposterAlphaRejectionValue =value;
	}

	Vec4 GrassGeometry::GetBounds() const
	{
		return m_Bounds;
	}

	void GrassGeometry::SetBounds(const Vec4 &bounds)
	{
		m_Bounds = bounds;
		if(m_GrassLayer)
			m_GrassLayer->setMapBounds(TBounds(m_Bounds.x, m_Bounds.y, m_Bounds.z, m_Bounds.w)); 
	}

	std::string GrassGeometry::GetColorMap() const
	{
		return m_ColorMapFilename;

	}

	void GrassGeometry::SetColorMap(const std::string &name)
	{
		m_ColorMapFilename = name;
		if(m_GrassLayer) 
			m_GrassLayer->setColorMap(m_ColorMapFilename); 
	}

	std::string GrassGeometry::GetMaterial() const
	{
		return m_Material; 
	}

	void GrassGeometry::SetMaterial(const std::string &name)
	{
		m_Material = name;
	}

	std::string GrassGeometry::GetFadeTech() const
	{
		return m_FadeTech;
	}

	void GrassGeometry::SetFadeTech(const std::string &tech)
	{
		m_FadeTech = tech;
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

	std::string GrassGeometry::GetRenderTechnique() const 
	{
		return m_RenderTechnique;
	}

	void GrassGeometry::SetRenderTechnique(const std::string &tech)
	{
		m_RenderTechnique = tech;
	}

	bool GrassGeometry::GetBlendWithGround() const
	{
		return m_Blend;

	}

	void GrassGeometry::SetBlendWithGround(bool value)
	{
		m_Blend = value;
	}

	Vec2 GrassGeometry::GetMaxSize() const 
	{
		return m_MaxSize;
	}

	void GrassGeometry::SetMaxSize(const Vec2 &size)
	{
		m_MaxSize = size;

		if(m_GrassLayer)
				m_GrassLayer->setMaximumSize(m_MaxSize.x,m_MaxSize.y); 
	}

	Vec2 GrassGeometry::GetMinSize() const
	{
		return m_MinSize;
	}

	void GrassGeometry::SetMinSize(const Vec2 &size)
	{
		m_MinSize = size;
		if(m_GrassLayer)
			m_GrassLayer->setMinimumSize(m_MinSize.x,m_MinSize.y); 
	}

	float GrassGeometry::GetSwaySpeed()const
	{
		return m_SwaySpeed;
	}

	void GrassGeometry::SetSwaySpeed(float speed)
	{
		m_SwaySpeed = speed;
		if(m_GrassLoader)
		{
			m_GrassLayer->setSwaySpeed(m_SwaySpeed);
		}
	}

	float GrassGeometry::GetSwayLength() const
	{
		return m_SwayLength;
	}

	void GrassGeometry::SetSwayLength(float length)
	{
		m_SwayLength = length;
		if(m_GrassLoader)
		{
			m_GrassLayer->setSwayLength(m_SwayLength);
		}
	}

	bool GrassGeometry::GetEnableSway() const
	{
		return m_EnableSway;
	}

	void GrassGeometry::SetEnableSway(bool value)
	{
		m_EnableSway = value;
		if(m_GrassLoader)
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

	float GrassGeometry::GetSwayDistribution() const
	{
		return m_SwayDistribution;
	}

	void GrassGeometry::SetSwayDistribution(float distribution)
	{
		m_SwayDistribution = distribution;
		if(m_GrassLoader)
		{
			m_GrassLayer->setSwayDistribution(m_SwayDistribution);
		}
	}

	float GrassGeometry::GetViewDistance() const
	{
		return m_ViewDist;
	}

	void GrassGeometry::SetViewDistance(float distance)
	{
		m_ViewDist = distance;
	}


	void GrassGeometry::OnLoad(LoadGFXComponentsMessagePtr message)
	{
		
		//assert(ogsm);
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
		//What camera should be used?
		
		m_PagedGeometry = new PagedGeometry(ocam, m_PageSize);
		
		GrassLoader* loader = new GrassLoader(m_PagedGeometry);
		loader->setRenderQueueGroup(Ogre::RENDER_QUEUE_MAIN);
		m_PagedGeometry->addDetailLevel<GrassPage>(m_ViewDist); 
		m_PagedGeometry->setPageLoader(loader);

		loader->setHeightFunction(GrassGeometry::GetTerrainHeight);
		m_GrassLayer = loader->addLayer(m_Material); 
		m_GrassLayer->setMaximumSize(m_MaxSize.x,m_MaxSize.y); 
		m_GrassLayer->setMinimumSize(m_MinSize.x,m_MinSize.y); 
		m_GrassLayer->setDensity(m_DensityFactor);
		m_GrassLayer->setMapBounds(m_MapBounds); 
		m_GrassLayer->setDensityMap(m_DensityMapFilename);   
		m_GrassLayer->setColorMap(m_ColorMapFilename); 

		//loader->setRenderQueueGroup();


		m_GrassLoader = loader;

		SetFadeTech(m_FadeTech);
		UpdateSway();
		
		
//		Root::Get().AddRenderListener(this);
	}

	float GrassGeometry::GetTerrainHeight(float x, float z)
	{
		if(m_Terrain)
			return m_Terrain->GetHeight(x,z);
		else 
			return 0;
	}

	void GrassGeometry::preViewportUpdate(const Ogre::RenderTargetViewportEvent& evt)
	{
		Ogre::Viewport *vp = evt.source;
		m_PagedGeometry->update();
		if(vp) 
			m_PagedGeometry->setCamera(vp->getCamera());
		if(m_GrassLoader ) 
			m_GrassLoader->updateAnimation();
	}



	void GrassGeometry::UpdateSway()
	{
		if(m_GrassLoader)
		{
			if(m_EnableSway)
			{
				m_GrassLayer->setAnimationEnabled(true);
				m_GrassLayer->setSwaySpeed(m_SwaySpeed);
				m_GrassLayer->setSwayLength(m_SwayLength);
				m_GrassLayer->setSwayDistribution(m_SwayDistribution);
				//Log::Print("speed:%f length:%f dist:%f",m_SwaySpeed,m_SwayLength,m_SwayDistribution);
			}
			else
			{
				m_GrassLayer->setAnimationEnabled(false);
			}
		}
	}

/*	void GrassGeometry::RenderUpdate(float delta)
	{
		if(m_GrassLoader ) m_GrassLoader->updateAnimation();
	}*/

/*	void GrassGeometry::Update()
	{
		IPagedGeometry::Update();
	}*/
}


