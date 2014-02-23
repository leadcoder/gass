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

#include "Plugins/PhysX3/PhysXVehicleComponent.h"
#include "Plugins/PhysX3/PhysXWheelComponent.h"
#include "Plugins/PhysX3/PhysXPhysicsSceneManager.h"
#include "Plugins/PhysX3/PhysXPhysicsSystem.h"
#include "Plugins/PhysX3/PhysXVehicleSceneQuery.h"
#include "Sim/Messages/GASSSoundSceneObjectMessages.h"

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
		m_ChassisDim(0,0,0)
	{
		//add some default gears, start with reverse!
		m_GearRatios.push_back(-4); //reverse
		m_GearRatios.push_back(0); //neutral
		m_GearRatios.push_back(4); //first gear
		m_GearRatios.push_back(2);
		m_GearRatios.push_back(1.5);
		m_GearRatios.push_back(1.1);
		m_GearRatios.push_back(1.0);
	}

	PhysXVehicleComponent::~PhysXVehicleComponent()
	{

	}

	void PhysXVehicleComponent::OnMassMessage(PhysicsBodyMassRequestPtr message)
	{
		SetMass(message->GetMass());
	}

	void PhysXVehicleComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("PhysXVehicleComponent",new Creator<PhysXVehicleComponent, IComponent>);
		REG_PROPERTY(float,Mass, PhysXVehicleComponent)
		REG_PROPERTY(float,ScaleMass, PhysXVehicleComponent)
		REG_PROPERTY(float,EnginePeakTorque, PhysXVehicleComponent)
		REG_PROPERTY(float,EngineMaxRotationSpeed,PhysXVehicleComponent)
		REG_PROPERTY(float,ClutchStrength, PhysXVehicleComponent)
		REG_VECTOR_PROPERTY(float,GearRatios,PhysXVehicleComponent)
		REG_PROPERTY(bool,UseAutoReverse, PhysXVehicleComponent)
		REG_PROPERTY(float,GearSwitchTime, PhysXVehicleComponent)
		REG_PROPERTY(SceneObjectRef,FrontLeftWheel, PhysXVehicleComponent)
		REG_PROPERTY(SceneObjectRef,FrontRightWheel, PhysXVehicleComponent)
		REG_PROPERTY(SceneObjectRef,RearLeftWheel, PhysXVehicleComponent)
		REG_PROPERTY(SceneObjectRef,RearRightWheel, PhysXVehicleComponent)
		REG_PROPERTY(SceneObjectRef,RearRightWheel, PhysXVehicleComponent)
		REG_VECTOR_PROPERTY(SceneObjectRef,ExtraWheels,PhysXVehicleComponent)
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
		GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXVehicleComponent::OnLocationLoaded,LocationLoadedMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXVehicleComponent::OnPositionChanged,PositionMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXVehicleComponent::OnWorldPositionChanged,WorldPositionMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXVehicleComponent::OnRotationChanged,RotationMessage,0 ));
		GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXVehicleComponent::OnWorldRotationChanged,WorldRotationMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXVehicleComponent::OnMassMessage,PhysicsBodyMassRequest,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXVehicleComponent::OnInput,InputControllerMessage,0));
		GetSceneObject()->GetScene()->RegisterForMessage(REG_TMESS(PhysXVehicleComponent::OnPostSceneObjectInitializedEvent,PostSceneObjectInitializedEvent,0));
		PhysXPhysicsSceneManagerPtr scene_manager = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<PhysXPhysicsSceneManager>();
		scene_manager->Register(shared_from_this());


		//Play engine sound
		MessagePtr sound_msg(new SoundParameterMessage(SoundParameterMessage::PLAY,0));
		GetSceneObject()->PostMessage(sound_msg);
	}

	Vec3 PhysXVehicleComponent::GetSize() const
	{
		return m_ChassisDim;
	}

	Float PhysXVehicleComponent::GetMaxSpeed() const
	{
		return 0;
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
		m_InReverseMode = false;
		for(size_t i = 0; i < m_Vehicle->mWheelsSimData.getNumWheels(); i++)
		{
			m_Vehicle->setWheelShapeMapping(static_cast<physx::PxI32>(i),static_cast<physx::PxU32>(i));
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

		MeshComponentPtr geom = GetSceneObject()->GetFirstComponentByClass<IMeshComponent>();
		GASSAssert(geom,"PhysXVehicleComponent::OnInitialize");
		PhysXPhysicsSceneManagerPtr scene_manager = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<PhysXPhysicsSceneManager>();
		GASSAssert(scene_manager,"PhysXVehicleComponent::OnInitialize");
		PhysXConvexMesh chassisMesh = scene_manager->CreateConvexMesh(col_mesh_id,geom);

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

			std::string col_mesh_id = child->GetName();
			ResourceComponentPtr res  = child->GetFirstComponentByClass<IResourceComponent>();
			if(res)
			{
				col_mesh_id = res->GetResource().Name();
			}
			MeshComponentPtr geom = child->GetFirstComponentByClass<IMeshComponent>();
			LocationComponentPtr location = child->GetFirstComponentByClass<ILocationComponent>();
			PhysXConvexMesh wheelMesh = scene_manager->CreateConvexMesh(col_mesh_id,geom);
			wheelConvexMeshes.push_back(wheelMesh.m_ConvexMesh);
			Vec3 pos = location->GetPosition();
			wheelCentreOffsets.push_back(PxVec3(pos.x,pos.y,pos.z));

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
		float tot = massRear*4 + massFront;


		//set mSprungMass from vehicle mass move this to wheel?
		susps[PxVehicleDrive4W::eFRONT_LEFT_WHEEL].mSprungMass=massFront*0.5f;
		susps[PxVehicleDrive4W::eFRONT_RIGHT_WHEEL].mSprungMass=massFront*0.5f;
		susps[PxVehicleDrive4W::eREAR_LEFT_WHEEL].mSprungMass=massRear;
		susps[PxVehicleDrive4W::eREAR_RIGHT_WHEEL].mSprungMass=massRear;

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
		std::vector<PxVec3> suspTravelDirections;
		std::vector<PxVec3> wheelCentreCMOffsets;
		std::vector<PxVec3> suspForceAppCMOffsets;
		std::vector<PxVec3> tireForceAppCMOffsets;
		suspTravelDirections.resize(num_wheels);
		wheelCentreCMOffsets.resize(num_wheels);
		suspForceAppCMOffsets.resize(num_wheels);
		tireForceAppCMOffsets.resize(num_wheels);
		
		for(PxU32 i=0;i<num_wheels;i++)
		{
			suspTravelDirections[i]= PxVec3(0,-1,0);
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
			wheelsSimData->setSuspTravelDirection(i,suspTravelDirections[i]);
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
		
		if(m_GearRatios.size() >= PxVehicleGearsData::eMAX_NUM_GEAR_RATIOS)
		{
			GASS_EXCEPT(Exception::ERR_INVALIDPARAMS, "To many gear in vehicle","PhysXVehicleComponent::OnPostSceneObjectInitializedEvent");
		}
		gears.mNumRatios = static_cast<physx::PxU32>(m_GearRatios.size());
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
		ackermann.mAxleSeparation= fabs(wheelCentreOffsets[PxVehicleDrive4W::eFRONT_LEFT_WHEEL].z - wheelCentreOffsets[PxVehicleDrive4W::eREAR_LEFT_WHEEL].z);
		ackermann.mFrontWidth=fabs(wheelCentreOffsets[PxVehicleDrive4W::eFRONT_RIGHT_WHEEL].x-wheelCentreOffsets[PxVehicleDrive4W::eFRONT_LEFT_WHEEL].x);
		ackermann.mRearWidth=fabs(wheelCentreOffsets[PxVehicleDrive4W::eREAR_RIGHT_WHEEL].x-wheelCentreOffsets[PxVehicleDrive4W::eREAR_LEFT_WHEEL].x);
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
			m_Vehicle->setWheelShapeMapping(static_cast<physx::PxI32>(i),static_cast<physx::PxU32>(i));
		}
		
		//Free the sim data because we don't need that any more.
		wheelsSimData->free();

		//Don't forget to add the actor to the scene.
		scene_manager->GetPxScene()->addActor(*m_Actor);

		//Set the transform and the instantiated car and set it be to be at rest.
		LocationComponentPtr location = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>();
		Vec3 pos = location->GetPosition();
		PxTransform startTransform(PxVec3(pos.x,pos.y,pos.z), PxQuat(0, 0, 0, 1));

		Reset();

		//Set the autogear mode of the instantiate car.
		m_Vehicle->mDriveDynData.setUseAutoGears(true);
		scene_manager->RegisterVehicle(m_Vehicle);

		for(size_t i= 0; i < wheel_objects.size(); i++)
		{
			m_AllWheels.push_back(wheel_objects[i]);
		}
		GetSceneObject()->SendImmediate(MessagePtr(new BodyLoadedMessage()));
		m_Initialized = true;
	}

	void PhysXVehicleComponent::OnPositionChanged(PositionMessagePtr message)
	{
		int this_id = (int)this; //we used address as id
		if(message->GetSenderID() != this_id) //Check if this message was from this class
		{
			Vec3 pos = message->GetPosition();
			SetPosition(pos);
		}
	}

	void PhysXVehicleComponent::OnWorldPositionChanged(WorldPositionMessagePtr message)
	{
		int this_id = (int)this; //we used address as id
		if(message->GetSenderID() != this_id) //Check if this message was from this class
		{
			Vec3 pos = message->GetPosition();
			SetPosition(pos);
		}
	}

	void PhysXVehicleComponent::OnRotationChanged(RotationMessagePtr message)
	{
		int this_id = (int)this; //we used address as id
		if(message->GetSenderID() != this_id) //Check if this message was from this class
		{
			Quaternion rot = message->GetRotation();
			SetRotation(rot);
		}
	}

	void PhysXVehicleComponent::OnWorldRotationChanged(WorldRotationMessagePtr message)
	{
		int this_id = (int)this; //we used address as id
		if(message->GetSenderID() != this_id) //Check if this message was from this class
		{
			Quaternion rot = message->GetRotation();
			SetRotation(rot);
		}
	}

	void PhysXVehicleComponent::OnLocationLoaded(LocationLoadedMessagePtr message)
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
		int from_id = (int)this; //use address as id
		Vec3 current_pos  = GetPosition();
		
		MessagePtr pos_msg(new WorldPositionMessage(current_pos ,from_id));
		GetSceneObject()->PostMessage(pos_msg);
		MessagePtr rot_msg(new WorldRotationMessage(GetRotation(),from_id));
		GetSceneObject()->PostMessage(rot_msg);

		PxShape* carShapes[PX_MAX_NUM_WHEELS+1];
		const PxU32 numShapes=m_Vehicle->getRigidDynamicActor()->getNbShapes();
		m_Vehicle->getRigidDynamicActor()->getShapes(carShapes,numShapes);

		if(m_AllWheels.size() == numShapes-1)
		{
			for(int i = 0; i < numShapes-1; i++)
			{
				SceneObjectPtr wheel(m_AllWheels[i]);
				if(wheel)
				{
					Vec3 pos = PxConvert::ToGASS(PxShapeExt::getGlobalPose(*carShapes[i]).p);
					MessagePtr pos_msg(new PositionMessage(pos,from_id));
					wheel->PostMessage(pos_msg);

					Quaternion rot = PxConvert::ToGASS(PxShapeExt::getGlobalPose(*carShapes[i]).q);
					MessagePtr rot_msg(new RotationMessage(rot,from_id));
					wheel->PostMessage(rot_msg);
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
				rawInputData.setDigitalSteerLeft(fabs(m_SteerInput));
				rawInputData.setDigitalSteerRight(0);
				rawInputData.setDigitalSteerLeft(fabs(m_SteerInput));
			
			}
			else
			{
				rawInputData.setDigitalSteerLeft(0);
				rawInputData.setDigitalSteerRight(m_SteerInput);
			}
		}
		else
		{
			if(m_ThrottleInput < 0)
			{
				rawInputData.setAnalogBrake(fabs(m_ThrottleInput));
				rawInputData.setAnalogAccel(0);
			}
			else
			{
				rawInputData.setAnalogAccel(fabs(m_ThrottleInput));
				rawInputData.setAnalogBrake(0);
			}
			rawInputData.setAnalogHandbrake(0);
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


		// Now filter the raw input values and apply them to focus vehicle
		// as floats for brake,accel,handbrake,steer and bools for gearup,geardown.
		if(m_UseDigitalInputs)
		{
			PxVehicleDrive4WSmoothDigitalRawInputsAndSetAnalogInputs(VehicleKeySmoothingData,VehicleSteerVsForwardSpeedTable,rawInputData,delta,*m_Vehicle);
		}
		else
		{
			PxVehicleDrive4WSmoothAnalogRawInputsAndSetAnalogInputs(VehiclePadSmoothingData,VehicleSteerVsForwardSpeedTable,rawInputData,delta,*m_Vehicle);
		}



		const PxF32 forwardSpeed = m_Vehicle->computeForwardSpeed();
		const PxF32 forwardSpeedAbs = PxAbs(forwardSpeed);
		const PxF32 sidewaysSpeedAbs = PxAbs(m_Vehicle->computeSidewaysSpeed());

		const PxU32 currentGear = driveDynData.getCurrentGear();
		const PxU32 targetGear = driveDynData.getTargetGear();

		MessagePtr physics_msg(new VelocityNotifyMessage(Vec3(0,0,-forwardSpeed),Vec3(0,0,0),from_id));
		GetSceneObject()->PostMessage(physics_msg);


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
		volume = sqrt(volume)*1.05;

		MessagePtr pitch_msg(new SoundParameterMessage(SoundParameterMessage::PITCH,pitch));
		GetSceneObject()->PostMessage(pitch_msg);

		MessagePtr volume_msg(new SoundParameterMessage(SoundParameterMessage::VOLUME,volume));
		GetSceneObject()->PostMessage(volume_msg);

		//std::cout << "Gear:" << currentGear << " RPS:" << engine_rot_speed << "\n";

		std::stringstream ss;
			ss  <<  GetSceneObject()->GetName();
			ss  <<  "\nGear::" << currentGear;
			ss  <<  "\nTarget:" << targetGear;
			ss  <<  "\nSpeed:" << forwardSpeed;
			

		GetSceneObject()->PostMessage(MessagePtr(new TextCaptionMessage(ss.str())));

		//std::cout << "current Gear:" << currentGear << " Target:" << targetGear << "\n";
		//std::cout << "Speed:" << forwardSpeed << " Sideways:" << sidewaysSpeedAbs << "\n";
		//std::cout << "Throttle:" << m_ThrottleInput << "\n";
		//std::cout << "Steer:" << m_SteerInput << "\n";

		//	MessagePtr physics_msg(new VelocityNotifyMessage(GetVelocity(true),GetAngularVelocity(true),from_id));
		//	GetSceneObject()->PostMessage(physics_msg);
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
			//If the car is travelling very slowly in forward gear without player input and the player subsequently presses the brake then we want the car to go into reverse gear
			//If the car is travelling very slowly in reverse gear without player input and the player subsequently presses the accel then we want the car to go into forward gear
			//If the car is in forward gear and is travelling backwards then we want to automatically put the car into reverse gear.
			//If the car is in reverse gear and is travelling forwards then we want to automatically put the car into forward gear.
			//(If the player brings the car to rest with the brake the player needs to release the brake then reapply it 
			//to indicate they want to toggle between forward and reverse.)

			const bool prevIsMovingForwardSlowly=m_IsMovingForwardSlowly;
			bool isMovingForwardSlowly=false;
			bool isMovingBackwards=false;
			const bool isInAir = focusVehicle.isInAir();
			if(!isInAir)
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
			m_Actor->setGlobalPose(physx::PxTransform(PxConvert::ToPx(value), m_Actor->getGlobalPose().q));
			
		}
	}

	Vec3  PhysXVehicleComponent::GetPosition() const
	{
		Vec3 pos(0,0,0);
		if(m_Actor)
		{
			pos = PxConvert::ToGASS(m_Actor->getGlobalPose().p);
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

	Quaternion PhysXVehicleComponent::GetRotation()
	{
		Quaternion q;

		if(m_Actor)
		{
			q = PxConvert::ToGASS(m_Actor->getGlobalPose().q);
		}
		return q;
	}

	void PhysXVehicleComponent::OnInput(InputControllerMessagePtr message)
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
	}
}
