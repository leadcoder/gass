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

#ifndef COMMON_HH
#define COMMON_HH

#include "Core/GASSConfig.h"

#ifdef _MSC_VER

#ifndef _CRT_SECURE_NO_WARNINGS
#	define _CRT_SECURE_NO_WARNINGS
#endif
#ifndef _SCL_SECURE_NO_WARNINGS
#	define _SCL_SECURE_NO_WARNINGS
#endif
// Turn off warnings generated by long std templates
// This warns about truncation to 255 characters in debug/browse info
//#   pragma warning (disable : 4786)


// disable: "truncation from 'double' to 'float'
//#   pragma warning (disable : 4305)

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
//#   pragma warning( disable : 4290 )


// disable: "forcing value to bool 'true' or 'false' (performance warning)"
//#   pragma warning( disable: 4800)

// disable: "C++ exception handler used, but unwind semantics are not enabled. Specify -GX"
//#   pragma warning( disable: 4530)

//# pragma warning( disable: 4996)
// disable: "signed/unsigned mismatch"
//#   pragma warning( disable: 4018)

// disable: unreferenced formal parameter
//#   pragma warning( disable: 4100)

// disable:  assignment operator could not be generated
//#   pragma warning( disable: 4512)



#else
#pragma GCC diagnostic ignored "-Wreorder"
#endif

#if defined ( _MSC_VER )
#   	if defined( GASS_BUILDTYPE_STATIC )
#			define GASSCoreExport
#		else
#		   	if defined( GASS_CORE_EXPORTS )
#		    	define GASSCoreExport __declspec( dllexport )
#		    	define TINYXML2_EXPORT
#   		else
#		    	define TINYXML2_IMPORT
#      			define GASSCoreExport __declspec( dllimport )
#   		endif
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
#include <set>

#ifdef _MSC_VER
	#define NOMINMAX
#endif
#include <algorithm>

//Force to use boost?
//#if defined( _MSC_VER ) && _MSC_VER < 1700
//	#define GASS_USE_BOOST
//#endif

#ifdef GASS_USE_BOOST
	#define GASS_USE_BOOST_FUNC_BIND
	#define GASS_USE_BOOST_PTR
	#define GASS_USE_BOOST_FILESYSTEM
	#define GASS_USE_BOOST_ANY
	#define GASS_USE_BOOST_THREADS
#endif

#ifdef GASS_USE_BOOST_THREADS
	#include <boost/thread/locks.hpp>
	#include <boost/thread/mutex.hpp>
	#define GASS_MUTEX boost::mutex
	#define GASS_MUTEX_LOCK(a) boost::lock_guard<boost::mutex> m_lock(a);
#else
	#include <mutex>
	#define GASS_MUTEX std::mutex
	#define GASS_MUTEX_LOCK(a) std::lock_guard<std::mutex> m_lock(a);
#endif

#ifdef GASS_USE_BOOST_ANY
	#include <boost/any.hpp>
	#define GASS_ANY boost::any
	#define GASS_ANY_CAST boost::any_cast
#else
	#include "Core/Utils/GASSAny.h"
	#define GASS_ANY any
	#define GASS_ANY_CAST any_cast
#endif

#ifdef GASS_USE_BOOST_FUNC_BIND
	#include <boost/bind.hpp>
	#include <boost/function.hpp>
	#define GASS_BIND boost::bind
	#define GASS_FUNCTION boost::function
	#define GASS_PLACEHOLDERS
#else
	#include <functional>
	#define GASS_PLACEHOLDERS std::placeholders
	#define GASS_BIND std::bind
	#define GASS_FUNCTION std::function
#endif

#ifdef GASS_USE_BOOST_PTR
	#include <boost/shared_ptr.hpp>
	#include <boost/weak_ptr.hpp>
	#include <boost/shared_ptr.hpp>
	#include <boost/enable_shared_from_this.hpp>
	#include <boost/make_shared.hpp>
	#define GASS_SHARED_PTR boost::shared_ptr
	#define GASS_WEAK_PTR boost::weak_ptr
	#define GASS_DYNAMIC_PTR_CAST boost::dynamic_pointer_cast
	#define GASS_STATIC_PTR_CAST boost::static_pointer_cast
	#define GASS_ENABLE_SHARED_FROM_THIS boost::enable_shared_from_this
	#define GASS_MAKE_SHARED boost::make_shared
#else
	#define GASS_SHARED_PTR std::shared_ptr
	#define GASS_WEAK_PTR std::weak_ptr
	#define GASS_DYNAMIC_PTR_CAST std::dynamic_pointer_cast
	#define GASS_STATIC_PTR_CAST std::static_pointer_cast
	#define GASS_ENABLE_SHARED_FROM_THIS std::enable_shared_from_this
	#define GASS_MAKE_SHARED std::make_shared
#endif

#define GASS_FORWARD_DECL(CLASS) class CLASS; typedef GASS_SHARED_PTR<CLASS> CLASS##Ptr; typedef GASS_WEAK_PTR<CLASS> CLASS##WeakPtr;
#define GASS_PTR_DECL(CLASS) typedef GASS_SHARED_PTR<CLASS> CLASS##Ptr; typedef GASS_WEAK_PTR<CLASS> CLASS##WeakPtr;
#define GASS_IFORWARD_DECL(CLASS) class I##CLASS; typedef GASS_SHARED_PTR<I##CLASS> CLASS##Ptr; typedef GASS_WEAK_PTR<I##CLASS> CLASS##WeakPtr;
#define GASS_IPTR_DECL(CLASS) typedef GASS_SHARED_PTR<I##CLASS> CLASS##Ptr; typedef GASS_WEAK_PTR<I##CLASS> CLASS##WeakPtr;
#define GASS_PTR_TO_INT(ptr) static_cast<int>(reinterpret_cast<size_t>(ptr))

#endif
