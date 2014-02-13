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
#ifndef LOOK_AT_COMPONENT_H
#define LOOK_AT_COMPONENT_H

#include "Sim/GASSBaseSceneComponent.h"
#include "Sim/Interface/GASSIShape.h"
#include "Sim/GASSCommon.h"
#include "Sim/GASSSceneOBjectRef.h"
#include "Core/Utils/GASSColorRGB.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"

namespace GASS
{

	/**
		Component that roatate scene object to target object 
	*/

	class LookAtComponent : public Reflection<LookAtComponent,BaseSceneComponent>
	{
	public:
		LookAtComponent(void);
		~LookAtComponent(void);
		static void RegisterReflection();
		virtual void OnInitialize();
		virtual void SceneManagerTick(double delta);
	protected:
		void OnTransformation(TransformationNotifyMessagePtr message);
		ADD_PROPERTY(SceneObjectRef,LookAt);
	private:
		Vec3 m_LookAtPos;
		Quaternion m_LookAtRot;
	};
}
#endif
