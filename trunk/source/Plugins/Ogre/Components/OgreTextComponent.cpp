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

#include <OgreSceneNode.h>
#include <OgreEntity.h>
#include <OgreSubEntity.h>
#include <OgreSceneManager.h>
#include <OgreTextureUnitState.h>
#include <OgreSkeletonInstance.h>
#include <OgreMaterialManager.h>
#include <OgreManualObject.h>
#include <OgreCommon.h>
#include <OgreUTFString.h>

#include "OgreTextComponent.h"
#include "Plugins/Ogre/Helpers/MovableTextOverlay.h"
#include "Plugins/Ogre/Helpers/RectLayoutManager.h"
#include "Plugins/Ogre/Components/OgreLocationComponent.h"
#include "Plugins/Ogre/Components/OgreMeshComponent.h"
#include "Plugins/Ogre/Components/OgreBillboardComponent.h"
#include "Plugins/Ogre/Components/OgreManualMeshComponent.h"


#include "Core/ComponentSystem/ComponentFactory.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/IMessage.h"
#include "Core/Utils/GASSLogManager.h"
#include "Sim/Scenario/Scenario.h"
#include "Sim/Scenario/Scene/SceneObject.h"
#include "Sim/SimEngine.h"






namespace GASS
{
	OgreTextComponent::OgreTextComponent(void) : m_Size (16),
		m_Offset (0.2),
		m_Color(1,1,1,1),
		m_TextObject(NULL),
		m_Attribs(NULL),
		m_Visible(true)
	{

	}

	OgreTextComponent::~OgreTextComponent(void)
	{
		if(m_TextObject)
		{
			delete m_TextObject;

			Ogre::SceneManager* sm = Ogre::Root::getSingleton().getSceneManagerIterator().getNext();
			if(sm)
			{
				Ogre::SceneManager* sm = Ogre::Root::getSingleton().getSceneManagerIterator().getNext();
				Ogre::Camera* ocam = sm->getCameraIterator().getNext();
				Ogre::RenderTarget *target = NULL;
				if (Ogre::Root::getSingleton().getRenderSystem()->getRenderTargetIterator().hasMoreElements())
				{
					target = Ogre::Root::getSingleton().getRenderSystem()->getRenderTargetIterator().getNext();
					target->removeListener(this);
				}
				
			}
		}
		if(m_Attribs)
			delete m_Attribs;
	}



	void OgreTextComponent::RegisterReflection()
	{
		GASS::ComponentFactory::GetPtr()->Register("TextComponent",new GASS::Creator<OgreTextComponent, IComponent>);
		RegisterProperty<std::string>("Text", &GASS::OgreTextComponent::GetText, &GASS::OgreTextComponent::SetText);
		RegisterProperty<float>("Offset", &GASS::OgreTextComponent::GetOffset, &GASS::OgreTextComponent::SetOffset);
		RegisterProperty<float>("CharacterSize", &GASS::OgreTextComponent::GetCharacterSize, &GASS::OgreTextComponent::SetCharacterSize);
		//RegisterProperty<Vec4>("TextColor", &GASS::OgreTextComponent::GetTextColor, &GASS::OgreTextComponent::SetTextColor);
		//TODO: add OSG attributes:  Font
	}

	Ogre::UTFString ConvertToUTF(Ogre::String String) 
	{ 
		Ogre::UTFString UTFString; 
		int i; 
		Ogre::UTFString::code_point cp; 
		for (i=0; i<(int)String.size(); ++i) 
		{ 
			cp = String[i];
			cp &= 0xFF; 
			UTFString.append(1, cp); 
		} 
		return UTFString; 
	}

