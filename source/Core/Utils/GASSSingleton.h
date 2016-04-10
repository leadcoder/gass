/****************************************************************************
* This file is part of GASS.                                                *
* See https://github.com/leadcoder/gass                                     *
*                                                                           *
* Copyright (c) 2008-2016 GASS team. See Contributors.txt for details.      *
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
#include <assert.h>
#include "Core/Common.h"

// disable: "no suitable definition provided for explicit template
// instantiation request" Occurs in VC7 for no justifiable reason on all
// #includes of Singleton
#ifdef WIN32
#   pragma warning( disable: 4661)
#   pragma warning( disable: 4530)
#endif
namespace GASS
{
	/** \addtogroup GASSCore
	*  @{
	*/
	/** \addtogroup Utility
	*  @{
	*/

	/**
		Template class that implement the singleton pattern
	*/

	template<typename T>
	class Singleton
	{
	protected:
		static T* m_Instance;
	public:
		Singleton()
		{
			assert(!m_Instance);
			//use a cunning trick to get the singleton pointing to the start of the whole, rather than
			//the start of the Singleton part of the object

			#if defined( _MSC_VER ) && _MSC_VER < 1200
                int offset = (int)(T*)1 - (int)(Singleton <T>*)(T*)1;
                m_Instance = (T*)((int)this + offset);
            #else
                m_Instance = static_cast< T* >( this );
            #endif
		}
		~Singleton()
		{
			assert(m_Instance);
			m_Instance=0;
		}
		static inline T& Get()
		{
			assert(m_Instance);
			return *m_Instance;
		}
		static inline T* GetPtr()
		{
			assert(m_Instance);
			return m_Instance;
		}
	};
	//template <typename T> T* Singleton <T>::m_Instance = 0;
}


