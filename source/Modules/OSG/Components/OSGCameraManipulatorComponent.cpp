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
#include "Modules/OSG/Components/OSGCameraManipulatorComponent.h"
#include "Modules/OSG/Components/OSGCameraComponent.h"
#include "Modules/OSG/Utils/MyTrackballManipulator.h"
#include "Modules/OSG/Utils/CustomTerrainManipulator.h"
#include "Modules/OSG/OSGGraphicsSceneManager.h"
#include "Modules/OSG/OSGConvert.h"
#include "Core/Math/GASSMath.h"


namespace GASS
{
	OSGCameraManipulatorComponent::OSGCameraManipulatorComponent() : m_OrbitMan(nullptr),
		
		m_ManName("Terrain")
		
	{

	}

	OSGCameraManipulatorComponent::~OSGCameraManipulatorComponent()
	{

	}

	void OSGCameraManipulatorComponent::RegisterReflection()
	{
		ComponentFactory::Get().Register<OSGCameraManipulatorComponent>();
		RegisterGetSet("Manipulator", &GASS::OSGCameraManipulatorComponent::GetManipulatorName, &GASS::OSGCameraManipulatorComponent::SetManipulatorName);
	}

	void OSGCameraManipulatorComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGCameraManipulatorComponent::OnTransformationChanged, TransformationChangedEvent,0));

		RegisterForPreUpdate<OSGGraphicsSceneManager>();
	
		if(m_ManName == "Trackball")
		{
			m_OrbitMan = new osgGA::MyTrackballManipulator();
		}
		else if(m_ManName == "Terrain")
		{
			m_OrbitMan = new osgGA::CustomTerrainManipulator();
		}
		else
		{
			GASS_EXCEPT(Exception::ERR_INVALIDPARAMS, "Manipulator not supported:" + m_ManName, " OSGCameraManipulatorComponent::OnInitialize");
		}
		m_OrbitMan->setAllowThrow(false);
	}

	void OSGCameraManipulatorComponent::OnTransformationChanged(TransformationChangedEventPtr event)
	{
		if (m_UpdateCameraFromLocation)
		{
			SetPosition(event->GetPosition());
			SetRotation(event->GetRotation());
		}
	}

	void OSGCameraManipulatorComponent::SetOrbitManPosition(osgGA::OrbitManipulator* man, const GASS::Vec3 &pos)
	{
		osg::Vec3d eye, center, up;
		man->getTransformation(eye, center, up);

		//Note: terrain manipulator will try to intersect to get new center pos and may hit skybox (or other unwanted geometries)
		man->setTransformation(OSGConvert::ToOSG(pos), center, up);
	}

	void OSGCameraManipulatorComponent::SetOrbitManRotation(osgGA::OrbitManipulator* man, const GASS::Quaternion &rot)
	{
		osg::Vec3d eye, center, up;
		man->getTransformation(eye, center, up);

		//create new center pos
		Vec3 dir = -rot.GetZAxis();
		//just pick a center pos 10000m away
		dir = dir * 10000.0;
		center = eye + OSGConvert::ToOSG(dir);
		//create new up vector
		up = OSGConvert::ToOSG(rot.GetYAxis());
		//Note: terrain manipulator will try to intersect to get new center pos and may hit sky box (or other unwanted geometries)
		man->setTransformation(eye, center, up);
	}

	void OSGCameraManipulatorComponent::SetPosition(const Vec3& pos)
	{
		if (m_OrbitMan)
		{
			SetOrbitManPosition(m_OrbitMan, pos);
		}
	}

	void OSGCameraManipulatorComponent::SetRotation(const Quaternion& rot)
	{
		if (m_OrbitMan)
		{
			SetOrbitManRotation(m_OrbitMan, rot);
		}
	}

	void OSGCameraManipulatorComponent::ExtractTransformationFromOrbitManipulator(osgGA::OrbitManipulator* man, GASS::Vec3 &pos, GASS::Quaternion &rot)
	{
		osg::Vec3d eye, center, up;
		//Camera position is eye
		man->getTransformation(eye, center, up);
		pos = OSGConvert::ToGASS(eye);

		//Get rotation axis
		const Vec3 z_axis = OSGConvert::ToGASS(eye - center).NormalizedCopy();
		const Vec3 y_axis = OSGConvert::ToGASS(up).NormalizedCopy();
		const Vec3 x_axis = (y_axis.Cross(z_axis)).NormalizedCopy();
		rot.FromAxes(x_axis, y_axis, z_axis);
	}

	void OSGCameraManipulatorComponent::SceneManagerTick(double /*delta*/)
	{
		if (!m_ReadyToRun && m_OrbitMan && m_OrbitMan->getNode())
		{
			//set ReadyToRun=true first frame
			m_ReadyToRun = true;
			const Vec3 initial_pos = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>()->GetWorldPosition();
			const Quaternion initial_rot = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>()->GetWorldRotation();
			SetPosition(initial_pos);
			SetRotation(initial_rot);
			
			osg::Vec3d eye, center, up;
			m_OrbitMan->getTransformation(eye, center, up);
			m_OrbitMan->setHomePosition(eye, center, up);
		}
		
		//update LocationComponent with current camera pos and rot
		Vec3 pos;
		Quaternion rot;
		ExtractTransformationFromOrbitManipulator(m_OrbitMan, pos, rot);

		m_UpdateCameraFromLocation = false;
		GetSceneObject()->GetFirstComponentByClass<ILocationComponent>()->SetWorldPosition(pos);
		GetSceneObject()->GetFirstComponentByClass<ILocationComponent>()->SetWorldRotation(rot);
		m_UpdateCameraFromLocation = true;
	}
}
