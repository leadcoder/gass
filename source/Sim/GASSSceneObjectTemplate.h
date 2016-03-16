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
#include "Core/Reflection/GASSBaseReflectionObject.h"
#include "Core/ComponentSystem/GASSComponentContainerTemplate.h"
#include "Core/MessageSystem/GASSIMessage.h"


namespace GASS
{
	typedef std::string SceneObjectID; 
	class SceneObjectTemplate;
	class BaseSceneComponent;

	typedef GASS_SHARED_PTR<SceneObjectTemplate> SceneObjectTemplatePtr;
	typedef GASS_WEAK_PTR<SceneObjectTemplate> SceneObjectTemplateWeakPtr;
	typedef GASS_SHARED_PTR<BaseSceneComponent> BaseSceneComponentPtr;
	typedef std::vector<SceneObjectTemplatePtr> SceneObjectTemplateVector;
	class GASSExport SceneObjectTemplate : public Reflection<SceneObjectTemplate , ComponentContainerTemplate>
	{
	public:
		SceneObjectTemplate();
		virtual ~SceneObjectTemplate();
		static	void RegisterReflection();
		void SetID(const SceneObjectID &id){m_ID = id;}
		SceneObjectID GetID() const {return m_ID;}
		void SetInstantiable(bool value) {m_Instantiable = value;}
		bool GetInstantiable() const {return m_Instantiable;}
		/**
		Convinces function for BaseSceneComponent's that call AddComponent on ComponentContainerTemplate
		*/
		BaseSceneComponentPtr AddBaseSceneComponent(const std::string &comp_name);


		/**
		Convinces function for BaseSceneComponent's that call GetComponent on ComponentContainerTemplate
		*/
		BaseSceneComponentPtr GetBaseSceneComponent(const std::string &comp_name);
	protected:
		SceneObjectID m_ID;
		bool m_Instantiable;
	};
}
