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

#include "PhysXPhysicsSystem.h"
#include "Sim/Interface/GASSIMaterialSystem.h"
#include <tinyxml.h>

using namespace physx;
namespace GASS
{

	class PxGASSErrorCallback : public PxErrorCallback
	{
	public:
		PxGASSErrorCallback() :PxErrorCallback(){}
		virtual ~PxGASSErrorCallback(){}

		virtual void PxGASSErrorCallback::reportError(PxErrorCode::Enum e, const char* message, const char* file, int line)
		{
			std::string errorCode;

			switch (e)
			{
			case PxErrorCode::eINVALID_PARAMETER:
				errorCode = "invalid parameter";
				break;
			case PxErrorCode::eINVALID_OPERATION:
				errorCode = "invalid operation";
				break;
			case PxErrorCode::eOUT_OF_MEMORY:
				errorCode = "out of memory";
				break;
			case PxErrorCode::eDEBUG_INFO:
				errorCode = "info";
				break;
			case PxErrorCode::eDEBUG_WARNING:
				errorCode = "warning";
				break;
			default:
				errorCode = "unknown error";
				break;
			}
			std::stringstream ss;
			ss << "PhysX Error Callback:" << file << "(" << line << ") :" << errorCode << " : " << message << "\n";
			GASS::LogManager::getSingleton().stream() << ss.str();

		}
	};

	PhysXPhysicsSystem::PhysXPhysicsSystem(): m_DefaultMaterial(NULL),
		m_PhysicsSDK(NULL),
		m_Foundation(NULL)
	{

	}

	PhysXPhysicsSystem::~PhysXPhysicsSystem()
	{
		if(m_PhysicsSDK)
			m_PhysicsSDK->release();
	}

	void PhysXPhysicsSystem::RegisterReflection()
	{
		SystemFactory::GetPtr()->Register("PhysXPhysicsSystem",new GASS::Creator<PhysXPhysicsSystem, ISystem>);
		REG_PROPERTY(int,MaxNumThreads,PhysXPhysicsSystem);
	}

	PxGASSErrorCallback myErrorCallback;
	void PhysXPhysicsSystem::Init()
	{
		SimEngine::Get().GetRuntimeController()->Register(shared_from_this(),m_TaskNodeName);

		bool recordMemoryAllocations = false;
		m_Foundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_DefaultAllocator, myErrorCallback);
		m_PhysicsSDK = PxCreatePhysics(PX_PHYSICS_VERSION, *m_Foundation, physx::PxTolerancesScale(), recordMemoryAllocations );
		PxInitExtensions(*m_PhysicsSDK );
		if(m_PhysicsSDK == NULL) 
		{
			GASS_EXCEPT(Exception::ERR_INTERNAL_ERROR,"Error creating PhysX device!", "PhysXPhysicsSystem::OnInit");
		}
		if(!PxInitExtensions(*m_PhysicsSDK))
			GASS_EXCEPT(Exception::ERR_INTERNAL_ERROR,"PxInitExtensions failed!", "PhysXPhysicsSystem::OnInit");

		m_DefaultMaterial = m_PhysicsSDK->createMaterial(0.5,0.5,0.5);

		m_Cooking = PxCreateCooking(PX_PHYSICS_VERSION, *m_Foundation, PxCookingParams());
		if(!m_Cooking)
			GASS_EXCEPT(Exception::ERR_INTERNAL_ERROR,"PxCreateCooking failed!", "PhysXPhysicsSystem::OnInit");

		//Create physx materials
		MaterialSystemPtr mat_system = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<IMaterialSystem>();
		IMaterialSystem::MaterialMap materials = mat_system->GetMaterials();
		IMaterialSystem::MaterialMap::iterator iter = materials.begin();
		while(iter != materials.end()) 
		{
			PhysicsMaterial mat_data = iter->second;
			m_Materials[iter->first] = GetPxSDK()->createMaterial(mat_data.StaticFriction, mat_data.DynamicFriction, mat_data.Restitution);
			iter++;
		}
		


			//Initialise the sdk.
		PxInitVehicleSDK(*m_PhysicsSDK);

		//Set the basis vectors.
		PxVec3 up(0,1,0);
		PxVec3 forward(0,0,-1);
		PxVehicleSetBasisVectors(up, forward);

		//Set the vehicle update mode to be immediate velocity changes.
		PxVehicleSetUpdateMode(PxVehicleUpdateMode::eVELOCITY_CHANGE);

		//load vehicle settings
		FilePath path("%GASS_DATA_HOME%/Physics/VehicleSettings.xml");
		
		LoadTires(path.GetFullPath());
		
		for(size_t i=0; i< m_DrivableMaterialNames.size() ; i++) 
		{
			//Create a new material.
			PhysicsMaterial mat_data = mat_system->GetMaterial(m_DrivableMaterialNames[i]);
			//TODO: load alla materials from start
			m_DrivableMaterials.push_back(GetPxSDK()->createMaterial(mat_data.StaticFriction, mat_data.DynamicFriction, mat_data.Restitution));
			//Set up the drivable surface type that will be used for the new material.
			physx::PxVehicleDrivableSurfaceType vdst;
			vdst.mType = static_cast<int>(i);
			m_VehicleDrivableSurfaceTypes.push_back(vdst);
		}
		
