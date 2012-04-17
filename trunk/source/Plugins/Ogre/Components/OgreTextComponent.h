
/****************************************************************************
*                                                                           *
* HiFiEngine                                                                *
* Copyright (C)2003 - 2005 Johan Hedstrom                                   *
* Email: hifiengine@gmail.com                                               *
* Web page: http://n00b.dyndns.org/HiFiEngine                               *
*                                                                           *
* HiFiEngine is only used with knowledge from the author. This software     *
* is not allowed to redistribute without permission from the author.        *
* For further license information, please turn to the product home page or  *
* contact author. Abuse against the HiFiEngine license is prohibited by law.*
*                                                                           *
*****************************************************************************/ 
#ifndef OGRE_TEXT_COMPONENT_H
#define OGRE_TEXT_COMPONENT_H

#include "Sim/Components/Graphics/Geometry/GASSIGeometryComponent.h"
#include "Sim/Components/GASSBaseSceneComponent.h"
#include "Sim/Scenario/Scene/Messages/GASSGraphicsSceneObjectMessages.h"
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
		virtual void OnCreate();
		virtual AABox GetBoundingBox()const;
		virtual Sphere GetBoundingSphere() const;
	protected:
		virtual void preViewportUpdate(const Ogre::RenderTargetViewportEvent& evt);
		void OnLoad(LoadGFXComponentsMessagePtr message);
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
