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

#include "Plugins/OSG/Components/OSGNodeProxyComponent.h"
#include "Plugins/OSG/OSGGraphicsSceneManager.h"
#include "Plugins/OSG/Components/OSGLocationComponent.h"
#include "Plugins/OSG/Components/OSGMeshComponent.h"

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
		GASS::ComponentFactory::GetPtr()->Register("OSGNodeProxyComponent",new GASS::Creator<OSGNodeProxyComponent, Component>);
		ADD_DEPENDENCY("OSGLocationComponent")
		RegisterProperty< std::vector<std::string> >("DescriptionList", &OSGNodeProxyComponent::GetDescriptionList, &OSGNodeProxyComponent::SetDescriptionList);
	}

	void OSGNodeProxyComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGNodeProxyComponent::OnLocationLoaded,LocationLoadedEvent,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGNodeProxyComponent::OnGeomChanged,GeometryChangedEvent,0));

		UpdateDescriptionFromNode();
	}

	void OSGNodeProxyComponent::OnLocationLoaded(LocationLoadedEventPtr message)
	{
		UpdateDescriptionFromNode();
	}

	void OSGNodeProxyComponent::OnGeomChanged(GeometryChangedEventPtr message)
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
		if(mesh && mesh->GetNode())
		{
			osg::Node::DescriptionList des_list = mesh->GetNode()->getDescriptions();
			for(size_t i =0 ; i < des_list.size(); i++)
			{
				std::string description = des_list[i];
				m_List.push_back(description); 
			}
		}
	}

	void OSGNodeProxyComponent::SetDescriptionList(const std::vector<std::string> &/*list*/)
	{
		//NOP, only used to reflect node description
	}

	std::vector<std::string> OSGNodeProxyComponent::GetDescriptionList() const
	{
		return m_List;
	}
}
