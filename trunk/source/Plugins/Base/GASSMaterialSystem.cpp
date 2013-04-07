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
#include "GASSMaterialSystem.h"
#include "Core/System/GASSSystemFactory.h"
#include "Core/Utils/GASSException.h"
#include <tinyxml.h>


namespace GASS
{
	MaterialSystem::MaterialSystem()
	{

	}

	MaterialSystem::~MaterialSystem()
	{

	}

	void MaterialSystem::RegisterReflection()
	{
		SystemFactory::GetPtr()->Register("MaterialSystem",new GASS::Creator<MaterialSystem, ISystem>);
	}

	void MaterialSystem::Init()
	{
		//Load all material files from gass data path
		std::vector<std::string> files;
		FilePath path("%GASS_DATA_HOME%/materials");
		Misc::GetFilesFromPath(files,path.GetFullPath(),true,true);
		for(size_t i = 0; i< files.size(); i++)
		{
			if(Misc::GetExtension(files[i]) == "gassmat")
				LoadMaterialFile(files[i]);
		}
	}

	void MaterialSystem::LoadMaterialFile(const std::string &file)
	{
		LogManager::getSingleton().stream() << "Start loading material file " << file;
		TiXmlDocument *xmlDoc = new TiXmlDocument(file.c_str());
		if (!xmlDoc->LoadFile())
			GASS_EXCEPT(Exception::ERR_CANNOT_READ_FILE,"Couldn't load:" + file, "MaterialSystem::LoadMaterialFile");
		
		TiXmlElement *xml_ml = xmlDoc->FirstChildElement("MaterialList");
		if(xml_ml)
		{
			TiXmlElement *xml_mat = xml_ml->FirstChildElement("Material");
			while(xml_mat)
			{
				PhysicsMaterial material;
				if(xml_mat->Attribute("Name"))
					material.Name = xml_mat->Attribute("Name");
				else
					GASS_EXCEPT(Exception::ERR_CANNOT_READ_FILE,"Couldn't find Name attribute in:" + file, "MaterialSystem::LoadMaterialFile");

				xml_mat->QueryDoubleAttribute("DynamicFriction",&material.DynamicFriction);
				xml_mat->QueryDoubleAttribute("StaticFriction",&material.StaticFriction);
				xml_mat->QueryDoubleAttribute("Restitution",&material.Restitution);

				//Add material
				AddMaterial(material);
					
				xml_mat = xml_mat->NextSiblingElement("Material");
			}
		}
		delete xmlDoc;
	}

	void MaterialSystem::AddMaterial(const PhysicsMaterial& mat) 
	{
		if(HasMaterial(mat.Name))
			GASS_EXCEPT(Exception::ERR_DUPLICATE_ITEM,"Material:" + mat.Name + " already added", "MaterialSystem::AddMaterial");
		m_Materials[mat.Name] = mat;
	}

	bool MaterialSystem::HasMaterial(const std::string material_name) const
	{
		return (m_Materials.end() != m_Materials.find(material_name));
	}

	PhysicsMaterial MaterialSystem::GetMaterial(const std::string material_name) const
	{
		PhysicsMaterial material;	
		MaterialMap::const_iterator iter = m_Materials.find(material_name);
		if(iter == m_Materials.end())
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"Material:" + material_name + " not found", "MaterialSystem::GetMaterial");
		return iter->second;
	}
}
