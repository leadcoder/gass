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

#include "HeightmapComponent.h"
#include "Plugins/Base/CoreMessages.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/Utils/GASSLogManager.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/Interface/GASSIGeometryComponent.h"
#include "Sim/GASSGraphicsMesh.h"
#include "Sim/Interface/GASSILocationComponent.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"

namespace GASS
{
	HeightmapComponent::HeightmapComponent(void) : m_Data(NULL)
	{

	}

	HeightmapComponent::~HeightmapComponent(void)
	{
		
	}

	void HeightmapComponent::RegisterReflection()
	{
		GASS::ComponentFactory::GetPtr()->Register("HeightmapComponent",new GASS::Creator<HeightmapComponent, IComponent>);
		GetClassRTTI()->SetMetaData(ClassMetaDataPtr(new ClassMetaData("HeightmapComponent", OF_VISIBLE)));
		
	}

	void HeightmapComponent::OnInitialize()
	{
		UpdateData();
	}

	void HeightmapComponent::UpdateData()
	{
	
		//generate heightmap
		//Float corner_x ;
		//Float corner_z;
		GeometryFlags flags;
		ScenePtr scene = GetSceneObject()->GetScene();
/*		for(int i = 0; i < m_Width; i++)
		{
			for(int j = 0; j < m_Height; j++)
			{
				Vec3 pos(m_Box.x + i*dist,0,m_Box.z + j*dist); 
				Float h = CollisionHelper::GetHeightAtPosition(scene, pos, flags, true);
			}
		}*/
	}

}
