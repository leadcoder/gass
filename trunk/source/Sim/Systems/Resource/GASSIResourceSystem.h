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
#include <string>

namespace GASS
{

	/**
		Interface that all resource systems must be derived from

		Note that interaction with this interface during RTC update is undefined 
		if running GASS in multi-threaded mode. Interaction with systems should 
		instead be done through messages.
	*/

	class GASSExport IResourceSystem
	{
	public:
		virtual ~IResourceSystem(){}

		/**
			Get the full resource path if resource is found in any resource group
			@file_name Resource name
			@file_path Will hold full path to resrouce on return
			@return true if resource is found
		*/
		virtual bool GetFullPath(const std::string &file_name,std::string &file_path) = 0;
		
		/**
			Add new resource location
			@path Full file path to resources
			@resource_group Add resources to this group
			@type  resrouce type FileSystem or Zip
			@recursive Search in subfolders or not
		*/
		virtual void AddResourceLocation(const std::string &path,const std::string &resource_group,const std::string &type, bool recursive) = 0;
		
		/**
			Remove resource location from resource group
			@path Full file path to the resource location
			@resource_group The resource group that hold the location
		*/
		virtual void RemoveResourceLocation(const std::string &path,const std::string &resource_group) = 0;

		/**
			Remove resource group
			@resource_group The resources group
		*/
		virtual void RemoveResourceGroup(const std::string &resource_group) = 0;
		
		/**
			Force load of resource group
			@resource_group The resources group
		*/
		virtual void LoadResourceGroup(const std::string &resource_group) = 0;
	protected:
	};

	typedef boost::shared_ptr<IResourceSystem> ResourceSystemPtr;
}
