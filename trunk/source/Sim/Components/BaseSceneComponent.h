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

#ifndef BASESCENECOMPONENT_HH
#define BASESCENECOMPONENT_HH

#include "Sim/Common.h"
#include "Sim/Scenario/Scene/SceneObject.h"
#include "Core/ComponentSystem/BaseComponent.h"
#include "Core/MessageSystem/IMessage.h"
namespace GASS
{
	//class SceneObject;
	
	//typedef boost::shared_ptr<SceneObject> SceneObjectPtr;

	class GASSExport BaseSceneComponent : public Reflection<BaseSceneComponent, BaseComponent> , public boost::enable_shared_from_this<BaseSceneComponent>, public IMessageListener
	{
		friend class SceneObject;
	public:
		BaseSceneComponent();
		virtual ~BaseSceneComponent();
		SceneObjectPtr GetSceneObject() const;
		//called when component is created
		virtual void OnCreate();
	protected:
		void InitializePointers();

	};
	typedef boost::shared_ptr<BaseSceneComponent> BaseSceneComponentPtr;
	typedef boost::weak_ptr<BaseSceneComponent> BaseSceneComponentWeakPtr;
}
#endif // #ifndef BASESCENECOMPONENT_HH
