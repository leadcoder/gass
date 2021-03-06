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

#include "PhysXPhysicsSystem.h"
#include "Sim/Interface/GASSIMaterialSystem.h"
#include "Core/Serialize/tinyxml2.h"

using namespace physx;
namespace GASS
{

	class PxGASSErrorCallback : public PxErrorCallback
	{
	public:
		PxGASSErrorCallback() :PxErrorCallback(){}
		~PxGASSErrorCallback() override{}

        void reportError(PxErrorCode::Enum code, const char* message, const char* file, int line) override
		{
			std::string errorCode;

			switch (code)
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
			GASS_LOG(LERROR) << ss.str();

		}
	};

	PhysXPhysicsSystem::PhysXPhysicsSystem(SimSystemManagerWeakPtr manager) : Reflection(manager) , m_DefaultMaterial(NULL),
		m_PhysicsSDK(NULL),
		m_Foundation(NULL)
	{
		m_UpdateGroup = UGID_SIM;

	}

	PhysXPhysicsSystem::~PhysXPhysicsSystem()
	{
		if(m_PhysicsSDK)
			m_PhysicsSDK->release();
	}

	void PhysXPhysicsSystem::RegisterReflection()
	{
		SystemFactory::GetPtr()->Register<PhysXPhysicsSystem>("PhysXPhysicsSystem");
		RegisterMember("MaxNumThreads", &GASS::PhysXPhysicsSystem::m_MaxNumThreads);
	}

	PxGASSErrorCallback myErrorCallback;
	void PhysXPhysicsSystem::OnSystemInit()
	{
		bool recordMemoryAllocations = false;
		m_Foundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_DefaultAllocator, myErrorCallback);
		m_PhysicsSDK = PxCreatePhysics(PX_PHYSICS_VERSION, *m_Foundation, physx::PxTolerancesScale(), recordMemoryAllocations );
		
		if(m_PhysicsSDK == NULL)
		{
			GASS_EXCEPT(Exception::ERR_INTERNAL_ERROR,"Error creating PhysX device!", "PhysXPhysicsSystem::OnInit");
		}
		if(!PxInitExtensions(*m_PhysicsSDK, nullptr))
			GASS_EXCEPT(Exception::ERR_INTERNAL_ERROR,"PxInitExtensions failed!", "PhysXPhysicsSystem::OnInit");

		//m_DefaultMaterial = m_PhysicsSDK->createMaterial(0.5,0.5,0.5);

		PxTolerancesScale scale;
		PxCookingParams params(scale);
		params.meshWeldTolerance = 0.001f;
		params.meshPreprocessParams = PxMeshPreprocessingFlags(PxMeshPreprocessingFlag::eWELD_VERTICES);

		m_Cooking = PxCreateCooking(PX_PHYSICS_VERSION, *m_Foundation, params);
		if(!m_Cooking)
			GASS_EXCEPT(Exception::ERR_INTERNAL_ERROR,"PxCreateCooking failed!", "PhysXPhysicsSystem::OnInit");

		//Initialize the SDK.
		PxInitVehicleSDK(*m_PhysicsSDK);

		//Create physx materials
		MaterialSystemPtr mat_system = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<IMaterialSystem>();
		IMaterialSystem::MaterialMap materials = mat_system->GetMaterials();
		IMaterialSystem::MaterialMap::iterator iter = materials.begin();
		while(iter != materials.end())
		{
			PhysicsMaterial mat_data = iter->second;
			m_Materials[iter->first] = GetPxSDK()->createMaterial(static_cast<float>(mat_data.StaticFriction), static_cast<float>(mat_data.DynamicFriction), static_cast<float>(mat_data.Restitution));
			++iter;
		}
		m_DefaultMaterial = m_Materials["DEFAULT"];



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
			//PhysicsMaterial mat_data = mat_system->GetMaterial(m_DrivableMaterialNames[i]);
			//TODO: load all materials from start
			
