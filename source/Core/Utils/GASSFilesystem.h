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

#define GASS_USE_GHC_FILESYSTEM

#ifdef GASS_USE_GHC_FILESYSTEM

#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4100)
#pragma warning (disable : 4127)
#pragma warning (disable : 4244)
#pragma warning (disable : 4701)
#pragma warning (disable : 4706)
#endif

#include "filesystem.hpp"

#ifdef _MSC_VER
#pragma warning (pop)
#endif

#define GASS_FILESYSTEM ghc::filesystem
#define GASS_TO_GENERIC_STRING(a) a.generic_string()
#define GASS_CURRENT_PATH GASS_FILESYSTEM::current_path
#define GASS_IS_DIRECTORY GASS_FILESYSTEM::is_directory
#define GASS_COPY_OPTION_OVERWRITE GASS_FILESYSTEM::copy_options::overwrite_existing

#else //keep this for now 

#if defined(GASS_USE_BOOST_FILESYSTEM)
#define BOOST_NO_CXX11_SCOPED_ENUMS
#include <boost/filesystem.hpp>
#undef BOOST_NO_CXX11_SCOPED_ENUMS
#define GASS_FILESYSTEM boost::filesystem

#if(BOOST_FILESYSTEM_VERSION > 2)
	#define GASS_TO_GENERIC_STRING(a) a.generic_string()
#else
	#define GASS_TO_GENERIC_STRING(a) a
#endif
#define GASS_CURRENT_PATH GASS_FILESYSTEM::current_path
#define GASS_IS_DIRECTORY GASS_FILESYSTEM::is_directory
#define GASS_COPY_OPTION_OVERWRITE GASS_FILESYSTEM::copy_option::overwrite_if_exists
#else //use experimental filesystem, TODO:test gcc
#include <filesystem>


//msvc2013
#if (_MSC_VER == 1800)
#define GASS_FILESYSTEM std::tr2::sys
#define GASS_TO_GENERIC_STRING(a) a
#define GASS_CURRENT_PATH GASS_FILESYSTEM::current_path<GASS_FILESYSTEM::path>
#define GASS_IS_DIRECTORY GASS_FILESYSTEM::is_directory<GASS_FILESYSTEM::path>
#define GASS_COPY_OPTION_OVERWRITE GASS_FILESYSTEM::copy_option::overwrite_if_exists
#elif (_MSC_VER == 1900) //MSVC2015
#define GASS_FILESYSTEM std::tr2::sys
#define GASS_TO_GENERIC_STRING(a) a.generic_string()
#define GASS_CURRENT_PATH GASS_FILESYSTEM::current_path
#define GASS_IS_DIRECTORY GASS_FILESYSTEM::is_directory
#define GASS_COPY_OPTION_OVERWRITE GASS_FILESYSTEM::copy_options::overwrite_existing
#elif (_MSC_VER > 1900) //MSVC2017
#define GASS_FILESYSTEM std::experimental::filesystem
#define GASS_TO_GENERIC_STRING(a) a.generic_string()
#define GASS_CURRENT_PATH GASS_FILESYSTEM::current_path
#define GASS_IS_DIRECTORY GASS_FILESYSTEM::is_directory
#define GASS_COPY_OPTION_OVERWRITE GASS_FILESYSTEM::copy_options::overwrite_existing
#endif

#endif
#endif


