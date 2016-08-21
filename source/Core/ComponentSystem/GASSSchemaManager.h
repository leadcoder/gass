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

#ifndef GASS_SCEHMA_MANAGER_H
#define GASS_SCEHMA_MANAGER_H

#include <Core/Common.h>
#include "Core/Reflection/GASSBaseReflectionObject.h"

namespace tinyxml2
{
	class XMLDocument;
}

namespace GASS
{
	/**
		Class holding meta data (as tinyxml2::XMLElement) for a specific class
		Class name is stored in m_Name
	*/
	class GASSCoreExport SchemaObject
	{
	public:
		SchemaObject();
		virtual ~SchemaObject();
		tinyxml2::XMLElement* GetObjectAnnotation(const std::string &annotation_tag_name) const;
		tinyxml2::XMLElement* GetPropertyAnnotation(const std::string &prop_name,const std::string &annotation_tag_name) const;
		tinyxml2::XMLDocument* m_Document;
		tinyxml2::XMLElement* m_Object;
		std::string m_Name;
	};
	/**
		Helper class is used for overriding default meta data for RTTI based classes and it's properties.
		New meta data is loaded from from xml-schema file (xsd-file) and replace default meta data with data found in 
		xsd file. Storing meta data in xsd file can be convenient for editor environment where, for instance, visibility flags 
		may alter on application basis. This class can also generate xsd files from all registered   Component and ComponentContainer
		that can be used as a starting point for meta data specialization.
	*/
	class GASSCoreExport SchemaManager 
	{
	public:
		SchemaManager();
		virtual ~SchemaManager();
		void Generate(const std::string& outpath);
		void LoadAllFromPath(const std::string filepath);
		void Load(const std::string filename);
		const SchemaObject* GetSchemaObject(const std::string &name) const;
	protected:
		void _Save(const std::string& outpath, const std::string &classname, BaseReflectionObjectPtr container);
		void _SaveProp(tinyxml2::XMLElement* parent, IProperty* prop) const;
		std::string _GetPropType(IProperty* prop) const;

		std::map<std::string,SchemaObject> m_Objects;
	};
	typedef GASS_SHARED_PTR<SchemaManager> SchemaManagerPtr;

}
#endif 
