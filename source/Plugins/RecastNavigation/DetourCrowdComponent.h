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
#include "Sim/GASS.h"
#include "Plugins/Base/CoreMessages.h"
#include "RecastNavigationMeshComponent.h"
#include "DetourCrowdAgentComponent.h"
class dtCrowd;
class dtNavMesh;
namespace GASS
{
	class DetourCrowdComponent : public Reflection<DetourCrowdComponent,BaseSceneComponent> 
	{
		friend class DetourCrowdAgentComponent;
	public:
		DetourCrowdComponent();
		~DetourCrowdComponent() override;
		static void RegisterReflection();
		void OnInitialize() override;
		void OnDelete() override;
		void SceneManagerTick(double delta_time) override;
		dtCrowd* GetCrowd() const {return m_Crowd;}
		void RegisterAgent(DetourCrowdAgentComponentPtr agent);
		void UnregisterAgent(DetourCrowdAgentComponentPtr agent);

		bool GetAnticipateTurns() const {return m_AnticipateTurns;}
		bool GetOptimizeVis() const { return m_OptimizeVis; }
		bool GetOptimizeTopo() const { return m_OptimizeTopo; }
		bool GetSeparation() const { return m_Separation; }
		bool GetObstacleAvoidance() const { return m_ObstacleAvoidance; }
		
	protected:
		//std::vector<std::string> GetScatteringSelection() const;
		//void SetScatteringSelection(const std::vector<std::string> &selection);
		
		//void SetNumberOfCharacters(int value);
		//int GetNumberOfCharacters() const;
		
		//void SetNavigationMesh(const std::string &value);
		//std::string GetNavigationMesh()const;

		//void SetScript(const std::string &value);
		//std::string GetScript() const;

		void SetDefaultAgentRadius(float value);
		float GetDefaultAgentRadius() const; 

		//void SetDefaultAgentHeight(float value);
		//float GetDefaultAgentHeight() const; 

		//void SetUpdateHealth(Float value);
		//Float GetUpdateHealth() const;
		void OnGoToPosisiton(GotoPositionRequestPtr message);
		RecastNavigationMeshComponentPtr  GetRecastNavigationMeshComponent() const {return m_Mesh.lock();}
		void InitCrowd(dtCrowd* crowd, dtNavMesh* nav);
		RecastNavigationMeshComponentPtr FindNavMesh(const std::string &name) const;
		//void CircularScattering(const std::vector<std::string>   &selection, int num_vehicles, const GASS::Vec3 &start_pos, double radius);
		//void NavMeshScattering(const std::vector<std::string>   &selection, int num_vehicles);
		bool GetShowDebug() const;
		void SetShowDebug(bool value);
		//void ReleaseAllChildren();
		void UpdateDebugData();
		
		//void OnChangeName(GASS::MessagePtr message);
		//void OnMoved(PositionRequestPtr message);
		//bool GetCircularScattering() const;
		//void SetCircularScattering(bool value);
		//bool GetNavMeshScattering() const;
		//void SetNavMeshScattering(bool value);
		//void SetCircularScatteringRadius(float value);
		//float GetCircularScatteringRadius() const;

		//bool GetRandomPointInCircle(Vec3 &pos, const Vec3 &center, float radius);

		//void SetSeparationWeight(float value);
		//float GetSeparationWeight() const {return m_SeparationWeight;} 
		
		SceneObjectWeakPtr m_DebugLines;
		std::vector<std::string> m_ScatteringSelection;
		//std::string m_NavMeshName;
		//std::string m_TerrainMapName;
		//Vec3 m_UpVector;
		//int m_NumberOfScattringCharacters;
		//float m_ScatteringRadius;
		bool m_ShowDebug;
		//std::string m_Script;
		bool m_Initialized;
		float m_DefaultAgentRadius;
		//float m_DefaultAgentHeight;
		dtCrowd* m_Crowd;
		RecastNavigationMeshComponentWeakPtr m_Mesh;

		bool m_AnticipateTurns;
		bool m_OptimizeVis;
		bool m_OptimizeTopo;
		bool m_Separation;
		bool m_ObstacleAvoidance;
		float m_SeparationWeight;
		std::vector<DetourCrowdAgentComponentPtr> m_Agents;
	};
	typedef GASS_SHARED_PTR<DetourCrowdComponent> DetourCrowdComponentPtr;
	typedef GASS_WEAK_PTR<DetourCrowdComponent> DetourCrowdComponentWeakPtr;
}

