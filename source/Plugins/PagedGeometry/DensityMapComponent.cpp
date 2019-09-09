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

#include "DensityMapComponent.h"
#include "Sim/Interface/GASSITerrainComponent.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSScene.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/ComponentSystem/GASSComponent.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "TreeGeometryComponent.h"

namespace GASS
{
	void DensityMapComponent::RegisterReflection()
	{
		std::vector<std::string> ext;
		ext.push_back("png");
		ext.push_back("tga");
		
		ComponentFactory::Get().Register<DensityMapComponent>();
		GetClassRTTI()->SetMetaData(ClassMetaDataPtr(new ClassMetaData("DensityMapComponent", OF_VISIBLE)));
		RegisterGetSet("DensityMap", &DensityMapComponent::GetDensityMap, &DensityMapComponent::SetDensityMap,PF_VISIBLE | PF_EDITABLE,"");
		RegisterProperty<FilePath>("Import", &DensityMapComponent::GetImport, &DensityMapComponent::SetImport,
			FilePathPropertyMetaDataPtr(new FilePathPropertyMetaData("Import density map",PF_VISIBLE | PF_EDITABLE, FilePathPropertyMetaData::IMPORT_FILE, ext)));
	}

	void DensityMapComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(DensityMapComponent::OnPaint,GrassPaintMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(DensityMapComponent::OnRoadMessage,RoadRequest,0));

		ScenePtr  scene = GetSceneObject()->GetScene();
		std::string scene_path = scene->GetSceneFolder().GetFullPath();

		std::string denmapname;
		if(m_DensityMapFilename != "")
		{
			denmapname = m_DensityMapFilename;
		}
		else
			denmapname = "mesh_density_map_" + GetName() + ".tga";
		const std::string fp_denmap = scene_path + "/" + denmapname;
		std::fstream fstr(fp_denmap.c_str(), std::ios::in|std::ios::binary);
		Ogre::DataStreamPtr stream = Ogre::DataStreamPtr(OGRE_NEW Ogre::FileStreamDataStream(&fstr, false));
		try
		{
			m_DensityImage.load(stream);
		}
		catch(...)
		{
			int densize = 1024;
			Ogre::uchar *data = OGRE_ALLOC_T(Ogre::uchar, densize * densize * 4, Ogre::MEMCATEGORY_GENERAL);
			memset(data, 0, densize * densize * 4);
			m_DensityImage.loadDynamicImage(data, densize, densize, 1, Ogre::PF_A8R8G8B8, true);
			m_DensityImage.save(fp_denmap);
		}
		stream.setNull();

