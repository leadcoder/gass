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
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSBaseSceneComponent.h"


namespace GASS
{
	class GASSExport SceneObjectRef 
	{
		friend class BaseSceneComponent;
	public:
		SceneObjectRef();
		SceneObjectRef(SceneObjectPtr obj);
		SceneObjectRef(SceneObjectGUID guid);
		virtual ~SceneObjectRef();
		SceneObjectPtr operator ->()
		{
			return m_RefObject.lock();
		}
		//bool IsValid() const {return SceneObjectPtr(m_RefObject,NO_THROW);}
		SceneObjectGUID GetRefGUID() const {return m_RefObjectGUID;}
		void SetRefGUID(const SceneObjectGUID &guid);
		SceneObjectPtr GetRefObject() const {return m_RefObject.lock();}
		bool IsValid() const {if(m_RefObject.lock()) return true; else return false;}
	protected:
		void UpdateRefPtr();
		void ResolveTemplateReferences(SceneObjectPtr template_root);


		friend std::ostream& operator << (std::ostream& os, const SceneObjectRef& sor)
		{
			os << sor.m_RefObjectGUID;
			return os;
		}
		friend std::istream& operator >> (std::istream& is, SceneObjectRef& sor)
		{
			std::string string_id;
			is >> string_id;
			const size_t id_len = string_id.length();
			if(id_len == 36) // Must be a real GUID
			{
				std::stringstream ss;
				ss << string_id;
				SceneObjectGUID guid;
				ss >> guid;
				sor.SetRefGUID(SceneObjectGUID(guid));
			}
			else // we have local id, save and remap to guid later
			{
				sor.m_LocalID = string_id ;
			}
			return is;
		}

		SceneObjectWeakPtr m_RefObject;
		SceneObjectGUID m_RefObjectGUID;
		std::string m_LocalID;
	};
}
