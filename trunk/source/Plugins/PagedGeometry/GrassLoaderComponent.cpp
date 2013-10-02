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


#include <OgreEntity.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>
#include "GrassLoaderComponent.h"
#include "GrassLayerComponent.h"
#include "PagedGeometry.h"
#include "ImpostorPage.h"
#include "BatchPage.h"
#include "GrassLoader.h"
#include "Sim/Interface/GASSITerrainComponent.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSSimSystemManager.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/ComponentSystem/GASSIComponent.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/Utils/GASSLogManager.h"
#include "Core/Utils/GASSException.h"
#include "Plugins/Ogre/IOgreSceneManagerProxy.h"

namespace GASS
{
	IHeightmapTerrainComponent* GrassLoaderComponent::m_Terrain = NULL;
	ICollisionSceneManager * GrassLoaderComponent::m_CollisionSM = NULL;

	GrassLoaderComponent::GrassLoaderComponent(void) : m_CustomBounds(0,0,0,0),
		m_PageSize(50),
		m_PagedGeometry(NULL),
		m_GrassLoader (NULL),
		m_ViewDist(50),
		m_LOD0(0),
		m_DensityMap(NULL),
		m_DensityMapSize(1024),
		m_SceneMan(NULL)
	{

	}

	GrassLoaderComponent::~GrassLoaderComponent(void)
	{

	}

