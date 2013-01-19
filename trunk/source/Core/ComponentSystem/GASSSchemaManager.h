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

#ifndef GASS_SCEHMA_MANAGER_H
#define GASS_SCEHMA_MANAGER_H

#include <Core/Common.h>
#include "Core/Reflection/GASSBaseReflectionObject.h"
#include "Core/Serialize/GASSIXMLSerialize.h"
#include "Core/Serialize/GASSISerialize.h"
#include "Core/ComponentSystem/GASSIComponent.h"
#include "Core/ComponentSystem/GASSIComponentTemplate.h"

class TiXmlDocument;

namespace GASS
{
	
	class GASSCoreExport SchemaObject
	{
	public:
		SchemaObject();
		virtual ~SchemaObject();
		TiXmlElement* GetObjectAnnotation(const std::string &annotation_tag_name) const;
		TiXmlElement* GetPropertyAnnotation(const std::string &prop_name,const std::string &annotation_tag_name) const;
		TiXmlDocument* m_Document;
		TiXmlElement* m_Object;
		std::string m_Name;
	};
	
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
		void Save(const std::string& outpath, const std::string &classname, BaseReflectionObjectPtr container);
		void SaveProp(TiXmlElement* parent, IProperty* prop) const;
		std::string GetPropType(IProperty* prop) const;
		std::map<std::string,SchemaObject> m_Objects;
	};
	typedef SPTR<SchemaManager> SchemaManagerPtr;

}
#endif 
