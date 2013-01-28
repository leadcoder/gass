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

#ifndef COMMON_HH
#define COMMON_HH

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

// disable: "non dll-interface class GASSCoreExport used as base for dll-interface class"
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
//#   pragma warning( disable: 4018)

#endif /* WIN32 */

#if defined ( WIN32 )
#   	if defined( GASS_CORE_EXPORTS )
#       	define GASSCoreExport __declspec( dllexport )
#   	else
#      		define GASSCoreExport __declspec( dllimport )
#   	endif
#else
#   define GASSCoreExport
#endif



#ifndef WIN32
extern "C" {

#include <unistd.h>
#include <dlfcn.h>

}
#endif

#ifdef WIN32
#include <windows.h>

#    define DYNLIB_HANDLE hInstance
#    define DYNLIB_LOAD( a ) LoadLibraryEx( a,0,LOAD_WITH_ALTERED_SEARCH_PATH )
#    define DYNLIB_GETSYM( a, b ) GetProcAddress( a, b )
#    define DYNLIB_UNLOAD( a ) FreeLibrary( a )

struct HINSTANCE__;
typedef struct HINSTANCE__* hInstance;

#else
#    define DYNLIB_HANDLE void*
#    define DYNLIB_LOAD( a ) dlopen( a, RTLD_LAZY | RTLD_GLOBAL)
#    define DYNLIB_GETSYM( a, b ) dlsym( a, b )
#    define DYNLIB_UNLOAD( a ) dlclose( a )
#endif

namespace GASS
{
	typedef double Float;
}


#include <assert.h>

//stl
#include <string>
#include <vector>
#include <map>
#include <list>

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/any.hpp>
#include <boost/bind.hpp>
#include <boost/filesystem.hpp>
#include <boost/enable_shared_from_this.hpp>



#define SPTR boost::shared_ptr
#define WPTR boost::weak_ptr
#define FDECL(CLASS) class CLASS; typedef SPTR<CLASS> ##CLASS##Ptr; typedef WPTR<CLASS> ##CLASS##WeakPtr;
#define PDECL(CLASS) typedef SPTR<CLASS> ##CLASS##Ptr; typedef WPTR<CLASS> ##CLASS##WeakPtr;

#define IFDECL(CLASS) class I##CLASS##; typedef SPTR<I##CLASS##> ##CLASS##Ptr; typedef WPTR<I##CLASS##> ##CLASS##WeakPtr;
#define IPDECL(CLASS) typedef SPTR<I##CLASS##> ##CLASS##Ptr; typedef WPTR<I##CLASS##> ##CLASS##WeakPtr;

#define DYNAMIC_PTR_CAST boost::dynamic_pointer_cast
#define STATIC_PTR_CAST boost::static_pointer_cast
#define SHARE_CLASS boost::enable_shared_from_this
#define NO_THROW boost::detail::sp_nothrow_tag()

#endif // #ifndef COMMON_HH
