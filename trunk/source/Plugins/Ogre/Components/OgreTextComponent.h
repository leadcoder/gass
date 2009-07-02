
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

#include "Sim/Components/Graphics/Geometry/IGeometryComponent.h"
#include "Sim/Components/BaseSceneComponent.h"
#include "Sim/Common.h"
#include "Core/MessageSystem/Message.h"
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
		void OnLoad(MessagePtr message);
		std::string GetText() const;
		void SetText(const std::string  &text);
		float GetOffset() const;
		void SetOffset(float offset);
		float GetCharacterSize() const;
		void SetCharacterSize(float size);
		bool GetShowNodeName() const;
		void SetShowNodeName(bool value);
	
		MovableTextOverlay* m_TextObject;
		MovableTextOverlayAttributes* m_Attribs;
		std::string m_TextToDisplay;
		float m_Offset;
		float m_Size;
		bool m_ShowNodeName;
		Vec4 m_Color;
	private:
	};
}
#endif
