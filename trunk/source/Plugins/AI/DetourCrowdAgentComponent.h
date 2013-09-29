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
		virtual void OnInitialize();
		virtual void OnDelete();

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
		bool GetRandomMeshPosition(Vec3 &pos);
		void SetSeparationWeight(float value);
		float GetSeparationWeight() const {return m_SeparationWeight;} 
	protected:
		dtCrowdAgentParams GetAgentParams() const;
		void UpdateAgentParams();
		void OnGoToPosition(GotoPositionMessagePtr message);
		void UpdateGeometry();
		void OnWorldPosition(WorldPositionMessagePtr message);
		void OnLoad(LocationLoadedMessagePtr message);
		
		void OnChangeName(GASS::MessagePtr message);
		
		
		void SetMaxAcceleration(float radius);
		float GetMaxAcceleration() const;

		float m_Radius;
		float m_MaxSpeed;
		float m_MaxAcceleration;
		float m_Height;
		float m_SeparationWeight;

		std::string m_Script;
		std::string m_Group;
		Vec3 m_LastPos;
		GraphicsMeshPtr m_MeshData;
		const dtCrowdAgent* m_Agent;
		int m_Index;

		Quaternion m_CurrentRot;
		Quaternion m_DesiredRot;
		double m_AccTime;
	};
	typedef SPTR<DetourCrowdAgentComponent> DetourCrowdAgentComponentPtr;
}
