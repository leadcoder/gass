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

#include "Sim/GASSCommon.h"
#include "Core/Math/GASSVector.h"
#include "Core/Utils/GASSColorRGBA.h"
#include "Core/Utils/GASSEnumBinder.h"
#include "Core/Math/GASSVector.h"


namespace GASS
{
	//FFP fog modes
	enum FogMode
	{
		FM_LINEAR,
		FM_EXP,
		FM_EXP2,
		FM_NONE
	};

	START_ENUM_BINDER(FogMode,FogModeBinder)
		BIND(FM_LINEAR)
		BIND(FM_EXP)
		BIND(FM_EXP2)
		BIND(FM_NONE)
	END_ENUM_BINDER(FogMode,FogModeBinder)

	/**
		Interface that all graphics scene managers should be derived from.
	*/
	class IGraphicsSceneManager
	{
		GASS_DECLARE_CLASS_AS_INTERFACE(IGraphicsSceneManager)
	public:
		/**
			Draw 3d-line for debugging
		*/
		virtual void DrawLine(const Vec3 &start_point, const Vec3 &end_point, const ColorRGBA &start_color , const ColorRGBA &end_color) = 0;
	};
	typedef GASS_SHARED_PTR<IGraphicsSceneManager> GraphicsSceneManagerPtr;
}
