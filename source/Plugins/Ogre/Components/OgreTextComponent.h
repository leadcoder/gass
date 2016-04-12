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
#include "Plugins/Ogre/OgreCommon.h"

#include "Sim/Interface/GASSIGeometryComponent.h"
#include "Sim/GASSBaseSceneComponent.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"

#include "Core/MessageSystem/GASSIMessage.h"


class MovableTextOverlay;
class MovableTextOverlayAttributes;

namespace GASS
{
	class OgreTextComponent : public Reflection<OgreTextComponent,BaseSceneComponent> ,  public Ogre::RenderTargetListener
	{
	public:
		OgreTextComponent(void);
		~OgreTextComponent(void);
		static void RegisterReflection();
		virtual void OnInitialize();
		virtual AABox GetBoundingBox()const;
		virtual Sphere GetBoundingSphere() const;
	protected:
		ADD_PROPERTY(bool,ScaleByDistance)
		virtual void preViewportUpdate(const Ogre::RenderTargetViewportEvent& evt);
		void OnGeomChanged(GeometryChangedEventPtr message);
		void OnCaptionMessage(TextCaptionRequestPtr message);
		std::string GetText() const;
		void SetText(const std::string  &text);
		float GetOffset() const;
		void SetOffset(float offset);
		int GetCharacterSize() const;
		void SetCharacterSize(int size);
		void OnVisibilityMessage(LocationVisibilityRequestPtr message);
		void OnDelete();
		MovableTextOverlay* m_TextObject;
		MovableTextOverlayAttributes* m_Attribs;
		std::string m_TextToDisplay;
		float m_Offset;
		int m_Size;
		Vec4 m_Color;
		bool m_Visible;
	private:
	};
}