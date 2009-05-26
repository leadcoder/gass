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
#include "Core/ComponentSystem/BaseObject.h"


namespace GASS
{	
	class SceneObjectManager;
	class SceneObject;
	typedef boost::shared_ptr<SceneObject> SceneObjectPtr;
	typedef boost::weak_ptr<SceneObject> SceneObjectWeakPtr;
	typedef std::vector<SceneObjectPtr> SceneObjectVector;
	class GASSExport SceneObject : public Reflection<SceneObject, BaseObject>
	{
	public:
		SceneObject();
		virtual ~SceneObject();
		static	void RegisterReflection();
		void SyncMessages(double delta_time);

		SceneObjectManager* GetSceneObjectManager() {return m_Manager;}
		MessageManager* GetMessageManager(){return m_MessageManager;}
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
		SceneObjectVector GetObjectsByName(const std::string &name);
		void GetObjectsByName(SceneObjectVector &objects, const std::string &name); 


	protected:
		SceneObjectManager* m_Manager;
		MessageManager* m_MessageManager;
	};

}
