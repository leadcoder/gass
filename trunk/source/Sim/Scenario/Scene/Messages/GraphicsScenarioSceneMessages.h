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

#ifndef GRAPHICS_SCENARIO_SCENE_MESSAGES_H
#define GRAPHICS_SCENARIO_SCENE_MESSAGES_H

#include "Sim/Common.h"
#include "Core/Math/Vector.h"
#include "Core/Math/Quaternion.h"
#include "Core/MessageSystem/BaseMessage.h"

namespace GASS
{
	class SceneObject;
	class ScenarioScene;
	typedef boost::shared_ptr<ScenarioScene> ScenarioScenePtr;
	typedef boost::shared_ptr<SceneObject> SceneObjectPtr;


	/**
	Message used for changing camera un viewport. 
	This message can be sent by user.
	*/
	class ChangeCameraMessage : public BaseMessage
	{
	public:
		/**
			Constructor
			@param camera The camera to activate
			@param viewport The name of the viewport target
		*/
		ChangeCameraMessage(SceneObjectPtr camera ,const std::string &viewport, SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay), m_Camera(camera), m_Viewport(viewport)
		  {

		  }
		  SceneObjectPtr GetCamera() const {return m_Camera;}
		  std::string GetViewport() const {return m_Viewport;}
	private:
		SceneObjectPtr m_Camera;
		std::string m_Viewport;
	};
	typedef boost::shared_ptr<ChangeCameraMessage> ChangeCameraMessagePtr;

	/**
		Message sent when viewport camera is changed 
	*/
	class CameraChangedNotifyMessage : public BaseMessage
	{
	public:
		/**
		Constructor
		@param camera Pointer to the new camera
		*/
		CameraChangedNotifyMessage(SceneObjectPtr camera , void* user_data, SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay), m_Camera(camera),m_UserData(user_data)
		  {

		  }
		  SceneObjectPtr GetCamera() const {return m_Camera;}
		  void* GetUserData() const {return m_UserData;}
	private:
		SceneObjectPtr m_Camera;
		void *m_UserData;
	};
	typedef boost::shared_ptr<CameraChangedNotifyMessage> CameraChangedNotifyMessagePtr;


	/**
		Message used to activate/deactive physics
	*/
	class ActivatePhysicsMessage : public BaseMessage
	{
	public:
		/**
		Constructor
		@param activate Indicate mode (0 == inactive, 1==active)
		*/
		ActivatePhysicsMessage(int activate, SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay) ,
			  m_Activate(activate){}
		  int Activate() const {return m_Activate;}
	private:
		int m_Activate;
	};
	typedef boost::shared_ptr<ActivatePhysicsMessage> ActivatePhysicsMessagePtr;


	/**
	Change time of day, this message can be used to change current time in
	scenarios that support dynamic lighting
	*/

	class TimeOfDayMessage : public BaseMessage
	{
	public:
		/**
		Constructor
		@param time Current time in hours (ie 12.5 == halfpast twelve)
		@param sun_set Sun set time in hours
		@param sun_rise Sun set rise in hours
		@param speed Time speed multiplier.
		*/
		TimeOfDayMessage(double time, double sun_set,double sun_rise, double speed, SenderID sender_id = -1, double delay= 0) :
		  BaseMessage( sender_id , delay),
			  m_Time(time),
			  m_Speed(speed),
			  m_SunRise(sun_rise),
			  m_SunSet(sun_set)
		  {

		  }
		  double GetTime() const {return m_Time;}
		  double GetSunSet() const {return m_SunSet;}
		  double GetSunRise() const {return m_SunRise;}
		  double GetSpeed() const {return m_Speed;}
	private:
		double m_Time;
		double m_Speed;
		double m_SunRise;
		double m_SunSet;
	};
	typedef boost::shared_ptr<TimeOfDayMessage> TimeOfDayMessagePtr;

	/**
	Change scene weather. 
	This message can be interpretaded by diffrent system,
	a sky system for the actual clouds and also maybee 
	water/sea system for changing waves, gfxsystem for the fog etc. 
	*/
	class WeatherMessage : public BaseMessage
	{
	public:
		/**
		Constructor
		@param fog_dist Distance where fog starts
		@param clouds Value between 0-1 to indicate cloud factor
		*/
		WeatherMessage(float fog_dist, float clouds, SenderID sender_id = -1, double delay= 0) :
		  BaseMessage( sender_id , delay),
			  m_FogDistance(fog_dist),
			  m_Clouds(clouds)
		  {

		  }
		  float GetFogDistance() const {return m_FogDistance;}
		  Vec3 GetFogColor() const {return m_FogColor;}
		  float GetClouds() const {return m_Clouds;}
	private:
		float m_FogDistance;
		float m_Clouds;
		Vec3 m_FogColor;
	};
	typedef boost::shared_ptr<WeatherMessage> WeatherMessagePtr;
}

#endif