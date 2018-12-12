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
#include "Sim/Interface/GASSIRenderWindow.h"
#include <string>

namespace GASS
{
	class GraphicsMaterial;

	/**
		Interface that all graphics systems should be derived from.

		Note that interaction with this interface during RTC update is undefined 
		if running GASS in multi-threaded mode. Interaction with systems should 
		instead be done through messages.
	*/

	typedef std::vector<RenderWindowPtr> RenderWindowVector;

	class IGraphicsSystem
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

		virtual RenderWindowPtr CreateRenderWindow(const std::string &name, int width, int height, void* external_handle = nullptr) = 0;

		/**
			Get material names from rendersystem. This function can be used by external components that want to enumerate materials inside the rendersytem. 
			Resource Manager can only handle files and not file content, one material file can contain many materials and thats why we need this function
			@param resource_group ResourceGroup
		*/

		virtual std::vector<std::string> GetMaterialNames(std::string resource_group = "") const = 0;
		virtual void AddMaterial(const GraphicsMaterial &material, const std::string &base_mat_name ="") = 0;
		virtual bool HasMaterial(const std::string &mat_name) const = 0;

		/**
			Print frame based debug text to screen 
		*/
		virtual void PrintDebugText(const std::string &message) = 0;
	};
	typedef GASS_SHARED_PTR<IGraphicsSystem> GraphicsSystemPtr;
}
