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

#ifndef GRAPHICS_SCENE_MESSAGES_H
#define GRAPHICS_SCENE_MESSAGES_H

#include "Sim/GASSCommon.h"
#include "Sim/Messages/GASSCoreSceneMessages.h"
#include "Core/Math/GASSVector.h"
#include "Core/Math/GASSQuaternion.h"
#include "Core/MessageSystem/GASSBaseMessage.h"

namespace GASS
{
	class SceneObject;
	class Scene;
	typedef boost::shared_ptr<Scene> ScenePtr;
	typedef boost::shared_ptr<SceneObject> SceneObjectPtr;


	//*********************************************************
	// ALL MESSAGES IN THIS SECTION CAN BE POSTED BY USER
	//*********************************************************

	/**
	Message used for changing camera un viewport. 
	This message can be sent by user.
	*/
	class ChangeCameraRequest : public SceneMessage
	{
	public:
		/**
			Constructor
			@param camera The camera to activate
			@param viewport The name of the viewport target
		*/
		ChangeCameraRequest(SceneObjectPtr camera ,const std::string &viewport, SenderID sender_id = -1, double delay= 0) : 
		  SceneMessage(sender_id , delay), m_Camera(camera), m_Viewport(viewport)
		  {

		  }
		  SceneObjectPtr GetCamera() const {return m_Camera;}
		  std::string GetViewport() const {return m_Viewport;}
	private:
		SceneObjectPtr m_Camera;
		std::string m_Viewport;
	};
	typedef boost::shared_ptr<ChangeCameraRequest> ChangeCameraRequestPtr;




	/**
	Change time of day, this message can be used to change current time in
	scenes that support dynamic lighting
	*/

	class TimeOfDayRequest : public BaseMessage
	{
	public:
		/**
		Constructor
		@param time Current time in hours (ie 12.5 == halfpast twelve)
		@param sun_set Sun set time in hours
		@param sun_rise Sun set rise in hours
		@param speed Time speed multiplier.
		*/
		TimeOfDayRequest(double time, double sun_set,double sun_rise, double speed, SenderID sender_id = -1, double delay= 0) :
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
	typedef boost::shared_ptr<TimeOfDayRequest> TimeOfDayRequestPtr;

	/**
	Change scene weather. 
	This message can be interpretaded by diffrent system,
	a sky system for the actual clouds and also maybee 
	water/sea system for changing waves, gfxsystem for the fog etc. 
	*/
	class WeatherRequest : public BaseMessage
	{
	public:
		/**
		Constructor
		@param fog_dist Distance where fog starts
		@param clouds Value between 0-1 to indicate cloud factor
		*/
		WeatherRequest(float fog_dist, float fog_density, float clouds, SenderID sender_id = -1, double delay= 0) :
		  BaseMessage( sender_id , delay),
			  m_FogDensity(fog_density),
			  m_FogDistance(fog_dist),
			  m_Clouds(clouds)
		  {

		  }
		  float GetFogDensity() const {return m_FogDensity;}
		  float GetFogDistance() const {return m_FogDistance;}
		  Vec3 GetFogColor() const {return m_FogColor;}
		  float GetClouds() const {return m_Clouds;}
	private:
		float m_FogDistance;
		float m_FogDensity;
		float m_Clouds;
		Vec3 m_FogColor;
	};
	typedef boost::shared_ptr<WeatherRequest> WeatherRequestPtr;


	/**
		Message sent by graphics system when viewport camera is changed 
	*/
	class CameraChangedEvent : public SceneMessage
	{
	public:
		/**
		Constructor
		@param camera Pointer to the new camera
		*/
		CameraChangedEvent(SceneObjectPtr camera , void* user_data, SenderID sender_id = -1, double delay= 0) : 
		  SceneMessage(sender_id , delay), m_Camera(camera),m_UserData(user_data)
		  {

		  }
		  SceneObjectPtr GetCamera() const {return m_Camera;}
		  void* GetUserData() const {return m_UserData;}
	private:
		SceneObjectPtr m_Camera;
		void *m_UserData;
	};
	typedef boost::shared_ptr<CameraChangedEvent> CameraChangedEventPtr;
}

#endif