			//m_DrivableMaterials.push_back(GetPxSDK()->createMaterial(mat_data.StaticFriction, mat_data.DynamicFriction, mat_data.Restitution));
			//Set up the drivable surface type that will be used for the new material.
			m_DrivableMaterials.push_back(m_Materials[m_DrivableMaterialNames[i]]);
			physx::PxVehicleDrivableSurfaceType vdst;
			vdst.mType = static_cast<int>(i);
			m_VehicleDrivableSurfaceTypes.push_back(vdst);
		}

		GASS_LOG(LINFO) << "Create  PxVehicleDrivableSurfaceToTireFrictionPairs...";
		m_SurfaceTirePairs=PxVehicleDrivableSurfaceToTireFrictionPairs::allocate((int)m_Tires.size(),(int)m_DrivableMaterials.size());
		m_SurfaceTirePairs->setup((int)m_Tires.size(),(int)m_DrivableMaterials.size(),(const PxMaterial**)&m_DrivableMaterials[0],&m_VehicleDrivableSurfaceTypes[0]);

		//m_SurfaceTirePairs = PxVehicleDrivableSurfaceToTireFrictionPairs::create((int)m_Tires.size(),(int)m_DrivableMaterials.size(),(const PxMaterial**)&m_DrivableMaterials[0],&m_VehicleDrivableSurfaceTypes[0]);
		GASS_LOG(LINFO) << "setup tire friction...";

		for(PxU32 i=0; i < m_DrivableMaterials.size(); i++)
		{
			for(PxU32 j=0;j < m_Tires.size();j++)
			{
				if(m_Tires[j].FrictionMultipliers.end() != m_Tires[j].FrictionMultipliers.find(m_DrivableMaterialNames[i]))
				{
					physx::PxReal friction = static_cast<float>(m_Tires[j].FrictionMultipliers[m_DrivableMaterialNames[i]]);
					m_SurfaceTirePairs->setTypePairFriction(i,j,friction);
				}
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
		GASS_LOG(LINFO) << "Start loading tire settings file " << file;
		tinyxml2::XMLDocument *xmlDoc = new tinyxml2::XMLDocument();
		if (xmlDoc->LoadFile(file.c_str()) != tinyxml2::XML_NO_ERROR)
			GASS_EXCEPT(Exception::ERR_CANNOT_READ_FILE,"Couldn't load:" + file, "PhysXPhysicsSystem::LoadTires");

		tinyxml2::XMLElement *xml_vs = xmlDoc->FirstChildElement("VehicleSettings");
		if(xml_vs)
		{
			tinyxml2::XMLElement *xml_sl = xml_vs->FirstChildElement("SurfaceList");
			if(xml_sl )
			{
				tinyxml2::XMLElement *xml_ds = xml_sl->FirstChildElement("DriveableSurface");
				while(xml_ds)
				{
					if(xml_ds->Attribute("MaterialName"))
						m_DrivableMaterialNames.push_back(xml_ds->Attribute("MaterialName"));
					else
						GASS_EXCEPT(Exception::ERR_CANNOT_READ_FILE,"Couldn't find MaterialName attribute in:" + file, "PhysXPhysicsSystem::LoadTires");
					xml_ds = xml_ds->NextSiblingElement("DriveableSurface");
				}
			}
			tinyxml2::XMLElement *xml_ml = xml_vs->FirstChildElement("TireList");
			if(xml_ml)
			{
				tinyxml2::XMLElement *xml_td = xml_ml->FirstChildElement("Tire");
				while(xml_td)
				{
					TireData data;
					if(xml_td->Attribute("Name"))
						data.Name = xml_td->Attribute("Name");
					else
						GASS_EXCEPT(Exception::ERR_CANNOT_READ_FILE,"Couldn't find Name attribute in:" + file, "PhysXPhysicsSystem::LoadTires");

					tinyxml2::XMLElement *xml_fm = xml_td->FirstChildElement("FrictionMultiplier");
					while(xml_fm)
					{
						std::string mat_name;
						double fm = 1;
						if(xml_fm->Attribute("MaterialName"))
							mat_name = xml_fm->Attribute("MaterialName");
						else
							GASS_EXCEPT(Exception::ERR_CANNOT_READ_FILE,"Couldn't find MaterialName attribute in:" + file, "PhysXPhysicsSystem::LoadTires");

						if(xml_fm->Attribute("Friction"))
							xml_fm->QueryDoubleAttribute("Friction",&fm);
						else
							GASS_EXCEPT(Exception::ERR_CANNOT_READ_FILE,"Couldn't find Multiplier attribute in:" + file, "PhysXPhysicsSystem::LoadTires");
						
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
