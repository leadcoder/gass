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
		virtual void SceneManagerTick(double delta_time);
		dtCrowd* GetCrowd() const {return m_Crowd;}
		void RegisterAgent(DetourCrowdAgentComponentPtr agent);
		void UnregisterAgent(DetourCrowdAgentComponentPtr agent);
	protected:
		std::vector<std::string> GetScatteringSelection() const;
		void SetScatteringSelection(const std::vector<std::string> &selection);
		
		void SetNumberOfCharacters(int value);
		int GetNumberOfCharacters() const;
		
		void SetNavigationMesh(const std::string &value);
		std::string GetNavigationMesh()const;

		void SetScript(const std::string &value);
		std::string GetScript() const;
		
		//void SetMaxSpeed(float value);
		//float GetMaxSpeed() const;
		//void SetMaxForce(float value);
		//float GetMaxForce() const; 
		void SetDefaultAgentRadius(float value);
		float GetDefaultAgentRadius() const; 

		void SetDefaultAgentHeight(float value);
		float GetDefaultAgentHeight() const; 
	


		void OnGoToPosisiton(GotoPositionMessagePtr message);
		RecastNavigationMeshComponentPtr  GetRecastNavigationMeshComponent() const {return RecastNavigationMeshComponentPtr(m_Mesh,boost::detail::sp_nothrow_tag());}
		void InitCrowd(dtCrowd* crowd, dtNavMesh* nav);
		RecastNavigationMeshComponentPtr FindNavMesh(const std::string &name) const;
		void CircularScattering(const std::vector<std::string>   &selection, int num_vehicles, const GASS::Vec3 &start_pos, double radius);
		void NavMeshScattering(const std::vector<std::string>   &selection, int num_vehicles);
		bool GetShowDebug() const;
		void SetShowDebug(bool value);
		void ReleaseAllChildren();
		void UpdateDebugData();
		void OnUnload(UnloadComponentsMessagePtr message);
		void OnChangeName(GASS::MessagePtr message);
		void OnLoad(LoadComponentsMessagePtr message);
		void OnMoved(PositionMessagePtr message);
		bool GetCircularScattering() const;
		void SetCircularScattering(bool value);
		bool GetNavMeshScattering() const;
		void SetNavMeshScattering(bool value);
		void SetCircularScatteringRadius(float value);
		float GetCircularScatteringRadius() const;

		bool GetRandomPointInCircle(Vec3 &pos, const Vec3 &center, float radius);

		void SetSeparationWeight(float value);
		float GetSeparationWeight() const {return m_SeparationWeight;} 
		
		SceneObjectWeakPtr m_DebugLines;
		std::vector<std::string> m_ScatteringSelection;
		std::string m_NavMeshName;
		std::string m_TerrainMapName;
		Vec3 m_UpVector;
		int m_NumberOfScattringCharacters;
		float m_ScatteringRadius;
		bool m_ShowDebug;
		std::string m_Script;
		bool m_Initialized;
		float m_DefaultAgentRadius;
		float m_DefaultAgentHeight;
		dtCrowd* m_Crowd;
		RecastNavigationMeshComponentWeakPtr m_Mesh;

		ADD_ATTRIBUTE(bool,AnticipateTurns);
		ADD_ATTRIBUTE(bool,OptimizeVis);
		ADD_ATTRIBUTE(bool,OptimizeTopo);
		ADD_ATTRIBUTE(bool,Separation);
		ADD_ATTRIBUTE(bool,ObstacleAvoidance);

		float m_SeparationWeight;

		std::vector<DetourCrowdAgentComponentPtr> m_Agents;

	};
	typedef boost::shared_ptr<DetourCrowdComponent> DetourCrowdComponentPtr;
	typedef boost::weak_ptr<DetourCrowdComponent> DetourCrowdComponentWeakPtr;
}

