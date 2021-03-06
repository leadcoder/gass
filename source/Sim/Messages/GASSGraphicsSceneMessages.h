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
#include "Sim/Messages/GASSCoreSceneMessages.h"
#include "Core/Math/GASSVector.h"
#include "Core/Utils/GASSColorRGBA.h"

namespace GASS
{
	class SceneObject;
	class Scene;
	typedef GASS_SHARED_PTR<Scene> ScenePtr;
	typedef GASS_SHARED_PTR<SceneObject> SceneObjectPtr;


	//*********************************************************
	// ALL MESSAGES IN THIS SECTION CAN BE POSTED BY USER
	//*********************************************************

	
	/**
	Change time of day, this message can be used to change current time in
	scenes that support dynamic lighting
	NOTE: this is only used in OGRE3D 
	*/

	class TimeOfDayRequest : public SceneRequestMessage
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
		  SceneRequestMessage( sender_id , delay),
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
	typedef GASS_SHARED_PTR<TimeOfDayRequest> TimeOfDayRequestPtr;

	/**
	Change scene weather. 
	This message can be interpretaded by diffrent system,
	a sky system for the actual clouds and also maybee 
	water/sea system for changing waves, gfxsystem for the fog etc.
	NOTE: this is only used in OGRE3D	
	*/
	class WeatherRequest : public SceneRequestMessage
	{
	public:
		/**
		Constructor
		@param fog_dist Distance where fog starts
		@param clouds Value between 0-1 to indicate cloud factor
		*/
		WeatherRequest(float fog_dist, float fog_density, float clouds, SenderID sender_id = -1, double delay= 0) :
		  SceneRequestMessage( sender_id , delay),
			  m_FogDensity(fog_density),
			  m_FogDistance(fog_dist),
			  m_Clouds(clouds)
		  {

		  }
		  float GetFogDensity() const {return m_FogDensity;}
		  float GetFogDistance() const {return m_FogDistance;}
		  float GetClouds() const {return m_Clouds;}
	private:
		float m_FogDistance;
		float m_FogDensity;
		float m_Clouds;
	};
	typedef GASS_SHARED_PTR<WeatherRequest> WeatherRequestPtr;

	
	//@deprecated not implemented in OSG
	class DrawCircleRequest : public SceneRequestMessage
	{
	public:
		DrawCircleRequest(const Vec3 &center, Float radius, const ColorRGBA &color,int segments, bool filled, SenderID sender_id = -1, double delay= 0) :
		  m_Center(center),
			  m_Radius(radius),
			  m_Color(color),
			  m_Segments(segments),
			  m_Filled(filled),
			  SceneRequestMessage(sender_id , delay)  
		  {

		  }
		  Vec3 GetCenter()const {return m_Center;}
		  Float GetRadius() const {return m_Radius;}
		  ColorRGBA GetColor()const {return m_Color;}
		  int  GetSegments() const {return m_Segments;}
		  bool GetFilled() const {return m_Filled;}
	private:
		Vec3 m_Center;
		Float m_Radius;
		ColorRGBA m_Color;
		int m_Segments;
		bool m_Filled;
	};
	typedef GASS_SHARED_PTR<DrawCircleRequest> DrawCircleRequestPtr;

	//@deprecated move this to interface and return Textbox object
	class CreateTextBoxRequest : public SceneRequestMessage
	{
	public:
		CreateTextBoxRequest(const std::string &text_area_id, const std::string &text ,const Vec4 &color, float pos_x,float pos_y, float width, float height, SenderID sender_id = -1, double delay= 0) :
		  m_BoxID(text_area_id),
			  m_Text(text),
			  m_Color(color),
			  m_PosX(pos_x),
			  m_PosY(pos_y),
			  m_Width(width),
			  m_Height(height),

			  SceneRequestMessage(sender_id , delay)  
		  {

		  }

		  std::string m_BoxID;
		  std::string m_Text;
		  Vec4 m_Color;
		  float m_PosX;
		  float m_PosY;
		  float m_Width;
		  float m_Height;
	};
	typedef GASS_SHARED_PTR<CreateTextBoxRequest> CreateTextBoxRequestPtr;


	//@deprecated
	class ExportMeshRequest : public SceneRequestMessage
	{
	public:
		ExportMeshRequest(const std::string &filename, SceneObjectPtr root_object, SenderID sender_id = -1, double delay= 0) :  m_Filename(filename),  m_RootObject(root_object),	  SceneRequestMessage(sender_id , delay)  
		{

		}
		std::string m_Filename;
		SceneObjectPtr m_RootObject;
	};
	typedef GASS_SHARED_PTR<ExportMeshRequest> ExportMeshRequestPtr;



	class TerrainChangedEvent : public SceneEventMessage
	{
	public:
		TerrainChangedEvent(SenderID sender_id = -1, double delay = 0) : SceneEventMessage(sender_id, delay)
		{

		}
	};
	typedef GASS_SHARED_PTR<TerrainChangedEvent> TerrainChangedEventPtr;
}