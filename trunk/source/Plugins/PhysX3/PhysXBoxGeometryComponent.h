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

#include "PhysXCommon.h"
#include "PhysXBaseGeometryComponent.h"


namespace GASS
{
	class ITerrainComponent;
	class IGeometryComponent;
	class PhysXPhysicsSceneManager;
	typedef WPTR<PhysXPhysicsSceneManager> PhysXPhysicsSceneManagerWeakPtr;
	typedef SPTR<IGeometryComponent> GeometryComponentPtr;

	class PhysXBoxGeometryComponent : public Reflection<PhysXBoxGeometryComponent,PhysXBaseGeometryComponent>
	{
	friend class PhysXPhysicsSceneManager;
	public:
		PhysXBoxGeometryComponent();
		virtual ~PhysXBoxGeometryComponent();
		static void RegisterReflection();
		virtual void OnInitialize();
	protected:
		physx::PxShape* CreateShape();
		void SetSize(const Vec3 &size);
		Vec3 GetSize() const;

		//debug functions
		void CreateDebugBox(const Vec3 &size,const Vec3 &offset);
		void UpdateDebug();
	protected:
		
		Vec3 m_Size; 
	};
}
