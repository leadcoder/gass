/****************************************************************************
* This file is part of GASS.                                                *
* See http://sourceforge.net/projects/gass/                                 *
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
#include "Sim/GASS.h"
#include <osg/Camera>
#include <osg/ref_ptr>
#include <osgGA/CameraManipulator>

namespace GASS
{
	class IOSGCameraManipulator
	{
	public:
		virtual ~IOSGCameraManipulator(){};
		virtual osg::ref_ptr<osgGA::CameraManipulator> GetManipulator() const = 0;
	protected:
	};

	typedef SPTR<IOSGCameraManipulator> OSGCameraManipulatorPtr;
	typedef WPTR<IOSGCameraManipulator> OSGCameraManipulatorWeakPtr;
}
