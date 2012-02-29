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

#ifndef GRAPHICS_SYSTEM_MESSAGES_H
#define GRAPHICS_SYSTEM_MESSAGES_H

#include "Sim/Common.h"
#include "Core/MessageSystem/BaseMessage.h"
#include "Core/MessageSystem/IMessage.h"
#include "Core/Math/Vector.h"
#include "Core/Math/Quaternion.h"
#include <string>

namespace GASS
{
	class Scenario;
	class ScenarioScene;
	class SceneObject;

	typedef boost::shared_ptr<ScenarioScene> ScenarioScenePtr;
	typedef boost::shared_ptr<Scenario> ScenarioPtr;
	typedef boost::shared_ptr<SceneObject> SceneObjectPtr;
	

	/**
	Message posted by the graphic scene manager to notify that the scene manager
	has loaded successfully. The message provided the name of the render system in use and
	a root node to the scene graph created by the scene manager. The root node is  a void-pointer
	and should be casted to the correct scene node type depending on what the render system in use.
	ie. If Ogre3d is the render system the GetSceneGraphRootNode will return a Ogre::SceneNode*, if OSG
	is used the root node will be osg::Node* etc.
	This message is usefull for render system extensions that don't want to link to a specific render system
	implementation.
	*/

	class GFXSceneManagerLoadedNotifyMessage : public BaseMessage
	{
	public:
		GFXSceneManagerLoadedNotifyMessage(const std::string &render_system, void* scene_graph_root_node,void* scene_graph_shadow_node,SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay),
			  m_RenderSystem(render_system),m_RootNode(scene_graph_root_node) ,m_ShadowNode(scene_graph_shadow_node) { }
		  std::string GetRenderSystem()const {return m_RenderSystem;}
		  void *GetSceneGraphRootNode()const {return m_RootNode;}
		  void *GetSceneGraphShadowNode()const {return m_ShadowNode;}


	private:
		void* m_RootNode;
		void* m_ShadowNode;
		std::string m_RenderSystem;
	};
	typedef boost::shared_ptr<GFXSceneManagerLoadedNotifyMessage> GFXSceneManagerLoadedNotifyMessagePtr;

	/**
		Message posted by the graphic system to notify that a new render window has been created.
		Suscribe to this message if you need to get hold of the render window handle,
	*/

	class MainWindowCreatedNotifyMessage : public BaseMessage
	{
	public:
		MainWindowCreatedNotifyMessage(void* render_window_handle, void* main_window_handle,SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay),
			  m_Handle(render_window_handle),m_MainHandle(main_window_handle) { }
		  void* GetRenderWindowHandle() const {return m_Handle;}
		  void* GetMainHandle() const {return m_MainHandle;}

	private:
		void* m_Handle;
		void* m_MainHandle;

	};
	typedef boost::shared_ptr<MainWindowCreatedNotifyMessage> MainWindowCreatedNotifyMessagePtr;


	/**
		If you have a created a external render window this messages should be posted by the external
		owner if the render window has moved or resized.
		A graphic system implementation should suscribe to this message to notify
		it's internal window system about the change
	*/

	class ViewportMovedOrResizedNotifyMessage : public BaseMessage
	{
	public:
		ViewportMovedOrResizedNotifyMessage (const std::string &viewport_name, int pos_x, int pos_y, int width,int height,SenderID sender_id = -1, double delay= 0) :
		  m_PosX(pos_x),
		  m_PosY(pos_y),
		  m_Width(width),
		  m_Height(height),
		  m_VPName(viewport_name),
		  BaseMessage(sender_id , delay)  {}
		  int GetPositionX()const {return m_PosX;}
		  int GetPositionY()const {return m_PosY;}
		  int GetWidth()const {return m_Width;}
		  int GetHeight()const {return m_Height;}
		  std::string GetViewportName() const {return m_VPName;}
	private:
		int m_Width,m_Height,m_PosX,m_PosY;
		std::string m_VPName;
	};
	typedef boost::shared_ptr<ViewportMovedOrResizedNotifyMessage> ViewportMovedOrResizedNotifyMessagePtr;


	//debug messages
	class DrawLineMessage : public BaseMessage
	{
	public:
		DrawLineMessage(const Vec3 &start, const Vec3 &end, const Vec4 &color,SenderID sender_id = -1, double delay= 0) :
		  m_Start(start),
		  m_End(end),
		  m_Color(color),
		  BaseMessage(sender_id , delay)  {}
		  Vec3 GetStart()const {return m_Start;}
		  Vec3 GetEnd()const {return m_End;}
		  Vec4 GetColor()const {return m_Color;}
	private:
		Vec3 m_Start,m_End;
		Vec4 m_Color;
	};
	typedef boost::shared_ptr<DrawLineMessage> DrawLineMessagePtr;


	class DrawCircleMessage : public BaseMessage
	{
	public:
		DrawCircleMessage(const Vec3 &center, Float radius, const Vec4 &color,int segments, bool filled, SenderID sender_id = -1, double delay= 0) :
		  m_Center(center),
		  m_Radius(radius),
		  m_Color(color),
		  m_Segments(segments),
		  m_Filled(filled),
		  BaseMessage(sender_id , delay)  
		  {

		  }
		  Vec3 GetCenter()const {return m_Center;}
		  Float GetRadius() const {return m_Radius;}
		  Vec4 GetColor()const {return m_Color;}
		  int  GetSegments() const {return m_Segments;}
		  bool GetFilled() const {return m_Filled;}
	private:
		Vec3 m_Center;
		Float m_Radius;
		Vec4 m_Color;
		int m_Segments;
		bool m_Filled;
	};
	typedef boost::shared_ptr<DrawCircleMessage> DrawCircleMessagePtr;

	class CreateTextBoxMessage : public BaseMessage
	{
	public:
		CreateTextBoxMessage(const std::string &text_area_id, const std::string &text ,const Vec4 &color, float pos_x,float pos_y, float width, float height, SenderID sender_id = -1, double delay= 0) :
		  m_BoxID(text_area_id),
		  m_Text(text),
		  m_Color(color),
		  m_PosX(pos_x),
		  m_PosY(pos_y),
		  m_Width(width),
		  m_Height(height),

		  BaseMessage(sender_id , delay)  
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
	typedef boost::shared_ptr<CreateTextBoxMessage> CreateTextBoxMessagePtr;
}

#endif
