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

#include "Sim/GASS.h"

namespace osg
{
	class Geode;
}


namespace GASS
{
	class OSGNodeProxyComponent : public Reflection<OSGNodeProxyComponent,Component>
	{
	public:
		OSGNodeProxyComponent (void);
		~OSGNodeProxyComponent (void) override;
		static void RegisterReflection();
		void OnInitialize() override;
	protected:
		void UpdateDescriptionFromNode();
		void OnLocationLoaded(LocationLoadedEventPtr message);
		void OnGeomChanged(GeometryChangedEventPtr message);
		void SetDescriptionList(const std::vector<std::string>  &list);
		std::vector<std::string> GetDescriptionList() const;

		std::vector<std::string> m_List;
	};
}

