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
#include "Sim/Common.h"
#include "Core/Math/Vector.h"
#include "Sim/Components/Graphics/ICameraComponent.h"
#include "Sim/Components/BaseSceneComponent.h"
#include "Sim/Scenario/Scene/SceneObjectMessages.h"
#include "Core/MessageSystem/IMessage.h"


namespace GASS
{
	class OSGCameraComponent : public Reflection<OSGCameraComponent,BaseSceneComponent>, public ICameraComponent
	{
	public:
		OSGCameraComponent();
		virtual ~OSGCameraComponent();
		static void RegisterReflection();
		virtual void OnCreate();
		//ICameraComponent interface
		virtual bool GetCameraToViewportRay(float screenx, float screeny, Vec3 &ray_start, Vec3 &ray_dir) const;

	protected:
		void OnPositionChanged(PositionMessagePtr message);
		void OnRotationChanged(RotationMessagePtr message);
		void OnLoad(LoadGFXComponentsMessagePtr message);
		void UpdateFromLocation();
	
		osg::Camera* m_OSGCamera;
	};
}

