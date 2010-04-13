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

#include "Sim/Components/Graphics/Geometry/IGeometryComponent.h"
#include "Sim/Components/BaseSceneComponent.h"
#include "Sim/Scenario/Scene/SceneObjectMessages.h"
#include "Sim/Systems/Input/ControlSetting.h"
#include "Sim/Scheduling/ITaskListener.h"

#include "Sim/Common.h"
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
		TransformationPackage(int id, unsigned int time_stamp,const Vec3 &pos,const Vec3 &vel,const Quaternion &rot,const Quaternion &ang_vel) : NetworkPackage(id), 
			Position(pos),
			Velocity(vel), 
			Rotation(rot),
			AngularVelocity(ang_vel),
			TimeStamp(time_stamp){}
		/*static void RegisterToFactory()
		{
			GASS::PackageFactory::GetPtr()->Register(TRANSFORMATION_DATA,new GASS::EnumCreator<TransformationPackage, NetworkPackage>);	
		}*/
		virtual ~TransformationPackage(){}
		int GetSize() {return sizeof(TransformationPackage);}
		void Assign(char* data)
		{
			*this = *(TransformationPackage*)data;
		}
		Vec3 Position;
		Quaternion Rotation;
		Vec3 Velocity;
		Quaternion AngularVelocity;
		unsigned int TimeStamp;
	};
	typedef boost::shared_ptr<TransformationPackage> TransformationPackagePtr;
	


	class SceneObject;
	
	typedef boost::shared_ptr<SceneObject> SceneObjectPtr;
	typedef boost::weak_ptr<SceneObject> SceneObjectWeakPtr;

	class RakNetLocationTransferComponent : public Reflection<RakNetLocationTransferComponent,BaseSceneComponent>, public ITaskListener
	{
	public:
		RakNetLocationTransferComponent();
		virtual ~RakNetLocationTransferComponent();
		static void RegisterReflection();
		virtual void OnCreate();
	private:
		void SetSendFrequency(float value) {m_SendFreq = value;}
		float GetSendFrequency() const {return m_SendFreq;}
		void OnLoad(LoadNetworkComponentsMessagePtr message);
		void OnUnload(UnloadComponentsMessagePtr message);
		void OnTransformationChanged(TransformationNotifyMessagePtr message);
		void OnDeserialize(NetworkDeserializeMessagePtr message);

		//ITaskListener
		void Update(double delta);
		TaskGroup GetTaskGroup() const;

		
		Vec3 m_Velocity;
		Quaternion m_AngularVelocity;

		Quaternion m_RotationHistory[3];
		Vec3 m_PositionHistory[3];
		unsigned int m_TimeStampHistory[3];
		double m_DeadReckoning;
		double m_LastSerialize;
		float m_SendFreq;
	};
	typedef boost::shared_ptr<RakNetLocationTransferComponent> RakNetLocationTransferComponentPtr;
}
#endif
