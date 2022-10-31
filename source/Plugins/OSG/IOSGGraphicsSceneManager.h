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
#include "Plugins/OSG/OSGCommon.h"

namespace GASS
{
	class IOSGGraphicsSceneManager
	{
	public:
		virtual ~IOSGGraphicsSceneManager(){};
		virtual osg::ref_ptr<osg::Group> GetOSGRootNode() = 0 ;
		virtual void SetMapNode(osg::Group* root) = 0;
		virtual osg::Group* GetMapNode() const= 0;
		virtual void SetMapIsRoot(bool ) = 0;
		virtual bool GetMapIsRoot() const = 0;
		virtual osg::ref_ptr<osg::Group> GetOSGShadowRootNode() = 0;
	};
	using IOSGGraphicsSceneManagerPtr = std::shared_ptr<IOSGGraphicsSceneManager>;
	using IOSGGraphicsSceneManagerWeakPtr = std::weak_ptr<IOSGGraphicsSceneManager>;
}

