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


#include <string>
#include <list>
#include "Core/Math/GASSVector.h"

namespace Ogre
{
	class OverlayManager;
	class Overlay;
	class OverlayContainer;
}

namespace GASS
{

	class DebugString
	{
	public:
		int m_PosX;
		int m_PosY;
		std::string m_Text;
	};

	class OgreDebugTextOutput
	{
	public:
		OgreDebugTextOutput();
		virtual ~OgreDebugTextOutput();
		inline void SetActive(bool active){m_Active = active;}
		inline bool GetActive()const {return m_Active;}
		void UpdateTextBox();
		void Print(const char *string, ...);
	private:
		void AddTextBox(
		const std::string& ID,
		const std::string& text,
		float x, float y,
		float  width, float  height);
		inline void SetCharHeight(int charHeight) { m_CharHeight = charHeight; }
		inline int GetCharHeight() { return m_CharHeight; }

		std::list<DebugString> m_DebugStringList;
		int m_NumDebugStrings;
		int m_MaxDebugStrings;
		bool m_Active;

		Ogre::OverlayManager*    m_OverlayMgr;
		Ogre::Overlay*           m_Overlay;
		Ogre::OverlayContainer*  m_Panel;
		int m_CharHeight;
	};
}

