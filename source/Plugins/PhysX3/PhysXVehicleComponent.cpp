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

#include "Plugins/PhysX3/PhysXVehicleComponent.h"
#include "Plugins/PhysX3/PhysXWheelComponent.h"
#include "Plugins/PhysX3/PhysXPhysicsSceneManager.h"
#include "Plugins/PhysX3/PhysXPhysicsSystem.h"
#include "Plugins/PhysX3/PhysXVehicleSceneQuery.h"
#include "Plugins/PhysX3/PhysXBodyComponent.h"
#include "Sim/Messages/GASSSoundSceneObjectMessages.h"
#include "Sim/Messages/GASSPlatformMessages.h"

using namespace physx;
namespace GASS
{
	PhysXVehicleComponent::PhysXVehicleComponent(): m_Actor(NULL),
		m_ThrottleInput(0),
		m_SteerInput(0),
		m_Vehicle(NULL),
		m_DigAccelInput(false),
		m_DigBrakeInput(false),
		m_IsMovingForwardSlowly(false),
		m_InReverseMode(false),
		m_UseDigitalInputs(false),
		m_UseAutoReverse(false),
		m_ScaleMass(1.0),
		m_Mass(1500),
		m_EnginePeakTorque(500),
		m_EngineMaxRotationSpeed(200),
		m_ClutchStrength(10),
		m_GearSwitchTime(0.5),
		m_ChassisDim(0,0,0),
		m_MaxSpeed(20),
		m_Debug(false),
		m_BreakInput(0),
		m_TrackTransformation(true)
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

	PhysXVehicleComponent::~PhysXVehicleComponent()
	{
		
	}

	void PhysXVehicleComponent::OnDelete()
	{
		if(m_Vehicle)
		{
			PhysXPhysicsSceneManagerPtr scene_manager = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<PhysXPhysicsSceneManager>();
			scene_manager->UnregisterVehicle(m_Vehicle);
			m_Vehicle->free();
			m_Vehicle = NULL;
			if(m_Actor)
			{
				scene_manager->GetPxScene()->removeActor(*m_Actor);
				m_Actor->release();
				m_Actor = NULL;
			}
		}
	}
	
	void PhysXVehicleComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("PhysXVehicleComponent",new Creator<PhysXVehicleComponent, Component>);
		GetClassRTTI()->SetMetaData(ClassMetaDataPtr(new ClassMetaData("PhysXVehicleComponent", OF_VISIBLE)));

		REG_PROPERTY(float,Mass, PhysXVehicleComponent)
		REG_PROPERTY(float,ScaleMass, PhysXVehicleComponent)
		REG_PROPERTY(float,EnginePeakTorque, PhysXVehicleComponent)
		REG_PROPERTY(float,EngineMaxRotationSpeed,PhysXVehicleComponent)
		REG_PROPERTY(float,ClutchStrength, PhysXVehicleComponent)
		REG_PROPERTY(std::vector<float> ,GearRatios,PhysXVehicleComponent)
		REG_PROPERTY(bool,UseAutoReverse, PhysXVehicleComponent)
		REG_PROPERTY(float,GearSwitchTime, PhysXVehicleComponent)
		REG_PROPERTY(SceneObjectRef,FrontLeftWheel, PhysXVehicleComponent)
		REG_PROPERTY(SceneObjectRef,FrontRightWheel, PhysXVehicleComponent)
		REG_PROPERTY(SceneObjectRef,RearLeftWheel, PhysXVehicleComponent)
		REG_PROPERTY(SceneObjectRef,RearRightWheel, PhysXVehicleComponent)
		REG_PROPERTY(SceneObjectRef,RearRightWheel, PhysXVehicleComponent)
		REG_PROPERTY(std::vector<SceneObjectRef>,ExtraWheels,PhysXVehicleComponent)
		REG_PROPERTY(Float,MaxSpeed, PhysXVehicleComponent)
		REG_PROPERTY2(bool,Debug, PhysXVehicleComponent, BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
	}

	PxVec3 PhysXVehicleComponent::ComputeDim(const PxConvexMesh* cm)
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

