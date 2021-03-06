/****************************************************************************
* This file is part of GASS.                                                *
* See https://github.com/leadcoder/gass                                     *
*                                                                           *
* Copyright (c) 2008-2016 GASS team. See Contributors.txt for details.      *
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

#include "GrassLoaderComponent.h"
#include "GrassLayerComponent.h"
#include "ImpostorPage.h"
#include "GrassLoader.h"
#include "Sim/Interface/GASSITerrainComponent.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/Utils/GASSCollisionHelper.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/ComponentSystem/GASSComponent.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/Utils/GASSException.h"
#include "Plugins/Ogre/GASSIOgreSceneManagerProxy.h"

namespace GASS
{
	IHeightmapTerrainComponent* GrassLoaderComponent::m_Terrain = NULL;

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

		ComponentFactory::GetPtr()->Register<GrassLoaderComponent>();
		GetClassRTTI()->SetMetaData(ClassMetaDataPtr(new ClassMetaData("GrassLoaderComponent", OF_VISIBLE)));
		RegisterGetSet("DensityMap", &GrassLoaderComponent::GetDensityMap, &GrassLoaderComponent::SetDensityMap,PF_VISIBLE,"Can only be specified in template");
		RegisterGetSet("ImportDensityMap", &GrassLoaderComponent::GetImportDensityMap, &GrassLoaderComponent::SetImportDensityMap, PF_VISIBLE | PF_EDITABLE,"Import density map",
			std::make_shared<FilePathPropertyMetaData>(FilePathPropertyMetaData::IMPORT_FILE, ext));
		RegisterGetSet("PageSize", &GrassLoaderComponent::GetPageSize, &GrassLoaderComponent::SetPageSize,PF_VISIBLE | PF_EDITABLE,"You need to reload scene before this property take effect");
		RegisterGetSet("CustomBounds", &GrassLoaderComponent::GetCustomBounds, &GrassLoaderComponent::SetCustomBounds,PF_VISIBLE,"");
		RegisterGetSet("ColorMap", &GrassLoaderComponent::GetColorMap, &GrassLoaderComponent::SetColorMap,PF_VISIBLE,"");
		RegisterGetSet("ViewDistance", &GrassLoaderComponent::GetViewDistance, &GrassLoaderComponent::SetViewDistance,PF_VISIBLE | PF_EDITABLE,"");
	}

	void GrassLoaderComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(GrassLoaderComponent::OnPaint,GrassPaintMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(GrassLoaderComponent::OnRoadMessage,RoadRequest,0));
	
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

		if(!user_bounds)
		{
			HeightmapTerrainComponentPtr terrain = GetSceneObject()->GetScene()->GetRootSceneObject()->GetFirstComponentByClass<IHeightmapTerrainComponent>(true);
			
			if(terrain)
			{
				GeometryComponentPtr geom = GASS_DYNAMIC_PTR_CAST<IGeometryComponent>(terrain);
				AABox aabox = geom->GetBoundingBox();
				m_MapBounds = TBounds(static_cast<float>(aabox.Min.x), 
									  static_cast<float>(aabox.Min.z),
									  static_cast<float>(aabox.Max.x),
									  static_cast<float>(aabox.Max.z));
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
				
#if (OGRE_19_RC1)
				m_DensityTexture = Ogre::TextureManager::getSingletonPtr()->createOrRetrieve(denmapname, GetSceneObject()->GetScene()->GetResourceGroupName()).first;
#else
				m_DensityTexture = Ogre::TextureManager::getSingletonPtr()->createOrRetrieve(denmapname, GetSceneObject()->GetScene()->GetResourceGroupName()).first.staticCast<Ogre::Texture>();
				//m_DensityTexture = Ogre::TextureManager::getSingletonPtr()->createOrRetrieve(denmapname, GetSceneObject()->GetScene()->GetResourceGroupName()).first;
#endif

			}
			else
			{
				Ogre::uchar *data = OGRE_ALLOC_T(Ogre::uchar, m_DensityMapSize * m_DensityMapSize * 4, Ogre::MEMCATEGORY_GENERAL);
				memset(data, 0, m_DensityMapSize * m_DensityMapSize * 4);
				m_DensityImage.loadDynamicImage(data, m_DensityMapSize, m_DensityMapSize, 1, Ogre::PF_A8R8G8B8, true);
#if (OGRE_19_RC1)
				m_DensityTexture = Ogre::TextureManager::getSingletonPtr()->createOrRetrieve(denmapname,GetSceneObject()->GetScene()->GetResourceGroupName()).first;
#else
				m_DensityTexture = Ogre::TextureManager::getSingletonPtr()->createOrRetrieve(denmapname,GetSceneObject()->GetScene()->GetResourceGroupName()).first.staticCast<Ogre::Texture>();
				//m_DensityTexture = Ogre::TextureManager::getSingletonPtr()->createOrRetrieve(denmapname,GetSceneObject()->GetScene()->GetResourceGroupName()).first;
#endif
				
			
				m_DensityTexture->loadImage(m_DensityImage);
			}
		}
		GetSceneObject()->PostEvent(GrassLoaderComponentLoadedPtr(new GrassLoaderComponentLoaded()));
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
		ComponentContainer::ComponentVector components;
		GetSceneObject()->GetComponentsByClassName(components, "GrassLayerComponent", true);
		for(int i = 0 ;  i < components.size(); i++)
		{
			GrassLayerComponentPtr layer = GASS_DYNAMIC_PTR_CAST<GrassLayerComponent>(components[i]);
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

	Vec4f GrassLoaderComponent::GetCustomBounds() const
	{
		return m_CustomBounds;
	}

	void GrassLoaderComponent::SetCustomBounds(const Vec4f &bounds)
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

	

	void GrassLoaderComponent::SaveXML(tinyxml2::XMLElement *obj_elem)
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

	/*float GrassLoaderComponent::GetCollisionSystemHeight(float x, float z)
	{
		return static_cast<float>(CollisionHelper::GetHeightAtPosition(GetSceneObject()->GetScene(), GASS::Vec3(x, 0, z), GEOMETRY_FLAG_GROUND));
	}*/

	float GrassLoaderComponent::GetTerrainHeight(float x, float z, void* /*user_data*/)
	{
		if(m_Terrain)
			return static_cast<float>(m_Terrain->GetHeightAtWorldLocation(x,z));
		return 0;
		/*else
		{
			//GrassLoaderComponent* grass = static_cast<GrassLoaderComponent*> (user_data);
			//return grass->GetCollisionSystemHeight(x, z);
		}*/
	}

	void GrassLoaderComponent::preViewportUpdate(const Ogre::RenderTargetViewportEvent& evt)
	{
		Ogre::Viewport *vp = evt.source;
		m_PagedGeometry->update();
		if(vp && vp->getCamera() && m_SceneMan == vp->getCamera()->getSceneManager())
			m_PagedGeometry->setCamera(vp->getCamera());
	}

	void GrassLoaderComponent::OnRoadMessage(RoadRequestPtr message)
	{
		//Ogre::Real brush_size = message->GetPaintWidth();
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
		ComponentContainer::ComponentVector components;
		GetSceneObject()->GetComponentsByClassName(components, "GrassLayerComponent", true);
		for(int i = 0 ;  i < components.size(); i++)
		{
			GrassLayerComponentPtr layer = GASS_DYNAMIC_PTR_CAST<GrassLayerComponent>(components[i]);

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
		int wsize = static_cast<int>(m_DensityImage.getWidth())-1;

		const Ogre::Real height = m_MapBounds.height();
		const Ogre::Real width = m_MapBounds.width();
		const Ogre::Real x_pos = (static_cast<float>(world_pos.x) - m_MapBounds.left)/width;
		const Ogre::Real y_pos = (static_cast<float>(world_pos.z) - m_MapBounds.top)/height;

		const Ogre::Real brush_size_texture_space_x = brush_size/width;
		const Ogre::Real brush_size_texture_space_y = brush_size/height;
		const Ogre::Real brush_inner_radius = (brush_inner_size*0.5f)/height;

		long startx = static_cast<long>((x_pos - brush_size_texture_space_x) * (wsize));
		long starty = static_cast<long>((y_pos - brush_size_texture_space_y) * (wsize));
		long endx = static_cast<long>((x_pos + brush_size_texture_space_x) * (wsize));
		long endy= static_cast<long>((y_pos + brush_size_texture_space_y) * (wsize));
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
				weight = 1.0f - (weight * weight);
				//weight = 1;

				float val = float(data[((texture_loc + x)*4)])/255.0f;
				val += weight*intensity;
				//val = std::min(val, 255.0f);
				//val = std::max(val, 0.0f);
				if(val > 1.0)
					val = 1;
				if(val < 0.0)
					val = 0;
			
				data[((texture_loc + x)*4)] = static_cast<Ogre::uchar>(val*255);

				//also update all layers!
				for(int i = 0; i < layer_data.size(); i++)
				{
					Ogre::uchar *l_data = layer_data[i];
					l_data [denmap_loc+ x] = static_cast<Ogre::uchar>(val*255);
				}
			}
		}

		float posL = static_cast<float>(world_pos.x) - brush_size;
		float posT = static_cast<float>(world_pos.z) - brush_size;
		float posR = static_cast<float>(world_pos.x) + brush_size;
		float posB = static_cast<float>(world_pos.z) + brush_size;
		Forests::TBounds bounds(posL, posT, posR, posB);
		m_PagedGeometry->reloadGeometryPages(bounds);
	}
}


