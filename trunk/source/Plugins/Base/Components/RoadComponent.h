/****************************************************************************
*                                                                           *
* GASS                                                                *
* Copyright (C)2003 - 2005 Johan Hedstrom                                   *
* Email: GASS@gmail.com                                               *
* Web page: http://n00b.dyndns.org/GASS                               *
*                                                                           *
* GASS is only used with knowledge from the author. This software     *
* is not allowed to redistribute without permission from the author.        *
* For further license information, please turn to the product home page or  *
* contact author. Abuse against the GASS license is prohibited by law.*
*                                                                           *
*****************************************************************************/ 

#pragma once
#include "Core/Common.h"
#include "Core/MessageSystem/IMessage.h"
#include "Core/Math/Vector.h"
#include "Sim/Common.h"
#include "Sim/Components/Graphics/ICameraComponent.h"
#include "Sim/Components/Graphics/Geometry/ILineComponent.h"
#include "Sim/Components/BaseSceneComponent.h"
#include "Sim/Components/BaseSceneComponent.h"
#include "Sim/Scenario/Scene/SceneObject.h"
#include "Sim/Scheduling/ITaskListener.h"
#include "Plugins/Base/CoreMessages.h"
#include "Sim/Utils/EnumBinder.h"

namespace GASS
{

	class TerrainLayerBinder : public EnumBinder<TerrainLayer,TerrainLayerBinder>
	{
	public:
		TerrainLayerBinder(){}
		TerrainLayerBinder(TerrainLayer type) : EnumBinder<TerrainLayer,TerrainLayerBinder>(type)
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

	class RoadComponent : public Reflection<RoadComponent,BaseSceneComponent>
	{
	public:
		RoadComponent();
		virtual ~RoadComponent();
		static void RegisterReflection();
		virtual void OnCreate();
	protected:
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

		void SetMaterial(const std::string value);
		std::string GetMaterial() const;

		
		void SetTerrainPaintLayer(TerrainLayerBinder value) {m_TerrainPaintLayer = value;}
		TerrainLayerBinder GetTerrainPaintLayer() const {return m_TerrainPaintLayer;}

		

		void UpdateRoadMesh();
		void OnUnload(UnloadComponentsMessagePtr message);	
		void OnLoad(LoadCoreComponentsMessagePtr message);
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

		TerrainLayerBinder m_TerrainPaintLayer;

	};

	typedef boost::shared_ptr<RoadComponent> RoadComponentPtr;
	typedef boost::weak_ptr<RoadComponent> RoadComponentWeakPtr;

}
