/****************************************************************************
* This file is part of GASS.                                                *
* See http://code.google.com/p/gass/                                        *
*                                                                           *
* Copyright (c) 2008-2015 GASS team. See Contributors.txt for details.      *
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

#ifndef BASE_COMPONENTCONTAINERTEMPLATEMANAGER_HH
#define BASE_COMPONENTCONTAINERTEMPLATEMANAGER_HH

#include <boost/enable_shared_from_this.hpp>
#include "Core/Common.h"

namespace GASS
{
	FDECL(ComponentContainer);
	FDECL(ComponentContainerTemplate);
	

	/** \addtogroup GASSCore
	*  @{
	*/
	/** \addtogroup Component
	*  @{
	*/

	/**
	A component container template manager is responsible for holding component container templates
	and creating component containers.
	For more information on container templates see the ComponentContainerTemplate class
	*/

	class GASSCoreExport ComponentContainerTemplateManager: public SHARE_CLASS<ComponentContainerTemplateManager>
	{
		friend class ComponentContainerTemplate;
	public:
		/** Map holding templates*/
		typedef std::map<std::string,ComponentContainerTemplatePtr> TemplateMap;
	public:
		ComponentContainerTemplateManager();
		virtual ~ComponentContainerTemplateManager();
		/**
			Create a new object from the template archive.
		@remarks
			This function do not initialize the object,
		@param name Template name
		*/
		ComponentContainerPtr CreateFromTemplate(const std::string &name) const;

		/**
			Add a template the template archive.
		@remarks
		@param obj
		*/
		void AddTemplate(ComponentContainerTemplatePtr obj);

		/**
			Get a template from template archive.
		@remarks
		@param name Name of the template
		*/
		ComponentContainerTemplatePtr GetTemplate(const std::string &name) const;

		/**
			Check if template exist template archive.
		@remarks
		@param name Name of the template
		*/
		bool HasTemplate(const std::string &name) const;


		/**
			Load templates from xml-file
		*/
		void Load(const std::string &filename);


		/**
			Load all templates from path
		*/
		void LoadFromPath(const std::string &path, bool recursive = true);

		/**
			Check if each new component container instance returned
			from the CreateFromTemplate function should have a
			unique name.
		*/

		bool GetAddObjectIDToName() const{return m_AddObjectIDToName;}
		/**
			Set whether each new component container instance returned
			from the CreateFromTemplate function should have a
			unique name or not.
		*/
		void SetAddObjectIDToName(bool value) {m_AddObjectIDToName = value;}

		/**
			Get unique name prefix, default is "_"
		*/
		std::string GetObjectIDPrefix() const {return m_ObjectIDPrefix;}
		/**
			Set unique name prefix
		*/
		void SetObjectIDPrefix(const std::string &prefix) {m_ObjectIDPrefix = prefix;}


		/**
			Get unique name suffix, default is ""
		*/
		std::string GetObjectIDSuffix() const {return m_ObjectIDSuffix;}
		/**
			Set unique name suffix
		*/
		void SetObjectIDSuffix(const std::string &suffix) {m_ObjectIDSuffix = suffix;}
		
		/**
		Get a vector of all templates hold by this manager
		*/
		std::vector<std::string> GetTemplateNames() const;

		/**
			Remove all templates
		*/
		void Clear();
	protected:
		ComponentContainerPtr _CreateComponentContainer(int &part_id, ComponentContainerTemplatePtr cc_temp) const;
		std::string _CreateUniqueName(ComponentContainerTemplatePtr cc_temp) const;

		bool m_AddObjectIDToName;
		std::string m_ObjectIDPrefix;
		std::string m_ObjectIDSuffix;
		TemplateMap m_TemplateMap;
	};
	typedef SPTR<ComponentContainerTemplateManager> ComponentContainerTemplateManagerPtr;
	typedef SPTR<ComponentContainerTemplateManager const> ComponentContainerTemplateManagerConstPtr;
}

#endif // #ifndef ComponentContainerTemplateManager_HH
