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
#include "Core/Math/GASSAABox.h"
#include "Plugins/Ogre/GASSOgreRenderQueueBinder.h"
#include <OgreEntity.h>
#include <OgreManualObject.h>

namespace GASS
{
	class OgreMaterialCache 
	{
	public:
		OgreMaterialCache()
		{

		}
		~OgreMaterialCache()
		{

		}
		static void Add(const std::string &name,Ogre::Entity* entity);
		static void Restore(const std::string &name, Ogre::Entity* entity);
		static void Add(Ogre::ManualObject* mo);
		static void Restore(Ogre::ManualObject* mo );
	private:		
		//material cache
		typedef	std::map<std::string, std::vector<std::string> > MaterialCacheMap;
		static MaterialCacheMap m_MaterialCacheMap;
	};
}