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



#ifndef GRASS_LOADER_COMPONENT_H
#define GRASS_LOADER_COMPONENT_H

#include "Sim/GASSCommon.h"

#if defined(_MSC_VER)
	#pragma warning(push)
	#pragma warning(disable : 4244)
#endif
#include "PagedGeometry.h"
#if defined(_MSC_VER)
	#pragma warning(pop)
#endif

#include "Sim/GASSBaseSceneComponent.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Interface/GASSITerrainComponent.h"
#include "Sim/Interface/GASSICollisionSceneManager.h"
#include <OgreRenderTargetListener.h>
#include "PGMessages.h"

namespace Forests
{
	class PagedGeometry;
	class GrassLoader;
	class GrassLayer;
	class GeometryPageManager;
	class DensityMap;
}

using namespace Forests;

namespace GASS
{
	
	class GrassLoaderComponent : public Reflection<GrassLoaderComponent,BaseSceneComponent> , public Ogre::RenderTargetListener
	{
	public:
		GrassLoaderComponent(void);
		~GrassLoaderComponent(void);
		
		static void RegisterReflection();
		virtual void OnInitialize();
		virtual void OnDelete();

		virtual void preViewportUpdate(const Ogre::RenderTargetViewportEvent& evt);
		virtual void SaveXML(tinyxml2::XMLElement *obj_elem);
		GrassLoader* GetGrassLoader() const {return m_GrassLoader;}
		TBounds GetMapBounds() const {return m_MapBounds;}
		Ogre::TexturePtr GetDensityTexture() const {return  m_DensityTexture;}
		std::string GetColorMap() const;
		void ReloadGeometry();
	protected:
		void SaveDensityMap();
		void OnRoadMessage(RoadRequestPtr message);
		std::string GetDensityMap() const;
		void SetDensityMap(const std::string &dm);
		float GetPageSize() const;
		void SetPageSize(float size);
		Vec4f GetCustomBounds() const;
		void SetCustomBounds(const Vec4f &bounds);
		void SetColorMap(const std::string &name);
		float GetViewDistance() const;
		void SetViewDistance(float distance);
		void SetImportDensityMap(const std::string &dm);
		std::string GetImportDensityMap() const;
protected:
		float GetCollisionSystemHeight(float x, float z);
		static float GetTerrainHeight(float x, float z, void* user_data);
		void OnPaint(GrassPaintMessagePtr message);
		void Paint(const Vec3 &world_pos, float brush_size, float brush_inner_size , float intensity);
		
		std::string m_ColorMapFilename;
		GrassLoader* m_GrassLoader;
		float m_ViewDist;
		PagedGeometry *m_PagedGeometry;
		TBounds m_MapBounds;
		float m_PageSize;
		std::string m_DensityMapFilename;
		Vec4f m_CustomBounds;
		static IHeightmapTerrainComponent *m_Terrain;
		static ICollisionSceneManager* m_CollisionSM;
		Ogre::TexturePtr m_DensityTexture;
		Ogre::Image m_DensityImage;
		GeometryPageManager* m_LOD0;
		Forests::DensityMap *m_DensityMap;
		int m_DensityMapSize;
		Ogre::SceneManager* m_SceneMan;
	};
	typedef GASS_SHARED_PTR<GrassLoaderComponent> GrassLoaderComponentPtr;
}

#endif
