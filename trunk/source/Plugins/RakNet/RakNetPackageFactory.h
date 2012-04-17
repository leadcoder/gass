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

#ifndef RAK_NET_PACKAGE_FACTORY_H
#define RAK_NET_PACKAGE_FACTORY_H
#include "Core/Common.h"
#include "Core/ComponentSystem/GASSIComponent.h"
#include "Core/Utils/GASSFactory.h"
#include "Sim/Scenario/Scene/Messages/GASSCoreSceneObjectMessages.h"
#include "Sim/Scenario/Scene/Messages/GASSNetworkSceneObjectMessages.h"

namespace GASS
{
	/**
		The one and only factory holding network packages.
		This is a singleton implementation of the EnumFactory template.
		By using the singleton pattern each new package type can
		register it	self in a easy way.
		A package have to reigister it self to this
		factory	if it want any type of serialization behavior.
	*/

	class PackageFactory : public Factory<NetworkPackage,int>
	{
	public:
		PackageFactory();
		virtual ~PackageFactory();
		static PackageFactory* GetPtr();
		static PackageFactory& Get();
	protected:
		static PackageFactory* m_Instance;
	protected:
	};
}

#endif 
