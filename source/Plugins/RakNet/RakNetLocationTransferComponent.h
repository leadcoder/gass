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

#ifndef RAK_NET_LOCATION_TRANSFER_COMPONENT_H
#define RAK_NET_LOCATION_TRANSFER_COMPONENT_H

#include "Sim/Interface/GASSIGeometryComponent.h"
#include "Sim/GASSComponent.h"

#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Messages/GASSNetworkSceneObjectMessages.h"
#include "Sim/Messages/GASSPhysicsSceneObjectMessages.h"
#include "Sim/GASSCommon.h"
#include "RakNetCommon.h"

namespace GASS
{
	enum
	{
		TRANSFORMATION_DATA = 11
	};

	class TransformationPackage : public NetworkPackage
	{
	public:
		TransformationPackage() : NetworkPackage(0),
			Position(Vec3(0,0,0)),
			Velocity(Vec3(0,0,0)),
			AngularVelocity(Vec3(0, 0, 0))
			
		{

		}
		TransformationPackage(int id ) : NetworkPackage(id) ,
			Position(Vec3(0, 0, 0)),
			Velocity(Vec3(0, 0, 0)),
			AngularVelocity(Vec3(0, 0, 0))
			
		{

		}
		TransformationPackage(int id, unsigned int time_stamp,const Vec3 &pos,const Vec3 &vel,const Quaternion &rot,const Vec3 &ang_vel) : NetworkPackage(id), 
			Position(pos),
			Velocity(vel), 
			Rotation(rot),
			AngularVelocity(ang_vel),
			TimeStamp(time_stamp){}
		~TransformationPackage() override{}
		int GetSize() override {return sizeof(TransformationPackage);}
		void Assign(char* data) override
		{
			*this = *(TransformationPackage*)data;
		}
		Vec3 Position;
		Quaternion Rotation;
		Vec3 Velocity;
		Vec3 AngularVelocity;
		unsigned int TimeStamp{0};
	};
	using TransformationPackagePtr = std::shared_ptr<TransformationPackage>;



	class LocationHistory
	{
	public:
		LocationHistory() : Position(0,0,0),Rotation(1,0,0,0) 
		{
		}
		~LocationHistory(){}

		Vec3 Position;
		Quaternion Rotation;
		unsigned int Time{0};
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

	using SceneObjectPtr = std::shared_ptr<SceneObject>;
	using SceneObjectWeakPtr = std::weak_ptr<SceneObject>;

	class RakNetLocationTransferComponent : public Reflection<RakNetLocationTransferComponent,Component>
	{
	public:
		RakNetLocationTransferComponent();
		~RakNetLocationTransferComponent() override;
		static void RegisterReflection();
		void OnInitialize() override;
		void OnDelete() override;
		void SceneManagerTick(double delta) override;
	private:
		void SetSendFrequency(float value) {m_SendFreq = value;}
		float GetSendFrequency() const {return m_SendFreq;}
		void OnTransformationChanged(TransformationChangedEventPtr message);
		void OnParentTransformationChanged(TransformationChangedEventPtr message);
		void OnVelocityNotify(PhysicsVelocityEventPtr message);
		void OnDeserialize(NetworkDeserializeEventPtr message);

		bool GetUpdatePosition() const {return m_UpdatePosition;}
		void SetUpdatePosition(bool value) {m_UpdatePosition=value;}
		bool GetUpdateRotation() const {return m_UpdateRotation;}
		void SetUpdateRotation(bool value) {m_UpdateRotation=value;}

		bool GetExtrapolatePosition() const {return m_ExtrapolatePosition;}
		void SetExtrapolatePosition(bool value) {m_ExtrapolatePosition=value;}

		bool GetExtrapolateRotation() const {return m_ExtrapolateRotation;}
		void SetExtrapolateRotation(bool value) {m_ExtrapolateRotation=value;}

		

		Vec3 m_Velocity;
		Vec3 m_AngularVelocity;

		std::vector<LocationHistory> m_LocationHistory;

		double m_DeadReckoning{0};
		double m_LastSerialize{0};
		float m_SendFreq{0};
		bool m_UpdatePosition{true};
		bool m_UpdateRotation{true};
		bool m_ExtrapolatePosition{true};
		bool m_ExtrapolateRotation{true};
		Vec3 m_ParentPos;
		int m_NumHistoryFrames{6};
		Quaternion m_ParentRot;
		ClientLocationMode m_ClientLocationMode{UNCHANGED};
	};
	using RakNetLocationTransferComponentPtr = std::shared_ptr<RakNetLocationTransferComponent>;
}
#endif
