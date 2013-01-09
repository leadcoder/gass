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

#ifndef GASS_RENDER_WINDOW_H
#define GASS_RENDER_WINDOW_H

#include "Core/Common.h"

namespace GASS
{
	class GASSExport IRenderWindow
	{
	public:
		RenderWindow(const std::string name,unsigned int &width, unsigned int &height, int &left, int &top) : m_Name(name), 
			m_Width(width),
		m_Height(height),
		m_Left(left),
		m_Top(top),
		m_Handel(NULL)
		{

		}
		std::string m_Name;
		unsigned int m_Width;
		unsigned int m_Height;
		unsigned int m_Left;
		unsigned int m_Top;
		void* m_Handel;
	};
}
#endif 
