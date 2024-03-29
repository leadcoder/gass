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


#include <map>

#include "Sim/GASS.h"
#include "Modules/OSG/OSGCommon.h"
#include "Modules/OSG/Components/OSGCameraComponent.h"

namespace GASS
{
	/**
		Collision system implementation that use osg for intersection test
	*/
	class OSGCollisionSystem : public Reflection<OSGCollisionSystem , SimSystem> , public ICollisionSystem
	{
	public:
		static void RegisterReflection();
		OSGCollisionSystem(SimSystemManagerWeakPtr manager);
		~OSGCollisionSystem() override;
		void OnSystemInit() override;
		std::string GetSystemName() const override {return "OSGCollisionSystem";}
		bool GetReadPagedLOD() { return m_ReadPagedLOD; }
	private:
		bool m_ReadPagedLOD = true;
	};
	using OSGCollisionSystemPtr = std::shared_ptr<OSGCollisionSystem>;
}

