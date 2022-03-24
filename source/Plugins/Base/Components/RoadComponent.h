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

#pragma once
#include "Core/Common.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/Math/GASSVector.h"
#include "Sim/GASSComponent.h"
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
		static NameEnumMap InitMapping()
		{
			NameEnumMap mapping;
			mapping["TERRAIN_LAYER_0"] = TL_0;
			mapping["TERRAIN_LAYER_1"] = TL_1;
			mapping["TERRAIN_LAYER_2"] = TL_2;
			mapping["TERRAIN_LAYER_3"] = TL_3;
			mapping["TERRAIN_LAYER_4"] = TL_4;
			return mapping;
		}
	};

	/**
		Component that create road geomtry
	*/

	class RoadComponent : public Reflection<RoadComponent,Component>
	{
	public:
		RoadComponent();
		~RoadComponent() override;
		static void RegisterReflection();
		void OnInitialize() override;
	protected:
		float m_CustomDitchTexturePercent{0};
		bool m_CAP{false};
		bool m_FadeStart{false};
		bool m_FadeEnd{false};
		
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
		void OnUpdate(WaypointListUpdatedMessagePtr message);

		bool m_Initialized{false};
		bool m_UseSkirts{false};
		bool m_ClampToTerrain{true};
		float m_TerrainPaintIntensity{0.01f};
		float m_TerrainPaintWidth{20};
		float m_TerrainFlattenWidth{30};
		float m_RoadWidth{10};
		float m_RoadOffset{0.3f};
		float m_DitchWidth{1};
		std::string m_Material;
		Vec2 m_TileScale;
		TerrainLayerBinder m_TerrainPaintLayer;
	};

	using RoadComponentPtr = std::shared_ptr<RoadComponent>;
	using RoadComponentWeakPtr = std::weak_ptr<RoadComponent>;

}

