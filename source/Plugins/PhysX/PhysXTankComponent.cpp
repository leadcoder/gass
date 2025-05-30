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

#include <memory>

#include "Plugins/PhysX/PhysXTankComponent.h"
#include "Plugins/PhysX/PhysXWheelComponent.h"
#include "Plugins/PhysX/PhysXPhysicsSceneManager.h"
#include "Plugins/PhysX/PhysXPhysicsSystem.h"
#include "Plugins/PhysX/PhysXVehicleSceneQuery.h"
#include "Plugins/PhysX/PhysXBodyComponent.h"
#include "Sim/Messages/GASSSoundSceneObjectMessages.h"
#include "Sim/Messages/GASSPlatformMessages.h"
#include "Sim/Interface/GASSITextComponent.h"
#include "Sim/Interface/GASSISoundComponent.h"

using namespace physx;
namespace GASS
{
	PhysXTankComponent::PhysXTankComponent(): 
		m_ChassisDim(0,0,0),
		
		m_MassOffset(0,0,0)
		
	{
		//add some default gears, start with reverse!
		m_GearRatios.push_back(-4); //reverse
		m_GearRatios.push_back(0); //neutral
		m_GearRatios.push_back(4); //first gear
		m_GearRatios.push_back(2);
		m_GearRatios.push_back(1.5f);
		m_GearRatios.push_back(1.1f);
		m_GearRatios.push_back(1.0f);
	}

	PhysXTankComponent::~PhysXTankComponent()
	{

	}


	void PhysXTankComponent::OnDelete()
	{
		if(m_Vehicle)
		{
			PhysXPhysicsSceneManagerPtr scene_manager = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<PhysXPhysicsSceneManager>();
			scene_manager->UnregisterVehicle(m_Vehicle);
			m_Vehicle->free();
			m_Vehicle = nullptr;
			if(m_Actor)
			{
				scene_manager->GetPxScene()->removeActor(*m_Actor);
				m_Actor->release();
				m_Actor = nullptr;
			}
		}
	}

	void PhysXTankComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register<PhysXTankComponent>();
		GetClassRTTI()->SetMetaData(std::make_shared<ClassMetaData>("PhysXTankComponent", OF_VISIBLE));

