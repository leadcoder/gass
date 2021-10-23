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

namespace GASS
{
	GASS_FORWARD_DECL(SceneObject);
	GASS_FORWARD_DECL(SceneObjectTemplate);

	/**
	This manager is responsible for holding SceneObjectTemplates
	and creating SceneObject.
	For more information see SceneObjectTemplate class
	*/

	class GASSExport SceneObjectTemplateManager : public GASS_ENABLE_SHARED_FROM_THIS<SceneObjectTemplateManager>
	{
		friend class SceneObjectTemplate;
	public:
		/** Map holding templates*/
		using TemplateMap = std::map<std::string, SceneObjectTemplatePtr>;
	public:
		SceneObjectTemplateManager();
		/**
			Create a new object from the template archive.
		@remarks
			This function do not initialize the object,
		@param name Template name
		*/
		SceneObjectPtr CreateFromTemplate(const std::string& name) const;

		/**
			Add a template the template archive.
		@remarks
		@param obj
		*/
		void AddTemplate(SceneObjectTemplatePtr obj);

		/**
			Get a template from template archive.
		@remarks
		@param name Name of the template
		*/
		SceneObjectTemplatePtr GetTemplate(const std::string& name) const;

		/**
			Check if template exist template archive.
		@remarks
		@param name Name of the template
		*/
		bool HasTemplate(const std::string& name) const;

		/**
			Load templates from xml-file
		*/
		void Load(const std::string& filename);

		/**
			Load all templates from path
		*/
		void LoadFromPath(const std::string& path, bool recursive = true);

		/**
		Get a vector of all templates hold by this manager
		*/
		std::vector<std::string> GetTemplateNames() const;

		/**
			Remove all templates
		*/
		void Clear();
	private:
		SceneObjectPtr _CreateSceneObject(SceneObjectTemplatePtr cc_temp) const;
		TemplateMap m_TemplateMap;
	};
	using SceneObjectTemplateManagerPtr = std::shared_ptr<SceneObjectTemplateManager>;
	using SceneObjectTemplateManagerConstPtr = std::shared_ptr<const SceneObjectTemplateManager>;
}