		//if(m_DensityMapFilename != "")
		//	LoadDensityMap(m_DensityMapFilename,CHANNEL_COLOR);
	}





	DensityMapComponent::DensityMapComponent(void)// : m_DensityMap(NULL)
	{

	}

	DensityMapComponent::~DensityMapComponent(void)
	{

	}


	void DensityMapComponent::SetImport(const FilePath &dm)
	{
		std::fstream fstr(dm.GetFullPath().c_str(), std::ios::in|std::ios::binary);
		Ogre::DataStreamPtr stream = Ogre::DataStreamPtr(OGRE_NEW Ogre::FileStreamDataStream(&fstr, false));
		try
		{
			m_DensityImage.load(stream);
		}
		catch(...)
		{
			return;	
		}

		ComponentContainer::ComponentVector components;
		GetSceneObject()->GetComponentsByClassName(components, "TreeGeometryComponent", true);
		for(int i = 0 ;  i < components.size(); i++)
		{
			TreeGeometryComponentPtr trees = GASS_DYNAMIC_PTR_CAST<TreeGeometryComponent>(components[i]);
			trees->UpdateArea(m_MapBounds.left, m_MapBounds.top,m_MapBounds.right, m_MapBounds.bottom);
		}
	}

	void DensityMapComponent::OnPaint(GrassPaintMessagePtr message)
	{
		Paint(message->GetPosition(), message->GetBrushSize(), message->GetBrushInnerSize(), message->GetIntensity());
	}

	void DensityMapComponent::Paint(const Vec3 &world_pos, float brush_size, float brush_inner_size , float intensity)
	{
		Ogre::uchar *data = static_cast<Ogre::uchar*>(m_DensityImage.getData());
		int wsize = static_cast<int>(m_DensityImage.getWidth())-1;

		const Ogre::Real height = m_MapBounds.height();
		const Ogre::Real width = m_MapBounds.width();
		const Ogre::Real x_pos = (static_cast<Ogre::Real>(world_pos.x) - m_MapBounds.left)/width;
		const Ogre::Real y_pos = (static_cast<Ogre::Real>(world_pos.z) - m_MapBounds.top)/height;

		const Ogre::Real brush_size_texture_space_x = brush_size/width;
		const Ogre::Real brush_size_texture_space_y = brush_size/height;
		const Ogre::Real brush_inner_radius = (brush_inner_size*0.5f)/height;

		long startx = static_cast<long >((x_pos - brush_size_texture_space_x) * (wsize));
		long starty = static_cast<long >((y_pos - brush_size_texture_space_y) * (wsize));
		long endx = static_cast<long >((x_pos + brush_size_texture_space_x) * (wsize));
		long endy= static_cast<long >((y_pos + brush_size_texture_space_y) * (wsize));
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
				weight = 1.0f - (weight * weight);
				//weight = 1;

				float val = float(data[(tmploc + x)*4])/255.0f;
				val += weight*intensity*3;
				//val = std::min(val, 255.0f);
				//val = std::max(val, 0.0f);
				if(val > 1.0)
					val = 1;
				if(val < 0.0)
					val = 0;
				data[(tmploc + x)*4] = static_cast<Ogre::uchar>(val*255);
			}
		}

	}


	float DensityMapComponent::GetDensityAt(float x, float z)
	{
		//assert(m_DensityMap);

		int mapWidth = (unsigned int)m_DensityImage.getWidth();
		int mapHeight = (int)m_DensityImage.getHeight();
		float boundsWidth = m_MapBounds.width();
		float boundsHeight = m_MapBounds.height();

		int xindex = static_cast<int>(static_cast<float>(mapWidth) * (x - static_cast<float>(m_MapBounds.left)) / boundsWidth);
		int zindex = static_cast<int>(static_cast<float>(mapHeight) * (z - static_cast<float>(m_MapBounds.top)) / boundsHeight);
		if (xindex < 0 || zindex < 0 || xindex >= mapWidth || zindex >= mapHeight)
			return 0.0f;


		//Ogre::uint8 *data = (Ogre::uint8*)m_DensityMap->data;
		Ogre::uint8 *data = m_DensityImage.getData();
		float val = data[(mapWidth * zindex + xindex)*4] / 255.0f;
		return val;
	}


	void DensityMapComponent::OnRoadMessage(RoadRequestPtr message)
	{

		ComponentContainer::ComponentVector components;
		GetSceneObject()->GetComponentsByClassName(components, "TreeGeometryComponent", true);

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
				{
					Paint(pos, message->GetPaintWidth(),message->GetPaintWidth(), -10);
					for(int j = 0 ;  j < components.size(); j++)
					{
						TreeGeometryComponentPtr trees = GASS_DYNAMIC_PTR_CAST<TreeGeometryComponent>(components[j]);
						trees->UpdateArea(pos, message->GetPaintWidth());
					}
				}
			}
		}
	}


	void DensityMapComponent::SaveXML(tinyxml2::XMLElement *obj_elem)
	{
		BaseSceneComponent::SaveXML(obj_elem);

		ScenePtr  scene = GetSceneObject()->GetScene();
		std::string scene_path = scene->GetSceneFolder().GetFullPath();
		std::string denmapname;
		if(m_DensityMapFilename != "")
		{
			denmapname = m_DensityMapFilename;
		}
		else
			denmapname = "mesh_density_map_" + GetName() + ".tga";
		const std::string fp_denmap = scene_path + "/" + denmapname;
		m_DensityImage.save(fp_denmap);
	}
}