		RegisterMember("Mass", &GASS::PhysXTankComponent::m_Mass);
		RegisterMember("MassOffset", &GASS::PhysXTankComponent::m_MassOffset);
		RegisterMember("ScaleMass", &GASS::PhysXTankComponent::m_ScaleMass);
		RegisterMember("EnginePeakTorque", &GASS::PhysXTankComponent::m_EnginePeakTorque);
		RegisterMember("EngineMaxRotationSpeed", &GASS::PhysXTankComponent::m_EngineMaxRotationSpeed);
		RegisterMember("ClutchStrength", &GASS::PhysXTankComponent::m_ClutchStrength);
		RegisterMember("GearRatios", &GASS::PhysXTankComponent::m_GearRatios);
		RegisterMember("UseAutoReverse", &GASS::PhysXTankComponent::m_UseAutoReverse);
		RegisterMember("GearSwitchTime", &GASS::PhysXTankComponent::m_GearSwitchTime);
		RegisterMember("Wheels", &GASS::PhysXTankComponent::m_Wheels);
		RegisterMember("MaxSpeed", &GASS::PhysXTankComponent::m_MaxSpeed);
		RegisterMember("SteerLimit", &GASS::PhysXTankComponent::m_SteerLimit);
		RegisterMember("Debug", &GASS::PhysXTankComponent::m_Debug, PF_VISIBLE | PF_EDITABLE,"");
	}

	PxVec3 PhysXTankComponent::ComputeDim(const PxConvexMesh* cm)
	{
		const PxU32 num_verts = cm->getNbVertices();
		const PxVec3* verts = cm->getVertices();
		PxVec3 cm_min(PX_MAX_F32,PX_MAX_F32,PX_MAX_F32);
		PxVec3 cm_max(-PX_MAX_F32,-PX_MAX_F32,-PX_MAX_F32);
		for(PxU32 i=0;i< num_verts; i++)
		{
			cm_min.x=PxMin(cm_min.x,verts[i].x);
			cm_min.y=PxMin(cm_min.y,verts[i].y);
			cm_min.z=PxMin(cm_min.z,verts[i].z);
			cm_max.x=PxMax(cm_max.x,verts[i].x);
			cm_max.y=PxMax(cm_max.y,verts[i].y);
			cm_max.z=PxMax(cm_max.z,verts[i].z);
		}
		const PxVec3 dims=cm_max-cm_min;
		return dims;
	}

	void PhysXTankComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXTankComponent::OnLocationLoaded,LocationLoadedEvent,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXTankComponent::OnTransformationChanged, TransformationChangedEvent, 0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXTankComponent::OnInput,InputRelayEvent,0));
		GetSceneObject()->GetScene()->RegisterForMessage(REG_TMESS(PhysXTankComponent::OnPostSceneObjectInitializedEvent,PostSceneObjectInitializedEvent,0));
		RegisterForPostUpdate<PhysXPhysicsSceneManager>();

		//assume sound is loaded now
		m_Sound = GetSceneObject()->GetFirstComponentByClass<ISoundComponent>().get();
		if (m_Sound)
			m_Sound->SetPlay(true);
	}

	Vec3 PhysXTankComponent::GetSize() const
	{
		return m_ChassisDim;
	}

	void PhysXTankComponent::Reset()
	{
		if(!m_Vehicle)
			return;

		//Set the car back to its rest state.
		m_Vehicle->setToRestState();
		//Set the car to first gear.
		m_Vehicle->mDriveDynData.forceGearChange(PxVehicleGearsData::eFIRST);
		m_IsMovingForwardSlowly = false;
		m_ThrottleInput = 0;
		m_SteerInput = 0;
		m_InReverseMode = false;

		for(size_t i = 0; i < m_Vehicle->mWheelsSimData.getNbWheels(); i++)
		{
			m_Vehicle->mWheelsSimData.setWheelShapeMapping(static_cast<physx::PxI32>(i),static_cast<physx::PxU32>(i));
		}
	}

	void PhysXTankComponent::OnPostSceneObjectInitializedEvent(PostSceneObjectInitializedEventPtr message)
	{
		if(message->GetSceneObject() != GetSceneObject())
			return;
		physx::PxVehicleChassisData chassis_data;

		chassis_data.mMass = m_Mass*m_ScaleMass;

		//Get chassis mesh
		std::string col_mesh_id = GetSceneObject()->GetName();
		ResourceComponentPtr res = GetSceneObject()->GetFirstComponentByClass<IResourceComponent>();
		if(res)
		{
			col_mesh_id = res->GetResource().Name();
		}

		MeshComponentPtr mesh = GetSceneObject()->GetFirstComponentByClass<IMeshComponent>();
		GASSAssert(mesh,"PhysXTankComponent::OnInitialize");
		PhysXPhysicsSceneManagerPtr scene_manager = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<PhysXPhysicsSceneManager>();
		GASSAssert(scene_manager,"PhysXTankComponent::OnInitialize");
		PhysXConvexMesh chassis_mesh = scene_manager->CreateConvexMesh(col_mesh_id,mesh);

		GeometryComponentPtr geom = GetSceneObject()->GetFirstComponentByClass<IGeometryComponent>();
		GASSAssert(geom,"PhysXTankComponent::OnInitialize");
		m_MeshBounds = geom->GetBoundingBox();

		std::vector<PxConvexMesh*> wheel_convex_meshes;
		std::vector<PxVec3 > wheel_centre_offsets;

		//Add wheels
		std::vector<SceneObjectPtr> wheel_objects;

		for(size_t i= 0; i < m_Wheels.size(); i++)
		{
			wheel_objects.push_back(m_Wheels[i].GetRefObject());
		}

		size_t num_wheels = wheel_objects.size();

		//Get data from wheels
		std::vector<PxVehicleWheelData> wheels;
		std::vector<PxVehicleSuspensionData> susps;
		std::vector<PxVehicleTireData> tires;
		wheels.resize(num_wheels);
		susps.resize(num_wheels);
		tires.resize(num_wheels);
		for(size_t i = 0 ; i < wheel_objects.size(); i++)
		{
			SceneObjectPtr child = wheel_objects[i];
			//we need to force geometry changed message to be fired before we can get data from wheel
			child->SyncMessages(0);

			std::string child_col_mesh_id = child->GetName();
			ResourceComponentPtr child_res = child->GetFirstComponentByClass<IResourceComponent>();
			if(child_res)
			{
				child_col_mesh_id = child_res->GetResource().Name();
			}
			MeshComponentPtr child_mesh = child->GetFirstComponentByClass<IMeshComponent>();
			LocationComponentPtr location = child->GetFirstComponentByClass<ILocationComponent>();
			PhysXConvexMesh wheel_mesh = scene_manager->CreateConvexMesh(child_col_mesh_id, child_mesh);
			wheel_convex_meshes.push_back(wheel_mesh.m_ConvexMesh);
			Vec3 pos = location->GetPosition();
			wheel_centre_offsets.push_back(PxConvert::ToPx(pos));

			PhysXWheelComponentPtr wheel_comp = child->GetFirstComponentByClass<PhysXWheelComponent>();
			wheels[i] = wheel_comp->GetWheelData();
			susps[i]  = wheel_comp->GetSuspensionData();
			tires[i]  = wheel_comp->GetTireData();
		}

		PxVehicleWheelsSimData* wheels_sim_data = PxVehicleWheelsSimData::allocate(static_cast<physx::PxU32>(num_wheels));
		PxVehicleDriveSimData drive_sim_data;

		//Extract the chassis AABB dimensions from the chassis convex mesh.
		const PxVec3 chassis_dims=ComputeDim(chassis_mesh.m_ConvexMesh);

		m_ChassisDim = PxConvert::ToGASS(chassis_dims);


		//The origin is at the center of the chassis mesh.
		//Set the center of mass to be below this point and a little towards the front.
		//const PxVec3 chassisCMOffset=PxVec3(0.0f,-chassisDims.y*0.5f+0.65f,0.125f);

		const PxVec3 chassis_cm_offset = PxConvert::ToPx(m_MassOffset);

		//Now compute the chassis mass and moment of inertia.
		//Use the moment of inertia of a cuboid as an approximate value for the chassis moi.
		PxVec3 chassis_moi
			((chassis_dims.y*chassis_dims.y + chassis_dims.z*chassis_dims.z)*chassis_data.mMass/12.0f,
			(chassis_dims.x*chassis_dims.x + chassis_dims.z*chassis_dims.z)*chassis_data.mMass/12.0f,
			(chassis_dims.x*chassis_dims.x + chassis_dims.y*chassis_dims.y)*chassis_data.mMass/12.0f);
		//A bit of tweaking here.  The car will have more responsive turning if we reduce the
		//y-component of the chassis moment of inertia.
		chassis_moi.y*=0.8f;

		//Let's set up the chassis data structure now.
		chassis_data.mMOI=chassis_moi;
		chassis_data.mCMOffset=chassis_cm_offset;

		//Work out the front/rear mass split from the cm offset.
		//This is a very approximate calculation with lots of assumptions.
		//massRear*zRear + massFront*zFront = mass*cm           (1)
		//massRear       + massFront        = mass                      (2)
		//Rearrange (2)
		//massFront = mass - massRear                                           (3)
		//Substitute (3) into (1)
		//massRear(zRear - zFront) + mass*zFront = mass*cm      (4)
		//Solve (4) for massRear
		//massRear = mass(cm - zFront)/(zRear-zFront)           (5)
		//Now we also have
		//zFront = (z-cm)/2                                                                     (6a)
		//zRear = (-z-cm)/2                                                                     (6b)
		//Substituting (6a-b) into (5) gives
		//massRear = 0.5*mass*(z-3cm)/z                                         (7)
	

		//set mSprungMass from vehicle mass move this to wheel?
		PxF32 sprung_mass= chassis_data.mMass / (PxF32)wheels.size();
		
		for(size_t i = 0; i < wheels.size() ; i++)
		{
			susps[i].mSprungMass=sprung_mass;
			//tires[i].mType = 0;
		}

		//apply mass scale to wheels
		for(size_t i = 0; i < wheels.size() ; i++)
		{
			//wheels[i].mMOI *= m_ScaleMass;
			wheels[i].mMaxHandBrakeTorque *= m_ScaleMass;
			wheels[i].mMaxBrakeTorque *= m_ScaleMass;
			wheels[i].mDampingRate *= m_ScaleMass;
			susps[i].mSpringStrength *= m_ScaleMass;
			susps[i].mSpringDamperRate *= m_ScaleMass;
			tires[i].mLongitudinalStiffnessPerUnitGravity *= m_ScaleMass;
		}

		//We need to set up geometry data for the suspension, wheels, and tires.
		//We already know the wheel centers described as offsets from the rigid body centre of mass.
		//From here we can approximate application points for the tire and suspension forces.
		//Lets assume that the suspension travel directions are absolutely vertical.
		//Also assume that we apply the tire and suspension forces 30cm below the centre of mass.
		std::vector<PxVec3> susp_travel_directions;
		std::vector<PxVec3> wheel_centre_cm_offsets;
		std::vector<PxVec3> susp_force_app_cm_offsets;
		std::vector<PxVec3> tire_force_app_cm_offsets;
		susp_travel_directions.resize(num_wheels);
		wheel_centre_cm_offsets.resize(num_wheels);
		susp_force_app_cm_offsets.resize(num_wheels);
		tire_force_app_cm_offsets.resize(num_wheels);

		for(PxU32 i=0;i<num_wheels;i++)
		{
			susp_travel_directions[i]= PxVec3(0,-1,0);
			wheel_centre_cm_offsets[i]= wheel_centre_offsets[i]-chassis_cm_offset;
			susp_force_app_cm_offsets[i]=PxVec3(wheel_centre_cm_offsets[i].x,-0.3f,wheel_centre_cm_offsets[i].z);
			tire_force_app_cm_offsets[i]=PxVec3(wheel_centre_cm_offsets[i].x,-0.3f,wheel_centre_cm_offsets[i].z);
		}

		//Now add the wheel, tire and suspension data.
		for(PxU32 i=0;i<num_wheels;i++)
		{
			wheels_sim_data->setWheelData(i,wheels[i]);
			wheels_sim_data->setTireData(i,tires[i]);
			wheels_sim_data->setSuspensionData(i,susps[i]);
			wheels_sim_data->setSuspTravelDirection(i,susp_travel_directions[i]);
			wheels_sim_data->setWheelCentreOffset(i,wheel_centre_cm_offsets[i]);
			wheels_sim_data->setSuspForceAppPointOffset(i,susp_force_app_cm_offsets[i]);
			wheels_sim_data->setTireForceAppPointOffset(i,tire_force_app_cm_offsets[i]);
		}

		//Now set up the engine, gears, clutch

		//Engine
		PxVehicleEngineData engine;

		engine.mPeakTorque=m_EnginePeakTorque;
		engine.mMaxOmega=m_EngineMaxRotationSpeed;//approx 6000 rpm

		/*engine.mDampingRateFullThrottle *= m_ScaleMass;
		engine.mDampingRateZeroThrottleClutchDisengaged *= m_ScaleMass;
		engine.mDampingRateZeroThrottleClutchEngaged *= m_ScaleMass;
		*/

		engine.mDampingRateZeroThrottleClutchEngaged = 2.0f;
		engine.mDampingRateZeroThrottleClutchDisengaged = 0.5f;
		engine.mDampingRateFullThrottle = 0.5f;

		drive_sim_data.setEngineData(engine);

		//Gears
		PxVehicleGearsData gears;

		if(m_GearRatios.size() >= PxVehicleGearsData::eGEARSRATIO_COUNT)
		{
			GASS_EXCEPT(Exception::ERR_INVALIDPARAMS, "To many gear in vehicle","PhysXTankComponent::OnPostSceneObjectInitializedEvent");
		}
		gears.mNbRatios= static_cast<physx::PxU32>(m_GearRatios.size());
		for(size_t i = 0 ; i< m_GearRatios.size(); i++)
		{
			gears.mRatios[i] = m_GearRatios[i];
		}
		gears.mSwitchTime = m_GearSwitchTime;
		drive_sim_data.setGearsData(gears);

		//Clutch
		PxVehicleClutchData clutch;
		clutch.mStrength = m_ClutchStrength*m_ScaleMass;
		drive_sim_data.setClutchData(clutch);

		//Ackermann steer accuracy
		/*PxVehicleAckermannGeometryData ackermann;
		ackermann.mAccuracy=1.0f;
		ackermann.mAxleSeparation= fabs(wheelCentreOffsets[PxVehicleDrive4W::eFRONT_LEFT_WHEEL].z - wheelCentreOffsets[PxVehicleDrive4W::eREAR_LEFT_WHEEL].z);
		ackermann.mFrontWidth=fabs(wheelCentreOffsets[PxVehicleDrive4W::eFRONT_RIGHT_WHEEL].x-wheelCentreOffsets[PxVehicleDrive4W::eFRONT_LEFT_WHEEL].x);
		ackermann.mRearWidth=fabs(wheelCentreOffsets[PxVehicleDrive4W::eREAR_RIGHT_WHEEL].x-wheelCentreOffsets[PxVehicleDrive4W::eREAR_LEFT_WHEEL].x);
		driveSimData.setAckermannGeometryData(ackermann);*/

		PhysXPhysicsSystemPtr system = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<PhysXPhysicsSystem>();

		//We need a rigid body actor for the vehicle.
		//Don't forget to add the actor the scene after setting up the associated vehicle.
		m_Actor = system->GetPxSDK()->createRigidDynamic(PxTransform(PxIdentity));

		const PxMaterial& wheel_material = *system->GetDefaultMaterial();
		PxFilterData wheel_coll_filter_data;
		wheel_coll_filter_data.word0=GEOMETRY_FLAG_RAY_CAST_WHEEL;
		GeometryFlags against = GeometryFlagManager::GetMask(GEOMETRY_FLAG_RAY_CAST_WHEEL);
		wheel_coll_filter_data.word1=against;

		//Create a query filter data for the car to ensure that cars
		//do not attempt to drive on themselves.
		PxFilterData veh_qry_filter_data;
		VehicleSetupVehicleShapeQueryFilterData(&veh_qry_filter_data);

		//Add all the wheel shapes to the actor.
		for(PxU32 i=0;i < num_wheels;i++)
		{
			PxConvexMeshGeometry wheel_geom(wheel_convex_meshes[i]);
			PxShape* wheel_shape = PxRigidActorExt::createExclusiveShape(*m_Actor, wheel_geom, wheel_material);
			wheel_shape->setQueryFilterData(veh_qry_filter_data);
			wheel_shape->setSimulationFilterData(wheel_coll_filter_data);
			wheel_shape->setLocalPose(PxTransform(PxIdentity));
		}

		PxConvexMeshGeometry chassis_convex_geom(chassis_mesh.m_ConvexMesh);
		const PxMaterial& chassis_material= *system->GetDefaultMaterial();
		PxShape* chassis_shape = PxRigidActorExt::createExclusiveShape(*m_Actor, chassis_convex_geom, chassis_material);
		chassis_shape->setQueryFilterData(veh_qry_filter_data);
		chassis_shape->userData = this;

		PxFilterData chassis_coll_filter_data;
		chassis_coll_filter_data.word0=GEOMETRY_FLAG_VEHICLE_CHASSIS;
		against = GeometryFlagManager::GetMask(GEOMETRY_FLAG_VEHICLE_CHASSIS);
		chassis_coll_filter_data.word1=against;
		chassis_shape->setSimulationFilterData(chassis_coll_filter_data);

		chassis_shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE,true);
		chassis_shape->setLocalPose(PxTransform(PxIdentity));

		m_Actor->setMass(chassis_data.mMass);
		m_Actor->setMassSpaceInertiaTensor(chassis_data.mMOI);
		m_Actor->setCMassLocalPose(PxTransform(chassis_data.mCMOffset, PxQuat(PxIdentity)));

		m_Vehicle = PxVehicleDriveTank::allocate(static_cast<physx::PxU32>(num_wheels));
		m_Vehicle->setup(system->GetPxSDK(), m_Actor, *wheels_sim_data,drive_sim_data, static_cast<physx::PxU32>(num_wheels));
		m_Vehicle->setDriveModel(PxVehicleDriveTankControlModel::eSTANDARD);
		//m_Vehicle->setDriveModel(PxVehicleDriveTankControlModel::eSPECIAL);
		for(size_t i = 0; i < num_wheels; i++)
		{
			m_Vehicle->mWheelsSimData.setWheelShapeMapping(static_cast<physx::PxI32>(i),static_cast<physx::PxU32>(i));
		}

		//Free the sim data because we don't need that any more.
		wheels_sim_data->free();

		//Don't forget to add the actor to the scene.
		scene_manager->GetPxScene()->addActor(*m_Actor);

		//Set the transform and the instantiated car and set it be to be at rest.
		LocationComponentPtr location = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>();
		const Vec3 pos = location->GetPosition();

		//NOTE: We dont use PhysXPhysicsSceneManager::WorldToLocal, 
		//location component is just created and the vehicle is not yet moved to it start location.
		m_Actor->setGlobalPose(physx::PxTransform(PxConvert::ToPx(pos), m_Actor->getGlobalPose().q));
		Reset();

		//Set the autogear mode of the instantiate car.
		m_Vehicle->mDriveDynData.setUseAutoGears(true);
		scene_manager->RegisterVehicle(m_Vehicle);

		for(size_t i= 0; i < wheel_objects.size(); i++)
		{
			m_AllWheels.push_back(wheel_objects[i]);
		}

		GetSceneObject()->SendImmediateEvent(std::make_shared<PhysicsBodyLoadedEvent>());
		m_Initialized = true;
	}
	
	void PhysXTankComponent::OnTransformationChanged(TransformationChangedEventPtr event)
	{
		if (m_TrackTransformation)
		{
			//First rotate to get correct wheel rotation in SetPosition
			SetRotation(event->GetRotation());
			SetPosition(event->GetPosition());
		}
	}

	void PhysXTankComponent::OnLocationLoaded(LocationLoadedEventPtr /*message*/)
	{
		PhysXPhysicsSceneManagerPtr sm = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<PhysXPhysicsSceneManager>();
		m_SceneManager = sm;
	}


	PxVehicleKeySmoothingData tank_key_smoothing_data=
	{
		{
			6.0f,	//rise rate eANALOG_INPUT_ACCEL=0,
				6.0f,	//rise rate eANALOG_INPUT_BRAKE,
				6.0f,	//rise rate eANALOG_INPUT_HANDBRAKE,
				2.5f,	//rise rate eANALOG_INPUT_STEER_LEFT,
				2.5f,	//rise rate eANALOG_INPUT_STEER_RIGHT,
		},
		{
			10.0f,	//fall rate eANALOG_INPUT_ACCEL=0,
				10.0f,	//fall rate eANALOG_INPUT_BRAKE,
				10.0f,	//fall rate eANALOG_INPUT_HANDBRAKE,
				5.0f,	//fall rate eANALOG_INPUT_STEER_LEFT,
				5.0f	//fall rate eANALOG_INPUT_STEER_RIGHT,
			}
	};

	PxVehiclePadSmoothingData tank_pad_smoothing_data=
	{
		{
			6.0f,	//rise rate eANALOG_INPUT_ACCEL=0,
				6.0f,	//rise rate eANALOG_INPUT_BRAKE,
				6.0f,	//rise rate eANALOG_INPUT_HANDBRAKE,
				2.5f,	//rise rate eANALOG_INPUT_STEER_LEFT,
				2.5f,	//rise rate eANALOG_INPUT_STEER_RIGHT,
		},
		{
			10.0f,	//fall rate eANALOG_INPUT_ACCEL=0
				10.0f,	//fall rate eANALOG_INPUT_BRAKE_LEFT
				10.0f,	//fall rate eANALOG_INPUT_BRAKE_RIGHT
				5.0f,	//fall rate eANALOG_INPUT_THRUST_LEFT
				5.0f	//fall rate eANALOG_INPUT_THRUST_RIGHT
			}
	};

	void PhysXTankComponent::SceneManagerTick(double delta)
	{
		if(!m_Vehicle)
			return;

		int from_id = GASS_PTR_TO_INT(this); //use address as id
		Vec3 current_pos  = GetPosition();
		Quaternion current_rot = GetRotation();

		m_TrackTransformation = false;
		GetSceneObject()->GetFirstComponentByClass<ILocationComponent>()->SetWorldPosition(current_pos);
		GetSceneObject()->GetFirstComponentByClass<ILocationComponent>()->SetWorldRotation(current_rot);
		m_TrackTransformation = true;
	
		PxShape* car_shapes[PX_MAX_NB_WHEELS+1];
		const PxU32 num_shapes=m_Vehicle->getRigidDynamicActor()->getNbShapes();
		m_Vehicle->getRigidDynamicActor()->getShapes(car_shapes,num_shapes);
		const PxRigidDynamic& vehicle_actor = *m_Vehicle->getRigidDynamicActor();

		if(m_AllWheels.size() == num_shapes-1)
		{
			const PhysXPhysicsSceneManagerPtr sm = m_SceneManager.lock();

			for(size_t i = 0; i < num_shapes-1; i++)
			{
				SceneObjectPtr wheel(m_AllWheels[i]);
				if(wheel)
				{
					const Vec3 pos = sm->LocalToWorld(PxShapeExt::getGlobalPose(*car_shapes[i], vehicle_actor).p);
					const Quaternion rot = PxConvert::ToGASS(PxShapeExt::getGlobalPose(*car_shapes[i],vehicle_actor).q);
					wheel->GetFirstComponentByClass<ILocationComponent>()->SetWorldPosition(pos);
					wheel->GetFirstComponentByClass<ILocationComponent>()->SetWorldRotation(rot);
				}
			}
		}
		else
		{
			GASS_EXCEPT(GASS::Exception::ERR_RT_ASSERTION_FAILED,"Physics shapes dont match visual geomtries","PhysXTankComponent::SceneManagerTick")
		}

		PxVehicleDriveTankRawInputData raw_input_data(m_Vehicle->getDriveModel());

		float left_throttle = m_ThrottleInput;
		float right_throttle = m_ThrottleInput;

		float left_break = 0;
		float right_break = 0;
		float accel = 0;
		//const PxF32 forward_speed = m_Vehicle->computeForwardSpeed();

		if(!m_InReverseMode && m_ThrottleInput < 0.0)
		{
			left_throttle = 0;
			right_throttle = 0;

			left_break = fabs(m_ThrottleInput);
			right_break = left_break;
			accel = 0;
		}
		else if(m_InReverseMode && m_ThrottleInput > 0.0)
		{
			left_throttle = 0;
			right_throttle = 0;

			left_break = fabs(m_ThrottleInput);
			right_break = left_break;
			accel = 0;
		}
		else
			accel = fabs(m_ThrottleInput);

		//damp steering at high speed to avoid drifting
		//const float max_speed = 72.0f;
		//float inter = (max_speed - forward_speed)/max_speed;
		
		//clamp 0..1
		//if(inter < 0.0f ) inter=0;
		//const float min_limit = m_SteerLimit*0.6;
		//const float limit = min_limit + inter*(m_SteerLimit-min_limit);
		//const float limit = m_SteerLimit;
		//GASS_PRINT("Speed(m/s): "<< forward_speed)
		//if(inter < 0.3f ) 
		//	inter=0.3;
		
		//GASS_PRINT("m_SteerInput" << m_SteerInput);
		//GASS_PRINT("m_ThrottleInput" << m_ThrottleInput);
		//GASS_PRINT("m_BreakInput" << m_BreakInput);
		/*std::stringstream ss;
		ss << "m_BreakInput" << m_BreakInput << "\n";
		ss << "m_ThrottleInput" << m_ThrottleInput << "\n";
		ss << "m_BreakInput" << m_BreakInput << "\n";

		LogManager::getSingleton().logMessage(ss.str(), LML_NORMAL);*/
		//GASS_PRINT("forward_speed" << forward_speed);
		
		//left_throttle = 1.0;
		//right_throttle = 1.0;
		
		if(m_SteerInput > 0.1)
		{
			left_break = fabs(m_SteerInput);
			//left_throttle = 0;
			//right_throttle = 1;
			//if (fabs(forward_speed) < 7.0)
			//left_throttle = 0;
			//left_break = fabs(m_SteerInput);
			left_throttle =  (1.0f - fabs(static_cast<float>(m_SteerInput)))*left_throttle;
			right_throttle = fabs(m_SteerInput)*right_throttle;
		}

		if(m_SteerInput < -0.1)
		{
			//if (fabs(forward_speed) < 7.0)
			//right_throttle = 0;
			right_break =  fabs(m_SteerInput);
			//left_throttle = 1;
			//right_throttle = 0;

			right_throttle = (1.0f - fabs(static_cast<float>(m_SteerInput)))*right_throttle;
			left_throttle = fabs(m_SteerInput)*left_throttle;
		}

		if(raw_input_data.getDriveModel() == PxVehicleDriveTankControlModel::eSTANDARD)
		{
			if (left_throttle < 0)
				 left_throttle = -left_throttle;
			if (right_throttle < 0)
				right_throttle = -right_throttle;
		}

		left_break = left_break + m_BreakInput;
		right_break = right_break + m_BreakInput;

		if (left_break > 1.0)
			left_break = 1.0;
		if (right_break > 1.0)
			right_break = 1.0;

		if(m_UseDigitalInputs)
		{
			(accel > 0) ? raw_input_data.setDigitalAccel(true) : raw_input_data.setDigitalAccel(false);
			(left_throttle > 0) ? raw_input_data.setDigitalLeftThrust(true) : raw_input_data.setDigitalLeftThrust(false);
			(right_throttle > 0) ? raw_input_data.setDigitalRightThrust(true) : raw_input_data.setDigitalRightThrust(false);
			(left_break > 0) ? raw_input_data.setDigitalLeftBrake(true) : raw_input_data.setDigitalLeftBrake(false);
			(right_break > 0) ? raw_input_data.setDigitalRightBrake(true) : raw_input_data.setDigitalRightBrake(false);
		}
		else
		{
			/*GASS_PRINT("accel" << accel);
						GASS_PRINT("left_throttle" << left_throttle);
						GASS_PRINT("right_throttle" << right_throttle);
						GASS_PRINT("left_break" << left_break);
						GASS_PRINT("right_break" << right_break);*/
			raw_input_data.setAnalogAccel(accel);
			raw_input_data.setAnalogLeftThrust(left_throttle);
			raw_input_data.setAnalogRightThrust(right_throttle);
			raw_input_data.setAnalogLeftBrake(left_break);
			raw_input_data.setAnalogRightBrake(right_break);
			
		}

		raw_input_data.setGearDown(false);
		raw_input_data.setGearUp(false);
		

		PxVehicleDriveDynData& drive_dyn_data=m_Vehicle->mDriveDynData;

		//Work out if the car is to flip from reverse to forward gear or from forward gear to reverse.
		//Store if the car is moving slowly to help decide if the car is to toggle from reverse to forward in the next update.

		if(m_UseAutoReverse)
		{
			bool toggle_auto_reverse = false;
			bool new_is_moving_forward_slowly = false;

			ProcessAutoReverse(*m_Vehicle, drive_dyn_data, raw_input_data, toggle_auto_reverse, new_is_moving_forward_slowly);

			m_IsMovingForwardSlowly = new_is_moving_forward_slowly;


			//If the car is to flip gear direction then switch gear as appropriate.
			if(toggle_auto_reverse)
			{
				m_InReverseMode = !m_InReverseMode;

				if(m_InReverseMode)
				{
					drive_dyn_data.forceGearChange(PxVehicleGearsData::eREVERSE);
				}
				else
				{
					drive_dyn_data.forceGearChange(PxVehicleGearsData::eFIRST);
				}
			}

			//If in reverse mode then swap the accel and brake.
			if(m_InReverseMode)
			{
				if(m_UseDigitalInputs)
				{
					//const bool accel=rawInputData.getDigitalAccel();
					//const bool brake=rawInputData.getDigitalBrake();
					//rawInputData.setDigitalAccel(brake);
					//rawInputData.setDigitalBrake(accel);
				}
				else
				{
					//const PxF32 accel=rawInputData.getAnalogAccel();
					//const PxF32 brake=rawInputData.getAnalogBrake();
					//rawInputData.setAnalogAccel(brake);
					//rawInputData.setAnalogBrake(accel);
				}
			}
		}


		// Now filter the raw input values and apply them to focus vehicle
		// as floats for brake,accel,handbrake,steer and bools for gearup,geardown.
		if(m_UseDigitalInputs)
		{
			PxVehicleDriveTankSmoothDigitalRawInputsAndSetAnalogInputs(tank_key_smoothing_data,raw_input_data, static_cast<float>(delta),*m_Vehicle);
		}
		else
		{
			PxVehicleDriveTankSmoothAnalogRawInputsAndSetAnalogInputs(tank_pad_smoothing_data,raw_input_data, static_cast<float>(delta),*m_Vehicle);
		}

		const PxF32 forward_speed = m_Vehicle->computeForwardSpeed();
		//const PxF32 forwardSpeedAbs = PxAbs(forwardSpeed);
		//const PxF32 sidewaysSpeedAbs = PxAbs(m_Vehicle->computeSidewaysSpeed());

		const PxU32 current_gear = drive_dyn_data.getCurrentGear();
		const PxU32 target_gear = drive_dyn_data.getTargetGear();

		GetSceneObject()->PostEvent(std::make_shared<PhysicsVelocityEvent>(Vec3(0,0,-forward_speed),Vec3(0,0,0),from_id));

		//pitch engine sound
		float pitch = 1.0;
		float volume = 0;

		//rps (rad/s)
		const float engine_rot_speed = fabs(drive_dyn_data.getEngineRotationSpeed());
		float norm_engine_rot_speed = engine_rot_speed/m_EngineMaxRotationSpeed;

		if(norm_engine_rot_speed > 1.0)
			norm_engine_rot_speed = 1.0;

		pitch += norm_engine_rot_speed;
		volume += norm_engine_rot_speed;
		volume = sqrt(volume)*1.05f;

		if (m_Sound)
		{
			m_Sound->SetPitch(pitch);
			m_Sound->SetPitch(volume);
		}

		GetSceneObject()->PostEvent(std::make_shared<VehicleEngineStatusMessage>(engine_rot_speed,forward_speed,current_gear));
		

		//std::cout << "current Gear:" << currentGear << " Target:" << targetGear << "\n";
		//std::cout << "Speed:" << forwardSpeed << " Sideways:" << sidewaysSpeedAbs << "\n";
		//std::cout << "Throttle:" << m_ThrottleInput << "\n";
		//std::cout << "Steer:" << m_SteerInput << "\n";

		//	MessagePtr physics_msg(new PhysicsVelocityEvent(GetVelocity(true),GetAngularVelocity(true),from_id));
		//	GetSceneObject()->PostMessage(physics_msg);

		bool col = CheckCollisions(current_pos,current_rot,forward_speed);

		if(m_Debug)
		{
			std::stringstream ss;
			ss  <<  GetSceneObject()->GetName();
			ss  <<  "\nGear::" << current_gear;
			ss  <<  "\nTarget:" << target_gear;
			ss  <<  "\nSpeed:" << forward_speed;
			ss  <<  "\nCollision:" << col;
			auto comp = GetSceneObject()->GetFirstComponentByClass<ITextComponent>();
			if (comp)
				comp->SetCaption(ss.str());
		}
	}


