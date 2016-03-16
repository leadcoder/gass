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
#include "Sim/GASSCommon.h"
#include "Core/MessageSystem/GASSBaseMessage.h"
#include "Sim/GASSPlatformType.h"
#include "Sim/GASSPlatformDetectionData.h"
#include "Core/Math/GASSVector.h"
#include "Sim/Interface/GASSIControlSettingsSystem.h"
#include "Sim/Messages/GASSCoreSceneObjectMessages.h"

namespace GASS
{
	class SensorMessage : public SceneObjectEventMessage
	{
	public:
		SensorMessage(const DetectionVector &detection_data, SenderID sender_id = -1, double delay= 0) : SceneObjectEventMessage(sender_id , delay) , 
			m_DetectionData(detection_data){}
		//const DetectionVector& GetDetectionVector() const {return m_DetectionData;}
		DetectionVector GetDetectionVector() const {return m_DetectionData;}
	private:
		DetectionVector m_DetectionData;
	};
	typedef GASS_SHARED_PTR<SensorMessage> SensorMessagePtr;


	class SensorGotTargetMessage : public SceneObjectEventMessage
	{
	public:
		SensorGotTargetMessage (const DetectionData &detection_data, SenderID sender_id = -1, double delay= 0) : SceneObjectEventMessage(sender_id , delay) , 
			m_DetectionData(detection_data){}
		DetectionData GetDetectionData() const {return m_DetectionData;}
	private:
		DetectionData m_DetectionData;
	};
	typedef GASS_SHARED_PTR<SensorGotTargetMessage> SensorGotTargetMessagePtr;


	class SensorLostTargetMessage : public SceneObjectEventMessage
	{
	public:
		SensorLostTargetMessage(const DetectionData &detection_data, SenderID sender_id = -1, double delay= 0) : SceneObjectEventMessage(sender_id , delay) , 
			m_DetectionData(detection_data){}
		DetectionData GetDetectionData() const {return m_DetectionData;}
	private:
		DetectionData m_DetectionData;
	};
	typedef GASS_SHARED_PTR<SensorLostTargetMessage> SensorLostTargetMessagePtr;


	class VehicleRadarEvent : public SceneObjectEventMessage
	{
	public:
		VehicleRadarEvent(bool has_isect,const Vec3 &isect_pos, Float dist, SceneObjectPtr isect_obj, SenderID sender_id = -1, double delay= 0) : SceneObjectEventMessage(sender_id , delay) , 
			m_HasIsect(has_isect),
			m_IsectPos(isect_pos),
			m_IsectObject(isect_obj),
			m_IsectDist(dist)
		{}

		bool m_HasIsect;
		Vec3 m_IsectPos;
		SceneObjectPtr m_IsectObject;
		Float m_IsectDist;
	private:
	};
	typedef GASS_SHARED_PTR<VehicleRadarEvent> VehicleRadarEventPtr;
}

