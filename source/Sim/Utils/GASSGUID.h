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

#include "Core/Common.h"

#ifdef GASS_USE_BOOST
	#pragma warning( push )
	# pragma warning( disable: 4996)
	#include <boost/uuid/uuid.hpp>
	#include <boost/uuid/uuid_generators.hpp>
	#include <boost/uuid/uuid_io.hpp>
	#pragma warning( pop )
	#define GASS_GUID boost::uuids::uuid
	#define GASS_GUID_NULL boost::uuids::nil_uuid()
	#define GASS_GUID_GENERATE boost::uuids::random_generator()()
#else
	#include "Sim/Utils/guid.h"
	#define GASS_GUID Guid
	#define GASS_GUID_NULL Guid()
	#define GASS_GUID_GENERATE GuidGenerator::newGuid()
#endif


