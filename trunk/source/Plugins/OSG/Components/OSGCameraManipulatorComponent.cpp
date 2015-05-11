/****************************************************************************
* This file is part of GASS.                                                *
* See http://code.google.com/p/gass/                                        *
*                                                                           *
* Copyright (c) 2008-2015 GASS team. See Contributors.txt for details.      *
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
#include <osgViewer/Viewer>
#include <osgViewer/CompositeViewer>
#include <osgGA/NodeTrackerManipulator>
#include <osgGA/TerrainManipulator>

#include <osgGA/TrackballManipulator>
#include <osg/MatrixTransform>
#include <osgShadow/ShadowTechnique>

#include <GL/gl.h>
#include <GL/glu.h>


#include "Plugins/OSG/Components/OSGCameraComponent.h"
#include "Plugins/OSG/Utils/MyTrackballManipulator.h"
#include "Plugins/OSG/Utils/CustomTerrainManipulator.h"

#include "Plugins/OSG/OSGNodeMasks.h"
#include "Plugins/OSG/OSGGraphicsSceneManager.h"
#include "Plugins/OSG/OSGGraphicsSystem.h"
#include "Plugins/OSG/OSGConvert.h"
#include "Plugins/OSG/Components/OSGLocationComponent.h"

namespace GASS
{
	OSGCameraManipulatorComponent::OSGCameraManipulatorComponent() : m_ManName("Terrain"),
		m_TerrainMan(NULL),
		m_InitPos(0,0,0),
		m_ReadyToRun(false)
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

		//osgGA::TrackballManipulator *Tman1 = new osgGA::TrackballManipulator();
		if(m_ManName == "Trackball")
		{
			m_Manipulator = new osgGA::MyTrackballManipulator();
		}
		else if(m_ManName == "Terrain")
		{
			m_TerrainMan = new osgGA::CustomTerrainManipulator();
			m_TerrainMan->setAllowThrow(false);
			m_Manipulator = m_TerrainMan;
			
		}
		//if(m_Manipulator)
		//	m_Manipulator->setAutoComputeHomePosition(false);
	}

	void OSGCameraManipulatorComponent::OnWorldPositionRequest(WorldPositionRequestPtr message)
	{
		int id = PTR_TO_INT(this);
		if(message->GetSenderID() != id)
		{
			SetPosition(message->GetPosition());
		}
	}

	void OSGCameraManipulatorComponent::OnWorldRotationRequest(WorldRotationRequestPtr message)
	{
		int id = PTR_TO_INT(this);
		if(message->GetSenderID() != id)
		{
			
			SetRotation(message->GetRotation());
		}
	}

	void OSGCameraManipulatorComponent::OnPositionRequest(PositionRequestPtr message)
	{
		int id = PTR_TO_INT(this);
		if(message->GetSenderID() != id)
		{
			SetPosition(message->GetPosition());
		}
	}

	void OSGCameraManipulatorComponent::OnRotationRequest(RotationRequestPtr message)
	{
		int id = PTR_TO_INT(this);
		if(message->GetSenderID() != id)
		{
			SetRotation(message->GetRotation());
		}
	}

	void OSGCameraManipulatorComponent::SetPosition(const Vec3& pos)
	{
		if(m_TerrainMan)
		{

			if(m_TerrainMan && !m_ReadyToRun)
			{
				//cache data and apply during scene tick
				m_InitPos = pos;
			}

			if(m_TerrainMan)
			{
				osg::Vec3d eye, center,up;
				m_TerrainMan->getTransformation(eye, center,up);
				osg::Vec3d dir = center - eye;
				dir.normalize();
				dir = dir * 10000.0;
				eye = OSGConvert::Get().ToOSG(pos);
				center = eye + dir;
				m_TerrainMan->setTransformation(eye, center,up);

			}
			else
			{
				osg::Matrixd vm = m_Manipulator->getMatrix();
				vm.setTrans(OSGConvert::Get().ToOSG(pos));
				m_Manipulator->setByMatrix(vm);
			}
		}
	}



	void OSGCameraManipulatorComponent::SetRotation(const Quaternion& rot)
	{

		if(m_TerrainMan && !m_ReadyToRun)
		{
			//cache data and apply during scene tick
			m_InitRot = rot;
		}

		if(m_TerrainMan)
		{
			Vec3 dir = -rot.GetZAxis();
			dir = dir * 10000.0;

			osg::Vec3d eye, center,up;
			m_TerrainMan->getTransformation(eye, center,up);
			center = eye + OSGConvert::Get().ToOSG(dir);
			up = OSGConvert::Get().ToOSG(rot.GetYAxis());
			m_TerrainMan->setTransformation(eye, center,up);

			//osg::Quat offset_rot = osg::Quat(Math::Deg2Rad(90),osg::Vec3(1,0,0));
			//osg::Quat rotation; 
			//osg::Vec3d eye;
			//m_TerrainMan->getTransformation(eye, rotation);
			//rotation = OSGConvert::Get().ToOSG(rot);
			//osgGA::OrbitManipulator* orb_man = m_TerrainMan;
			//orb_man->setTransformation(eye, offset_rot*rotation);
		}
		else
		{
			osg::Matrixd vm = m_Manipulator->getMatrix();
			osg::Vec3d translation = vm.getTrans();
			vm.identity();
			//vm.setRotate(OSGConvert::Get().ToOSG(message->GetRotation()));
			osg::Quat rotation = osg::Quat(Math::Deg2Rad(90),osg::Vec3(1,0,0)) * OSGConvert::Get().ToOSG(rot);
			vm.setTrans(translation);
			vm.setRotate(rotation);
			m_Manipulator->setByMatrix(vm);
		}
	}

	void OSGCameraManipulatorComponent::SceneManagerTick(double delta)
	{
		//update location
		if(m_Manipulator.valid())
		{
			if(!m_ReadyToRun && m_Manipulator && m_Manipulator->getNode())
			{
				m_ReadyToRun = true;
				SetPosition(m_InitPos);
				SetRotation(m_InitRot);
				Vec3 eye(m_InitPos.x,m_InitPos.y+200,m_InitPos.z);
				osg::Vec3d up(1.0,0,0);
				m_TerrainMan->setTransformation(OSGConvert::Get().ToOSG(eye), OSGConvert::Get().ToOSG(m_InitPos),up);
				m_Manipulator->setHomePosition(OSGConvert::Get().ToOSG(eye), OSGConvert::Get().ToOSG(m_InitPos),up);
			}
			int id = PTR_TO_INT(this);
			if(m_TerrainMan)
			{
				osg::Quat offset_rot = osg::Quat(Math::Deg2Rad(90),osg::Vec3(1,0,0));
				osg::Quat rotation; 
				osg::Vec3d eye;
				m_TerrainMan->getTransformation(eye, rotation);
				GetSceneObject()->PostRequest(WorldPositionRequestPtr(new WorldPositionRequest(OSGConvert::Get().ToGASS(eye),id)));
			}else
			{
				osg::Matrixd vm = m_Manipulator->getMatrix();
				osg::Vec3d translation,scale;
				osg::Quat rotation;
				osg::Quat so;
				
				vm.decompose(translation,rotation, scale, so );
				GetSceneObject()->PostRequest(WorldPositionRequestPtr(new WorldPositionRequest(OSGConvert::Get().ToGASS(translation),id)));
			}
			//GetSceneObject()->PostMessage(MessagePtr(new WorldPositionRequest(OSGConvert::Get().ToGASS(translation))));
		}
	}
}

