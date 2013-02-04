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



#ifndef GRASS_GEOMETRY_COMPONENT_H
#define GRASS_GEOMETRY_COMPONENT_H


#include <PagedGeometry.h>
#include <GrassLoader.h>
#include <OgreRenderTargetListener.h>
#include "Sim/GASSBaseSceneComponent.h"
#include "Sim/Messages/GASSCoreSceneObjectMessages.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Interface/GASSIGeometryComponent.h"
#include "Sim/Interface/GASSITerrainComponent.h"
#include "Sim/Interface/GASSICollisionSceneManager.h"
#include "Plugins/Ogre/OgreMaterial.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "GrassMaterial.h"
#include "PGMessages.h"

namespace Forests
{
	class GrassLayer;
	class GeometryPageManager;
}

using namespace Forests;

namespace GASS
{
	class ITerrainComponent;

	START_ENUM_BINDER(FadeTechnique,FadeTechniqueBinder)
		BIND(FADETECH_ALPHAGROW);
		BIND(FADETECH_ALPHA);
		BIND(FADETECH_GROW);
	END_ENUM_BINDER(FadeTechnique,FadeTechniqueBinder);

	START_ENUM_BINDER(GrassTechnique,RenderTechniqueBinder)
		BIND(GRASSTECH_QUAD);
		BIND(GRASSTECH_CROSSQUADS);
		BIND(GRASSTECH_SPRITE);
	END_ENUM_BINDER(GrassTechnique,RenderTechniqueBinder);

	class GrassLayerComponent : public Reflection<GrassLayerComponent,BaseSceneComponent> , public Ogre::RenderTargetListener
	{
	public:
		GrassLayerComponent(void);
		~GrassLayerComponent(void);
		static void RegisterReflection();
		virtual void OnInitialize();
		GrassLayer *GetLayer() const {return m_GrassLayer;}
	protected:
		void OnLoad(GrassLoaderComponentLoadedPtr message);
		void OnDelete();
		void OnPaint(GrassPaintMessagePtr message);
		void OnRoadMessage(RoadMessagePtr message);
		void Paint(const Vec3 &world_pos, float brush_size, float brush_inner_size , float intensity);
		std::string GetDensityMap() const;
		void SetDensityMap(const std::string &dm);
		float GetDensityFactor() const;
		void SetDensityFactor(float factor);
		std::string GetColorMap() const;
		void SetColorMap(const std::string &name);
		OgreMaterial GetMaterial() const;
		void SetMaterial(const OgreMaterial &name);
		FadeTechniqueBinder GetFadeTechnique() const;
		void SetFadeTechnique(const FadeTechniqueBinder &tech);
		RenderTechniqueBinder GetRenderTechnique() const;
		void SetRenderTechnique(const RenderTechniqueBinder &tech);
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
protected:
		
		float GetCollisionSystemHeight(float x, float z);
		HeightmapTerrainComponentPtr GetTerrainComponent(SceneObjectPtr obj);
		static float GetTerrainHeight(float x, float z, void* user_data);
		void UpdateSway();
		void update();
		std::string m_ColorMapFilename;
		std::string m_DensityMapFilename;
		
		OgreMaterial m_Material;
		GrassLoader* m_GrassLoader;
		
		Vec2 m_MaxSize;
		Vec2 m_MinSize;
		bool m_EnableSway;
		float m_SwaySpeed;
		float m_SwayLength;
		float m_SwayDistribution;
		FadeTechniqueBinder m_FadeTechnique;
		bool m_Blend;
		RenderTechniqueBinder m_RenderTechnique;
		GrassLayer *m_GrassLayer;
		float m_DensityFactor;
		
		float m_ImposterAlphaRejectionValue;
		static IHeightmapTerrainComponent *m_Terrain;
	};

	typedef SPTR<GrassLayerComponent> GrassLayerComponentPtr;
}

#endif
