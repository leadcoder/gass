#include "OgreText.h"
 
#ifndef OGRE_18
	#include <Overlay/OgreOverlayManager.h>
	#include <Overlay/OgreOverlayContainer.h>
#endif 


template<> TextRenderer* Ogre::Singleton<TextRenderer>::msSingleton = 0;


 
TextRenderer::TextRenderer()
{
	//_overlayMgr = Ogre::OverlayManager::getSingletonPtr();
    _overlayMgr = Ogre::OverlayManager::getSingletonPtr();
 
    _overlay = _overlayMgr->create("overlay1");
    _panel = static_cast<Ogre::OverlayContainer*>(_overlayMgr->createOverlayElement("Panel", "container1"));
    _panel->setDimensions(1, 1);
    _panel->setPosition(0, 0);
 
    _overlay->add2D(_panel);
 
    _overlay->show();
}
 
void TextRenderer::addTextBox(const std::string& ID,
                const std::string& text,
                Ogre::Real x, Ogre::Real y,
                Ogre::Real width, Ogre::Real height,
                const Ogre::ColourValue& color)
{
    Ogre::OverlayElement* textBox = _overlayMgr->createOverlayElement("TextArea", ID);
    textBox->setDimensions(width, height);
	textBox->setMetricsMode(Ogre::GMM_RELATIVE);//GMM_PIXELS);
    textBox->setPosition(x, y);
    textBox->setWidth(width);
    textBox->setHeight(height);
    textBox->setParameter("font_name", "GASSFont");
    textBox->setParameter("char_height", "0.03");
    textBox->setColour(color);
 
    textBox->setCaption(text);
 
    _panel->addChild(textBox);
}
 
void TextRenderer::removeTextBox(const std::string& ID)
{
    _panel->removeChild(ID);
    _overlayMgr->destroyOverlayElement(ID);
}
 
void TextRenderer::setText(const std::string& ID, const std::string& Text)
{
    Ogre::OverlayElement* textBox = _overlayMgr->getOverlayElement(ID);
    textBox->setCaption(Text);
}

bool TextRenderer::hasTextBox(const std::string& ID)
{
    return _overlayMgr->hasOverlayElement(ID);
}
 
const std::string& TextRenderer::getText(const std::string& ID)
{
    Ogre::OverlayElement* textBox = _overlayMgr->getOverlayElement(ID);
    return textBox->getCaption();
}