	void OgreTextComponent::OnCreate()
	{
		//this one should load after mesh entities
		GetSceneObject()->RegisterForMessage(REG_TMESS( OgreTextComponent::OnLoad,LoadGFXComponentsMessage,2));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OgreTextComponent::OnCaptionMessage,TextCaptionMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS( OgreTextComponent::OnVisibilityMessage,VisibilityMessage ,0));
	}

	std::string OgreTextComponent::GetText() const
	{
		return m_TextToDisplay;
	}

	void OgreTextComponent::SetText(const std::string  &text)
	{
		m_TextToDisplay = text;
		if(m_TextObject)
		{

			m_TextToDisplay = Misc::Replace(m_TextToDisplay, "\\r", "\r");
			m_TextToDisplay = Misc::Replace(m_TextToDisplay, "\\n", "\n");
			if(m_TextToDisplay != "")
			{
				m_TextObject->enable(true);
				m_TextObject->setCaption(ConvertToUTF(m_TextToDisplay));
			}
		}
	}

	float OgreTextComponent::GetOffset() const
	{
		return m_Offset;
	}

	void OgreTextComponent::SetOffset(float offset)
	{
		m_Offset = offset;
		//if(m_TextObject)
		//	m_TextObject->setAdditionalHeight(offset);
	}


	float OgreTextComponent::GetCharacterSize() const
	{
		return m_Size;
	}

	void OgreTextComponent::SetCharacterSize(float size)
	{
		m_Size= size;
//		if(m_TextObject)
//			m_TextObject->setCharacterHeight(size);
	}

	void OgreTextComponent::OnCaptionMessage(TextCaptionMessagePtr message)
	{
		std::string caption = message->GetCaption();
		SetText(caption);
	}

	void OgreTextComponent::OnLoad(LoadGFXComponentsMessagePtr message)
	{
		//Ogre::String test = Ogre::String(new_text);
		Ogre::ColourValue color;
		color.r = m_Color.x;
		color.g = m_Color.y;
		color.b = m_Color.z;
		color.a = m_Color.w;

		m_TextToDisplay = Misc::Replace(m_TextToDisplay, "\\r", "\r");
		m_TextToDisplay = Misc::Replace(m_TextToDisplay, "\\n", "\n");

		
		Ogre::SceneManager* sm = Ogre::Root::getSingleton().getSceneManagerIterator().getNext();
		Ogre::Camera* ocam = sm->getCameraIterator().getNext();

		Ogre::RenderTarget *target = NULL;
		if (Ogre::Root::getSingleton().getRenderSystem()->getRenderTargetIterator().hasMoreElements())
		{
			target = Ogre::Root::getSingleton().getRenderSystem()->getRenderTargetIterator().getNext();
			target->addListener(this);
		}

		//ocam->getViewport()->getTarget()->addListener(this);
		
		m_Attribs = new MovableTextOverlayAttributes("Attrs1",ocam,"BlueHighway",m_Size,color,"RedTransparent");
		//m_TextObject = new MovableText(m_Name + "Text", ConvertToUTF(m_TextToDisplay), "BlueHighway",m_Size,color);

		Ogre::MovableObject* mobj = NULL;
		OgreMeshComponentPtr mesh = GetSceneObject()->GetFirstComponentByClass<OgreMeshComponent>();
		
		if(mesh)
		{
			mobj = mesh->GetOgreEntity();

		}
		else
		{
			OgreBillboardComponentPtr billboard = GetSceneObject()->GetFirstComponentByClass<OgreBillboardComponent>();
			if(billboard)
				mobj = billboard->GetBillboardSet();
			else
			{
				boost::shared_ptr<OgreManualMeshComponent> line = GetSceneObject()->GetFirstComponentByClass<OgreManualMeshComponent>();
				if(line)
					mobj = line->GetManualObject();
			}
		}
		if(mobj == NULL)
			LogManager::getSingleton().stream() << "WARNING:Failed to find moveable object for text component: " << m_Name;

		if(mobj)
		{
			static unsigned int obj_id = 0;
			obj_id++;
			std::stringstream ss;
			std::string name;
			ss << m_Name << obj_id;
			ss >> name;

			m_TextObject = new MovableTextOverlay(name,ConvertToUTF(m_TextToDisplay), mobj, m_Attribs);
			m_TextObject->enable(true); 
			m_TextObject->setUpdateFrequency(0.01);// set update frequency to 0.01 seconds
		}
	}


	void OgreTextComponent::preViewportUpdate(const Ogre::RenderTargetViewportEvent& evt)
	{
		Ogre::Viewport *vp = evt.source;
		if(vp && m_TextObject)
		{
			
			m_Attribs->mpCam = vp->getCamera();
			RectLayoutManager m(0,0,vp->getActualWidth(),
			vp->getActualHeight());
			m.setDepth(0);

			m_TextObject->update(0.1);
			if (m_TextObject->isOnScreen() && m_TextToDisplay != "" && m_Visible)
			{
				//visible++;

				RectLayoutManager::Rect r(	m_TextObject->getPixelsLeft(),
					m_TextObject->getPixelsTop(),
					m_TextObject->getPixelsRight(),
					m_TextObject->getPixelsBottom());

				RectLayoutManager::RectList::iterator it = m.addData(r);
				if (it != m.getListEnd())
				{
					m_TextObject->setPixelsTop((*it).getTop());
					m_TextObject->enable(true);
				}
				else
					m_TextObject->enable(false);
			}
			else
				m_TextObject->enable(false);

		}
	}

	AABox OgreTextComponent::GetBoundingBox() const 
	{
		AABox box;
		return box;
	}

	Sphere OgreTextComponent::GetBoundingSphere() const
	{
		Sphere sphere;
		return sphere;
	}

	void OgreTextComponent::OnVisibilityMessage(VisibilityMessagePtr message)
	{
		m_Visible = message->GetValue();
	}
}
