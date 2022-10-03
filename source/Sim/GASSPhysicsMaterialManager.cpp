#include "GASSPhysicsMaterialManager.h"
#include "Sim/GASSSystemFactory.h"
#include "Core/Utils/GASSException.h"
#include "Core/Utils/GASSFileUtils.h"
#include "Sim/GASSGeometryFlags.h"
#include "Core/Serialize/tinyxml2.h"
#include "Core/Utils/GASSLogger.h"


namespace GASS
{
	PhysicsMaterialManager::PhysicsMaterialManager()
	{
		
	}

	void PhysicsMaterialManager::LoadMaterialFile(const std::string &file)
	{
		GASS_LOG(LINFO) << "Start loading material file " << file;
		auto *xml_doc = new tinyxml2::XMLDocument();
		if (xml_doc->LoadFile(file.c_str()) != tinyxml2::XML_NO_ERROR)
			GASS_EXCEPT(Exception::ERR_CANNOT_READ_FILE,"Couldn't load:" + file, "PhysicsMaterialManager::LoadMaterialFile");
		
		tinyxml2::XMLElement *xml_ml = xml_doc->FirstChildElement("MaterialList");
		if(xml_ml)
		{
			tinyxml2::XMLElement *xml_mat = xml_ml->FirstChildElement("Material");
			while(xml_mat)
			{
				PhysicsMaterial material;
				if(xml_mat->Attribute("Name"))
					material.Name = xml_mat->Attribute("Name");
				else
					GASS_EXCEPT(Exception::ERR_CANNOT_READ_FILE,"Couldn't find Name attribute in:" + file, "PhysicsMaterialManager::LoadMaterialFile");

				xml_mat->QueryDoubleAttribute("DynamicFriction",&material.DynamicFriction);
				xml_mat->QueryDoubleAttribute("StaticFriction",&material.StaticFriction);
				xml_mat->QueryDoubleAttribute("Restitution",&material.Restitution);

				//Add material
				AddMaterial(material);
					
				xml_mat = xml_mat->NextSiblingElement("Material");
			}
		}
		delete xml_doc;
	}

	void PhysicsMaterialManager::LoadGeometryFlagsFile(const std::string &file)
	{
		std::map<GeometryFlags, GeometryFlags> m_collision_mask_map;
		GASS_LOG(LINFO) << "Start loading collision matrix file " << file;
		auto *xml_doc = new tinyxml2::XMLDocument();
		if (xml_doc->LoadFile(file.c_str()) != tinyxml2::XML_NO_ERROR)
			GASS_EXCEPT(Exception::ERR_CANNOT_READ_FILE,"Couldn't load:" + file, "PhysicsMaterialManager::LoadMaterialFile");
		
		tinyxml2::XMLElement *xml_geom_list = xml_doc->FirstChildElement("GeometryList");
		if(xml_geom_list)
		{
			tinyxml2::XMLElement *xml_geom = xml_geom_list->FirstChildElement("Geometry");
			while(xml_geom)
			{
				std::string geom_name;
				if(xml_geom->Attribute("Name"))
					geom_name = xml_geom->Attribute("Name");
				else
					GASS_EXCEPT(Exception::ERR_CANNOT_READ_FILE,"Couldn't find Name attribute in:" + file, "PhysicsMaterialManager::LoadMaterialFile");
				
				GeometryFlagsBinder gf;
				std::stringstream ss_gf(geom_name);
				ss_gf >> gf;
				
				std::string value = xml_geom->Attribute("CollisionMask");
				//parse mask
				GeometryFlagsBinder gf_mask;
				std::stringstream ss_gf_mask(value);
				ss_gf_mask >> gf_mask;

				m_collision_mask_map[gf.GetValue()] = gf_mask.GetValue();

				//int mask = static_cast<int> (gf_mask.GetValue());
				
				xml_geom = xml_geom->NextSiblingElement("Geometry");
			}
		}
		delete xml_doc;
	}

	void PhysicsMaterialManager::AddMaterial(const PhysicsMaterial& mat) 
	{
		if(HasMaterial(mat.Name))
			GASS_EXCEPT(Exception::ERR_DUPLICATE_ITEM,"Material:" + mat.Name + " already added", "PhysicsMaterialManager::AddMaterial");
		m_Materials[mat.Name] = mat;
	}

	bool PhysicsMaterialManager::HasMaterial(const std::string material_name) const
	{
		return (m_Materials.end() != m_Materials.find(material_name));
	}

	PhysicsMaterial PhysicsMaterialManager::GetMaterial(const std::string material_name) const
	{
		PhysicsMaterial material;	
		auto iter = m_Materials.find(material_name);
		if(iter == m_Materials.end())
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"Material:" + material_name + " not found", "PhysicsMaterialManager::GetMaterial");
		return iter->second;
	}
}
