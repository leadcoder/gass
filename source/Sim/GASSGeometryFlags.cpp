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
#include "GASSGeometryFlags.h"
#include "Core/Math/GASSVector.h"
#include "Core/Utils/GASSLogger.h"
#include "Core/Serialize/tinyxml2.h"

namespace GASS
{
   // template<> std::map<std::string ,GeometryFlags> MultiEnumBinder<GeometryFlags,GeometryFlagsBinder>::m_NameToEnumMap;
	
	std::map<GeometryFlags, GeometryFlags> GeometryFlagManager::m_CollisionMaskMap;

	GeometryFlags GeometryFlagManager::GetMask(GeometryFlags geom_flag)
	{
		const std::map<GeometryFlags, GeometryFlags>::const_iterator iter = m_CollisionMaskMap.find(geom_flag);
		if(iter != m_CollisionMaskMap.end())
		{
			return iter->second;
		}
		else
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"Failed to find collision mask", "GeometryFlagManager::GetMask");

	}

	void GeometryFlagManager::LoadGeometryFlagsFile(const std::string &file)
	{

		GASS_LOG(LINFO) << "Start loading collision matrix file " << file;
		auto *xml_doc = new tinyxml2::XMLDocument();
		if (xml_doc->LoadFile(file.c_str()) != tinyxml2::XML_NO_ERROR)
		{
			delete xml_doc;
			GASS_EXCEPT(Exception::ERR_CANNOT_READ_FILE,"Couldn't load:" + file, "GeometryFlagManager::LoadMaterialFile");
		}

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
				{
					delete xml_doc;
					GASS_EXCEPT(Exception::ERR_CANNOT_READ_FILE,"Couldn't find Name attribute in:" + file, "GeometryFlagManager::LoadMaterialFile");
				}
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
		delete xml_doc;
	}
}
