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
#include "GASSMaterialSystem.h"
#include "Sim/GASSSystemFactory.h"
#include "Core/Utils/GASSException.h"
#include "Core/Utils/GASSFileUtils.h"
#include "Sim/GASSGeometryFlags.h"
#include "Core/Serialize/tinyxml2.h"
#include "Core/Utils/GASSLogger.h"


namespace GASS
{
	MaterialSystem::MaterialSystem(SimSystemManagerWeakPtr manager) : Reflection(manager)
	{
		m_UpdateGroup = UGID_PRE_SIM;
	}

	MaterialSystem::~MaterialSystem()
	{

	}

	void MaterialSystem::RegisterReflection()
	{
		SystemFactory::GetPtr()->Register<MaterialSystem>("MaterialSystem");
	}

	void MaterialSystem::OnSystemInit()
	{
		//Load all material files from gass data path
		std::vector<std::string> files;
		FilePath path("%GASS_DATA_HOME%/physics");
		FileUtils::GetFilesFromPath(files,path.GetFullPath(),true,true);
		for(size_t i = 0; i< files.size(); i++)
		{
			if(FileUtils::GetExtension(files[i]) == "gassmat")
				LoadMaterialFile(files[i]);
			//else if(FileUtils::GetExtension(files[i]) == "gassgeom")
			//	LoadGeometryFlagsFile(files[i]);
		}
	}

	void MaterialSystem::LoadMaterialFile(const std::string &file)
	{
		GASS_LOG(LINFO) << "Start loading material file " << file;
		tinyxml2::XMLDocument *xmlDoc = new tinyxml2::XMLDocument();
		if (xmlDoc->LoadFile(file.c_str()) != tinyxml2::XML_NO_ERROR)
			GASS_EXCEPT(Exception::ERR_CANNOT_READ_FILE,"Couldn't load:" + file, "MaterialSystem::LoadMaterialFile");
		
		tinyxml2::XMLElement *xml_ml = xmlDoc->FirstChildElement("MaterialList");
		if(xml_ml)
		{
			tinyxml2::XMLElement *xml_mat = xml_ml->FirstChildElement("Material");
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

	void MaterialSystem::LoadGeometryFlagsFile(const std::string &file)
	{
		std::map<GeometryFlags, GeometryFlags> m_CollisionMaskMap;
		GASS_LOG(LINFO) << "Start loading collision matrix file " << file;
		tinyxml2::XMLDocument *xmlDoc = new tinyxml2::XMLDocument();
		if (xmlDoc->LoadFile(file.c_str()) != tinyxml2::XML_NO_ERROR)
			GASS_EXCEPT(Exception::ERR_CANNOT_READ_FILE,"Couldn't load:" + file, "MaterialSystem::LoadMaterialFile");
		
		tinyxml2::XMLElement *xml_geom_list = xmlDoc->FirstChildElement("GeometryList");
		if(xml_geom_list)
		{
			tinyxml2::XMLElement *xml_geom = xml_geom_list->FirstChildElement("Geometry");
			while(xml_geom)
			{
				std::string geom_name;
				if(xml_geom->Attribute("Name"))
					geom_name = xml_geom->Attribute("Name");
				else
					GASS_EXCEPT(Exception::ERR_CANNOT_READ_FILE,"Couldn't find Name attribute in:" + file, "MaterialSystem::LoadMaterialFile");
				
				GeometryFlagsBinder gf;
				std::stringstream ss_gf(geom_name);
				ss_gf >> gf;
				
				std::string value = xml_geom->Attribute("CollisionMask");
				//parse mask
				GeometryFlagsBinder gf_mask;
				std::stringstream ss_gf_mask(value);
				ss_gf_mask >> gf_mask;

				m_CollisionMaskMap[gf.GetValue()] = gf_mask.GetValue();

				//int mask = static_cast<int> (gf_mask.GetValue());
				
				xml_geom = xml_geom->NextSiblingElement("Geometry");
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
