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
#ifndef TREE_GEOMETRY_COMPONENT_H
#define TREE_GEOMETRY_COMPONENT_H
#include "Sim/GASSCommon.h"
#include "Sim/GASSBaseSceneComponent.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "PGMessages.h"
#include "PGIncludes.h"


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
	class IHeightmapTerrainComponent;
	class DensityMapComponent;
	typedef GASS_SHARED_PTR<DensityMapComponent> DensityMapComponentPtr;


	class TreeGeometryComponent : public Reflection<TreeGeometryComponent,BaseSceneComponent>  , public Ogre::RenderTargetListener
	{
	public:
		TreeGeometryComponent(void);
		~TreeGeometryComponent(void) override;
		static void RegisterReflection();
		void OnInitialize() override;
		void OnDelete() override;
		void UpdateArea(float start_x, float start_z, float end_x, float end_z);
		void UpdateArea(const Vec3 &world_pos, float radius);
	protected:
		void preViewportUpdate(const Ogre::RenderTargetViewportEvent& evt) override;
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

		Vec4f GetCustomBounds() const
		{
			return m_CustomBounds;
		}

		void SetCustomBounds(const Vec4f &value)
		{
			m_CustomBounds = value;
		}


		Vec2f GetMaxMinScale() const
		{
			return m_MaxMinScale;
		}

		void SetMaxMinScale(const Vec2f &value)
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
		

		//expose impostor regeneration for gui
		void SetRegenerateAllImpostors(bool value);
		bool GetRegenerateAllImpostors() const;

		//support dynamic lighting
		void SetDynamicImpostorLighting(bool value);
		bool GetDynamicImpostorLighting() const;
		
		DensityMapComponentPtr m_DensityMap;
		//Ogre::PixelBox *m_DensityMap;
		RandomTable* m_RandomTable;
		Vec2f m_MaxMinScale;
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
		Vec4f m_CustomBounds;
		PagedGeometry *m_PagedGeometry;
		TBounds m_MapBounds;
		float m_ImposterAlphaRejectionValue;
		std::string m_MeshFileName;
		static IHeightmapTerrainComponent *m_Terrain;
		int m_ImposterResolution;

		TreeLoader2D *m_TreeLoader2d;
		TreeLoader3D *m_TreeLoader3d;

		Ogre::Entity *m_TreeEntity;
		bool m_DynamicImpostorLighting;
		Ogre::SceneManager* m_SceneMan;
	};
	typedef GASS_SHARED_PTR<TreeGeometryComponent> TreeGeometryComponentPtr;
}

#endif
