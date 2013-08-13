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

#pragma once

#include "Sim/GASSCommon.h"
#include "Sim/Interface/GASSIRenderWindow.h"
#include "Core/System/GASSISystem.h"
#include <string>

namespace GASS
{
	

	/**
		Interface that all graphics systems should be derived from.

		Note that interaction with this interface during RTC update is undefined 
		if running GASS in multi-threaded mode. Interaction with systems should 
		instead be done through messages.
	*/

	typedef std::vector<RenderWindowPtr> RenderWindowVector;

	class GASSExport IGraphicsSystem
	{
	public:
		virtual ~IGraphicsSystem(){}

		/**
			Get main render window
		*/
		virtual RenderWindowPtr GetMainRenderWindow() const = 0;

		/**
			Get all render windows
		*/
		virtual RenderWindowVector GetRenderWindows() const = 0;
		
		/**
			Create new render window 
			@param name Unique window name
			@param width Window width in pixels
			@param height Window height in pixels
			@param handle Main window handle (parent window)
		*/

		virtual RenderWindowPtr CreateRenderWindow(const std::string &name, int width, int height, void* external_handle = 0) = 0;
		/**
			Create new viewport in render window 
			@param name Unique viewport name
			@param render_window Name of host render window
			@param left Normalized viewport left position (where render window width pixels  goes from 0-1)
			@param top Normalized viewport top position (where render window height pixels  goes from 0-1)
			@param width Normalized viewport width  (where render window width == 1.0)
			@param height Normalized viewport height (where render window height == 1.0)
		*/
//		virtual void CreateViewport(const std::string &name, const std::string &render_window, float  left, float top, float width, float height) = 0;

		/**
			Get material names from rendersystem. This function can be used by external components that want to enumerate materials inside the rendersytem. 
			Resource Manager can only handle files and not file content, one material file can contain many materials and thats why we need this function
			@param resource_group ResourceGroup
		*/

		virtual std::vector<std::string> GetMaterialNames(std::string resource_group = "") const = 0;
	protected:
	};
	typedef SPTR<IGraphicsSystem> GraphicsSystemPtr;
}
