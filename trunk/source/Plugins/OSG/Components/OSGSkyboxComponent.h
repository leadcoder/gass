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
#include "Sim/Components/Graphics/Geometry/IMeshComponent.h"
#include "Sim/Components/Graphics/Geometry/IGeometryComponent.h"
#include "Sim/Components/BaseSceneComponent.h"
#include "Sim/Common.h"
#include "Core/Math/Vector.h"
#include "Core/Math/AABox.h"
#include "Core/Math/Sphere.h"
#include "Core/MessageSystem/IMessage.h"


class osg::Billboard;
class osg::Image;
class osg::Drawable;
class osg::Geometry;
class osg::TextureCubeMap;

namespace GASS
{
	


	class OSGSkyboxComponent : public Reflection<OSGSkyboxComponent,BaseSceneComponent>
	{
	public:
		OSGSkyboxComponent (void);
		~OSGSkyboxComponent (void);
		static void RegisterReflection();
		virtual void OnCreate();
		Vec3 GetEyePosition();
	protected:
		void OnChangeCamera(CameraChangedNotifyMessagePtr message);
		void SetMaterial(const std::string &mat) {m_Material = mat;}
		std::string GetMaterial()const {return m_Material;}
		void OnLoad(LoadGFXComponentsMessagePtr message);
		std::string GetTexturePath(const std::string &side) const;

		osg::TextureCubeMap* ReadCubeMap();
		osg::Node* CreateSkyBox();
		std::string m_Material;
		GASS::SceneObjectWeakPtr m_ActiveCameraObject;
	};
}

