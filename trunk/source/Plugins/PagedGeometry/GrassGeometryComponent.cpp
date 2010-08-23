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
#include "GrassGeometryComponent.h"
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
	ITerrainComponent* GrassGeometryComponent::m_Terrain = NULL;
	GrassGeometryComponent::GrassGeometryComponent(void) : m_DensityFactor(0.001),
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

	GrassGeometryComponent::~GrassGeometryComponent(void)
	{
		
	}


	void GrassGeometryComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("GrassGeometryComponent",new Creator<GrassGeometryComponent, IComponent>);
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

	void GrassGeometryComponent::OnCreate()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(GrassGeometryComponent::OnLoad,LoadGFXComponentsMessage,999));
		GetSceneObject()->RegisterForMessage(REG_TMESS(GrassGeometryComponent::OnUnload,UnloadComponentsMessage,0));
	}

	std::string GrassGeometryComponent::GetDensityMap() const
	{

		return m_DensityMapFilename;


	}
	void GrassGeometryComponent::SetDensityMap(const std::string &dm)
	{
		m_DensityMapFilename = Misc::GetFilename(dm);
		if(m_GrassLayer)
			m_GrassLayer->setDensityMap(m_DensityMapFilename);
	}

	float GrassGeometryComponent::GetDensityFactor() const
	{
		return m_DensityFactor;
	}

	void GrassGeometryComponent::SetDensityFactor(float factor)
	{
		m_DensityFactor = factor;
		if(m_GrassLayer)
			m_GrassLayer->setDensity(m_DensityFactor);
	}

	float GrassGeometryComponent::GetPageSize() const
	{
		return m_PageSize;
	}

	void GrassGeometryComponent::SetPageSize(float size)
	{
		m_PageSize = size;

	}
	float GrassGeometryComponent::GetImposterAlphaRejectionValue() const
	{
		return m_ImposterAlphaRejectionValue;
	}

	void GrassGeometryComponent::SetImposterAlphaRejectionValue(float value)
	{
		m_ImposterAlphaRejectionValue =value;
	}

	Vec4 GrassGeometryComponent::GetBounds() const
	{
		return m_Bounds;
	}

	void GrassGeometryComponent::SetBounds(const Vec4 &bounds)
	{
		m_Bounds = bounds;
		if(m_GrassLayer)
			m_GrassLayer->setMapBounds(TBounds(m_Bounds.x, m_Bounds.y, m_Bounds.z, m_Bounds.w)); 
	}

	std::string GrassGeometryComponent::GetColorMap() const
	{
		return m_ColorMapFilename;

	}

	void GrassGeometryComponent::SetColorMap(const std::string &name)
	{
		m_ColorMapFilename = Misc::GetFilename(name);
		if(m_GrassLayer) 
			m_GrassLayer->setColorMap(m_ColorMapFilename); 
	}

	std::string GrassGeometryComponent::GetMaterial() const
	{
		return m_Material; 
	}

	void GrassGeometryComponent::SetMaterial(const std::string &name)
	{
		m_Material = name;
	}

	std::string GrassGeometryComponent::GetFadeTech() const
	{
		return m_FadeTech;
	}

	void GrassGeometryComponent::SetFadeTech(const std::string &tech)
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

	std::string GrassGeometryComponent::GetRenderTechnique() const 
	{
		return m_RenderTechnique;
	}

	void GrassGeometryComponent::SetRenderTechnique(const std::string &tech)
	{
		m_RenderTechnique = tech;
	}

	bool GrassGeometryComponent::GetBlendWithGround() const
	{
		return m_Blend;

	}

	void GrassGeometryComponent::SetBlendWithGround(bool value)
	{
		m_Blend = value;
	}

	Vec2 GrassGeometryComponent::GetMaxSize() const 
	{
		return m_MaxSize;
	}

	void GrassGeometryComponent::SetMaxSize(const Vec2 &size)
	{
		m_MaxSize = size;

		if(m_GrassLayer)
				m_GrassLayer->setMaximumSize(m_MaxSize.x,m_MaxSize.y); 
	}

	Vec2 GrassGeometryComponent::GetMinSize() const
	{
		return m_MinSize;
	}

	void GrassGeometryComponent::SetMinSize(const Vec2 &size)
	{
		m_MinSize = size;
		if(m_GrassLayer)
			m_GrassLayer->setMinimumSize(m_MinSize.x,m_MinSize.y); 
	}

	float GrassGeometryComponent::GetSwaySpeed()const
	{
		return m_SwaySpeed;
	}

	void GrassGeometryComponent::SetSwaySpeed(float speed)
	{
		m_SwaySpeed = speed;
		if(m_GrassLoader)
		{
			m_GrassLayer->setSwaySpeed(m_SwaySpeed);
		}
	}

	float GrassGeometryComponent::GetSwayLength() const
	{
		return m_SwayLength;
	}

	void GrassGeometryComponent::SetSwayLength(float length)
	{
		m_SwayLength = length;
		if(m_GrassLoader)
		{
			m_GrassLayer->setSwayLength(m_SwayLength);
		}
	}

	bool GrassGeometryComponent::GetEnableSway() const
	{
		return m_EnableSway;
	}

	void GrassGeometryComponent::SetEnableSway(bool value)
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

	float GrassGeometryComponent::GetSwayDistribution() const
	{
		return m_SwayDistribution;
	}

	void GrassGeometryComponent::SetSwayDistribution(float distribution)
	{
		m_SwayDistribution = distribution;
		if(m_GrassLoader)
		{
			m_GrassLayer->setSwayDistribution(m_SwayDistribution);
		}
	}

	float GrassGeometryComponent::GetViewDistance() const
	{
		return m_ViewDist;
	}

	void GrassGeometryComponent::SetViewDistance(float distance)
	{
		m_ViewDist = distance;
	}


	void GrassGeometryComponent::OnUnload(UnloadComponentsMessagePtr message)
	{


		if(m_PagedGeometry)
		{

			m_PagedGeometry->removeDetailLevels();
			m_PagedGeometry->reloadGeometry();

			if(m_PagedGeometry->getPageLoader())
				delete m_PagedGeometry->getPageLoader();
			delete m_PagedGeometry;
		}

		Ogre::SceneManager* sm = Ogre::Root::getSingleton().getSceneManagerIterator().getNext();
		Ogre::Camera* ocam = sm->getCameraIterator().getNext();
		Ogre::RenderTarget *target = NULL;
		if (Ogre::Root::getSingleton().getRenderSystem()->getRenderTargetIterator().hasMoreElements())
			target = Ogre::Root::getSingleton().getRenderSystem()->getRenderTargetIterator().getNext();
		target->removeListener(this);
	}


	TerrainComponentPtr GrassGeometryComponent::GetTerrainComponent(SceneObjectPtr obj)
	{
		TerrainComponentPtr terrain = obj->GetFirstComponent<ITerrainComponent>();
		if(terrain) 
			return terrain;

		IComponentContainer::ComponentContainerIterator cc_iter = obj->GetChildren();
		while(cc_iter.hasMoreElements())
		{
			SceneObjectPtr child = boost::shared_static_cast<SceneObject>(cc_iter.getNext());
			terrain = GetTerrainComponent(child);
			if(terrain) 
				return terrain;
		}
		return terrain;
	}


	void GrassGeometryComponent::OnLoad(LoadGFXComponentsMessagePtr message)
	{
		//assert(ogsm);
		Ogre::SceneManager* sm = Ogre::Root::getSingleton().getSceneManagerIterator().getNext();
		Ogre::Camera* ocam = sm->getCameraIterator().getNext();
		Ogre::RenderTarget *target = NULL;
		if (Ogre::Root::getSingleton().getRenderSystem()->getRenderTargetIterator().hasMoreElements())
			target = Ogre::Root::getSingleton().getRenderSystem()->getRenderTargetIterator().getNext();
		target->addListener(this);
	
		bool user_bounds = true;
		if(m_Bounds.x == 0 && m_Bounds.y == 0 && m_Bounds.z == 0 && m_Bounds.w == 0)
		{
			user_bounds = false;
		}

		if(!user_bounds)
		{
			TerrainComponentPtr terrain = GetTerrainComponent(GetSceneObject());
			if(!terrain)
			{
				SceneObjectPtr root = GetSceneObject()->GetObjectUnderRoot()->GetParentSceneObject();
				terrain = GetTerrainComponent(root);
			}
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

		loader->setHeightFunction(GrassGeometryComponent::GetTerrainHeight);
		m_GrassLayer = loader->addLayer(m_Material); 
		m_GrassLayer->setMaximumSize(m_MaxSize.x,m_MaxSize.y); 
		m_GrassLayer->setMinimumSize(m_MinSize.x,m_MinSize.y); 
		m_GrassLayer->setDensity(m_DensityFactor);
		m_GrassLayer->setMapBounds(m_MapBounds); 
		if(m_DensityMapFilename != "")
			m_GrassLayer->setDensityMap(m_DensityMapFilename);   

		if(m_ColorMapFilename != "")
			m_GrassLayer->setColorMap(m_ColorMapFilename); 

		//loader->setRenderQueueGroup();


		m_GrassLoader = loader;

		SetFadeTech(m_FadeTech);
		UpdateSway();
		
		
//		Root::Get().AddRenderListener(this);
	}

	float GrassGeometryComponent::GetTerrainHeight(float x, float z, void* user_data)
	{
		if(m_Terrain)
			return m_Terrain->GetHeight(x,z);
		else 
			return 0;
	}

	void GrassGeometryComponent::preViewportUpdate(const Ogre::RenderTargetViewportEvent& evt)
	{
		Ogre::Viewport *vp = evt.source;
		m_PagedGeometry->update();
		if(vp) 
			m_PagedGeometry->setCamera(vp->getCamera());
		//if(m_GrassLoader ) 
		//	m_GrassLoader->updateAnimation();
	}



	void GrassGeometryComponent::UpdateSway()
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

/*	void GrassGeometryComponent::RenderUpdate(float delta)
	{
		if(m_GrassLoader ) m_GrassLoader->updateAnimation();
	}*/

/*	void GrassGeometryComponent::Update()
	{
		IPagedGeometry::Update();
	}*/
}


