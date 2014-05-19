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
		virtual ~DetourCrowdComponent();
		static void RegisterReflection();
		virtual void OnInitialize();
		virtual void OnDelete();
		virtual void SceneManagerTick(double delta_time);
		dtCrowd* GetCrowd() const {return m_Crowd;}
		void RegisterAgent(DetourCrowdAgentComponentPtr agent);
		void UnregisterAgent(DetourCrowdAgentComponentPtr agent);
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
		RecastNavigationMeshComponentPtr  GetRecastNavigationMeshComponent() const {return RecastNavigationMeshComponentPtr(m_Mesh,NO_THROW);}
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

		ADD_PROPERTY(bool,AnticipateTurns);
		ADD_PROPERTY(bool,OptimizeVis);
		ADD_PROPERTY(bool,OptimizeTopo);
		ADD_PROPERTY(bool,Separation);
		ADD_PROPERTY(bool,ObstacleAvoidance);
		float m_SeparationWeight;
		std::vector<DetourCrowdAgentComponentPtr> m_Agents;
	};
	typedef SPTR<DetourCrowdComponent> DetourCrowdComponentPtr;
	typedef WPTR<DetourCrowdComponent> DetourCrowdComponentWeakPtr;
}

