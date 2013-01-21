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

#include "Plugins/OSG/OSGGraphicsSceneManager.h"
#include "Plugins/OSG/OSGGraphicsSystem.h"
#include "Plugins/OSG/Components/OSGLocationComponent.h"
#include "Plugins/OSG/Components/OSGMeshComponent.h"

#include "Plugins/OSG/OSGConvert.h"
#include "Plugins/OSG/Components/OSGNodeProxyComponent.h"
#include "Plugins/OSG/OSGNodeMasks.h"
#include "Plugins/OSG/OSGNodeData.h"

#include <osgDB/ReadFile> 
#include <osgUtil/Optimizer>
#include <osg/MatrixTransform>
#include <osgShadow/ShadowTechnique>
#include <osg/Material>
#include <osg/BlendFunc>



namespace GASS
{

	OSGNodeProxyComponent::OSGNodeProxyComponent() 
	{
		
	}


	OSGNodeProxyComponent::~OSGNodeProxyComponent()
	{

	}

	void OSGNodeProxyComponent::RegisterReflection()
	{
		GASS::ComponentFactory::GetPtr()->Register("OSGNodeProxyComponent",new GASS::Creator<OSGNodeProxyComponent, IComponent>);
		RegisterVectorProperty< std::string >("DescriptionList", &OSGNodeProxyComponent::GetDescriptionList, &OSGNodeProxyComponent::SetDescriptionList);
	}

	void OSGNodeProxyComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGNodeProxyComponent::OnLocationLoaded,LocationLoadedMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGNodeProxyComponent::OnGeomChanged,GeometryChangedMessage,0));

		UpdateDescriptionFromNode();
	}

	void OSGNodeProxyComponent::OnLocationLoaded(LocationLoadedMessagePtr message)
	{
		UpdateDescriptionFromNode();
	}

	void OSGNodeProxyComponent::OnGeomChanged(GeometryChangedMessagePtr message)
	{
		UpdateDescriptionFromNode();
	}

	void OSGNodeProxyComponent::UpdateDescriptionFromNode()
	{
		m_List.clear();
		OSGLocationComponentPtr location = GetSceneObject()->GetFirstComponentByClass<OSGLocationComponent>();
		if(location && location->GetOSGNode())
		{
			osg::Node::DescriptionList des_list = location->GetOSGNode()->getDescriptions();
			for(size_t i =0 ; i < des_list.size(); i++)
			{
				std::string description = des_list[i];
				m_List.push_back(description); 
			}
		}

		OSGMeshComponentPtr mesh = GetSceneObject()->GetFirstComponentByClass<OSGMeshComponent>();
		if(mesh && mesh->GetMeshNode())
		{
			osg::Node::DescriptionList des_list = mesh->GetMeshNode()->getDescriptions();
			for(size_t i =0 ; i < des_list.size(); i++)
			{
				std::string description = des_list[i];
				m_List.push_back(description); 
			}
		}
	}

	void OSGNodeProxyComponent::SetDescriptionList(const std::vector<std::string> &list)
	{
		//m_List  = filename;
	}

	std::vector<std::string> OSGNodeProxyComponent::GetDescriptionList() const
	{
		return m_List;
	}
	
}
