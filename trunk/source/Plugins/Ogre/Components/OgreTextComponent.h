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
#ifndef OGRE_TEXT_COMPONENT_H
#define OGRE_TEXT_COMPONENT_H

#include "Sim/Components/Graphics/Geometry/GASSIGeometryComponent.h"
#include "Sim/Components/GASSBaseSceneComponent.h"
#include "Sim/Scene/GASSGraphicsSceneObjectMessages.h"
#include "Sim/GASSCommon.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include <OgreRenderTargetListener.h>

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
		virtual void preViewportUpdate(const Ogre::RenderTargetViewportEvent& evt);
		void OnGeomChanged(GeometryChangedMessagePtr message);
		void OnCaptionMessage(TextCaptionMessagePtr message);
		std::string GetText() const;
		void SetText(const std::string  &text);
		float GetOffset() const;
		void SetOffset(float offset);
		float GetCharacterSize() const;
		void SetCharacterSize(float size);
		void OnVisibilityMessage(VisibilityMessagePtr message);
	
		MovableTextOverlay* m_TextObject;
		MovableTextOverlayAttributes* m_Attribs;
		std::string m_TextToDisplay;
		float m_Offset;
		float m_Size;
		Vec4 m_Color;
		bool m_Visible;
	private:
	};
}
#endif
