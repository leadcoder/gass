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
#include "Core/Common.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/Math/GASSVector.h"
#include "Sim/GASSCommon.h"
#include "Sim/Interface/GASSICameraComponent.h"
#include "Sim/GASSBaseSceneComponent.h"
#include "Sim/GASSBaseSceneComponent.h"
#include "Sim/GASSSceneObjectRef.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Plugins/Base/CoreMessages.h"
#include "Core/Utils/GASSEnumBinder.h"
#include "Core/Utils/GASSColorRGB.h"

namespace GASS
{
	/**
		Component that create  line/path (with material and width) 
	*/

	class LineGeometryComponent : public Reflection<LineGeometryComponent,BaseSceneComponent>
	{
	public:
		LineGeometryComponent();
		virtual ~LineGeometryComponent();
		static void RegisterReflection();
		virtual void OnInitialize();
	protected:
		ADD_PROPERTY(float,CustomDitchTexturePercent)
		ADD_PROPERTY(bool,FadeStart)
		ADD_PROPERTY(bool,FadeEnd)
		ADD_PROPERTY(bool,ClampToTerrain)
		ADD_PROPERTY(ColorRGBA,Color)
		ADD_PROPERTY(Vec2,TileScale)
		ADD_PROPERTY(SceneObjectRef,WapointListObject)
		ADD_PROPERTY(bool,RotateTexture)
		
		
		void SetWidth(float value) {m_Width = value;UpdateMesh();}
		float GetWidth() const {return m_Width;}

		void SetOffset(float value) {m_Offset = value;UpdateMesh();}
		float GetOffset() const {return m_Offset;}
		
		void SetMaterial(const std::string &value);
		std::string GetMaterial() const;

		//Vec2 GetTileScale() const {return m_TileScale;}
		//void SetTileScale(const Vec2 &value) {m_TileScale = value;}
		
		void UpdateMesh();
		void OnUpdate(UpdateWaypointListMessagePtr message);

		bool m_Initialized;
		float m_Width;
		float m_Offset;
		std::string m_Material;
	};

	typedef SPTR<LineGeometryComponent> LineComponentPtr;
	typedef WPTR<LineGeometryComponent> LineComponentWeakPtr;

}

