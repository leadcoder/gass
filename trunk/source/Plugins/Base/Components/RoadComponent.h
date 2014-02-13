/****************************************************************************
* This file is part of GASS.                                                *
* See http://code.google.com/p/gass/                                 *
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

#pragma once
#include "Core/Common.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/Math/GASSVector.h"
#include "Sim/GASSCommon.h"
#include "Sim/Interface/GASSICameraComponent.h"
#include "Sim/GASSBaseSceneComponent.h"
#include "Sim/GASSBaseSceneComponent.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Plugins/Base/CoreMessages.h"
#include "Core/Utils/GASSEnumBinder.h"

namespace GASS
{

	class TerrainLayerBinder : public SingleEnumBinder<TerrainLayer,TerrainLayerBinder>
	{
	public:
		TerrainLayerBinder(){}
		TerrainLayerBinder(TerrainLayer type) : SingleEnumBinder<TerrainLayer,TerrainLayerBinder>(type)
		{
		}
		virtual ~TerrainLayerBinder(){}
		static void Register()
		{
			Bind("TERRAIN_LAYER_0", TL_0);
			Bind("TERRAIN_LAYER_1", TL_1);
			Bind("TERRAIN_LAYER_2", TL_2);
			Bind("TERRAIN_LAYER_3", TL_3);
			Bind("TERRAIN_LAYER_4", TL_4);
		}
	};

	/**
		Component that create road geomtry
	*/

	class RoadComponent : public Reflection<RoadComponent,BaseSceneComponent>
	{
	public:
		RoadComponent();
		virtual ~RoadComponent();
		static void RegisterReflection();
		virtual void OnInitialize();
	protected:
		ADD_PROPERTY(float,CustomDitchTexturePercent)
		ADD_PROPERTY(bool,CAP)
		ADD_PROPERTY(bool,FadeStart)
		ADD_PROPERTY(bool,FadeEnd)
		
		void SetFlattenTerrain(bool value);
		bool GetFlattenTerrain() const;

		void SetPaintTerrain(bool value);
		bool GetPaintTerrain() const;

		void SetUseSkirts(bool value) {m_UseSkirts = value; UpdateRoadMesh();}
		bool GetUseSkirts() const {return m_UseSkirts;}

		void SetClampToTerrain(bool value) {m_ClampToTerrain = value; UpdateRoadMesh();}
		bool GetClampToTerrain() const {return m_ClampToTerrain;}

		void SetTerrainFlattenWidth(float value) {m_TerrainFlattenWidth = value; }
		float GetTerrainFlattenWidth() const {return m_TerrainFlattenWidth;}

		void SetTerrainPaintWidth(float value) {m_TerrainPaintWidth = value; }
		float GetTerrainPaintWidth() const {return m_TerrainPaintWidth;}

		void SetTerrainPaintIntensity(float value) {m_TerrainPaintIntensity = value;}
		float GetTerrainPaintIntensity() const {return m_TerrainPaintIntensity;}

		void SetRoadWidth(float value) {m_RoadWidth = value;UpdateRoadMesh();}
		float GetRoadWidth() const {return m_RoadWidth;}

		void SetRoadOffset(float value) {m_RoadOffset = value;UpdateRoadMesh();}
		float GetRoadOffset() const {return m_RoadOffset;}

		void SetDitchWidth(float value) {m_DitchWidth = value;UpdateRoadMesh();}
		float GetDitchWidth() const {return m_DitchWidth;}

		void SetMaterial(const std::string &value);
		std::string GetMaterial() const;

		Vec2 GetTileScale() const {return m_TileScale;}
		void SetTileScale(const Vec2 &value) {m_TileScale = value;}
		
		void SetTerrainPaintLayer(TerrainLayerBinder value) {m_TerrainPaintLayer = value;}
		TerrainLayerBinder GetTerrainPaintLayer() const {return m_TerrainPaintLayer;}
		void UpdateRoadMesh();
		void OnUpdate(UpdateWaypointListMessagePtr message);

		bool m_Initialized;
		bool m_UseSkirts;
		bool m_ClampToTerrain;
		float m_TerrainPaintIntensity;
		float m_TerrainPaintWidth;
		float m_TerrainFlattenWidth;
		float m_RoadWidth;
		float m_RoadOffset;
		float m_DitchWidth;
		std::string m_Material;
		Vec2 m_TileScale;
		TerrainLayerBinder m_TerrainPaintLayer;
	};

	typedef SPTR<RoadComponent> RoadComponentPtr;
	typedef WPTR<RoadComponent> RoadComponentWeakPtr;

}

