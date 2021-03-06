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
#include "Core/Utils/GASSEnumBinder.h"
#include <GASSOgreCommon.h>
#include <OgreRenderQueue.h>

namespace GASS
{
	START_ENUM_BINDER(Ogre::RenderQueueGroupID,RenderQueueBinder)
		BIND2("RENDER_QUEUE_BACKGROUND",Ogre::RENDER_QUEUE_BACKGROUND)
		BIND2("RENDER_QUEUE_SKIES_EARLY",Ogre::RENDER_QUEUE_SKIES_EARLY)
		BIND2("RENDER_QUEUE_1",Ogre::RENDER_QUEUE_1)
		BIND2("RENDER_QUEUE_2",Ogre::RENDER_QUEUE_2)
		BIND2("RENDER_QUEUE_WORLD_GEOMETRY_1",Ogre::RENDER_QUEUE_WORLD_GEOMETRY_1)
		BIND2("RENDER_QUEUE_3",Ogre::RENDER_QUEUE_3)
		BIND2("RENDER_QUEUE_4",Ogre::RENDER_QUEUE_4)
		BIND2("RENDER_QUEUE_MAIN",Ogre::RENDER_QUEUE_MAIN)
		BIND2("RENDER_QUEUE_6",Ogre::RENDER_QUEUE_6)
		BIND2("RENDER_QUEUE_7",Ogre::RENDER_QUEUE_7)
		BIND2("RENDER_QUEUE_WORLD_GEOMETRY_2",Ogre::RENDER_QUEUE_WORLD_GEOMETRY_2)
		BIND2("RENDER_QUEUE_8",Ogre::RENDER_QUEUE_8)
		BIND2("RENDER_QUEUE_9",Ogre::RENDER_QUEUE_9)
		BIND2("RENDER_QUEUE_SKIES_LATE",Ogre::RENDER_QUEUE_SKIES_LATE)
		BIND2("RENDER_QUEUE_OVERLAY",Ogre::RENDER_QUEUE_OVERLAY)
		BIND2("RENDER_QUEUE_MAX",Ogre::RENDER_QUEUE_MAX)
	END_ENUM_BINDER(Ogre::RenderQueueGroupID,RenderQueueBinder);
}