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

#ifndef GASS_I_SERIALIZE_H
#define GASS_I_SERIALIZE_H

#include "Core/Common.h"

namespace GASS
{
	class ISerializer;

	/** \addtogroup GASSCore
	*  @{
	*/
	/** \addtogroup Serialize
	*  @{
	*/

	/**
		Interface for binary serialization, classes that want to support 
		binary serialization should be derived from this interface
	*/
	class GASSCoreExport ISerialize
	{
	public:
		virtual ~ISerialize(){}

		/**
			Serialize functions that provide a the serialize object as argument
			(SerialSaver or SerialLoader).
		*/
		virtual bool Serialize(ISerializer*) = 0;
	};
	typedef SPTR<ISerialize> SerializePtr;
}

#endif // #ifndef ISERIALIZE_HH