#define THRESHOLD_FORWARD_SPEED (0.1f)
#define THRESHOLD_SIDEWAYS_SPEED (0.2f)
#define THRESHOLD_ROLLING_BACKWARDS_SPEED (0.1f)


	void PhysXTankComponent::ProcessAutoReverse(const physx::PxVehicleWheels& focusVehicle,
		const physx::PxVehicleDriveDynData& driveDynData,
		const physx::PxVehicleDriveTankRawInputData& tankRawInputs,
		bool& toggleAutoReverse,
		bool& newIsMovingForwardSlowly) const
	{
		newIsMovingForwardSlowly = false;
		toggleAutoReverse = false;

		if(driveDynData.getUseAutoGears())
		{
			//If the car is traveling very slowly in forward gear without player input and the player subsequently presses the brake then we want the car to go into reverse gear
			//If the car is traveling very slowly in reverse gear without player input and the player subsequently presses the accel then we want the car to go into forward gear
			//If the car is in forward gear and is traveling backwards then we want to automatically put the car into reverse gear.
			//If the car is in reverse gear and is traveling forwards then we want to automatically put the car into forward gear.
			//(If the player brings the car to rest with the brake the player needs to release the brake then reapply it
			//to indicate they want to toggle between forward and reverse.)

			const bool prev_is_moving_forward_slowly=m_IsMovingForwardSlowly;
			bool is_moving_forward_slowly=false;
			bool is_moving_backwards=false;
			//const bool isInAir = false;//focusVehicle.isInAir();
			//if(!isInAir)
			{
				bool accel_left,accel_right,brake_left,brake_right;
				if(m_UseDigitalInputs)
				{

					accel_left = tankRawInputs.getDigitalLeftThrust();
					accel_right = tankRawInputs.getDigitalRightThrust(); 
					brake_left = tankRawInputs.getDigitalLeftBrake();  
					brake_right = tankRawInputs.getDigitalRightBrake();

					//accelRaw=carRawInputs.getDigitalAccel();
					//brakeRaw=carRawInputs.getDigitalRightBrake() || carRawInputs.getDigitalLeftBrake();
					//handbrakeRaw= false;
				}
				else
				{

					accel_left = tankRawInputs.getAnalogLeftThrust() > 0 ? true : false;
					accel_right = tankRawInputs.getAnalogRightThrust() > 0 ? true : false; 
					brake_left = tankRawInputs.getAnalogLeftBrake() > 0 ? true : false;  
					brake_right = tankRawInputs.getAnalogRightBrake() > 0 ? true : false;

					/*
					accelRaw=carRawInputs.getAnalogAccel() > 0 ? true : false;
					brakeRaw= false;//carRawInputs.getAnalogLeftBrake() > 0 && carRawInputs.getAnalogRightBrake() > 0 ? true : false;
		//			handbrakeRaw=carRawInputs.getAnalogHandbrake() > 0 ? true : false;
					handbrakeRaw= false;*/
				}

				const PxF32 forward_speed = focusVehicle.computeForwardSpeed();
				const PxF32 forward_speed_abs = PxAbs(forward_speed);
				const PxF32 sideways_speed_abs = PxAbs(focusVehicle.computeSidewaysSpeed());
				const PxU32 current_gear = driveDynData.getCurrentGear();
				const PxU32 target_gear = driveDynData.getTargetGear();

				//Check if the car is rolling against the gear (backwards in forward gear or forwards in reverse gear).
				if(PxVehicleGearsData::eFIRST == current_gear  && forward_speed < -THRESHOLD_ROLLING_BACKWARDS_SPEED)
				{
					is_moving_backwards = true;
				}
				else if(PxVehicleGearsData::eREVERSE == current_gear && forward_speed > THRESHOLD_ROLLING_BACKWARDS_SPEED)
				{
					is_moving_backwards = true;
				}

				//Check if the car is moving slowly.
				if(forward_speed_abs < THRESHOLD_FORWARD_SPEED && sideways_speed_abs < THRESHOLD_SIDEWAYS_SPEED)
				{
					is_moving_forward_slowly=true;
				}

				//Now work if we need to toggle from forwards gear to reverse gear or vice versa.
				if(is_moving_backwards)
				{
					if(!accel_left && !accel_right && !brake_left && !brake_right && (current_gear == target_gear))			
					{
						//The car is rolling against the gear and the player is doing nothing to stop this.
						toggleAutoReverse = true;
					}
				}
				else if(prev_is_moving_forward_slowly && is_moving_forward_slowly)
				{
					if((current_gear > PxVehicleGearsData::eNEUTRAL) && brake_left && brake_right && !accel_left && !accel_right && (current_gear == target_gear))
					{
						//The car was moving slowly in forward gear without player input and is now moving slowly with player input that indicates the
						//player wants to switch to reverse gear.
						toggleAutoReverse = true;
					}
					//else if(currentGear == PxVehicleGearsData::eREVERSE && accelLeft && accelRight && !brakeLeft && !brakeRight && (currentGear == targetGear))
					else if(current_gear == PxVehicleGearsData::eREVERSE && brake_left && brake_right && !accel_left && !accel_right && (current_gear == target_gear))
					{
						//The car was moving slowly in reverse gear without player input and is now moving slowly with player input that indicates the
						//player wants to switch to forward gear.
						toggleAutoReverse = true;
					}
				}

				//If the car was brought to rest through braking then the player needs to release the brake then reapply
				//to indicate that the gears should toggle between reverse and forward.
				/*if(isMovingForwardSlowly && (!brakeLeft || !brakeRight) && (!accelLeft || !accelRight))
				{
					newIsMovingForwardSlowly = true;
				}*/
				newIsMovingForwardSlowly = is_moving_forward_slowly;
			}
		}
	}

	void PhysXTankComponent::SetPosition(const Vec3 &value)
	{
		if(m_Actor)
		{
			Reset();
			const PhysXPhysicsSceneManagerPtr sm = m_SceneManager.lock();
			const Vec3 vehicle_pos = GetPosition();

			m_Actor->setGlobalPose(physx::PxTransform(sm->WorldToLocal(value), m_Actor->getGlobalPose().q));

			SceneObject::ComponentVector components;
			GetSceneObject()->GetComponentsByClass<IPhysicsBodyComponent>(components, true);

			for(size_t i = 0 ; i < components.size(); i++)
			{
				//PhysXBodyComponentPtr body = GASS_STATIC_PTR_CAST<PhysXBodyComponent>(components[i]);
				//const Vec3 offset_to_child = body->GetPosition() - vehicle_pos;
				//const Vec3 new_pos = offset_to_child + value;
				//body->SetPosition(new_pos);

				PhysXBodyComponentPtr body = GASS_STATIC_PTR_CAST<PhysXBodyComponent>(components[i]);
				LocationComponentPtr body_location = body->GetSceneObject()->GetFirstComponentByClass<ILocationComponent>();
				const Vec3 offset_to_vehicle = body->GetPosition() - vehicle_pos;
				body_location->SetPosition(offset_to_vehicle + value);
			}

			if(m_Vehicle)
			{
				PxShape* car_shapes[PX_MAX_NB_WHEELS+1];
				const PxU32 num_shapes= m_Vehicle->getRigidDynamicActor()->getNbShapes();
				m_Vehicle->getRigidDynamicActor()->getShapes(car_shapes,num_shapes);
				const PxRigidDynamic& vehicle_actor = *m_Vehicle->getRigidDynamicActor();

				if(m_AllWheels.size() == num_shapes-1)
				{
					for(size_t i = 0; i < num_shapes-1; i++)
					{
						SceneObjectPtr wheel(m_AllWheels[i]);
						if(wheel)
						{
							Vec3 pos = sm->LocalToWorld(PxShapeExt::getGlobalPose(*car_shapes[i],vehicle_actor).p);
							wheel->GetFirstComponentByClass<ILocationComponent>()->SetPosition(pos);
							Quaternion rot = PxConvert::ToGASS(PxShapeExt::getGlobalPose(*car_shapes[i],vehicle_actor).q);
							wheel->GetFirstComponentByClass<ILocationComponent>()->SetRotation(rot);
						}
					}
				}
				else
				{
					GASS_EXCEPT(GASS::Exception::ERR_RT_ASSERTION_FAILED,"Physics shapes dont match visual geomtries","PhysXTankComponent::SceneManagerTick")
				}
			}
		}
	}

	Vec3  PhysXTankComponent::GetPosition() const
	{
		Vec3 pos(0, 0, 0);
		if (m_Actor)
		{
			PhysXPhysicsSceneManagerPtr sm = m_SceneManager.lock();
			pos = sm->LocalToWorld(m_Actor->getGlobalPose().p);
		}
		return pos;
	}

	void PhysXTankComponent::SetRotation(const Quaternion &rot)
	{
		if(m_Actor)
		{
			Reset();
			m_Actor->setGlobalPose(physx::PxTransform(m_Actor->getGlobalPose().p,PxConvert::ToPx(rot)));
		}
	}

	Quaternion PhysXTankComponent::GetRotation() const
	{
		Quaternion q;

		if(m_Actor)
		{
			q = PxConvert::ToGASS(m_Actor->getGlobalPose().q);
		}
		return q;
	}

	void PhysXTankComponent::OnInput(InputRelayEventPtr message)
	{
		std::string name = message->GetController();
		float value = message->GetValue();

		if (name == "Throttle")
		{
			if(value > 0)
			{
				m_DigAccelInput = true;
				m_DigBrakeInput = false;
			}
			else if(value < 0)
			{
				m_DigAccelInput = false;
				m_DigBrakeInput = true;
			}
			else
			{
				m_DigAccelInput = false;
				m_DigBrakeInput = false;
			}
			m_ThrottleInput = value;
		}
		else if (name == "Steer")
		{
			m_SteerInput = value;
		}
		else if (name == "Break")
		{
			m_BreakInput = value;
		}
		
	}

	bool PhysXTankComponent::CheckCollisions(const Vec3 &/*pos*/, const Quaternion &/*rot*/, Float /*speed*/) const
	{
		/*PhysXPhysicsSceneManagerPtr scene_manager = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<PhysXPhysicsSceneManager>();

		Vec3 mesh_offset = (m_MeshBounds.Min +  m_MeshBounds.Max)*0.5;
		Vec3 mesh_size = m_MeshBounds.GetSize();

		//do ray at chassis center and each bounding box corner
		Vec3 dir = -rot.GetZAxis();


		const Float z_nudge_offset = 1.4;
		Vec3 ray_start = pos + dir*(-mesh_offset.z + (mesh_size.z + z_nudge_offset)*0.5);
		ray_start = ray_start + rot.GetYAxis()*mesh_offset.y;

		const Vec3 x_offset = rot.GetXAxis()*mesh_size.x*0.5;
		const Vec3 y_offset = rot.GetYAxis()*(mesh_size.y*0.4);


		std::vector<Vec3> start_point_vec;
		start_point_vec.push_back(ray_start);

		start_point_vec.push_back(ray_start + y_offset);
		start_point_vec.push_back(ray_start - y_offset);

		start_point_vec.push_back(ray_start - x_offset + y_offset);
		start_point_vec.push_back(ray_start + x_offset + y_offset);

		start_point_vec.push_back(ray_start - x_offset - y_offset);
		start_point_vec.push_back(ray_start + x_offset - y_offset);

		start_point_vec.push_back(ray_start - x_offset + y_offset);
		start_point_vec.push_back(ray_start + x_offset + y_offset);

		start_point_vec.push_back(ray_start - x_offset - y_offset);
		start_point_vec.push_back(ray_start + x_offset - y_offset);

		std::vector<Vec3> dir_vec;

		dir_vec.push_back(dir);
		dir_vec.push_back(dir);
		dir_vec.push_back(dir);

		dir_vec.push_back(dir);
		dir_vec.push_back(dir);
		dir_vec.push_back(dir);
		dir_vec.push_back(dir);


		dir_vec.push_back(dir - rot.GetXAxis()*0.2);
		dir_vec.push_back(dir + rot.GetXAxis()*0.2);

		dir_vec.push_back(dir - rot.GetXAxis()*0.2);
		dir_vec.push_back(dir + rot.GetXAxis()*0.2);


		const Float check_range = 30;

		CollisionResult final_res;
		final_res.Coll = false;
		final_res.CollDist = FLT_MAX;

		for(size_t i =0; i < start_point_vec.size(); i++)
		{
		CollisionResult temp_res;
		scene_manager->Raycast(start_point_vec[i],dir_vec[i]*check_range, GEOMETRY_FLAG_SCENE_OBJECTS, temp_res, false);
		if(temp_res.Coll && temp_res.CollDist < final_res.CollDist)
		{
		final_res = temp_res;
		}

		//debug rays?
		if(m_Debug)
		{
		const ColorRGBA color(1,1,1,1);
		Vec3 end_pos = start_point_vec[i] + dir_vec[i]*check_range;
		if(temp_res.Coll)
		end_pos = temp_res.CollPosition;
		GetSceneObject()->GetScene()->PostMessage(SceneMessagePtr(new DrawLineRequest(start_point_vec[i], end_pos, color,color)));
		}
		}

		SceneObjectPtr col_obj = final_res.CollSceneObject.lock();
		if(final_res.Coll && col_obj)
		{
		Vec3 center_pos = col_obj->GetFirstComponentByClass<ILocationComponent>()->GetPosition();
		GetSceneObject()->PostEvent(SceneObjectEventMessagePtr(new VehicleRadarEvent(true, center_pos, final_res.CollDist, col_obj)));
		return true;
		}
		else
		{
		GetSceneObject()->PostEvent(SceneObjectEventMessagePtr(new VehicleRadarEvent(false,Vec3(0,0,0),0,SceneObjectPtr())));
		}*/

		//hack to get all vehicles

		/*const Float check_range = 30;
		bool col = false;
		Float col_dist = FLT_MAX;
		SceneObjectPtr col_obj;
		Vec3 col_point(0,0,0);
		Vec3 col_velocity(0,0,0);
		Vec3 col_size(0,0,0);



		SceneObjectTemplate::ComponentVector components;
		GetSceneObject()->GetParentSceneObject()->GetParentSceneObject()->GetParentSceneObject()->GetComponentsByClass<PhysXTankComponent>(components);

		for(int i = 0;  i < components.size(); i++)
		{
		PhysXTankComponentPtr comp = GASS_DYNAMIC_PTR_CAST<PhysXTankComponent>(components[i]);
		if(comp.get() != this)
		{

		Vec3 obj_pos =  comp->GetSceneObject()->GetFirstComponentByClass<ILocationComponent>()->GetPosition();
		Float dist = (obj_pos - pos).Length();
		if(dist < check_range &&
		dist < col_dist)
		{
		col = true;
		col_dist = dist;
		col_obj = comp->GetSceneObject();
		col_point = obj_pos;
		}
		}
		}

		GetSceneObject()->PostEvent(SceneObjectEventMessagePtr(new VehicleRadarEvent(col, col_point, col_dist, col_obj)));*/
		return false;
	}
}
