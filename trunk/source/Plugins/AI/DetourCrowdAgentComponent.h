
/****************************************************************************
*                                                                           *
* HiFiEngine                                                                *
* Copyright (C)2003 - 2005 Johan Hedstrom                                   *
* Email: hifiengine@gmail.com                                               *
* Web page: http://n00b.dyndns.org/HiFiEngine                               *
*                                                                           *
* HiFiEngine is only used with knowledge from the author. This software     *
* is not allowed to redistribute without permission from the author.        *
* For further license information, please turn to the product home page or  *
* contact author. Abuse against the HiFiEngine license is prohibited by law.*
*                                                                           *
*****************************************************************************/ 
#pragma once 
#include "Sim/GASS.h"
#include "Plugins/Game/GameMessages.h"
#include "DetourCrowd/DetourCrowd.h"
struct dtCrowdAgent;

namespace GASS
{
	class DetourCrowdAgentComponent :  public Reflection<DetourCrowdAgentComponent , BaseSceneComponent>
	{
	public:
		DetourCrowdAgentComponent(void);
		virtual ~DetourCrowdAgentComponent(void);
		static void RegisterReflection();
		void OnInitialize();
		std::string GetScript() const;
		void SetScript(const std::string &name);
		std::string GetGroup() const;
		void SetGroup(const std::string &name);
		void Init(dtCrowd* crowd);
		void UpdateLocation(double delta_time);
		int GetIndex() const {return m_Index;}
		void SetRadius(float radius);
		float GetRadius() const;
		void SetHeight(float radius);
		float GetHeight() const;
		void SetMaxSpeed(float radius);
		float GetMaxSpeed() const;
	protected:
		dtCrowdAgentParams GetAgentParams() const;
		void UpdateAgentParams();
		void OnGoToPosisiton(GotoPositionMessagePtr message);
		void UpdateGeometry();
		void OnWorldPosition(WorldPositionMessagePtr message);
		void OnLoad(LocationLoadedMessagePtr message);
		void OnUnload(UnloadComponentsMessagePtr message);
		void OnChangeName(GASS::MessagePtr message);
		
		
		void SetMaxAcceleration(float radius);
		float GetMaxAcceleration() const;

		float m_Radius;
		float m_MaxSpeed;
		float m_MaxAcceleration;
		float m_Height;

		std::string m_Script;
		std::string m_Group;
		Vec3 m_LastPos;
		ManualMeshDataPtr m_MeshData;
		const dtCrowdAgent* m_Agent;
		int m_Index;

		Quaternion m_CurrentRot;
		Quaternion m_DesiredRot;
		double m_AccTime;
	};
	typedef boost::shared_ptr<DetourCrowdAgentComponent> DetourCrowdAgentComponentPtr;
}
