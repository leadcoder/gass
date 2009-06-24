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

#pragma once


#include "Sim/Common.h"
#include "Core/Math/Vector.h"
#include "Sim/Components/Graphics/ICameraComponent.h"
#include "Sim/Components/BaseSceneComponent.h"
#include "Core/MessageSystem/Message.h"

namespace Ogre
{
	class Camera;
}

namespace GASS
{
	class OgreCameraComponent : public Reflection<OgreCameraComponent,BaseSceneComponent>, public ICameraComponent
	{
	public:
		OgreCameraComponent();
		virtual ~OgreCameraComponent();
		static void RegisterReflection();
		virtual void OnCreate();
		virtual bool GetCameraToViewportRay(float screenx, float screeny, Vec3 &ray_start, Vec3 &ray_dir) const;
		inline Ogre::Camera* GetOgreCamera(){return m_Camera;}
		
	protected:
		float GetFarClipDistance() const;
		void SetFarClipDistance(float value);
		float GetNearClipDistance() const;
		void SetNearClipDistance(float value);
		void OnLoad(MessagePtr message);
		Ogre::Camera* m_Camera;
		float m_NearClip;
		float m_FarClip;
	};

	typedef boost::shared_ptr<OgreCameraComponent> OgreCameraComponentPtr;
	
}

