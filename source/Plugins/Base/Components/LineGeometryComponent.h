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
#include "Core/Common.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/Math/GASSVector.h"
#include "Sim/GASSComponent.h"
#include "Sim/GASSSceneObjectRef.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Plugins/Base/CoreMessages.h"
#include "Core/Utils/GASSColorRGB.h"

namespace GASS
{
	/**
		Component that create  line/path (with material and width) 
	*/

	class LineGeometryComponent : public Reflection<LineGeometryComponent,Component>
	{
	public:
		LineGeometryComponent();
		~LineGeometryComponent() override;
		static void RegisterReflection();
		void OnInitialize() override;
	protected:
		float m_CustomDitchTexturePercent{0};
		bool m_FadeStart{false};
		bool m_FadeEnd{false};
		bool m_ClampToTerrain{true};
		ColorRGBA m_Color;
		Vec2 m_TileScale;
		SceneObjectRef m_WapointListObject;
		bool m_RotateTexture{false};
		
		void SetWidth(float value) {m_Width = value;UpdateMesh();}
		float GetWidth() const {return m_Width;}

		void SetOffset(float value) {m_Offset = value;UpdateMesh();}
		float GetOffset() const {return m_Offset;}
		
		void SetMaterial(const std::string &value);
		std::string GetMaterial() const;

		//Vec2 GetTileScale() const {return m_TileScale;}
		//void SetTileScale(const Vec2 &value) {m_TileScale = value;}
		
		void UpdateMesh();
		void OnUpdate(WaypointListUpdatedMessagePtr message);

		bool m_Initialized{false};
		float m_Width{10};
		float m_Offset{0.3f};
		std::string m_Material;
	};

	using LineComponentPtr = std::shared_ptr<LineGeometryComponent>;
	using LineComponentWeakPtr = std::weak_ptr<LineGeometryComponent>;

}

