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

#pragma once
#include "PhysXCommon.h"
namespace physx
{
	class PxScene;
}

namespace GASS
{
	class PhysXBodyComponent;
	typedef boost::shared_ptr<PhysXBodyComponent> PhysXBodyComponentPtr;

	class PhysXPhysicsSceneManager  : public Reflection<PhysXPhysicsSceneManager, BaseSceneManager>
	{
	public:
		PhysXPhysicsSceneManager();
		virtual ~PhysXPhysicsSceneManager();
		static void RegisterReflection();
		virtual void OnCreate();
		physx::PxScene* GetPxScene() {return m_PxScene;}
	protected:
		void SystemTick(double delta);
		void OnLoad(LoadSceneManagersMessagePtr message);
		void OnUnload(UnloadSceneManagersMessagePtr message);
		void OnLoadSceneObject(SceneObjectCreatedNotifyMessagePtr message);
		void SetGravity(float gravity);
		float GetGravity() const;
	private:
		float m_Gravity;
		bool m_Paused;
		bool m_Init;
		physx::PxScene *m_PxScene;
		physx::PxDefaultCpuDispatcher* m_CpuDispatcher;
		//physx::PxSimulationFilterShader m_DefaultFilterShader;
		//physx::PxDefaultSimulationFilterShader m_DefaultFilterShader;
		std::vector<PhysXBodyComponentPtr> m_Bodies;
	};
	typedef boost::shared_ptr<PhysXPhysicsSceneManager> PhysXPhysicsSceneManagerPtr;
}
