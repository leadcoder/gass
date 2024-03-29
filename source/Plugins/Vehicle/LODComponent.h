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

#ifndef LOD_COMPONENT_H
#define LOD_COMPONENT_H

#include "Sim/GASSCommon.h"
#include "Sim/GASSComponent.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Messages/GASSGraphicsSystemMessages.h"
#include "Sim/Messages/GASSPlatformMessages.h"

namespace GASS
{
	class SceneObject;
	using SceneObjectPtr = std::shared_ptr<SceneObject>;
	using SceneObjectWeakPtr = std::weak_ptr<SceneObject>;

	class LODComponent :  public Reflection<LODComponent,Component>
	{
	public:
		LODComponent();
		~LODComponent() override;
		static void RegisterReflection();
		void OnInitialize() override;
		void OnDelete() override;
	private:
		void OnCameraMoved(TransformationChangedEventPtr message);
		void OnObjectMoved(TransformationChangedEventPtr message);
		void OnCameraChanged(CameraChangedEventPtr message);

		void SetMediumLODDistance(float value) {m_MediumLODDistance = value;}
		void SetLowLODDistance(float value) {m_LowLODDistance = value;}
		float GetMediumLODDistance() const {return m_MediumLODDistance;}
		float GetLowLODDistance() const {return m_LowLODDistance;}
		void UpdateLOD();
		
		float m_MediumLODDistance{10};
		float m_LowLODDistance{20};
		SceneObjectWeakPtr m_ActiveCameraObject;
		Vec3 m_ObjectPosition;
		Vec3 m_CameraPosition;
		LODMessage::LODLevel m_CurrentLevel{LODMessage::LOD_HIGH};
	};
}
#endif
