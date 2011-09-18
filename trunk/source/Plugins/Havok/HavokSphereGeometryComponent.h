/****************************************************************************
* This file is part of GASS.                                                *
* See http://cHavok.google.com/p/gass/                                 *
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

#include "Sim/Components/BaseSceneComponent.h"
#include "Sim/Scenario/Scene/SceneObjectMessages.h"
#include "Core/MessageSystem/IMessage.h"
#include "Core/Math/AABox.h"
#include "Core/Math/Quaternion.h"
#include "HavokPhysicsSceneManager.h"
#include "HavokBaseGeometryComponent.h"

namespace GASS
{
	class IGeometryComponent;
	class HavokBodyComponent;
	class HavokPhysicsSceneManager;
	typedef boost::weak_ptr<HavokPhysicsSceneManager> HavokPhysicsSceneManagerWeakPtr;
	typedef boost::shared_ptr<IGeometryComponent> GeometryComponentPtr;

	class HavokSphereGeometryComponent : public Reflection<HavokSphereGeometryComponent,HavokBaseGeometryComponent>
	{
	public:
		HavokSphereGeometryComponent();
		virtual ~HavokSphereGeometryComponent();
		static void RegisterReflection();
		virtual void OnCreate();
	
	protected:
		virtual hkpShape*  CreateHavokShape();
		void SetRadius(Float value);
		Float GetRadius() const;
		void SetSizeFromMesh(bool value);	
		void UpdateBodyMass();
		
		//debug functions
		void CreateDebugSphere(Float radius,const Vec3 &offset);
		void UpdateDebug();
	protected:
		Float m_Radius; 
		hkpSphereShape* m_SphereShape;
	};
}
