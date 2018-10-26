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
#include "Sim/Interface/GASSISceneManager.h"
#include "Core/Reflection/GASSBaseReflectionObject.h"
#include "Core/Serialize/GASSIXMLSerialize.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Sim/GASSSimSystem.h"


namespace GASS
{

	/**
		This is a convenience class that implements the ISceneManager interface. 
		It also implements the IXMLSerialize interface to enable the 
		derived scene manager to support xml serialization.
		New scene managers can derive from this class to reduce some implementation. 
		To avoid this class to be instantiated the constructors could be declared 
		private, however because we derived from the reflection class this is
		impossible, so we keep it public for now.
	*/

	class GASSExport BaseSceneManager : public Reflection<BaseSceneManager, BaseReflectionObject> , public GASS_ENABLE_SHARED_FROM_THIS<BaseSceneManager>, public virtual ISceneManager, public IXMLSerialize, public IMessageListener, public ISystemListener
	{
	public:
		BaseSceneManager();
		virtual ~BaseSceneManager();

		static void RegisterReflection();

		//ISceneManager
		virtual std::string GetName() const {return m_Name;}
		virtual void SetName(const std::string &name) {m_Name = name;}
		virtual ScenePtr GetScene() const {return m_Scene.lock();}//allow null pointer}
		virtual void SetScene(ScenePtr owner){m_Scene = owner;}
		
		virtual void RegisterPreUpdate(SceneManagerListenerPtr listener);
		virtual void RegisterPostUpdate(SceneManagerListenerPtr listener);

		virtual void OnUpdate(double /*delta_time*/) {};
	
		//ISystemListener
		void OnPreSystemUpdate(double delta_time)
		{
			if (m_PreSystemUpdate)
			{
				_UpdatePreListeners(delta_time);
				OnUpdate(delta_time);
				_UpdatePostListeners(delta_time);
			}
		}

		void OnPostSystemUpdate(double delta_time)
		{
			if (!m_PreSystemUpdate)
			{
				_UpdatePreListeners(delta_time);
				OnUpdate(delta_time);
				_UpdatePostListeners(delta_time);
			}
		}

		//IXMLSerialize
		virtual void LoadXML(tinyxml2::XMLElement *xml_elem);
		virtual void SaveXML(tinyxml2::XMLElement *xml_elem);
	protected:

		template <class T>
		void RegisterForPostUpdate()
		{
			GASS_SHARED_PTR<T> system = SimEngine::GetPtr()->GetSimSystemManager()->GetFirstSystemByClass<T>();
			system->RegisterListener(shared_from_this());
			m_PreSystemUpdate = true;
		}

		template <class T>
		void RegisterForPreUpdate()
		{
			GASS_SHARED_PTR<T> system = SimEngine::GetPtr()->GetSimSystemManager()->GetFirstSystemByClass<T>();
			system->RegisterListener(shared_from_this());
			m_PreSystemUpdate = false;
		}
	private:
		void _UpdatePreListeners(double delta_time);
		void _UpdatePostListeners(double delta_time);
		std::string m_Name;
		SceneWeakPtr m_Scene;
		std::vector<SceneManagerListenerWeakPtr> m_PostListeners;
		std::vector<SceneManagerListenerWeakPtr> m_PreListeners;
		bool m_PreSystemUpdate;
	};

	typedef GASS_SHARED_PTR<BaseSceneManager> BaseSceneManagerPtr;
}