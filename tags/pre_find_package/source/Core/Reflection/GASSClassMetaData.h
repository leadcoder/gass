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

#ifndef GASS_OBJECT_META_DATA_H
#define GASS_OBJECT_META_DATA_H

#include "Core/Common.h"
#include <assert.h>
#include <sstream>
#include <vector>

namespace GASS
{
	/**
		Flags that indicate the state of this class
	*/
	enum ClassFlags 
	{
		OF_VISIBLE  =   1 << 0,
	};
	
	/**
		Class holding meta data for RTTI based classes like component containers or components.
		This is the base class for class meta data and implements some basic settings like annotation/description and
		class flags that can be used inside editor environment to give class information. 
		Properties have it' own meta data found in GASSPropertyMetaData.h
		More complex meta data can be created by inheritance from this class.
		Class meta data is static stored and it's purpose is to reflect 
		settings for all instances of the owner RTTI class.
	*/

	class ClassMetaData
	{
	public:
		ClassMetaData(const std::string &annotation, ClassFlags flags = static_cast<ClassFlags>(0)): m_Annotation(annotation), m_Flags(flags) {}
		std::string GetAnnotation() const {return m_Annotation;}
		void SetAnnotation(const std::string &value) {m_Annotation= value;}
		ClassFlags GetFlags() const {return m_Flags;}
		void SetFlags(ClassFlags value) {m_Flags= value;}
	private:
		std::string m_Annotation;
		ClassFlags m_Flags;
	};
	typedef SPTR<ClassMetaData> ClassMetaDataPtr;
}
#endif