	void PhysXVehicleComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXVehicleComponent::OnLocationLoaded,LocationLoadedEvent,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXVehicleComponent::OnTransformationChanged, TransformationChangedEvent,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXVehicleComponent::OnInput,InputRelayEvent,0));
		GetSceneObject()->GetScene()->RegisterForMessage(REG_TMESS(PhysXVehicleComponent::OnPostSceneObjectInitializedEvent,PostSceneObjectInitializedEvent,0));
		PhysXPhysicsSceneManagerPtr scene_manager = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<PhysXPhysicsSceneManager>();
		scene_manager->Register(shared_from_this());

		//Play engine sound
		SoundParameterRequestPtr sound_msg(new SoundParameterRequest(SoundParameterRequest::PLAY,0));
		GetSceneObject()->PostRequest(sound_msg);
	}

	Vec3 PhysXVehicleComponent::GetSize() const
	{
		return m_ChassisDim;
	}

	void PhysXVehicleComponent::Reset()
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
		m_BreakInput = 0;
		m_InReverseMode = false;

		for(size_t i = 0; i < m_Vehicle->mWheelsSimData.getNbWheels(); i++)
		{
			m_Vehicle->mWheelsSimData.setWheelShapeMapping(static_cast<physx::PxI32>(i),static_cast<physx::PxU32>(i));
		}
	}

	void PhysXVehicleComponent::OnPostSceneObjectInitializedEvent(PostSceneObjectInitializedEventPtr message)
	{
		if(message->GetSceneObject() != GetSceneObject())
			return;
		physx::PxVehicleChassisData chassisData;

		chassisData.mMass = m_Mass*m_ScaleMass;

		//Get chassis mesh
		std::string col_mesh_id = GetSceneObject()->GetName();
		ResourceComponentPtr res = GetSceneObject()->GetFirstComponentByClass<IResourceComponent>();
		if(res)
		{
			col_mesh_id = res->GetResource().Name();
		}

		MeshComponentPtr mesh = GetSceneObject()->GetFirstComponentByClass<IMeshComponent>();
		GASSAssert(mesh,"PhysXVehicleComponent::OnInitialize");
		PhysXPhysicsSceneManagerPtr scene_manager = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<PhysXPhysicsSceneManager>();
		GASSAssert(scene_manager,"PhysXVehicleComponent::OnInitialize");
		PhysXConvexMesh chassisMesh = scene_manager->CreateConvexMesh(col_mesh_id,mesh);

		GeometryComponentPtr geom = GetSceneObject()->GetFirstComponentByClass<IGeometryComponent>();
		GASSAssert(geom,"PhysXVehicleComponent::OnInitialize");
		m_MeshBounds = geom->GetBoundingBox();

		std::vector<PxConvexMesh*> wheelConvexMeshes;
		std::vector<PxVec3 > wheelCentreOffsets;
		//Add wheels
		std::vector<SceneObjectPtr> wheel_objects;

		wheel_objects.push_back(m_FrontLeftWheel.GetRefObject());
		wheel_objects.push_back(m_FrontRightWheel.GetRefObject());

		wheel_objects.push_back(m_RearLeftWheel.GetRefObject());
		wheel_objects.push_back(m_RearRightWheel.GetRefObject());



		for(size_t i= 0; i < m_ExtraWheels.size(); i++)
		{
			wheel_objects.push_back(m_ExtraWheels[i].GetRefObject());
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
			ResourceComponentPtr child_res  = child->GetFirstComponentByClass<IResourceComponent>();
			if(child_res)
			{
				child_col_mesh_id = child_res->GetResource().Name();
			}
			MeshComponentPtr child_mesh = child->GetFirstComponentByClass<IMeshComponent>();
			LocationComponentPtr location = child->GetFirstComponentByClass<ILocationComponent>();
			PhysXConvexMesh wheelMesh = scene_manager->CreateConvexMesh(child_col_mesh_id, child_mesh);
			wheelConvexMeshes.push_back(wheelMesh.m_ConvexMesh);
			Vec3 pos = location->GetPosition();
			wheelCentreOffsets.push_back(PxConvert::ToPx(pos));

			PhysXWheelComponentPtr wheel_comp = child->GetFirstComponentByClass<PhysXWheelComponent>();
			wheels[i] = wheel_comp->GetWheelData();
			susps[i]  = wheel_comp->GetSuspensionData();
			tires[i]  = wheel_comp->GetTireData();
		}

		PxVehicleWheelsSimData* wheelsSimData = PxVehicleWheelsSimData::allocate(static_cast<physx::PxU32>(num_wheels));
		PxVehicleDriveSimData4W driveSimData;

		//Extract the chassis AABB dimensions from the chassis convex mesh.
		const PxVec3 chassisDims=ComputeDim(chassisMesh.m_ConvexMesh);

		m_ChassisDim = PxConvert::ToGASS(chassisDims);


		//The origin is at the center of the chassis mesh.
		//Set the center of mass to be below this point and a little towards the front.
		const PxVec3 chassisCMOffset=PxVec3(0.0f,-chassisDims.y*0.5f+0.65f,0.125f);

		//Now compute the chassis mass and moment of inertia.
		//Use the moment of inertia of a cuboid as an approximate value for the chassis moi.
		PxVec3 chassisMOI
			((chassisDims.y*chassisDims.y + chassisDims.z*chassisDims.z)*chassisData.mMass/12.0f,
			(chassisDims.x*chassisDims.x + chassisDims.z*chassisDims.z)*chassisData.mMass/12.0f,
			(chassisDims.x*chassisDims.x + chassisDims.y*chassisDims.y)*chassisData.mMass/12.0f);
		//A bit of tweaking here.  The car will have more responsive turning if we reduce the
		//y-component of the chassis moment of inertia.
		chassisMOI.y*=0.8f;

		//Let's set up the chassis data structure now.
		chassisData.mMOI=chassisMOI;
		chassisData.mCMOffset=chassisCMOffset;

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
		PxF32 massRear=0.5f * chassisData.mMass * (chassisDims.z-3*chassisCMOffset.z)/chassisDims.z;
		const PxF32 massFront=chassisData.mMass - massRear;
		massRear = massRear/PxF32 (wheels.size()-2);
		//float tot = massRear*4 + massFront;


		//set mSprungMass from vehicle mass move this to wheel?
		susps[PxVehicleDrive4WWheelOrder::eFRONT_LEFT].mSprungMass=massFront*0.5f;
		susps[PxVehicleDrive4WWheelOrder::eFRONT_RIGHT].mSprungMass=massFront*0.5f;
		susps[PxVehicleDrive4WWheelOrder::eREAR_LEFT].mSprungMass=massRear;
		susps[PxVehicleDrive4WWheelOrder::eREAR_RIGHT].mSprungMass=massRear;

		//Set mass of extra wheels
		for(size_t i = 4; i < wheels.size() ; i++)
		{
			susps[i].mSprungMass=massRear;
			//wheels[i].mMaxSteer=0.0f;
			//wheels[i].mMaxHandBrakeTorque=4000.0f*m_ScaleMass;
			//wheels[i].mMaxBrakeTorque = 1500.0f*m_ScaleMass;
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
		std::vector<PxVec3> suGASS_SHARED_PTRavelDirections;
		std::vector<PxVec3> wheelCentreCMOffsets;
		std::vector<PxVec3> suspForceAppCMOffsets;
		std::vector<PxVec3> tireForceAppCMOffsets;
		suGASS_SHARED_PTRavelDirections.resize(num_wheels);
		wheelCentreCMOffsets.resize(num_wheels);
		suspForceAppCMOffsets.resize(num_wheels);
		tireForceAppCMOffsets.resize(num_wheels);

		for(PxU32 i=0;i<num_wheels;i++)
		{
			suGASS_SHARED_PTRavelDirections[i]= PxVec3(0,-1,0);
			wheelCentreCMOffsets[i]= wheelCentreOffsets[i]-chassisCMOffset;
			suspForceAppCMOffsets[i]=PxVec3(wheelCentreCMOffsets[i].x,-0.3f,wheelCentreCMOffsets[i].z);
			tireForceAppCMOffsets[i]=PxVec3(wheelCentreCMOffsets[i].x,-0.3f,wheelCentreCMOffsets[i].z);
		}

		//Now add the wheel, tire and suspension data.
		for(PxU32 i=0;i<num_wheels;i++)
		{
			wheelsSimData->setWheelData(i,wheels[i]);
			wheelsSimData->setTireData(i,tires[i]);
			wheelsSimData->setSuspensionData(i,susps[i]);
			wheelsSimData->setSuspTravelDirection(i,suGASS_SHARED_PTRavelDirections[i]);
			wheelsSimData->setWheelCentreOffset(i,wheelCentreCMOffsets[i]);
			wheelsSimData->setSuspForceAppPointOffset(i,suspForceAppCMOffsets[i]);
			wheelsSimData->setTireForceAppPointOffset(i,tireForceAppCMOffsets[i]);
		}

		//Now set up the differential, engine, gears, clutch, and ackermann steering.

		//Diff
		PxVehicleDifferential4WData diff;
		diff.mType=PxVehicleDifferential4WData::eDIFF_TYPE_LS_4WD;
		driveSimData.setDiffData(diff);

		//Engine
		PxVehicleEngineData engine;

		engine.mPeakTorque=m_EnginePeakTorque;
		engine.mMaxOmega=m_EngineMaxRotationSpeed;//approx 6000 rpm
		/*engine.mDampingRateFullThrottle *= m_ScaleMass;
		engine.mDampingRateZeroThrottleClutchDisengaged *= m_ScaleMass;
		engine.mDampingRateZeroThrottleClutchEngaged *= m_ScaleMass;
		*/
		driveSimData.setEngineData(engine);

		//Gears
		PxVehicleGearsData gears;

		if(m_GearRatios.size() >= PxVehicleGearsData::eGEARSRATIO_COUNT)
		{
			GASS_EXCEPT(Exception::ERR_INVALIDPARAMS, "To many gear in vehicle","PhysXVehicleComponent::OnPostSceneObjectInitializedEvent");
		}
		gears.mNbRatios = static_cast<physx::PxU32>(m_GearRatios.size());
		for(size_t i = 0 ; i< m_GearRatios.size(); i++)
		{
			gears.mRatios[i] = m_GearRatios[i];
		}
		gears.mSwitchTime = m_GearSwitchTime;
		driveSimData.setGearsData(gears);

		//Clutch
		PxVehicleClutchData clutch;
		clutch.mStrength = m_ClutchStrength*m_ScaleMass;
		driveSimData.setClutchData(clutch);

		//Ackermann steer accuracy
		PxVehicleAckermannGeometryData ackermann;
		ackermann.mAccuracy=1.0f;
		ackermann.mAxleSeparation= fabs(wheelCentreOffsets[PxVehicleDrive4WWheelOrder::eFRONT_LEFT].z - wheelCentreOffsets[PxVehicleDrive4WWheelOrder::eREAR_LEFT].z);
		ackermann.mFrontWidth=fabs(wheelCentreOffsets[PxVehicleDrive4WWheelOrder::eFRONT_RIGHT].x-wheelCentreOffsets[PxVehicleDrive4WWheelOrder::eFRONT_LEFT].x);
		ackermann.mRearWidth=fabs(wheelCentreOffsets[PxVehicleDrive4WWheelOrder::eREAR_RIGHT].x-wheelCentreOffsets[PxVehicleDrive4WWheelOrder::eREAR_LEFT].x);
		driveSimData.setAckermannGeometryData(ackermann);

		//////////////
		//PxRigidDynamic* vehActor=createVehicleActor4W(chassisData,&wheelConvexMeshes4[0],chassisMesh.m_ConvexMesh,*scene_manager->GetPxScene(),*system->GetPxSDK(),*system->GetDefaultMaterial());
		//m_Actor = vehActor;

		PhysXPhysicsSystemPtr system = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<PhysXPhysicsSystem>();

		//We need a rigid body actor for the vehicle.
		//Don't forget to add the actor the scene after setting up the associated vehicle.
		m_Actor = system->GetPxSDK()->createRigidDynamic(PxTransform::createIdentity());


		const PxMaterial& wheelMaterial = *system->GetDefaultMaterial();
		PxFilterData wheelCollFilterData;
		wheelCollFilterData.word0=GEOMETRY_FLAG_RAY_CAST_WHEEL;
		GeometryFlags against = GeometryFlagManager::GetMask(GEOMETRY_FLAG_RAY_CAST_WHEEL);
		wheelCollFilterData.word1=against;

		//Create a query filter data for the car to ensure that cars
		//do not attempt to drive on themselves.
		PxFilterData vehQryFilterData;
		VehicleSetupVehicleShapeQueryFilterData(&vehQryFilterData);

		//Add all the wheel shapes to the actor.
		for(PxU32 i=0;i < num_wheels;i++)
		{
			PxConvexMeshGeometry wheelGeom(wheelConvexMeshes[i]);
			PxShape* wheelShape = m_Actor->createShape(wheelGeom,wheelMaterial);
			wheelShape->setQueryFilterData(vehQryFilterData);
			wheelShape->setSimulationFilterData(wheelCollFilterData);
			wheelShape->setLocalPose(PxTransform::createIdentity());
		}

		PxConvexMeshGeometry chassisConvexGeom(chassisMesh.m_ConvexMesh);
		const PxMaterial& chassisMaterial= *system->GetDefaultMaterial();
		PxShape* chassisShape = m_Actor->createShape(chassisConvexGeom,chassisMaterial);
		chassisShape->setQueryFilterData(vehQryFilterData);
		chassisShape->userData = this;

		PxFilterData chassisCollFilterData;
		chassisCollFilterData.word0=GEOMETRY_FLAG_VEHICLE_CHASSIS;
		against = GeometryFlagManager::GetMask(GEOMETRY_FLAG_VEHICLE_CHASSIS);
		chassisCollFilterData.word1=against;
		chassisShape->setSimulationFilterData(chassisCollFilterData);

		chassisShape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE,true);
		chassisShape->setLocalPose(PxTransform::createIdentity());

		m_Actor->setMass(chassisData.mMass);
		m_Actor->setMassSpaceInertiaTensor(chassisData.mMOI);
		m_Actor->setCMassLocalPose(PxTransform(chassisData.mCMOffset,PxQuat::createIdentity()));

		m_Vehicle = PxVehicleDrive4W::allocate(static_cast<physx::PxU32>(num_wheels));
		m_Vehicle->setup(system->GetPxSDK(),m_Actor,*wheelsSimData,driveSimData,4-static_cast<physx::PxU32>(num_wheels));

		for(size_t i = 0; i < num_wheels; i++)
		{
			m_Vehicle->mWheelsSimData.setWheelShapeMapping(static_cast<physx::PxI32>(i),static_cast<physx::PxU32>(i));
		}

		//Free the sim data because we don't need that any more.
		wheelsSimData->free();

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

		GetSceneObject()->SendImmediateEvent(PhysicsBodyLoadedEventPtr(new PhysicsBodyLoadedEvent()));
		m_Initialized = true;
	}

	void PhysXVehicleComponent::OnTransformationChanged(TransformationChangedEventPtr event)
	{
		if (m_TrackTransformation)
		{
			//First rotate to get correct wheel rotation in SetPosition
			SetRotation(event->GetRotation());
			SetPosition(event->GetPosition());
		}
	}

	void PhysXVehicleComponent::OnLocationLoaded(LocationLoadedEventPtr message)
	{
		PhysXPhysicsSceneManagerPtr sm = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<PhysXPhysicsSceneManager>();
		m_SceneManager = sm;
	}


	PxVehicleKeySmoothingData VehicleKeySmoothingData=
	{
		{
			3.0f,	//rise rate eANALOG_INPUT_ACCEL
				3.0f,	//rise rate eANALOG_INPUT_BRAKE
				10.0f,	//rise rate eANALOG_INPUT_HANDBRAKE
				2.5f,	//rise rate eANALOG_INPUT_STEER_LEFT
				2.5f,	//rise rate eANALOG_INPUT_STEER_RIGHT
		},
		{
			5.0f,	//fall rate eANALOG_INPUT__ACCEL
				5.0f,	//fall rate eANALOG_INPUT__BRAKE
				10.0f,	//fall rate eANALOG_INPUT__HANDBRAKE
				5.0f,	//fall rate eANALOG_INPUT_STEER_LEFT
				5.0f	//fall rate eANALOG_INPUT_STEER_RIGHT
			}
	};

	PxVehiclePadSmoothingData VehiclePadSmoothingData=
	{
		{
			6.0f,	//rise rate eANALOG_INPUT_ACCEL
				6.0f,	//rise rate eANALOG_INPUT_BRAKE
				12.0f,	//rise rate eANALOG_INPUT_HANDBRAKE
				2.5f,	//rise rate eANALOG_INPUT_STEER_LEFT
				2.5f,	//rise rate eANALOG_INPUT_STEER_RIGHT
		},
		{
			10.0f,	//fall rate eANALOG_INPUT_ACCEL
				10.0f,	//fall rate eANALOG_INPUT_BRAKE
				12.0f,	//fall rate eANALOG_INPUT_HANDBRAKE
				5.0f,	//fall rate eANALOG_INPUT_STEER_LEFT
				5.0f	//fall rate eANALOG_INPUT_STEER_RIGHT
			}
	};

	PxF32 VehicleSteerVsForwardSpeedData[2*8]=
	{
		0.0f,		0.75f,
		5.0f,		0.75f,
		30.0f,		0.125f,
		120.0f,		0.1f,
		PX_MAX_F32, PX_MAX_F32,
		PX_MAX_F32, PX_MAX_F32,
		PX_MAX_F32, PX_MAX_F32,
		PX_MAX_F32, PX_MAX_F32
	};
	PxFixedSizeLookupTable<8> VehicleSteerVsForwardSpeedTable(VehicleSteerVsForwardSpeedData,4);

	void PhysXVehicleComponent::SceneManagerTick(double delta)
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

		PxShape* carShapes[PX_MAX_NB_WHEELS+1];
		const PxU32 numShapes=m_Vehicle->getRigidDynamicActor()->getNbShapes();
		m_Vehicle->getRigidDynamicActor()->getShapes(carShapes,numShapes);
		const PxRigidDynamic& vehicleActor = *m_Vehicle->getRigidDynamicActor();

		if(m_AllWheels.size() == numShapes-1)
		{
			const PhysXPhysicsSceneManagerPtr sm = m_SceneManager.lock();

			for(size_t i = 0; i < numShapes-1; i++)
			{
				SceneObjectPtr wheel(m_AllWheels[i]);
				if(wheel)
				{
					Vec3 pos = sm->LocalToWorld(PxShapeExt::getGlobalPose(*carShapes[i],vehicleActor).p);
					wheel->GetFirstComponentByClass<ILocationComponent>()->SetWorldPosition(pos);
					Quaternion rot = PxConvert::ToGASS(PxShapeExt::getGlobalPose(*carShapes[i],vehicleActor).q);
					wheel->GetFirstComponentByClass<ILocationComponent>()->SetWorldRotation(rot);
				}
			}
		}
		else
		{
			GASS_EXCEPT(GASS::Exception::ERR_RT_ASSERTION_FAILED,"Physics shapes dont match visual geomtries","PhysXVehicleComponent::SceneManagerTick")
		}

		PxVehicleDrive4WRawInputData rawInputData;

		if(m_UseDigitalInputs)
		{
			rawInputData.setDigitalAccel(m_DigAccelInput);
			rawInputData.setDigitalBrake(m_DigBrakeInput);
			rawInputData.setDigitalHandbrake(0);
			if(m_SteerInput > 0)
			{
				rawInputData.setDigitalSteerLeft(true);
				rawInputData.setDigitalSteerRight(0);
			}
			else
			{
				rawInputData.setDigitalSteerLeft(0);
				rawInputData.setDigitalSteerRight(true);
			}
		}
		else
		{
			if (fabs(m_ThrottleInput) > 0)
			{
				if (m_ThrottleInput < 0)
				{
					rawInputData.setAnalogBrake(fabs(m_ThrottleInput) + m_BreakInput);
					rawInputData.setAnalogAccel(0);
				}
				else
				{
					rawInputData.setAnalogAccel(fabs(m_ThrottleInput));
					rawInputData.setAnalogBrake(m_BreakInput);
				}
			}
			else
			{
				rawInputData.setAnalogAccel(0);
				rawInputData.setAnalogBrake(m_BreakInput);
			}

			
			rawInputData.setAnalogHandbrake(m_BreakInput);
			rawInputData.setAnalogSteer(m_SteerInput);
		}
		rawInputData.setGearDown(false);
		rawInputData.setGearUp(false);

		PxVehicleDriveDynData& driveDynData=m_Vehicle->mDriveDynData;

		//Work out if the car is to flip from reverse to forward gear or from forward gear to reverse.
		//Store if the car is moving slowly to help decide if the car is to toggle from reverse to forward in the next update.

		if(m_UseAutoReverse)
		{
			bool toggleAutoReverse = false;
			bool newIsMovingForwardSlowly = false;

			ProcessAutoReverse(*m_Vehicle, driveDynData, rawInputData, toggleAutoReverse, newIsMovingForwardSlowly);

			m_IsMovingForwardSlowly = newIsMovingForwardSlowly;


			//If the car is to flip gear direction then switch gear as appropriate.
			if(toggleAutoReverse)
			{
				m_InReverseMode = !m_InReverseMode;

				if(m_InReverseMode)
				{
					driveDynData.forceGearChange(PxVehicleGearsData::eREVERSE);
				}
				else
				{
					driveDynData.forceGearChange(PxVehicleGearsData::eFIRST);
				}
			}

			//If in reverse mode then swap the accel and brake.
			if(m_InReverseMode)
			{
				if(m_UseDigitalInputs)
				{
					const bool accel=rawInputData.getDigitalAccel();
					const bool brake=rawInputData.getDigitalBrake();
					rawInputData.setDigitalAccel(brake);
					rawInputData.setDigitalBrake(accel);
				}
				else
				{
					const PxF32 accel=rawInputData.getAnalogAccel();
					const PxF32 brake=rawInputData.getAnalogBrake();
					rawInputData.setAnalogAccel(brake);
					rawInputData.setAnalogBrake(accel);
				}
			}
		}

		bool isVehicleInAir  = false;
		// Now filter the raw input values and apply them to focus vehicle
		// as floats for brake,accel,handbrake,steer and bools for gearup,geardown.
		if(m_UseDigitalInputs)
		{

			PxVehicleDrive4WSmoothDigitalRawInputsAndSetAnalogInputs(VehicleKeySmoothingData, VehicleSteerVsForwardSpeedTable, rawInputData, static_cast<float>(delta), isVehicleInAir, *m_Vehicle);
		}
		else
		{
			PxVehicleDrive4WSmoothAnalogRawInputsAndSetAnalogInputs(VehiclePadSmoothingData, VehicleSteerVsForwardSpeedTable, rawInputData, static_cast<float>(delta), isVehicleInAir, *m_Vehicle);
		}

		const PxF32 forwardSpeed = m_Vehicle->computeForwardSpeed();
		//const PxF32 forwardSpeedAbs = PxAbs(forwardSpeed);
		//const PxF32 sidewaysSpeedAbs = PxAbs(m_Vehicle->computeSidewaysSpeed());

		const PxU32 currentGear = driveDynData.getCurrentGear();
		const PxU32 targetGear = driveDynData.getTargetGear();

		GetSceneObject()->PostEvent(PhysicsVelocityEventPtr(new PhysicsVelocityEvent(Vec3(0,0,-forwardSpeed),Vec3(0,0,0),from_id)));

		//pitch engine sound
		float pitch = 1.0;
		float volume = 0;

		//rps (rad/s)
		const float engine_rot_speed = fabs(driveDynData.getEngineRotationSpeed());
		float norm_engine_rot_speed = engine_rot_speed/m_EngineMaxRotationSpeed;

		if(norm_engine_rot_speed > 1.0)
			norm_engine_rot_speed = 1.0;

		pitch += norm_engine_rot_speed;
		volume += norm_engine_rot_speed;
		volume = sqrt(volume)*1.05f;

		SoundParameterRequestPtr pitch_msg(new SoundParameterRequest(SoundParameterRequest::PITCH,pitch));
		GetSceneObject()->PostRequest(pitch_msg);

		SoundParameterRequestPtr volume_msg(new SoundParameterRequest(SoundParameterRequest::VOLUME,volume));
		GetSceneObject()->PostRequest(volume_msg);

		GetSceneObject()->PostEvent(VehicleEngineStatusMessagePtr(new VehicleEngineStatusMessage(engine_rot_speed,forwardSpeed,currentGear)));

		if(m_Debug)
		{
			std::stringstream ss;
			ss  <<  GetSceneObject()->GetName();
			ss  <<  "\nGear::" << currentGear;
			ss  <<  "\nTarget:" << targetGear;
			ss  <<  "\nSpeed:" << forwardSpeed;
			GetSceneObject()->PostRequest(TextCaptionRequestPtr(new TextCaptionRequest(ss.str())));
		}

		GetSceneObject()->PostEvent(VehicleEngineStatusMessagePtr(new VehicleEngineStatusMessage(engine_rot_speed,forwardSpeed,currentGear)));

		//std::cout << "current Gear:" << currentGear << " Target:" << targetGear << "\n";
		//std::cout << "Speed:" << forwardSpeed << " Sideways:" << sidewaysSpeedAbs << "\n";
		//std::cout << "Throttle:" << m_ThrottleInput << "\n";
		//std::cout << "Steer:" << m_SteerInput << "\n";

		//	MessagePtr physics_msg(new PhysicsVelocityEvent(GetVelocity(true),GetAngularVelocity(true),from_id));
		//	GetSceneObject()->PostMessage(physics_msg);

		bool col = CheckCollisions(current_pos,current_rot,forwardSpeed);

		if(m_Debug)
		{
			std::stringstream ss;
			ss  <<  GetSceneObject()->GetName();
			ss  <<  "\nGear::" << currentGear;
			ss  <<  "\nTarget:" << targetGear;
			ss  <<  "\nSpeed:" << forwardSpeed;
			ss  <<  "\nCollision:" << col;
			GetSceneObject()->PostRequest(TextCaptionRequestPtr(new TextCaptionRequest(ss.str())));
		}
	}


