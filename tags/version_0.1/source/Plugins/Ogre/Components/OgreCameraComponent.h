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
#include "Sim/Scenario/Scene/Messages/GraphicsSceneObjectMessages.h"
#include "Sim/Utils/EnumBinder.h"
#include "Plugins/Ogre/IOgreCameraProxy.h"
#include "Plugins/Ogre/OgrePolygonMode.h"

namespace Ogre
{
	class Camera;
}

namespace GASS
{
	class OgreCameraComponent : public Reflection<OgreCameraComponent,BaseSceneComponent>, public ICameraComponent, public IOgreCameraProxy
	{
	public:
		OgreCameraComponent();
		virtual ~OgreCameraComponent();
		static void RegisterReflection();
		virtual void OnCreate();
		virtual bool GetCameraToViewportRay(float screenx, float screeny, Vec3 &ray_start, Vec3 &ray_dir) const;
		inline Ogre::Camera* GetOgreCamera() const {return m_Camera;}
		std::vector<std::string> GetPostFilters() const;
		void SetPostFilters(const std::vector<std::string> &filters);
	protected:
		void OnParameter(CameraParameterMessagePtr message);
		float GetFarClipDistance() const;
		void SetFarClipDistance(float value);
		float GetNearClipDistance() const;
		void SetNearClipDistance(float value);
		float GetFov() const;
		void SetFov(float value);
		bool GetOrtho() const;
		void SetOrtho(bool value);
		bool GetClipToFog() const {return m_ClipToFog;}
		void SetClipToFog(bool value) {m_ClipToFog = value;}
		PolygonModeWrapper GetPolygonMode() const {return m_PolygonMode;}
		void SetPolygonMode(PolygonModeWrapper value);
		
		void OnLoad(LoadGFXComponentsMessagePtr message);
		Ogre::Camera* m_Camera;
		float m_NearClip;
		float m_FarClip;
		float m_Fov;
		bool m_Ortho;
		bool m_ClipToFog;
		std::vector<std::string> m_PostFilters;
		PolygonModeWrapper m_PolygonMode;

	};

	typedef boost::shared_ptr<OgreCameraComponent> OgreCameraComponentPtr;
	
}
