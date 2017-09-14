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
#include "Plugins/OSG/Components/OSGCameraManipulatorComponent.h"
#include "Plugins/OSG/Components/OSGCameraComponent.h"
#include "Plugins/OSG/Utils/MyTrackballManipulator.h"
#include "Plugins/OSG/Utils/CustomTerrainManipulator.h"
#include "Plugins/OSG/OSGGraphicsSceneManager.h"
#include "Plugins/OSG/OSGConvert.h"
#include "Core/Math/GASSMath.h"


namespace GASS
{
	OSGCameraManipulatorComponent::OSGCameraManipulatorComponent() : m_OrbitMan(NULL),
		m_InitPos(0,0,0),
		m_ReadyToRun(false),
		m_ManName("Terrain")
	{

	}

	OSGCameraManipulatorComponent::~OSGCameraManipulatorComponent()
	{

	}

	void OSGCameraManipulatorComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("OSGCameraManipulatorComponent",new Creator<OSGCameraManipulatorComponent, Component>);
		RegisterProperty<std::string>("Manipulator", &GASS::OSGCameraManipulatorComponent::GetManipulatorName, &GASS::OSGCameraManipulatorComponent::SetManipulatorName);
	}

	void OSGCameraManipulatorComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGCameraManipulatorComponent::OnWorldPositionRequest,WorldPositionRequest,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGCameraManipulatorComponent::OnWorldRotationRequest,WorldRotationRequest,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGCameraManipulatorComponent::OnPositionRequest,PositionRequest,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGCameraManipulatorComponent::OnRotationRequest,RotationRequest,0));

		GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<OSGGraphicsSceneManager>()->Register(shared_from_this());

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

	void OSGCameraManipulatorComponent::OnWorldPositionRequest(WorldPositionRequestPtr message)
	{
		const int id = GASS_PTR_TO_INT(this);
		if(message->GetSenderID() != id)
		{
			SetPosition(message->GetPosition());
		}
	}

	void OSGCameraManipulatorComponent::OnWorldRotationRequest(WorldRotationRequestPtr message)
	{
		const int id = GASS_PTR_TO_INT(this);
		if(message->GetSenderID() != id)
		{
			SetRotation(message->GetRotation());
		}
	}

	void OSGCameraManipulatorComponent::OnPositionRequest(PositionRequestPtr message)
	{
		int id = GASS_PTR_TO_INT(this);
		if(message->GetSenderID() != id)
		{
			SetPosition(message->GetPosition());
		}
	}

	void OSGCameraManipulatorComponent::OnRotationRequest(RotationRequestPtr message)
	{
		int id = GASS_PTR_TO_INT(this);
		if(message->GetSenderID() != id)
		{
			SetRotation(message->GetRotation());
		}
	}

	void OSGCameraManipulatorComponent::_SetOrbitManPosition(osgGA::OrbitManipulator* man, const GASS::Vec3 &pos)
	{
		osg::Vec3d eye, center, up;
		man->getTransformation(eye, center, up);

		//Note: terrain manipulator will try to intersect to get new center pos and may hit skybox (or other unwanted geometries)
		man->setTransformation(OSGConvert::ToOSG(pos), center, up);
		//Keep update direction or center point?
		/*osg::Vec3d dir = center - eye;
		dir.normalize();
		dir = dir * 10000.0;
		eye = OSGConvert::ToOSG(pos);
		center = eye + dir;
		m_OrbitMan->setTransformation(eye, center, up);*/
	}

	void OSGCameraManipulatorComponent::_SetOrbitManRotation(osgGA::OrbitManipulator* man, const GASS::Quaternion &rot)
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
		//If not initialized, store position as initial value
		if (!m_ReadyToRun) 
		{
			//just save pos, will be  apply on first scene tick
			m_InitPos = pos;
		}
		else if (m_OrbitMan)
		{
			_SetOrbitManPosition(m_OrbitMan, pos);
		}
	}

	void OSGCameraManipulatorComponent::SetRotation(const Quaternion& rot)
	{
		if (!m_ReadyToRun)
		{
			m_InitRot = rot;
		}
		else if (m_OrbitMan)
		{
			_SetOrbitManRotation(m_OrbitMan, rot);
		}
	}

	void OSGCameraManipulatorComponent::_ExtractTransformationFromOrbitManipulator(osgGA::OrbitManipulator* man, GASS::Vec3 &pos, GASS::Quaternion &rot)
	{
		osg::Vec3d eye, center, up;
		//Camera position is eye
		man->getTransformation(eye, center, up);
		pos = OSGConvert::ToGASS(eye);

		//Get rotation axis
		const GASS::Vec3 z_axis = OSGConvert::ToGASS(eye - center).NormalizedCopy();
		const GASS::Vec3 y_axis = OSGConvert::ToGASS(up).NormalizedCopy();
		const GASS::Vec3 x_axis = (y_axis.Cross(z_axis)).NormalizedCopy();
		rot.FromAxes(x_axis, y_axis, z_axis);
	}

	void OSGCameraManipulatorComponent::SceneManagerTick(double /*delta*/)
	{
		if (!m_ReadyToRun && m_OrbitMan && m_OrbitMan->getNode())
		{
			//set ReadyToRun=true first, used inside SetPosition and SetRotation
			m_ReadyToRun = true;
			SetPosition(m_InitPos);
			SetRotation(m_InitRot);
			
			osg::Vec3d eye, center, up;
			m_OrbitMan->getTransformation(eye, center, up);
			m_OrbitMan->setHomePosition(eye, center, up);
		}
		
		const int id = GASS_PTR_TO_INT(this);

		//update LocationComponent with current camera pos and rot
		GASS::Vec3 pos;
		GASS::Quaternion rot;
		_ExtractTransformationFromOrbitManipulator(m_OrbitMan, pos, rot);
		GetSceneObject()->PostRequest(WorldPositionRequestPtr(new WorldPositionRequest(pos, id)));
		GetSceneObject()->PostRequest(WorldRotationRequestPtr(new WorldRotationRequest(rot, id)));
	}
}
