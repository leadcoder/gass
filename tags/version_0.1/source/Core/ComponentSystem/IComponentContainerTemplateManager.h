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

#ifndef ICOMPONENTCONTAINERTEMPLATEMANAGER_HH
#define ICOMPONENTCONTAINERTEMPLATEMANAGER_HH

#include "Core/Common.h"
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

namespace GASS
{
	class IComponentContainer;
	class IComponentContainerTemplate;

	typedef boost::shared_ptr<IComponentContainerTemplate> ComponentContainerTemplatePtr;
	typedef boost::shared_ptr<IComponentContainer> ComponentContainerPtr;

	/** \addtogroup GASSCore
	*  @{
	*/
	/** \addtogroup Component
	*  @{
	*/

	/**
	A component container template manager is responsible for holding component container templates
	and creating component containers.
	For more information on container templates see the IComponentContainerTemplate interface
	The component container template manager interface is very simple and a implementation only
	needs to provied three functions, CreateFromTemplate, AddTemplate and GetTemplate
	*/

	class GASSCoreExport IComponentContainerTemplateManager
	{
		friend class IComponentContainerTemplate;
	public:
		typedef std::map<std::string,IComponentContainerTemplate*> TemplateMap;
	public:
		virtual ~IComponentContainerTemplateManager(){}
		/**
			Create a new component container from the template archive.
		@remarks
		@param name Template name
		*/
		virtual ComponentContainerPtr CreateFromTemplate(const std::string &name) = 0;
		/**
			Add a template the template archive.
		@remarks
		@param obj
		*/
		virtual void AddTemplate(ComponentContainerTemplatePtr obj) = 0;
		/**
			Get a template from template archive.
		@remarks
		@param name Name of the template
		*/
		virtual ComponentContainerTemplatePtr GetTemplate(const std::string &name) = 0;

		/**
			Should all objects returned by this manager have unique names?
		*/
		virtual bool GetAddObjectIDToName() const = 0;
		/**
			Set whether objects created by this manager should have unique names or not,
		*/
		virtual void SetAddObjectIDToName(bool value) = 0;

		/**
			Get unique name prefix
		*/
		virtual std::string GetObjectIDPrefix() const = 0;
		/**
			Set unique name prefix
		*/
		virtual void SetObjectIDPrefix(const std::string &sufix) = 0;

		/**
			Get unique name suffix, default is ""
		*/
		virtual std::string GetObjectIDSuffix() const = 0;
		/**
			Set unique name suffix
		*/
		virtual void SetObjectIDSuffix(const std::string &suffix) = 0;
	protected:

	};

	typedef boost::shared_ptr<IComponentContainerTemplateManager> ComponentContainerTemplateManagerPtr;
	typedef boost::weak_ptr<IComponentContainerTemplateManager> ComponentContainerTemplateManagerWeakPtr;
}

#endif // #ifndef ICOMPONENTCONTAINERTEMPLATEMANAGER_HH
