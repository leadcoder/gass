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
#include "DetourCrowd.h"
struct dtCrowdAgent;

namespace GASS
{

	template <class T>
	class Smoother
	{
	private:

		//this holds the history
		std::vector<T>  m_History;

		int           m_iNextUpdateSlot;

		//an example of the 'zero' value of the type to be smoothed. This
		//would be something like Vector2D(0,0)
		T             m_ZeroValue;

	public:

		//to instantiate a Smoother pass it the number of samples you want
		//to use in the smoothing, and an exampe of a 'zero' type
		Smoother(int SampleSize, T ZeroValue):m_History(SampleSize, ZeroValue),
			m_ZeroValue(ZeroValue),
			m_iNextUpdateSlot(0)
		{}

		//each time you want to get a new average, feed it the most recent value
		//and this method will return an average over the last SampleSize updates
		T Update(const T& MostRecentValue)
		{  
			//overwrite the oldest value with the newest
			m_History[m_iNextUpdateSlot++] = MostRecentValue;

			//make sure m_iNextUpdateSlot wraps around. 
			if (m_iNextUpdateSlot == m_History.size()) m_iNextUpdateSlot = 0;

			//now to calculate the average of the history list
			T sum = m_ZeroValue;

			std::vector<T>::iterator it = m_History.begin();

			for (it; it != m_History.end(); ++it)
			{
				sum += *it;
			}

			return sum * (1.0/ (double)m_History.size());
		}
	};



	class DetourCrowdComponent;
	typedef SPTR<DetourCrowdComponent> DetourCrowdComponentPtr;
	typedef WPTR<DetourCrowdComponent> DetourCrowdComponentWeakPtr;

	class DetourCrowdAgentComponent :  public Reflection<DetourCrowdAgentComponent , BaseSceneComponent>
	{
	public:
		DetourCrowdAgentComponent(void);
		virtual ~DetourCrowdAgentComponent(void);
		static void RegisterReflection();
		virtual void OnInitialize();
		virtual void OnDelete();
		void Init(dtCrowd* crowd);
		void UpdateLocation(double delta_time);
		int GetIndex() const {return m_Index;}
		void SetRadius(float radius);
		float GetRadius() const;
		void SetHeight(float radius);
		float GetHeight() const;
		void SetMaxSpeed(float radius);
		float GetMaxSpeed() const;
		//bool GetRandomMeshPosition(Vec3 &pos);
		void SetSeparationWeight(float value);
		float GetSeparationWeight() const {return m_SeparationWeight;} 
	protected:
		DetourCrowdComponentPtr GetCrowdComp() const {return DetourCrowdComponentPtr(m_CrowdComp,NO_THROW);}
		dtCrowdAgentParams GetAgentParams() const;
		void UpdateAgentParams();
		void OnGoToPosition(GotoPositionMessagePtr message);
		void OnSetDesiredSpeed(DesiredSpeedMessagePtr message);
		void UpdateGeometry();
		void OnWorldPosition(WorldPositionMessagePtr message);
		void OnLoad(LocationLoadedMessagePtr message);
		void OnChangeName(GASS::MessagePtr message);
		void SetMaxAcceleration(float radius);
		float GetMaxAcceleration() const;
		Vec3 m_TargetPos;
		float m_Radius;
		float m_MaxSpeed;
		float m_MaxAcceleration;
		float m_Height;
		float m_SeparationWeight;

		//std::string m_Script;
		//std::string m_Group;
		//Vec3 m_LastPos;
		GraphicsMeshPtr m_MeshData;
		const dtCrowdAgent* m_Agent;
		int m_Index;
		Smoother<Vec3> m_VelSmoother;
		//Quaternion m_CurrentRot;
		//Quaternion m_DesiredRot;
		Vec3 m_CurrentDir;
		double m_AccTime;
		Vec3 m_LastPos;
		DetourCrowdComponentWeakPtr m_CrowdComp;
	};
	typedef SPTR<DetourCrowdAgentComponent> DetourCrowdAgentComponentPtr;
}
