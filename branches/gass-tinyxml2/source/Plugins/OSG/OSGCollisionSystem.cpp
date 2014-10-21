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

#include "Plugins/OSG/OSGCollisionSystem.h"
#include "Plugins/OSG/OSGCollisionSceneManager.h"
#include "Plugins/OSG/OSGConvert.h"
#include "Plugins/OSG/Components/OSGMeshComponent.h"
#include "Plugins/OSG/OSGGraphicsSceneManager.h"
#include "Plugins/OSG/OSGNodeMasks.h"
#include "Plugins/OSG/OSGGraphicsSystem.h"
#include "Plugins/OSG/OSGNodeData.h"


#include <osgUtil/IntersectionVisitor>
#include <osgUtil/LineSegmentIntersector>


namespace GASS
{
	OSGCollisionSystem::OSGCollisionSystem()
	{
	}

	OSGCollisionSystem::~OSGCollisionSystem()
	{

	}

	void OSGCollisionSystem::RegisterReflection()
	{
		SystemFactory::GetPtr()->Register("OSGCollisionSystem",new GASS::Creator<OSGCollisionSystem, ISystem>);
		
	}

	void OSGCollisionSystem::Init()
	{
		SceneManagerFactory::GetPtr()->Register("OSGCollisionSceneManager",new GASS::Creator<OSGCollisionSceneManager, ISceneManager>);
		SimEngine::Get().GetRuntimeController()->Register(shared_from_this(),m_TaskNodeName);
	}
}
