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

#ifndef GUI_SCEHMA_LOADER_H
#define GUI_SCEHMA_LOADER_H
#include "Sim/GASSCommon.h"
#include "EditorCommon.h"
#include <Core/Common.h>
#include "Core/Reflection/GASSBaseReflectionObject.h"
#include "Core/Serialize/GASSIXMLSerialize.h"
#include "Core/Serialize/GASSISerialize.h"
#include "Core/ComponentSystem/GASSComponent.h"


class TiXmlDocument;

namespace GASS
{
	enum ControlType
	{
		CT_EDIT,
		CT_FILE_DIALOG,
		CT_COLOR_DIALOG,
		CT_OBJECT_REFERENCE,
		CT_COMBO_BOX,
		//CT_CONTENT_COMBO,
	};

	class EditorModuleExport PropertySettings
	{
	public:
		PropertySettings() : Visible(true) ,Editable(true), GUIControlType(CT_EDIT){}
		virtual ~PropertySettings() {}
		bool Visible;
		bool Editable;
		std::string Documentation;
		std::string Name;
		std::string DisplayName;
		std::string RestrictionProxyProperty;
		ControlType GUIControlType;
		std::string FileControlSettings;
		std::string ResourceGroup;
		std::string ResourceType;
		std::vector<std::string> Restrictions;
		std::vector<std::string> ReferenceComponentFilters;
		std::vector<std::string> ReferenceNameFilters;
		std::vector<std::string> ReferenceIDFilters;
	};

	// Objects settings loaded from xsd file
	class EditorModuleExport ObjectSettings
	{
	public:
		ObjectSettings() : Visible(true) {}
		virtual ~ObjectSettings() {}
		bool Visible;
		std::string Documentation;
		std::string Name;
		std::string DisplayName;
		const PropertySettings* GetProperty(const std::string &name) const;
		void AddProperty(const PropertySettings &ps) {m_PropertySettingsMap[ps.Name] = ps;}
	private:
		std::map<std::string,PropertySettings> m_PropertySettingsMap;
	};

	class EditorModuleExport GUISchemaLoader
	{
	public:
		GUISchemaLoader();
		virtual ~GUISchemaLoader();
		void LoadAllFromPath(const std::string filepath);
		void Load(const std::string filename);
		const ObjectSettings*  GetObjectSettings(const std::string &name) const;
		void UpdateMetaDataForAllObjects();
	protected:
		void UpdateObjectMetaData(const std::string &classname, BaseReflectionObjectPtr object);
		PropertySettings LoadProperty(TiXmlElement* elem);
		bool LoadBoolAttribute(TiXmlElement* elem, const std::string &name);
		std::map<std::string, ObjectSettings> m_Settings;
	};
	typedef SPTR<GUISchemaLoader> GUISchemaLoaderPtr;

}
#endif 
