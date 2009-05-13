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

#include <OgreEntity.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>
#include "IPagedGeometry.h"
#include "Main/SceneNodes/ICameraNode.h"
#include "Main/SceneNodes/Players/BasePlayer.h"
#include "Main/Level.h"
#include "PagedGeometryManager.h"
#include "Main/SceneNodes/FreeCamera.h"
#include "PagedGeometry.h"
#include "ImpostorPage.h"
#include "BatchPage.h"
//#include "EntityPage.h"
#include "Main/SceneNodes/BaseObject.h"
#include "Main/Geometry/ITerrainGeometry.h"
#include "Main/ObjectContainer.h"
#include "Main/Helpers/Log.h"

namespace HiFi
{
	IMPLEMENT_RTTI_PROP(IPagedGeometry,IGeometry);
	IMPLEMENT_PROPERTIES(IPagedGeometry,CExtraProp);

	bool IPagedGeometry::OnRegister()
	{
		REGISTER_PROP(String,IPagedGeometry,m_DensityMapFilename,"DensityMap",CProperty::STREAM|CProperty::READONLY,"");
		REGISTER_PROP(Float,IPagedGeometry,m_DensityFactor,"DensityFactor",CProperty::STREAM|CProperty::READONLY,"");
		REGISTER_PROP(Float,IPagedGeometry,m_PageSize,"PageSize",CProperty::STREAM|CProperty::READONLY,"");
		REGISTER_PROP(Float,IPagedGeometry,m_ImposterAlphaRejectionValue,"ImposterAlphaRejectionValue",CProperty::STREAM|CProperty::READONLY,"");
		REGISTER_PROP(Vect4D,IPagedGeometry,m_Bounds,"Bounds",CProperty::STREAM|CProperty::READONLY,"");
		return true;
	}

	IPagedGeometry::IPagedGeometry(void) : IGeometry()
	{
		m_DensityFactor = 0.001;
		m_Bounds.Set(0,0,0,0);
		m_PageSize = 50;
		m_PagedGeometry = NULL;
	}

	IPagedGeometry::~IPagedGeometry(void)
	{
		if(m_PagedGeometry)
		{
			Root::Get().GetMainWindow()->removeListener(this);
			if(m_PagedGeometry->getPageLoader())
				delete m_PagedGeometry->getPageLoader();
			delete m_PagedGeometry;
			Root::Get().RemoveSimulationListener(this);
		}

	}

	void IPagedGeometry::Shutdown()
	{

	}


	ITerrainGeometry * IPagedGeometry::FindTerrain()
	{
		//Try to find terrain geometry under the same scene node that this geometry is attached
		assert(m_SceneNode);
		BaseObject* obj = DYNAMIC_CAST(BaseObject,m_SceneNode->GetRoot());
		BaseObject::GeometryVector* geom_vec =obj->GetGeometryVector();
		for(int i = 0;  i < geom_vec->size(); i++)
		{
			ITerrainGeometry * geom = DYNAMIC_CAST(ITerrainGeometry, geom_vec->at(i));
			if(geom) return geom;
		}

		//ok, try other objects in the static_object container and hope that its initilized
		ObjectContainer::BaseObjectVector *all_objects = Root::Get().GetLevel()->GetStaticObjectContainer()->GetObjects();
		ObjectContainer::BaseObjectIter obj_iter;
		for(obj_iter = all_objects->begin(); obj_iter != all_objects->end(); obj_iter++)
		{
			BaseObject* obj =*obj_iter;
			BaseObject::GeometryVector* geom_vec = obj->GetGeometryVector();
			for(int i = 0;  i < geom_vec->size(); i++)
			{
				ITerrainGeometry * geom = DYNAMIC_CAST(ITerrainGeometry, geom_vec->at(i));
				if(geom) return geom;
			}
		}
		Log::Warning("ITerrainGeometry::FindTerrain - Could not find terrrain object");
		return NULL;
	}

	void IPagedGeometry::Init(ISceneNode* node)
	{
		IGeometry::Init(node);
		bool user_bounds = true;
		if(m_Bounds.x == 0 && m_Bounds.y == 0 && m_Bounds.z == 0 && m_Bounds.w == 0)
		{
			user_bounds = false;
		}

		if(!user_bounds)
		{
			ITerrainGeometry * terrain  = FindTerrain();
			if(terrain)
			{
				Vec3 bmin,bmax;
				terrain->GetBounds(bmin,bmax);

				m_Bounds.x = bmin.x;
				m_Bounds.y = bmin.z;

				m_Bounds.z = bmax.x;
				m_Bounds.w = bmax.z;
			}
			else
			{
				m_Bounds.Set(0,0,2000,2000);
			}
			m_MapBounds = TBounds(m_Bounds.x, m_Bounds.y, m_Bounds.z, m_Bounds.w);
		}
		else m_MapBounds = TBounds(m_Bounds.x, m_Bounds.y, m_Bounds.z, m_Bounds.w);
		//What camera should be used?
		Ogre::Camera* ocam = NULL;
		if(Root::Get().GetDefaultLocalPlayer()) //check if we have a player, use his cam in that case
		{
			ICameraNode* cam = Root::Get().GetDefaultLocalPlayer()->GetCamera();
			assert(cam);
			ocam = cam->GetOgreCamera();
		}
		else //get first camera object
		{
			Ogre::Viewport* vp = Root::Get().GetMainWindow()->getViewport(0);
			ocam = vp->getCamera();
		}
		assert(ocam);

		m_PagedGeometry = new PagedGeometry(ocam, m_PageSize);
		Root::Get().GetMainWindow()->addListener(this);
		Root::Get().AddSimulationListener(this);
		//m_PagedGeometry = new PagedGeometry(ocam, m_PageSize); infinit bounds
	}

	void IPagedGeometry::Update()
	{
		IGeometry::Update();
		//m_PagedGeometry->update();
	}
	void IPagedGeometry::SimulationUpdate(float delta)
	{

	}

	void IPagedGeometry::preViewportUpdate(const Ogre::RenderTargetViewportEvent& evt)
	{
		Ogre::Viewport *vp = evt.source;
		m_PagedGeometry->update();
		if(vp) 
			m_PagedGeometry->setCamera(vp->getCamera());
	}
}


