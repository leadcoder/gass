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

/*
This class is based on the Game Programming Gems 5 article
"Using Templates for Reflection in C++" by Dominic Filion.
*/


#ifndef GASS_OBJECT_META_DATA_H
#define GASS_OBJECT_META_DATA_H


#include "Core/Common.h"
#include <assert.h>
#include <sstream>
#include <vector>

namespace GASS
{
	enum ObjectFlags
	{
		OF_VISIBLE  =   1 << 0,
	};
	
	class ObjectMetaData
	{
	public:
		ObjectMetaData(const std::string &annotation, ObjectFlags flags = static_cast<ObjectFlags>(0)): m_Annotation(annotation), m_Flags(flags) {}
		std::string GetAnnotation() const {return m_Annotation;}
		void SetAnnotation(const std::string &value) {m_Annotation= value;}
		ObjectFlags GetFlags() const {return m_Flags;}
		void SetFlags(ObjectFlags value) {m_Flags= value;}
	private:
		std::string m_Annotation;
		ObjectFlags m_Flags;
	};
	typedef SPTR<ObjectMetaData> ObjectMetaDataPtr;
}
#endif
