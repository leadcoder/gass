/****************************************************************************
* This file is part of GASS.                                                *
* See http://code.google.com/p/gass/                                        *
*                                                                           *
* Copyright (c) 2008-2015 GASS team. See Contributors.txt for details.      *
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

#include "Plugins/Ogre/OgreDebugTextOutput.h"
#include "Sim/GASSCommon.h"
#include "Core/Utils/GASSLogManager.h"
#include <Ogre.h>
#include <Overlay/OgreOverlayElement.h>
#include <Overlay/OgreOverlayManager.h>
#include <Overlay/OgreOverlayContainer.h>
#include <stdlib.h>
#include <stdio.h>

namespace GASS
{
	OgreDebugTextOutput::OgreDebugTextOutput()
		:
		m_NumDebugStrings(0),
		m_MaxDebugStrings(100),
		m_Active (true),
		m_OverlayMgr(NULL),
		m_Overlay(NULL),
		m_Panel(NULL),
		m_CharHeight(18)
	{

	}

	OgreDebugTextOutput::~OgreDebugTextOutput()
	{

	}

	void OgreDebugTextOutput::Print(const char *string, ...)
	{
		if(!m_Active) return;
		char		text[1024]; // Holds Our String
		va_list		ap; // Pointer To List Of Arguments

		if(m_NumDebugStrings > m_MaxDebugStrings) return;

		if (string == NULL) // If There's No Text
			return; // Do Nothing
		va_start(ap, string); // Parses The String For Variables
		vsprintf(text, string, ap); // And Converts Symbols To Actual Numbers
		va_end(ap);

		DebugString ds;
		ds.m_PosX = -1;
		ds.m_PosY = -1;
		ds.m_Text = text;
		m_DebugStringList.push_back(ds);
		m_NumDebugStrings++;
	}

	void OgreDebugTextOutput::AddTextBox(
		const std::string& ID,
		const std::string& text,
		float x, float y,
		float  width, float height)
	{
		char buffer[8];
		Ogre::OverlayElement* textBox = m_OverlayMgr->createOverlayElement("TextArea", ID);
		textBox->setDimensions(width, height);
		textBox->setMetricsMode(Ogre::GMM_PIXELS);
		textBox->setPosition(x, y);
		textBox->setWidth(width);
		textBox->setHeight(height);
		textBox->setParameter("font_name", "GASSFont");
		sprintf(buffer, "%i",m_CharHeight);
		//itoa(m_CharHeight, buffer, 10);
		textBox->setParameter("char_height", buffer);
		textBox->setColour(Ogre::ColourValue::Green);
		textBox->setCaption(text);
		m_Panel->addChild(textBox);
	}

	void OgreDebugTextOutput::UpdateTextBox()
	{
		if(m_OverlayMgr ==NULL)
		{
			m_OverlayMgr = Ogre::OverlayManager::getSingletonPtr();
			m_Overlay = m_OverlayMgr->create("DebugOverlay");
			m_Panel = static_cast<Ogre::OverlayContainer*>(m_OverlayMgr->createOverlayElement("Panel", "DebugContainer"));
			m_Panel->setDimensions(1, 1);
			m_Panel->setPosition(0, 0);
			m_Overlay->add2D(m_Panel);
			m_Overlay->show();
			AddTextBox("DebugPrint", "Hello", 10, 50, 100, 20);
		}

		int line = 0;
		int line_size = 18;
		std::list < DebugString>::iterator iter;
		std::string dout;
		for(iter = m_DebugStringList.begin();iter != m_DebugStringList.end(); ++iter)
		{
			DebugString ds = (DebugString) *iter;
			dout += ds.m_Text;
			dout += "\n";
		}
		Ogre::OverlayElement* textBox = m_OverlayMgr->getOverlayElement("DebugPrint");
		textBox->setCaption(dout);
		m_DebugStringList.clear();
		m_NumDebugStrings = 0;
	}

}
