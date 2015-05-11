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

#include "LookAtComponent.h"
#include "Plugins/Base/CoreMessages.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Plugins/Base/GASSCoreSceneManager.h"
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
	LookAtComponent::LookAtComponent(void)
	{

	}

	LookAtComponent::~LookAtComponent(void)
	{
		
	}

	void LookAtComponent::RegisterReflection()
	{
		GASS::ComponentFactory::GetPtr()->Register("LookAtComponent",new GASS::Creator<LookAtComponent, Component>);
		GetClassRTTI()->SetMetaData(ClassMetaDataPtr(new ClassMetaData("LookAtComponent", OF_VISIBLE)));
		REG_PROPERTY(SceneObjectRef,LookAt,GASS::LookAtComponent);
	}

	void LookAtComponent::OnInitialize()
	{
		InitializeSceneObjectRef();
		m_LookAt->RegisterForMessage(REG_TMESS(LookAtComponent::OnTransformation,TransformationChangedEvent,0));
		CoreSceneManagerPtr scene_manager = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<CoreSceneManager>();
		scene_manager->Register(shared_from_this());

	}

	void LookAtComponent::SceneManagerTick(double delta)
	{
		
		Vec3 my_pos  = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>()->GetWorldPosition();
		Quaternion my_rot  = GetSceneObject()->GetParentSceneObject()->GetFirstComponentByClass<ILocationComponent>()->GetWorldRotation();
		Mat4 rot_mat;
		rot_mat.Identity();
		//m_LookAtRot.ToRotationMatrix(rot_mat);
		my_rot.ToRotationMatrix(rot_mat);
		Vec3 right_vec = rot_mat.GetXAxis();


		Vec3 look_dir = m_LookAtPos  - my_pos;
		look_dir.Normalize();
		Quaternion q;

		Vec3 z = Math::Cross(look_dir,right_vec);
		z.Normalize();
		Vec3 x = Math::Cross(look_dir,z);
		z.Normalize();
		q.FromAxes(x,look_dir,z);
		GetSceneObject()->PostRequest(WorldRotationRequestPtr(new WorldRotationRequest(q)));
	}

	void LookAtComponent::OnTransformation(TransformationChangedEventPtr message)
	{
		m_LookAtPos = message->GetPosition();
		m_LookAtRot = message->GetRotation();
	}
}
