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

#ifndef RAK_NET_LOCATION_TRANSFER_COMPONENT_H
#define RAK_NET_LOCATION_TRANSFER_COMPONENT_H

#include "Sim/Components/Graphics/Geometry/GASSIGeometryComponent.h"
#include "Sim/Components/GASSBaseSceneComponent.h"
#include "Sim/Components/Network/GASSINetworkComponent.h"
#include "Sim/Scene/GASSCoreSceneObjectMessages.h"
#include "Sim/Scene/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Scene/GASSNetworkSceneObjectMessages.h"
#include "Sim/Scene/GASSPhysicsSceneObjectMessages.h"

#include "Sim/Systems/Input/GASSIControlSettingsSystem.h"
#include "Sim/Scheduling/GASSITaskListener.h"

#include "Sim/GASSCommon.h"
#include "Plugins/RakNet/RakNetMessages.h"
#include "Plugins/RakNet/RakNetPackageFactory.h"


namespace GASS
{
	enum
	{
		TRANSFORMATION_DATA = 11
	};

	class TransformationPackage : public NetworkPackage
	{
	public:
		TransformationPackage() 
		{

		}
		TransformationPackage(int id ) : NetworkPackage(id) 
		{

		}
		TransformationPackage(int id, unsigned int time_stamp,const Vec3 &pos,const Vec3 &vel,const Quaternion &rot,const Vec3 &ang_vel) : NetworkPackage(id), 
			Position(pos),
			Velocity(vel), 
			Rotation(rot),
			AngularVelocity(ang_vel),
			TimeStamp(time_stamp){}
		virtual ~TransformationPackage(){}
		int GetSize() {return sizeof(TransformationPackage);}
		void Assign(char* data)
		{
			*this = *(TransformationPackage*)data;
		}
		Vec3 Position;
		Quaternion Rotation;
		Vec3 Velocity;
		Vec3 AngularVelocity;
		unsigned int TimeStamp;
	};
	typedef boost::shared_ptr<TransformationPackage> TransformationPackagePtr;



	class LocationHistory
	{
	public:
		LocationHistory() : Position(0,0,0),Rotation(1,0,0,0), Time(0)
		{
		}
		~LocationHistory(){}

		Vec3 Position;
		Quaternion Rotation;
		unsigned int Time;
	};


	enum ClientLocationMode
	{
		UNCHANGED = 0,
		FORCE_ATTACHED_TO_PARENT_AND_SEND_RELATIVE = 1,
		FORCE_ATTACHED_TO_PARENT_AND_SEND_WORLD = 2
	};

	inline std::ostream& operator << (std::ostream& os, const ClientLocationMode& clm)
	{
		os << ((int) clm);
		return os;
	}

	inline std::istream& operator >> (std::istream& os, ClientLocationMode& clm)
	{
		int value;
		os >> value;
		clm = (ClientLocationMode) value;
		return os;
	}

	class SceneObject;

	typedef boost::shared_ptr<SceneObject> SceneObjectPtr;
	typedef boost::weak_ptr<SceneObject> SceneObjectWeakPtr;

	class RakNetLocationTransferComponent : public Reflection<RakNetLocationTransferComponent,BaseSceneComponent>, public INetworkComponent
	{
	public:

		

		RakNetLocationTransferComponent();
		virtual ~RakNetLocationTransferComponent();
		static void RegisterReflection();
		virtual void OnInitialize();
		//INetworkComponent
		virtual bool IsRemote() const;
		virtual void SceneManagerTick(double delta);
	private:
		void SetSendFrequency(float value) {m_SendFreq = value;}
		float GetSendFrequency() const {return m_SendFreq;}
		void OnLoad(LoadComponentsMessagePtr message);
		void OnUnload(UnloadComponentsMessagePtr message);
		void OnTransformationChanged(TransformationNotifyMessagePtr message);
		void OnParentTransformationChanged(TransformationNotifyMessagePtr message);
		void OnVelocityNotify(VelocityNotifyMessagePtr message);
		void OnDeserialize(NetworkDeserializeMessagePtr message);

		bool GetUpdatePosition() const {return m_UpdatePosition;}
		void SetUpdatePosition(bool value) {m_UpdatePosition=value;}
		bool GetUpdateRotation() const {return m_UpdateRotation;}
		void SetUpdateRotation(bool value) {m_UpdateRotation=value;}

		
		

		Vec3 m_Velocity;
		Vec3 m_AngularVelocity;

		std::vector<LocationHistory> m_LocationHistory;

		double m_DeadReckoning;
		double m_LastSerialize;
		float m_SendFreq;
		//int m_RelativeToParent;
		bool m_UpdatePosition;
		bool m_UpdateRotation;
		Vec3 m_ParentPos;
		int m_NumHistoryFrames;
		Quaternion m_ParentRot;
		ADD_ATTRIBUTE(ClientLocationMode,ClientLocationMode);
	};
	typedef boost::shared_ptr<RakNetLocationTransferComponent> RakNetLocationTransferComponentPtr;
}
#endif
