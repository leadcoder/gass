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

#ifndef GRAPHICS_SYSTEM_MESSAGES_H
#define GRAPHICS_SYSTEM_MESSAGES_H

#include "Sim/GASSCommon.h"
#include "Sim/Messages/GASSCoreSystemMessages.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include <string>

namespace GASS
{
	class Scene;
	class Scene;
	class SceneObject;
	class ICameraComponent;
	class IViewport;

	typedef GASS_SHARED_PTR<Scene> ScenePtr;
	typedef GASS_SHARED_PTR<Scene> ScenePtr;
	typedef GASS_SHARED_PTR<SceneObject> SceneObjectPtr;
	typedef GASS_SHARED_PTR<ICameraComponent> CameraComponentPtr;
	typedef GASS_SHARED_PTR<IViewport> ViewportPtr;


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

	class GraphicsSceneManagerLoadedEvent : public SystemEventMessage
	{
	public:
		GraphicsSceneManagerLoadedEvent(const std::string &render_system, void* scene_graph_root_node,void* scene_graph_shadow_node,SenderID sender_id = -1, double delay= 0) :
		  SystemEventMessage(sender_id , delay),
			  m_RenderSystem(render_system),m_RootNode(scene_graph_root_node) ,m_ShadowNode(scene_graph_shadow_node) { }
		  std::string GetRenderSystem()const {return m_RenderSystem;}
		  void *GetSceneGraphRootNode()const {return m_RootNode;}
		  void *GetSceneGraphShadowNode()const {return m_ShadowNode;}
	private:
		void* m_RootNode;
		void* m_ShadowNode;
		std::string m_RenderSystem;
	};
	typedef GASS_SHARED_PTR<GraphicsSceneManagerLoadedEvent> GraphicsSceneManagerLoadedEventPtr;


	class PreGraphicsSystemUpdateEvent : public SystemEventMessage
	{
	public:
		PreGraphicsSystemUpdateEvent(double dt,SenderID sender_id = -1, double delay= 0) :
		  SystemEventMessage(sender_id , delay),
			  m_DeltaTime(dt){ }
		  double GetDeltaTime()const {return m_DeltaTime;}
	private:
		double m_DeltaTime;
	};
	typedef GASS_SHARED_PTR<PreGraphicsSystemUpdateEvent> PreGraphicsSystemUpdateEventPtr;

	class PostGraphicsSystemUpdateEvent : public SystemEventMessage
	{
	public:
		PostGraphicsSystemUpdateEvent(double dt,SenderID sender_id = -1, double delay= 0) :
		  SystemEventMessage(sender_id , delay),
			  m_DeltaTime(dt){ }
		  double GetDeltaTime()const {return m_DeltaTime;}
	private:
		double m_DeltaTime;
	};
	typedef GASS_SHARED_PTR<PostGraphicsSystemUpdateEvent> PostGraphicsSystemUpdateEventPtr;


	/**
	Message posted by the graphic system to notify that a new render window has been created.
	Subscribe to this message if you need to get hold of the render window handle,
	*/

	class RenderWindowCreatedEvent : public SystemEventMessage
	{
	public:
		RenderWindowCreatedEvent(void* render_window_handle, SenderID sender_id = -1, double delay= 0) :
		  SystemEventMessage(sender_id , delay),
			  m_Handle(render_window_handle)
		  {

		  }
		  void* GetRenderWindowHandle() const {return m_Handle;}
	private:
		void* m_Handle;
	};
	typedef GASS_SHARED_PTR<RenderWindowCreatedEvent> MainWindowCreatedEventPtr;



	/**
	Event fired when viewport change camera
	*/

	class CameraChangedEvent : public SystemEventMessage
	{
	public:
		/**
		Constructor
		@param camera Pointer to the new camera
		*/
		CameraChangedEvent(ViewportPtr viewport, SenderID sender_id = -1, double delay= 0) : 
		  SystemEventMessage(sender_id , delay), m_Viewport(viewport)
		  {

		  }
		  ViewportPtr  GetViewport() const {return m_Viewport;}
	private:
		ViewportPtr m_Viewport;
	};
	typedef GASS_SHARED_PTR<CameraChangedEvent> CameraChangedEventPtr;

	
	class ViewportMovedOrResizedEvent : public SystemEventMessage
	{
	public:
		ViewportMovedOrResizedEvent (const std::string &viewport_name, int pos_x, int pos_y, int width,int height,SenderID sender_id = -1, double delay= 0) :
		  m_PosX(pos_x),
			  m_PosY(pos_y),
			  m_Width(width),
			  m_Height(height),
			  m_VPName(viewport_name),
			  SystemEventMessage(sender_id , delay)  {}
		  int GetPositionX()const {return m_PosX;}
		  int GetPositionY()const {return m_PosY;}
		  int GetWidth()const {return m_Width;}
		  int GetHeight()const {return m_Height;}
		  std::string GetViewportName() const {return m_VPName;}
	private:
		int m_Width,m_Height,m_PosX,m_PosY;
		std::string m_VPName;
	};
	typedef GASS_SHARED_PTR<ViewportMovedOrResizedEvent> ViewportMovedOrResizedEventPtr;


	class RenderWindowResizedEvent : public SystemEventMessage
	{
	public:
		RenderWindowResizedEvent(const std::string &window_name, int width,int height,SenderID sender_id = -1, double delay= 0) :
		  m_Width(width),
			  m_Height(height),
			  m_WinName(window_name),
			  SystemEventMessage(sender_id , delay)  {}
		  int GetWidth()const {return m_Width;}
		  int GetHeight()const {return m_Height;}
		  std::string GetWindowName() const {return m_WinName;}
	private:
		int m_Width,m_Height;
		std::string m_WinName;
	};
	typedef GASS_SHARED_PTR<RenderWindowResizedEvent> RenderWindowResizedEventPtr;

	/**
	Message used for changing camera for viewport. 
	This message can be sent by user.
	@deprecated
	*/
	class ChangeCameraRequest : public SystemRequestMessage
	{
	public:
		/**
		Constructor
		@param camera The camera to activate
		@param viewport The name of the viewport target
		*/
		ChangeCameraRequest(CameraComponentPtr camera ,const std::string &viewport="", SenderID sender_id = -1, double delay= 0) : 
		  SystemRequestMessage(sender_id , delay), m_Camera(camera), m_Viewport(viewport)
		  {

		  }
		  CameraComponentPtr GetCamera() const {return m_Camera;}
		  std::string GetViewport() const {return m_Viewport;}
	private:
		CameraComponentPtr m_Camera;
		std::string m_Viewport;
	};
	typedef GASS_SHARED_PTR<ChangeCameraRequest> ChangeCameraRequestPtr;


	/**
	Message used for reloading materials. 
	This message can be sent by user.
	@deprecated
	*/
	class ReloadMaterial : public SystemRequestMessage
	{
	public:
		/**
		Constructor
		@param name The name of the material to reload 
		*/
		ReloadMaterial(const std::string &mat_name="", SenderID sender_id = -1, double delay= 0) : 
		  SystemRequestMessage(sender_id , delay), m_MaterialName(mat_name)
		  {

		  }
		  
		  std::string GetMaterialName() const {return m_MaterialName;}
	private:
		std::string m_MaterialName;
	};
	typedef GASS_SHARED_PTR<ReloadMaterial> ReloadMaterialPtr;




	
}

#endif
