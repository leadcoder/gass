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
#include "Core/ComponentSystem/BaseComponentContainerTemplate.h"
#include "Core/MessageSystem/IMessage.h"


namespace GASS
{
	typedef std::string SceneObjectID; 
	class SceneObjectTemplate;
	typedef boost::shared_ptr<SceneObjectTemplate> SceneObjectTemplatePtr;
	typedef boost::weak_ptr<SceneObjectTemplate> SceneObjectTemplateWeakPtr;
	typedef std::vector<SceneObjectTemplatePtr> SceneObjectTemplateVector;
	class GASSExport SceneObjectTemplate : public Reflection<SceneObjectTemplate , BaseComponentContainerTemplate>
	{
	public:
		SceneObjectTemplate();
		virtual ~SceneObjectTemplate();
		static	void RegisterReflection();
		void SetID(const SceneObjectID &id){m_ID = id;}
		SceneObjectID GetID() const {return m_ID;}
	protected:
		SceneObjectID m_ID;
	};
}
