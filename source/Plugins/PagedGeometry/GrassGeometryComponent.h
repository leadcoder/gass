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



#ifndef GRASS_GEOMETRY_COMPONENT_H
#define GRASS_GEOMETRY_COMPONENT_H

#include "Sim/GASSCommon.h"
#include "PagedGeometry.h"
#include "Sim/GASSBaseSceneComponent.h"
#include "Sim/Messages/GASSCoreSceneObjectMessages.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Interface/GASSIGeometryComponent.h"
#include "Sim/Interface/GASSITerrainComponent.h"
#include "Sim/Interface/GASSICollisionSceneManager.h"

#include "Core/MessageSystem/GASSIMessage.h"
#include <OgreRenderTargetListener.h>
#include "Plugins/Ogre/GASSOgreMaterial.h"
#include "PGMessages.h"

namespace Forests
{
	class PagedGeometry;
	class GrassLoader;
	class GrassLayer;
	class GeometryPageManager;
}

using namespace Forests;

namespace GASS
{
	class IHeightmapTerrainComponent;

	class GrassGeometryComponent : public Reflection<GrassGeometryComponent,BaseSceneComponent> , public Ogre::RenderTargetListener
	{
	public:
		GrassGeometryComponent(void);
		~GrassGeometryComponent(void);
		static void RegisterReflection();
		virtual void OnInitialize();
		virtual void OnDelete();
		virtual void preViewportUpdate(const Ogre::RenderTargetViewportEvent& evt);
		virtual void SaveXML(tinyxml2::XMLElement *obj_elem);
	protected:
		void OnPaint(GrassPaintMessagePtr message);
		std::string GetDensityMap() const;
		void SetDensityMap(const std::string &dm);
		float GetDensityFactor() const;
		void SetDensityFactor(float factor);
		float GetPageSize() const;
		void SetPageSize(float size);
		float GetImposterAlphaRejectionValue() const;
		void SetImposterAlphaRejectionValue(float value);
		Vec4 GetBounds() const;
		void SetBounds(const Vec4 &bounds);
		std::string GetColorMap() const;
		void SetColorMap(const std::string &name);
		OgreMaterial GetMaterial() const;
		void SetMaterial(const OgreMaterial &name);
		std::string GetFadeTech() const;
		void SetFadeTech(const std::string &tech);
		std::string GetRenderTechnique() const;
		void SetRenderTechnique(const std::string &tech);
		bool GetBlendWithGround() const;
		void SetBlendWithGround(bool value);
		Vec2 GetMaxSize() const;
		void SetMaxSize(const Vec2 &size);
		Vec2 GetMinSize() const;
		void SetMinSize(const Vec2 &size);
		float GetSwaySpeed()const;
		void SetSwaySpeed(float speed);
		float GetSwayLength() const;
		void SetSwayLength(float length);
		bool GetEnableSway() const;
		void SetEnableSway(bool value);
		float GetSwayDistribution() const;
		void SetSwayDistribution(float distribution);
		float GetViewDistance() const;
		void SetViewDistance(float distance);
		float GetCollisionSystemHeight(float x, float z);
		HeightmapTerrainComponentPtr GetTerrainComponent(SceneObjectPtr obj);
		static float GetTerrainHeight(float x, float z, void* user_data);
		void UpdateSway();

		std::string m_ColorMapFilename;
		OgreMaterial m_Material;
		GrassLoader* m_GrassLoader;
		Vec2 m_MaxSize;
		Vec2 m_MinSize;
		bool m_EnableSway;
		float m_SwaySpeed;
		float m_SwayLength;
		float m_SwayDistribution;
		std::string m_FadeTech;
		float m_ViewDist;
		bool m_Blend;
		std::string m_RenderTechnique;
		PagedGeometry *m_PagedGeometry;
		GrassLayer *m_GrassLayer;
		TBounds m_MapBounds;
		float m_PageSize;
		float m_DensityFactor;
		std::string m_DensityMapFilename;
		Vec4 m_Bounds;
		float m_ImposterAlphaRejectionValue;
		static IHeightmapTerrainComponent *m_Terrain;
		static ICollisionSceneManager* m_CollisionSM;
		Ogre::TexturePtr m_DensityTexture;
		Ogre::Image m_DensityImage;
		GeometryPageManager* m_LOD0;
		void update();
	};
}

#endif
