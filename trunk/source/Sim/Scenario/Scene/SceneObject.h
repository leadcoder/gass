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

#pragma once

#include "Sim/Common.h"
#include "Core/Reflection/BaseReflectionObject.h"
#include "Core/ComponentSystem/BaseComponentContainer.h"
#include "Core/MessageSystem/Message.h"


namespace GASS
{	
	class SceneObjectManager;
	class SceneObject;
	typedef boost::shared_ptr<SceneObject> SceneObjectPtr;
	typedef boost::weak_ptr<SceneObject> SceneObjectWeakPtr;
	typedef std::vector<SceneObjectPtr> SceneObjectVector;

	/**
		This class is derived from the BaseComponentContainer class and extend
		the BaseComponentContainer with message functionality. 
		To communicate with components owned by the SceneObject a message manager is used.
		Some basic object messages is enumerated below.

		As the name indicates a SceneObject is a object in a scenario scene.
		The SceneObject is owned by a SceneObjectManager which in turn is owned
		by a ScenarioScene. 
		By design the SceneObject class is not intended to be derived from, 
		Instead new functionality should be added through components
	*/
	class GASSExport SceneObject : public Reflection<SceneObject, BaseComponentContainer>
	{
	public:

		// Todo: Explain each individual message

		//We divided messages in two catagories, notify and request
		//Messages with prefix OBJECT_RM, is a request message
		//Messages with prefix OBJECT_NM, is a notify message
		enum ObjectMessage
		{
			//-----------------Request section-------------
			/** \brief Message data: 
				Vec3 = "Position" - Position (relative to parent) change for SceneObject is requested */
			OBJECT_RM_POSITION, 

			/** \brief Message data: 
				Quaternion = "Rotation" - Rotation (relative to parent) change for SceneObject is requested */
			OBJECT_RM_ROTATION,

			OBJECT_RM_WORLD_POSITION, 
			OBJECT_RM_WORLD_ROTATION,

			OBJECT_RM_VISIBILITY,
			OBJECT_RM_COLLISION_SETTINGS,
			OBJECT_RM_PHYSICS_JOINT_PARAMETER,
			OBJECT_RM_PHYSICS_BODY_PARAMETER,
			OBJECT_RM_SOUND_PARAMETER,
			
			/** \brief Message data: 
			MeshParameterType = "Type" - See MeshParameterType for data fields*/
			OBJECT_RM_MESH_PARAMETER,

			
			OBJECT_RM_LOAD_USER_COMPONENTS,
			OBJECT_RM_LOAD_PHYSICS_COMPONENTS,
			OBJECT_RM_LOAD_GFX_COMPONENTS,
			OBJECT_RM_UNLOAD_COMPONENTS,

			
			//--------------------Notify section------------------------
			OBJECT_NM_PARENT_CHANGED,
			
			/** \brief message data: 
				Vec3 = "Velocity"
				Vec3 = "AngularVelocity" */
			OBJECT_NM_PHYSICS_VELOCITY,

			/** \brief message data: 
				Vec3 = "Position"		- Position (relative to parent) is changed for SceneObject
				Vec3 = "Scale"			- Scale is changed for SceneObject
				Quaternion = "Rotation"	- Position (relative to parent) is changed for SceneObject
			*/
			OBJECT_NM_TRANSFORMATION_CHANGED,
			
			
		};

		// parameters that belong to ObjectMessage OBJECT_NM_PHYSICS_VELOCITY
		enum PhysicsParameterType
		{
			AXIS1_VELOCITY,
			AXIS2_VELOCITY,
			AXIS1_FORCE,
			AXIS2_FORCE,
			TORQUE,
			FORCE,
		};

		// parameters that belong to ObjectMessage OBJECT_RM_SOUND_PARAMETER
		enum SoundParameterType
		{
			PLAY,
			STOP,
			PAUSE,
			PITCH,
			LOOP,
			VOLUME,
		};
		
		// parameters that belong to ObjectMessage OBJECT_RM_MESH_PARAMETER
		enum MeshParameterType
		{
			/** \brief Message data: 
			Vec2 = "Speed" - Texture scroll speed in x,y(s,t) direction*/
			ANIMATE_TEX_COORD, 
		};


		SceneObject();
		virtual ~SceneObject();
		static	void RegisterReflection();
		void SyncMessages(double delta_time);

		SceneObjectManager* GetSceneObjectManager() {return m_Manager;}
		//MessageManager* GetMessageManager(){return m_MessageManager;}
		void OnCreate();
	

		//public for now, not possible to get derived manager to get hold of this otherwise
		void SetSceneObjectManager(SceneObjectManager* manager);

		/**
			Get owner object that is direct under scene root
		*/
		SceneObjectPtr GetObjectUnderRoot();

		template <class T>
		boost::shared_ptr<T> GetFirstComponent()
		{
			boost::shared_ptr<T> ret;
			for(int i = 0 ; i < m_ComponentVector.size(); i++)
			{
				ret = boost::shared_dynamic_cast<T>(m_ComponentVector[i]);
				if(ret)
					break;
			}
			
			return ret;
		}



		//convenience functions 
		SceneObjectPtr GetParentSceneObject()
		{
			//no dynamic cast because we are sure that all objects are derived from the SceneObject
			return boost::shared_static_cast<SceneObject>(GetParent());
		}
		void GetComponentsByClass(ComponentVector &components, const std::string &class_name);

		//should we return result or pass it as ref arg?
		SceneObjectVector GetObjectsByName(const std::string &name, bool exact_math = true);
		void GetObjectsByName(SceneObjectVector &objects, const std::string &name,bool exact_math = true); 

		int RegisterForMessage(ObjectMessage type, MessageFunc callback, int priority = 0);
		void UnregisterForMessage(ObjectMessage type, MessageFunc callback);
		void PostMessage(MessagePtr message);
		void SendImmediate(MessagePtr message);
	protected:
		SceneObjectManager* m_Manager;
		MessageManager* m_MessageManager;
	};

}