	void GrassLoaderComponent::RegisterReflection()
	{
		std::vector<std::string> ext;
		ext.push_back("png");
		ext.push_back("tga");

		ComponentFactory::GetPtr()->Register("GrassLoaderComponent",new Creator<GrassLoaderComponent, IComponent>);
		GetClassRTTI()->SetMetaData(ClassMetaDataPtr(new ClassMetaData("GrassLoaderComponent", OF_VISIBLE)));
		RegisterProperty<std::string>("DensityMap", &GrassLoaderComponent::GetDensityMap, &GrassLoaderComponent::SetDensityMap,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("Can only be specified in template",PF_VISIBLE)));
		RegisterProperty<std::string>("ImportDensityMap", &GrassLoaderComponent::GetImportDensityMap, &GrassLoaderComponent::SetImportDensityMap,
			FilePathPropertyMetaDataPtr(new FilePathPropertyMetaData("Import density map",PF_VISIBLE | PF_EDITABLE, FilePathPropertyMetaData::IMPORT_FILE, ext)));
		RegisterProperty<float>("PageSize", &GrassLoaderComponent::GetPageSize, &GrassLoaderComponent::SetPageSize,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("You need to reload scene before this property take effect",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<Vec4>("CustomBounds", &GrassLoaderComponent::GetCustomBounds, &GrassLoaderComponent::SetCustomBounds,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE)));
		RegisterProperty<std::string>("ColorMap", &GrassLoaderComponent::GetColorMap, &GrassLoaderComponent::SetColorMap,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE)));
		RegisterProperty<float>("ViewDistance", &GrassLoaderComponent::GetViewDistance, &GrassLoaderComponent::SetViewDistance,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
	}

	void GrassLoaderComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(GrassLoaderComponent::OnPaint,GrassPaintMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(GrassLoaderComponent::OnRoadMessage,RoadMessage,0));
	
		m_SceneMan = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<IOgreSceneManagerProxy>()->GetOgreSceneManager();
		Ogre::Camera* ocam = NULL;
		if(m_SceneMan->hasCamera("DummyCamera"))
			ocam = m_SceneMan->getCamera("DummyCamera");
		else
			ocam = m_SceneMan->createCamera("DummyCamera");


		Ogre::RenderSystem::RenderTargetIterator iter = Ogre::Root::getSingleton().getRenderSystem()->getRenderTargetIterator();
		while (iter.hasMoreElements())
		{
			Ogre::RenderWindow* target = dynamic_cast<Ogre::RenderWindow*>(iter.getNext());
			if(target)
				target->addListener(this);
		}
	
		bool user_bounds = true;
		if(m_CustomBounds.x == 0 && m_CustomBounds.y == 0 && m_CustomBounds.z == 0 && m_CustomBounds.w == 0)
		{
			user_bounds = false;
		}

		m_CollisionSM = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<ICollisionSceneManager>().get();

		if(!user_bounds)
		{
			HeightmapTerrainComponentPtr terrain = GetSceneObject()->GetScene()->GetRootSceneObject()->GetFirstComponentByClass<IHeightmapTerrainComponent>(true);
			
			if(terrain)
			{
				GeometryComponentPtr geom = DYNAMIC_PTR_CAST<IGeometryComponent>(terrain);
				AABox aabox = geom->GetBoundingBox();
				m_MapBounds = TBounds(aabox.m_Min.x, aabox.m_Min.z, aabox.m_Max.x, aabox.m_Max.z);
				//for speed we save the raw pointer , we will access this for each height callback
				m_Terrain = terrain.get();
			}
			else
			{
				GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"No terrain found , you need to specify custom bounds if no terrain present","GrassLoaderComponent::OnLoad");
			}
		}
		else
		{
			m_MapBounds = TBounds(m_CustomBounds.x, m_CustomBounds.y, m_CustomBounds.z, m_CustomBounds.w);
		}
		//What camera should be used?

		m_PagedGeometry = new PagedGeometry(ocam, m_PageSize);

		m_GrassLoader = new GrassLoader(m_PagedGeometry);
		m_GrassLoader->setRenderQueueGroup(Ogre::RENDER_QUEUE_MAIN);
		m_LOD0 = m_PagedGeometry->addDetailLevel<GrassPage>(m_ViewDist);

		m_PagedGeometry->setPageLoader(m_GrassLoader);
		m_GrassLoader->setHeightFunction(GrassLoaderComponent::GetTerrainHeight,this);
		if(m_DensityMapFilename == "")
		{
			//create from in run time?
			//try to load 
			ScenePtr  scene = GetSceneObject()->GetScene();
			std::string scene_path = scene->GetSceneFolder().GetFullPath();
			const std::string denmapname = "density_map_" + GetName() + ".tga";
			if(scene_path != "")
			{
				const std::string fp_denmap = scene_path + "/" + denmapname;
				std::fstream fstr(fp_denmap.c_str(), std::ios::in|std::ios::binary);
				Ogre::DataStreamPtr stream = Ogre::DataStreamPtr(OGRE_NEW Ogre::FileStreamDataStream(&fstr, false));
				try
				{
					m_DensityImage.load(stream);
				}
				catch(...)
				{
				
					Ogre::uchar *data = OGRE_ALLOC_T(Ogre::uchar, m_DensityMapSize * m_DensityMapSize * 4, Ogre::MEMCATEGORY_GENERAL);
					memset(data, 0, m_DensityMapSize * m_DensityMapSize * 4);
					m_DensityImage.loadDynamicImage(data, m_DensityMapSize, m_DensityMapSize, 1, Ogre::PF_A8R8G8B8, true);
					m_DensityImage.save(fp_denmap);
				}
				stream.setNull();
				m_DensityTexture = Ogre::TextureManager::getSingletonPtr()->createOrRetrieve(denmapname, GetSceneObject()->GetScene()->GetResourceGroupName()).first;
			}
			else
			{
				Ogre::uchar *data = OGRE_ALLOC_T(Ogre::uchar, m_DensityMapSize * m_DensityMapSize * 4, Ogre::MEMCATEGORY_GENERAL);
				memset(data, 0, m_DensityMapSize * m_DensityMapSize * 4);
				m_DensityImage.loadDynamicImage(data, m_DensityMapSize, m_DensityMapSize, 1, Ogre::PF_A8R8G8B8, true);
				m_DensityTexture = Ogre::TextureManager::getSingletonPtr()->createOrRetrieve(denmapname,GetSceneObject()->GetScene()->GetResourceGroupName()).first;
				m_DensityTexture->loadImage(m_DensityImage);
			}
		}
		GetSceneObject()->PostMessage(MessagePtr(new GrassLoaderComponentLoaded()));
	}

	void GrassLoaderComponent::OnDelete()
	{
		if(m_PagedGeometry)
		{
			m_PagedGeometry->removeDetailLevels();
			m_PagedGeometry->reloadGeometry();

			if(m_PagedGeometry->getPageLoader())
				delete m_PagedGeometry->getPageLoader();
			delete m_PagedGeometry;
		}

		Ogre::RenderSystem::RenderTargetIterator iter = Ogre::Root::getSingleton().getRenderSystem()->getRenderTargetIterator();
		while (iter.hasMoreElements())
		{
			Ogre::RenderWindow* target = dynamic_cast<Ogre::RenderWindow*>(iter.getNext());
			if(target)
				target->removeListener(this);
		}
	}

	std::string GrassLoaderComponent::GetDensityMap() const
	{
		return m_DensityMapFilename;
	}

	void GrassLoaderComponent::SetDensityMap(const std::string &dm)
	{
		m_DensityMapFilename = dm;
	}


	std::string GrassLoaderComponent::GetImportDensityMap() const
	{
		return "";
	}

	void GrassLoaderComponent::SetImportDensityMap(const std::string &dm)
	{
		if(m_GrassLoader ==NULL)
			return;
		std::fstream fstr(dm.c_str(), std::ios::in|std::ios::binary);
		Ogre::DataStreamPtr stream = Ogre::DataStreamPtr(OGRE_NEW Ogre::FileStreamDataStream(&fstr, false));
		try
		{
			m_DensityImage.load(stream);
		}
		catch(...)
		{
			return;	
		}
		SaveDensityMap();
		//reload
		const std::string denmapname = "density_map_" + GetName() + ".tga";
		m_DensityTexture->loadImage(m_DensityImage);
		//reload all terrain grass
		IComponentContainer::ComponentVector components;
		GetSceneObject()->GetComponentsByClass(components, "GrassLayerComponent", true);
		for(int i = 0 ;  i < components.size(); i++)
		{
			GrassLayerComponentPtr layer = DYNAMIC_PTR_CAST<GrassLayerComponent>(components[i]);
			layer->GetLayer()->setDensityMap(m_DensityTexture,CHANNEL_BLUE);
		}

		m_PagedGeometry->reloadGeometry();
	}

	float GrassLoaderComponent::GetPageSize() const
	{
		return m_PageSize;
	}

	void GrassLoaderComponent::SetPageSize(float size)
	{
		m_PageSize = size;
	}

	Vec4 GrassLoaderComponent::GetCustomBounds() const
	{
		return m_CustomBounds;
	}

	void GrassLoaderComponent::SetCustomBounds(const Vec4 &bounds)
	{
		m_CustomBounds = bounds;
	}

	std::string GrassLoaderComponent::GetColorMap() const
	{
		return m_ColorMapFilename;
	}

	void GrassLoaderComponent::SetColorMap(const std::string &name)
	{
		m_ColorMapFilename = name;//FileUtils::GetFilename(name);
	}

	float GrassLoaderComponent::GetViewDistance() const
	{
		return m_ViewDist;
	}

	void GrassLoaderComponent::SetViewDistance(float distance)
	{
		m_ViewDist = distance;
	}

	void GrassLoaderComponent::ReloadGeometry()
	{
		if(m_PagedGeometry)
		{
			m_PagedGeometry->reloadGeometry();
		}
	}

	

	void GrassLoaderComponent::SaveXML(TiXmlElement *obj_elem)
	{
		BaseSceneComponent::SaveXML(obj_elem);
		SaveDensityMap();
	}

	void GrassLoaderComponent::SaveDensityMap()
	{
		ScenePtr scene = GetSceneObject()->GetScene();
		std::string scene_path = scene->GetSceneFolder().GetFullPath();
		std::string denmapname;
		if(m_DensityMapFilename != "")
		{
			denmapname = m_DensityMapFilename;
		}
		else
			denmapname = "density_map_" + GetName() + ".tga";
		const std::string fp_denmap = scene_path + "/" + denmapname;
		m_DensityImage.save(fp_denmap);
	}

	float GrassLoaderComponent::GetCollisionSystemHeight(float x, float z)
	{
		if(m_CollisionSM)
		{
			GASS::CollisionRequest request;
			request.LineStart.Set(x,-1000,z);
			request.LineEnd.Set(x,2000,z);
			request.Type = COL_LINE;
			
			request.ReturnFirstCollisionPoint = false;
			request.CollisionBits = GEOMETRY_FLAG_SCENE_OBJECTS;
			GASS::CollisionResult result;
			result.Coll = false;
			m_CollisionSM->Force(request,result);
			if(result.Coll)
				return result.CollPosition.y;
		}
		return 0;
	}

	float GrassLoaderComponent::GetTerrainHeight(float x, float z, void* user_data)
	{
		if(m_Terrain)
			return m_Terrain->GetHeightAtWorldLocation(x,z);
		else
		{
			GrassLoaderComponent* grass = static_cast<GrassLoaderComponent*> (user_data);
			return grass->GetCollisionSystemHeight(x, z);
		}
	}

	void GrassLoaderComponent::preViewportUpdate(const Ogre::RenderTargetViewportEvent& evt)
	{
		Ogre::Viewport *vp = evt.source;
		m_PagedGeometry->update();
		if(vp && vp->getCamera() && m_SceneMan == vp->getCamera()->getSceneManager())
			m_PagedGeometry->setCamera(vp->getCamera());
	}

	void GrassLoaderComponent::OnRoadMessage(RoadMessagePtr message)
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

	void GrassLoaderComponent::OnPaint(GrassPaintMessagePtr message)
	{
		Paint(message->GetPosition(), message->GetBrushSize(), message->GetBrushInnerSize(), message->GetIntensity());
	}

	void GrassLoaderComponent::Paint(const Vec3 &world_pos, float brush_size, float brush_inner_size , float intensity)
	{

		int denmap_width = 0;
		std::vector<Ogre::uchar*> layer_data;
		IComponentContainer::ComponentVector components;
		GetSceneObject()->GetComponentsByClass(components, "GrassLayerComponent", true);
		for(int i = 0 ;  i < components.size(); i++)
		{
			GrassLayerComponentPtr layer = DYNAMIC_PTR_CAST<GrassLayerComponent>(components[i]);

			Forests::DensityMap *dmap = layer->GetLayer()->getDensityMap();
			if(dmap)
			{
				Ogre::PixelBox pbox = dmap->getPixelBox();
				Ogre::uchar *l_data = static_cast<Ogre::uchar*>(dmap->getPixelBox().data);
				denmap_width = static_cast<int>(pbox.getWidth()-1);
				layer_data.push_back(l_data );
			}
		}

		Ogre::uchar *data = m_DensityImage.getData();
		int wsize = m_DensityImage.getWidth()-1;

		const Ogre::Real height = m_MapBounds.height();
		const Ogre::Real width = m_MapBounds.width();
		const Ogre::Real x_pos = (world_pos.x - m_MapBounds.left)/width;
		const Ogre::Real y_pos = (world_pos.z - m_MapBounds.top)/height;

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
			int texture_loc = y * (wsize+1);
			int denmap_loc = y * (denmap_width+1);
			
			for (long x = startx; x <= endx; ++x)
			{

				Ogre::Real tsXdist = (x / (float)wsize) - x_pos;
				Ogre::Real tsYdist = (y / (float)wsize) - y_pos;

				Ogre::Real dist = Ogre::Math::Sqrt(tsYdist * tsYdist + tsXdist * tsXdist);

				Ogre::Real weight = std::min((Ogre::Real)1.0,((dist - brush_inner_radius )/ Ogre::Real(0.5 * brush_size_texture_space_x - brush_inner_radius)));
				if( weight < 0) weight = 0;
				weight = 1.0 - (weight * weight);
				//weight = 1;

				float val = float(data[((texture_loc + x)*4)])/255.0f;
				val += weight*intensity;
				//val = std::min(val, 255.0f);
				//val = std::max(val, 0.0f);
				if(val > 1.0)
					val = 1;
				if(val < 0.0)
					val = 0;
			
				data[((texture_loc + x)*4)] = val*255;

				//also update all layers!
				for(int i = 0; i < layer_data.size(); i++)
				{
					Ogre::uchar *l_data = layer_data[i];
					l_data [denmap_loc+ x] = val*255;
				}
			}
		}

		float posL = world_pos.x - brush_size;
		float posT = world_pos.z - brush_size;
		float posR = world_pos.x + brush_size;
		float posB = world_pos.z + brush_size;
		Forests::TBounds bounds(posL, posT, posR, posB);
		m_PagedGeometry->reloadGeometryPages(bounds);
	}
}


