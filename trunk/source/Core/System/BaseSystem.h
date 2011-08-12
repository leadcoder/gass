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

#ifndef BASESYSTEM_HH
#define BASESYSTEM_HH

#include "Core/Reflection/RTTI.h"
#include "Core/Reflection/Reflection.h"
#include "Core/Reflection/BaseReflectionObject.h"
#include "Core/System/ISystem.h"
#include "Core/Serialize/IXMLSerialize.h"
#include <string>

namespace GASS
{
	class ISystemManager;
	typedef boost::weak_ptr<ISystemManager> SystemManagerWeakPtr;


	/** \addtogroup GASSCore
	*  @{
	*/
	/** \addtogroup System
	*  @{
	*/

	/**
		Convinience class that implement the ISystem and 
		IXMLSerialize interfaces, 
		A system that want attribute xml-serialization can derived
		from this class.
	*/

	class GASSCoreExport BaseSystem : public Reflection<BaseSystem, BaseReflectionObject>, public ISystem, public IXMLSerialize
	{
	public:
		BaseSystem();
		virtual ~BaseSystem();

		static void RegisterReflection();

		//ISystem interface
		virtual void OnCreate(){};
		std::string GetName() const {return m_Name;}
		void SetName(const std::string &name) {m_Name = name;}
		SystemManagerPtr GetOwner() const {return SystemManagerPtr(m_Owner);}
		void SetOwner(SystemManagerPtr owner){m_Owner = owner;}
		virtual SystemType GetSystemType() const {return "BaseSystem";}

		//IXMLSerialize interface
		virtual void LoadXML(TiXmlElement *xml_elem);
		virtual void SaveXML(TiXmlElement *xml_elem);
	protected:
		std::string m_Name;
		SystemManagerWeakPtr m_Owner;
	};
}
#endif // #ifndef BASESYSTEM_HH
