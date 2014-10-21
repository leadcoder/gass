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

#ifndef DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION 0x0800
#endif
#ifdef WIN32
// Turn off warnings generated by long std templates
// This warns about truncation to 255 characters in debug/browse info
#   pragma warning (disable : 4786)

// disable: "conversion from 'double' to 'float', possible loss of data
#   pragma warning (disable : 4244)

// disable: "truncation from 'double' to 'float'
#   pragma warning (disable : 4305)

// disable: "<type> needs to have dll-interface to be used by clients'
// Happens on STL member variables which are not public therefore is ok
#   pragma warning (disable : 4251)

// disable: "non dll-interface class GASSExport used as base for dll-interface class"
// Happens when deriving from Singleton because bug in compiler ignores
// template export
#   pragma warning (disable : 4275)

// disable: "C++ Exception Specification ignored"
// This is because MSVC 6 did not implement all the C++ exception
// specifications in the ANSI C++ draft.
#   pragma warning( disable : 4290 )

// disable: "no suitable definition provided for explicit template
// instantiation request" Occurs in VC7 for no justifiable reason on all
// #includes of Singleton
#   pragma warning( disable: 4661)


#   pragma warning( disable: 4530)


// disable: "forcing value to bool 'true' or 'false' (performance warning)"
#   pragma warning( disable: 4800)


// disable: "C++ exception handler used, but unwind semantics are not enabled. Specify -GX"
#   pragma warning( disable: 4530)

// disable: "signed/unsigned mismatch"
#   pragma warning( disable: 4018)


//class inherits via dominance warning
#  pragma warning( disable: 4250)

#endif /* WIN32 */


	#if defined ( WIN32 )
    #   if defined(GASS_PLUGIN_EXPORTS)
    #       define GASSPluginExport __declspec( dllexport )
    #   else
	#		define GASSPluginExport __declspec( dllimport )
	#   endif
	#  	if defined( GASS_EXPORTS)
	#      	define GASSExport __declspec( dllexport )
	#  	else
	#  		define GASSExport __declspec( dllimport )
	#  	endif
    #else
    #   define GASSPluginExport
    #   define GASSExport
    #endif

#include "Core/Common.h"