		m_SurfaceTirePairs=PxVehicleDrivableSurfaceToTireFrictionPairs::allocate((int)m_Tires.size(),(int)m_DrivableMaterials.size());
		m_SurfaceTirePairs->setup((int)m_Tires.size(),(int)m_DrivableMaterials.size(),(const PxMaterial**)&m_DrivableMaterials[0],&m_VehicleDrivableSurfaceTypes[0]);
	
		//m_SurfaceTirePairs = PxVehicleDrivableSurfaceToTireFrictionPairs::create((int)m_Tires.size(),(int)m_DrivableMaterials.size(),(const PxMaterial**)&m_DrivableMaterials[0],&m_VehicleDrivableSurfaceTypes[0]);
		for(PxU32 i=0; i < m_DrivableMaterials.size(); i++)
		{
			for(PxU32 j=0;j<m_Tires.size();j++)
			{
				if(m_Tires[j].FrictionMultipliers.end() != m_Tires[j].FrictionMultipliers.find(m_DrivableMaterialNames[i]))
					m_SurfaceTirePairs->setTypePairFriction(i,j,m_Tires[j].FrictionMultipliers[m_DrivableMaterialNames[i]]);
				else
				{
					//exception?
					m_SurfaceTirePairs->setTypePairFriction(i,j,1.0);
				}
			}
		}

		//physx::PxExtensionVisualDebugger::connect(mSDK->getPvdConnectionManager(), "127.0.0.1", 5425, 10, true,physx::PxGetDefaultDebuggerFlags());
	}

	physx::PxMaterial* PhysXPhysicsSystem::GetMaterial(const std::string &name) const
	{
		std::map<std::string,physx::PxMaterial*>::const_iterator iter = m_Materials.find(name);
		if(iter != m_Materials.end())
			return iter->second;
		GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Failed to find physics material:" + name,"PhysXPhysicsSystem::GetMaterial");
	}

	int PhysXPhysicsSystem::GetTireIDFromName(const std::string &name) const
	{
		for(PxU32 i=0;i < m_Tires.size();i++)
		{
			if(m_Tires[i].Name == name)
				return i;
		}
		GASS_EXCEPT(Exception::ERR_CANNOT_READ_FILE,"Tire named:" + name + " not found", "PhysXPhysicsSystem::GetTyreIDFromName");
	}

	void PhysXPhysicsSystem::LoadTires(const std::string &file)
	{
		LogManager::getSingleton().stream() << "Start loading tire settings file " << file;
		TiXmlDocument *xmlDoc = new TiXmlDocument(file.c_str());
		if (!xmlDoc->LoadFile())
			GASS_EXCEPT(Exception::ERR_CANNOT_READ_FILE,"Couldn't load:" + file, "MaterialSystem::LoadMaterialFile");

		TiXmlElement *xml_vs = xmlDoc->FirstChildElement("VehicleSettings");
		if(xml_vs)
		{
			TiXmlElement *xml_sl = xml_vs->FirstChildElement("SurfaceList");
			if(xml_sl )
			{
				TiXmlElement *xml_ds = xml_sl->FirstChildElement("DriveableSurface");
				while(xml_ds)
				{
					if(xml_ds->Attribute("MaterialName"))
						m_DrivableMaterialNames.push_back(xml_ds->Attribute("MaterialName"));
					else
						GASS_EXCEPT(Exception::ERR_CANNOT_READ_FILE,"Couldn't find MaterialName attribute in:" + file, "PhysXPhysicsSystem::LoadTires");
					xml_ds = xml_ds->NextSiblingElement("DriveableSurface");
				}
			}
			TiXmlElement *xml_ml = xml_vs->FirstChildElement("TireList");
			if(xml_ml)
			{
				TiXmlElement *xml_td = xml_ml->FirstChildElement("Tire");
				while(xml_td)
				{
					TireData data;
					if(xml_td->Attribute("Name"))
						data.Name = xml_td->Attribute("Name");
					else
						GASS_EXCEPT(Exception::ERR_CANNOT_READ_FILE,"Couldn't find Name attribute in:" + file, "PhysXPhysicsSystem::LoadTires");

					TiXmlElement *xml_fm = xml_td->FirstChildElement("FrictionMultiplier");
					while(xml_fm)
					{
						std::string mat_name;
						double fm = 1;
						if(xml_fm->Attribute("MaterialName"))
							mat_name = xml_fm->Attribute("MaterialName");
						else
							GASS_EXCEPT(Exception::ERR_CANNOT_READ_FILE,"Couldn't find MaterialName attribute in:" + file, "PhysXPhysicsSystem::LoadTires");
						xml_fm->QueryDoubleAttribute("Multiplier",&fm);
						data.FrictionMultipliers[mat_name] = fm;
						xml_fm = xml_fm->NextSiblingElement("FrictionMultiplier");
					}
					m_Tires.push_back(data);
					xml_td = xml_td->NextSiblingElement("Tire");
				}
			}
		}
		delete xmlDoc;
	}
}
