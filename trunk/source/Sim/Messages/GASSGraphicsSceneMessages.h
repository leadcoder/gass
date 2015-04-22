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
#include "Core/Utils/GASSColorRGB.h"
#include "Core/Math/GASSQuaternion.h"
#include "Core/MessageSystem/GASSBaseMessage.h"

namespace GASS
{
	class SceneObject;
	class Scene;
	typedef SPTR<Scene> ScenePtr;
	typedef SPTR<SceneObject> SceneObjectPtr;


	//*********************************************************
	// ALL MESSAGES IN THIS SECTION CAN BE POSTED BY USER
	//*********************************************************

	
	/**
	Change time of day, this message can be used to change current time in
	scenes that support dynamic lighting
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
	typedef SPTR<TimeOfDayRequest> TimeOfDayRequestPtr;

	/**
	Change scene weather. 
	This message can be interpretaded by diffrent system,
	a sky system for the actual clouds and also maybee 
	water/sea system for changing waves, gfxsystem for the fog etc. 
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
		  Vec3 GetFogColor() const {return m_FogColor;}
		  float GetClouds() const {return m_Clouds;}
	private:
		float m_FogDistance;
		float m_FogDensity;
		float m_Clouds;
		Vec3 m_FogColor;
	};
	typedef SPTR<WeatherRequest> WeatherRequestPtr;

	//debug messages
	class DrawLineRequest : public SceneRequestMessage
	{
	public:
		DrawLineRequest(const Vec3 &start, const Vec3 &end, const ColorRGBA &color_start, const ColorRGBA &color_end, SenderID sender_id = -1, double delay= 0) : SceneRequestMessage(sender_id , delay) ,
			m_Start(start),
			m_End(end),
			m_ColorStart(color_start),
			m_ColorEnd(color_end)
		{

		}

		Vec3 GetStart()const {return m_Start;}
		Vec3 GetEnd()const {return m_End;}
		ColorRGBA GetColorStart()const {return m_ColorEnd;}
		ColorRGBA GetColorEnd()const {return m_ColorEnd;}
	private:
		Vec3 m_Start,m_End;
		ColorRGBA m_ColorStart,m_ColorEnd;
	};
	typedef SPTR<DrawLineRequest> DrawLineRequestPtr;


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
	typedef SPTR<DrawCircleRequest> DrawCircleRequestPtr;

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
	typedef SPTR<CreateTextBoxRequest> CreateTextBoxRequestPtr;



	class ExportMeshRequest : public SceneRequestMessage
	{
	public:
		ExportMeshRequest(const std::string &filename, SceneObjectPtr root_object, SenderID sender_id = -1, double delay= 0) :  m_Filename(filename),  m_RootObject(root_object),	  SceneRequestMessage(sender_id , delay)  
		{

		}
		std::string m_Filename;
		SceneObjectPtr m_RootObject;
	};
	typedef SPTR<ExportMeshRequest> ExportMeshRequestPtr;

}

#endif