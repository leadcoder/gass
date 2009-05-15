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
#   pragma warning (disable : 4541)
#include "Sim/Common.h"
#include <osg/PositionAttitudeTransform>
#include "Core/Math/Vector.h"
#include "Core/Math/Quaternion.h"
#include "Sim/Components/Graphics/ILocationComponent.h"
#include "Sim/Components/BaseSceneComponent.h"
#include "Core/MessageSystem/Message.h"


namespace Ogre
{
	class SceneNode;
}

namespace GASS
{
	class OSGLocationComponent : public Reflection<OSGLocationComponent,BaseSceneComponent>, public ILocationComponent
	{
	public:
		OSGLocationComponent();
		virtual ~OSGLocationComponent();
		static void RegisterReflection();
		virtual void OnCreate();
	
		virtual void SetScale(const Vec3 &value);
		//virtual void SetVisible(bool value);
		//virtual bool GetVisible();
		virtual void SetPosition(const Vec3 &value);
		virtual Vec3 GetPosition() const;
		virtual void SetEulerRotation(const Vec3 &value);
		virtual void SetRotation(const Quaternion &value);
		virtual Vec3 GetEulerRotation() const;
		virtual Quaternion GetRotation() const;
		virtual Vec3 GetScale(){return m_Scale;}
		osg::ref_ptr<osg::PositionAttitudeTransform> GetOSGNode() {return m_TransformNode;}
	protected:
		void OnLoad(MessagePtr message);
		void PositionChanged(MessagePtr message);
		void RotationChanged(MessagePtr message);
		Vec3 m_Pos;
		Vec3 m_LastPos;

		//! relative rotation of the scene node.
		Vec3 m_Rot;
		Vec3 m_LastRot;

		//! relative scale of the scene node.
		Vec3 m_Scale;
		osg::ref_ptr<osg::PositionAttitudeTransform> m_TransformNode;
		osg::ref_ptr<osg::PositionAttitudeTransform> m_RotTransformNode;
	};
	typedef boost::shared_ptr<OSGLocationComponent>  OSGLocationComponentPtr;
}

