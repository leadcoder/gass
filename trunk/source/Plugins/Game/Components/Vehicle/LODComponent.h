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

#ifndef LOD_COMPONENT_H
#define LOD_COMPONENT_H

#include "Sim/Components/GASSBaseSceneComponent.h"
#include "Sim/Scene/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Scene/GASSCoreSceneObjectMessages.h"
#include "Sim/Scene/GASSCoreSceneMessages.h"
#include "Sim/Scene/GASSGraphicsSceneMessages.h"
#include "Sim/GASSCommon.h"
#include "Plugins/Game/GameMessages.h"


namespace GASS
{
	class SceneObject;
	typedef boost::shared_ptr<SceneObject> SceneObjectPtr;
	typedef boost::weak_ptr<SceneObject> SceneObjectWeakPtr;

	class LODComponent :  public Reflection<LODComponent,BaseSceneComponent>
	{
	public:
		LODComponent();
		virtual ~LODComponent();
		static void RegisterReflection();
		virtual void OnInitialize();
		virtual void OnDelete();
	private:
		void OnCameraMoved(TransformationNotifyMessagePtr message);
		void OnObjectMoved(TransformationNotifyMessagePtr message);
		void OnChangeCamera(CameraChangedNotifyMessagePtr message);

		void SetMediumLODDistance(float value) {m_MediumLODDistance = value;}
		void SetLowLODDistance(float value) {m_LowLODDistance = value;}
		float GetMediumLODDistance() const {return m_MediumLODDistance;}
		float GetLowLODDistance() const {return m_LowLODDistance;}
		void UpdateLOD();
		
		float m_MediumLODDistance;
		float m_LowLODDistance;
		SceneObjectWeakPtr m_ActiveCameraObject;
		Vec3 m_ObjectPosition;
		Vec3 m_CameraPosition;
		LODMessage::LODLevel m_CurrentLevel;
	};
}
#endif
