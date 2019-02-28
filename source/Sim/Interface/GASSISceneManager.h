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

#pragma once

#include "Sim/GASSCommon.h"

namespace GASS
{
	class Scene;
	typedef GASS_SHARED_PTR<Scene> ScenePtr;
	typedef GASS_WEAK_PTR<Scene> SceneWeakPtr;

	class GASSExport ISceneManagerListener
	{
	public:
		virtual ~ISceneManagerListener() {}
		virtual void SceneManagerTick(double delta) = 0;
	};

	typedef GASS_WEAK_PTR<ISceneManagerListener> SceneManagerListenerWeakPtr;
	typedef GASS_SHARED_PTR<ISceneManagerListener> SceneManagerListenerPtr;

	/**
		Interface that all scene managers must implement.
		A scene manager in GASS is responsible for handling 
		a certain part of a scene, for example graphics, 
		physics or sound. Therefore a scene manager is owned
		by a scene.
	*/

	class ISceneManager
	{
		GASS_DECLARE_CLASS_AS_INTERFACE(ISceneManager)
	public:
		virtual void OnCreate() = 0;
		virtual void OnInit() = 0;
		virtual void OnShutdown() = 0;
		virtual std::string GetName() const = 0;
		virtual void SetName(const std::string &name) = 0;
		virtual ScenePtr GetScene() const = 0;
		virtual void SetScene(ScenePtr owner) = 0;
		virtual void RegisterPreUpdate(SceneManagerListenerPtr listener) = 0;
		virtual void RegisterPostUpdate(SceneManagerListenerPtr listener) = 0;
		//virtual void Unregister(SceneManagerListenerPtr listener) = 0;
		virtual bool GetSerialize() const =0;
	};

	typedef GASS_SHARED_PTR<ISceneManager> SceneManagerPtr;

	
}
