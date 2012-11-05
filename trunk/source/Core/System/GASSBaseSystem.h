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

#include "Core/Reflection/GASSRTTI.h"
#include "Core/Reflection/GASSReflection.h"
#include "Core/Reflection/GASSBaseReflectionObject.h"
#include "Core/System/GASSISystem.h"
#include "Core/Serialize/GASSIXMLSerialize.h"
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
		virtual void Init(){};
		virtual std::string GetSystemName() const {return "BaseSystem";}
		virtual void OnCreate(SystemManagerPtr owner){m_Owner = owner;}
		virtual void Register(SystemListenerPtr listener){};
		virtual void Unregister(SystemListenerPtr listener){};
		SystemManagerPtr GetOwner() const {return SystemManagerPtr(m_Owner);}
		
		//IXMLSerialize interface
		virtual void LoadXML(TiXmlElement *xml_elem);
		virtual void SaveXML(TiXmlElement *xml_elem);
		virtual int GetUpdateBucket() const;
		virtual void SetUpdateBucket(int priority);
	protected:
		SystemManagerWeakPtr m_Owner;
	};
}
#endif // #ifndef BASESYSTEM_HH
