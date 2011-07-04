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
#ifndef TREE_GEOMETRY_COMPONENT_H
#define TREE_GEOMETRY_COMPONENT_H

#include "PagedGeometry.h"
#include "Sim/Components/BaseSceneComponent.h"
#include "Sim/Scenario/Scene/SceneObjectMessages.h"
#include "Sim/Components/Graphics/Geometry/IGeometryComponent.h"
#include "Core/MessageSystem/IMessage.h"
#include "PGMessages.h"
#include <OgreRenderTargetListener.h>
namespace Ogre
{
    class PixelBox;
}
namespace Forests
{
	class PagedGeometry;
	class TreeLoader2D;
	class TreeLoader3D;

}
class RandomTable;

using namespace Forests;


namespace GASS
{
	class ITerrainComponent;
	class DensityMapComponent;
	typedef boost::shared_ptr<DensityMapComponent> DensityMapComponentPtr;


	class TreeGeometryComponent : public Reflection<TreeGeometryComponent,BaseSceneComponent>  , public Ogre::RenderTargetListener
	{
	public:
		TreeGeometryComponent(void);
		~TreeGeometryComponent(void);
		static void RegisterReflection();
		virtual void OnCreate();
		void UpdateArea(Float start_x,Float start_z,Float end_x,Float end_z);
		void Paint(const Vec3 &world_pos, float brush_size, float brush_inner_size , float intensity);
	protected:
		virtual void preViewportUpdate(const Ogre::RenderTargetViewportEvent& evt);
		//void LoadDensityMap(const std::string &mapFile, int channel);
		//float GetDensityAt(float x, float z);

		void OnLoad(LoadGFXComponentsMessagePtr message);
		void OnUnload(UnloadComponentsMessagePtr message);
		static float GetTerrainHeight(float x, float z, void* user_data);
		std::string GetMesh() const
		{
			return m_MeshFileName;
		}

		void SetMesh(const std::string &mesh)
		{
			m_MeshFileName = mesh;
		}

		std::string GetColorMap() const
		{
			return m_ColorMapFilename;
		}

		void SetColorMap(const std::string &colormap)
		{
			m_ColorMapFilename = colormap;
		}

		std::string GetDensityMap() const
		{
			return m_DensityMapFilename;
		}

		void SetDensityMap(const std::string &colormap)
		{
			m_DensityMapFilename = colormap;
		}

		float GetDensityFactor() const
		{
			return m_DensityFactor;
		}

		void SetDensityFactor(float value)
		{
			m_DensityFactor = value;
		}

		float GetMeshDistance() const
		{
			return m_MeshDist;
		}

		void SetMeshDistance(float value)
		{
			m_MeshDist = value;
		}

		float GetMeshFadeDistance() const
		{
			return m_MeshFadeDist;
		}

		void SetMeshFadeDistance(float value)
		{
			m_MeshFadeDist = value;
		}


		float GetImposterDistance() const
		{
			return m_ImposterDist;
		}

		void SetImposterDistance(float value)
		{
			m_ImposterDist = value;
		}


		float GetImposterFadeDistance() const
		{
			return m_ImposterFadeDist;
		}

		void SetImposterFadeDistance(float value)
		{
			m_ImposterFadeDist = value;
		}

		Vec4 GetCustomBounds() const
		{
			return m_CustomBounds;
		}

		void SetCustomBounds(const Vec4 &value)
		{
			m_CustomBounds = value;
		}


		Vec2 GetMaxMinScale() const
		{
			return m_MaxMinScale;
		}

		void SetMaxMinScale(const Vec2 &value)
		{
			m_MaxMinScale = value;
		}

		bool GetCastShadows() const
		{
			return m_CastShadows;
		}

		void SetCastShadows(bool value)
		{
			m_CastShadows = value;
		}

		bool GetCreateShadowMap() const
		{
			return m_CreateShadowMap;
		}

		void SetCreateShadowMap(bool value)
		{
			m_CreateShadowMap = value;
		}

		bool GetPrecalcHeight() const
		{
			return m_PrecalcHeight;
		}

		void SetPrecalcHeight(bool value)
		{
			m_PrecalcHeight = value;
		}

		float GetPageSize() const
		{
			return m_PageSize;
		}

		void SetPageSize(float size)
		{
			m_PageSize = size;

		}
		float GetImposterAlphaRejectionValue() const
		{
			return m_ImposterAlphaRejectionValue;
		}

		void SetImposterAlphaRejectionValue(float value)
		{
			m_ImposterAlphaRejectionValue =value;
		}

		int GetImposterResolution() const
		{
			return m_ImposterResolution;
		}

		void SetImposterResolution(int value)
		{
			m_ImposterResolution=value;
		}
		

		void OnPaint(GrassPaintMessagePtr message);
		

		DensityMapComponentPtr m_DensityMap;
		//Ogre::PixelBox *m_DensityMap;
		RandomTable* m_RandomTable;
		Vec2 m_MaxMinScale;
		bool m_CastShadows;
		float m_MeshDist;
		float m_ImposterDist;
		float m_MeshFadeDist;
		float m_ImposterFadeDist;
		bool m_PrecalcHeight;
		std::string m_ColorMapFilename;
		std::string m_DensityMapFilename;
		float m_DensityFactor;
		bool m_CreateShadowMap;
		float m_PageSize;
		Vec4 m_CustomBounds;
		PagedGeometry *m_PagedGeometry;
		TBounds m_MapBounds;
		float m_ImposterAlphaRejectionValue;
		std::string m_MeshFileName;
		static ITerrainComponent *m_Terrain;
		int m_ImposterResolution;

		TreeLoader2D *m_TreeLoader2d;
		TreeLoader3D *m_TreeLoader3d;

		Ogre::Entity *m_TreeEntity;
	};
	typedef boost::shared_ptr<TreeGeometryComponent> TreeGeometryComponentPtr;
}

#endif