#define THRESHOLD_FORWARD_SPEED (0.1f)
#define THRESHOLD_SIDEWAYS_SPEED (0.2f)
#define THRESHOLD_ROLLING_BACKWARDS_SPEED (0.1f)


	void PhysXVehicleComponent::ProcessAutoReverse(const physx::PxVehicleWheels& focusVehicle,
		const physx::PxVehicleDriveDynData& driveDynData,
		const physx::PxVehicleDrive4WRawInputData& carRawInputs,
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

			const bool prevIsMovingForwardSlowly=m_IsMovingForwardSlowly;
			bool isMovingForwardSlowly=false;
			bool isMovingBackwards=false;
			//const bool isInAir = false;//focusVehicle.isInAir();
			//if(!isInAir)
			{
				bool accelRaw,brakeRaw,handbrakeRaw;
				if(m_UseDigitalInputs)
				{
					accelRaw=carRawInputs.getDigitalAccel();
					brakeRaw=carRawInputs.getDigitalBrake();
					handbrakeRaw=carRawInputs.getDigitalHandbrake();
				}
				else
				{
					accelRaw=carRawInputs.getAnalogAccel() > 0 ? true : false;
					brakeRaw=carRawInputs.getAnalogBrake() > 0 ? true : false;
					handbrakeRaw=carRawInputs.getAnalogHandbrake() > 0 ? true : false;
				}

				const PxF32 forwardSpeed = focusVehicle.computeForwardSpeed();
				const PxF32 forwardSpeedAbs = PxAbs(forwardSpeed);
				const PxF32 sidewaysSpeedAbs = PxAbs(focusVehicle.computeSidewaysSpeed());
				const PxU32 currentGear = driveDynData.getCurrentGear();
				const PxU32 targetGear = driveDynData.getTargetGear();

				//Check if the car is rolling against the gear (backwards in forward gear or forwards in reverse gear).
				if(PxVehicleGearsData::eFIRST == currentGear  && forwardSpeed < -THRESHOLD_ROLLING_BACKWARDS_SPEED)
				{
					isMovingBackwards = true;
				}
				else if(PxVehicleGearsData::eREVERSE == currentGear && forwardSpeed > THRESHOLD_ROLLING_BACKWARDS_SPEED)
				{
					isMovingBackwards = true;
				}

				//Check if the car is moving slowly.
				if(forwardSpeedAbs < THRESHOLD_FORWARD_SPEED && sidewaysSpeedAbs < THRESHOLD_SIDEWAYS_SPEED)
				{
					isMovingForwardSlowly=true;
				}

				//Now work if we need to toggle from forwards gear to reverse gear or vice versa.
				if(isMovingBackwards)
				{
					if(!accelRaw && !brakeRaw && !handbrakeRaw && (currentGear == targetGear))
					{
						//The car is rolling against the gear and the player is doing nothing to stop this.
						toggleAutoReverse = true;
					}
				}
				else if(prevIsMovingForwardSlowly && isMovingForwardSlowly)
				{
					if((currentGear > PxVehicleGearsData::eNEUTRAL) && brakeRaw && !accelRaw && (currentGear == targetGear))
					{
						//The car was moving slowly in forward gear without player input and is now moving slowly with player input that indicates the
						//player wants to switch to reverse gear.
						toggleAutoReverse = true;
					}
					else if(currentGear == PxVehicleGearsData::eREVERSE && accelRaw && !brakeRaw && (currentGear == targetGear))
					{
						//The car was moving slowly in reverse gear without player input and is now moving slowly with player input that indicates the
						//player wants to switch to forward gear.
						toggleAutoReverse = true;
					}
				}

				//If the car was brought to rest through braking then the player needs to release the brake then reapply
				//to indicate that the gears should toggle between reverse and forward.
				/*if(isMovingForwardSlowly && !brakeRaw && !accelRaw && !handbrakeRaw)
				{
				newIsMovingForwardSlowly = true;
				}*/
				newIsMovingForwardSlowly = isMovingForwardSlowly;
			}
		}
	}

	void PhysXVehicleComponent::SetPosition(const Vec3 &value)
	{
		if(m_Actor)
		{
			Reset();
			
			const PhysXPhysicsSceneManagerPtr sm = m_SceneManager.lock();
			const Vec3 org_pos = GetPosition();
			//Vec3 trans_vec = value - org_pos;

			m_Actor->setGlobalPose(physx::PxTransform(sm->WorldToLocal(value), m_Actor->getGlobalPose().q));

			ComponentContainer::ComponentVector components;
			GetSceneObject()->GetComponentsByClassName(components,"PhysXBodyComponent");

			for(int i = 0 ; i < components.size(); i++)
			{
				PhysXBodyComponentPtr body = GASS_STATIC_PTR_CAST<PhysXBodyComponent>(components[i]);
				//if(body.get() != this)
				{
					//LocationComponentPtr location = body->GetSceneObject()->GetFirstComponentByClass<ILocationComponent>();
					Vec3 offset_to_child = body->GetPosition() - org_pos;
					Vec3 new_pos = offset_to_child + value;//trans_vec;
					body->SetPosition(new_pos);
				}
			}

			if(m_Vehicle)
			{
				PxShape* carShapes[PX_MAX_NB_WHEELS+1];
				const PxU32 numShapes= m_Vehicle->getRigidDynamicActor()->getNbShapes();
				m_Vehicle->getRigidDynamicActor()->getShapes(carShapes,numShapes);
				const PxRigidDynamic& vehicleActor = *m_Vehicle->getRigidDynamicActor();

				if(m_AllWheels.size() == numShapes-1)
				{
					for(size_t i = 0; i < numShapes-1; i++)
					{
						SceneObjectPtr wheel(m_AllWheels[i]);
						if(wheel)
						{
							Vec3 pos = sm->LocalToWorld(PxShapeExt::getGlobalPose(*carShapes[i], vehicleActor).p);
							wheel->GetFirstComponentByClass<ILocationComponent>()->SetPosition(pos);
							Quaternion rot = PxConvert::ToGASS(PxShapeExt::getGlobalPose(*carShapes[i],vehicleActor).q);
							wheel->GetFirstComponentByClass<ILocationComponent>()->SetRotation(rot);
						}
					}
				}
				else
				{
					GASS_EXCEPT(GASS::Exception::ERR_RT_ASSERTION_FAILED,"Physics shapes dont match visual geomtries","PhysXVehicleComponent::SetPosition")
				}
			}
		}
	}

	Vec3  PhysXVehicleComponent::GetPosition() const
	{
		Vec3 pos(0,0,0);
		if(m_Actor)
		{
			PhysXPhysicsSceneManagerPtr sm = m_SceneManager.lock();
			pos = sm->LocalToWorld(m_Actor->getGlobalPose().p);
		}
		return pos;
	}

	void PhysXVehicleComponent::SetRotation(const Quaternion &rot)
	{
		if(m_Actor)
		{
			Reset();
			m_Actor->setGlobalPose(physx::PxTransform(m_Actor->getGlobalPose().p,PxConvert::ToPx(rot)));
		}
	}

	Quaternion PhysXVehicleComponent::GetRotation() const
	{
		Quaternion q;

		if(m_Actor)
		{
			q = PxConvert::ToGASS(m_Actor->getGlobalPose().q);
		}
		return q;
	}

	void PhysXVehicleComponent::OnInput(InputRelayEventPtr message)
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

	bool PhysXVehicleComponent::CheckCollisions(const Vec3 &/*pos*/, const Quaternion &/*rot*/, Float /*speed*/) const
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



		ComponentContainerTemplate::ComponentVector components;
		GetSceneObject()->GetParentSceneObject()->GetParentSceneObject()->GetParentSceneObject()->GetComponentsByClass<PhysXVehicleComponent>(components);

		for(int i = 0;  i < components.size(); i++)
		{
		PhysXVehicleComponentPtr comp = GASS_DYNAMIC_PTR_CAST<PhysXVehicleComponent>(components[i]);
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
