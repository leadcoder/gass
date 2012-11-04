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

#include "Plugins/PhysX3/PhysXCarComponent.h"
#include "Plugins/PhysX3/PhysXWheelComponent.h"
#include "Plugins/PhysX3/PhysXPhysicsSceneManager.h"
#include "Plugins/PhysX3/PhysXPhysicsSystem.h"
#include "Plugins/PhysX3/PhysXVehicleSceneQuery.h"

using namespace physx;
namespace GASS
{
	PhysXCarComponent::PhysXCarComponent(): m_Actor(NULL),
		m_ThrottleInput(0),
		m_SteerInput(0),
		m_Vehicle(NULL)
	{
		m_ChassisData.mMass = 1500;
	}

	PhysXCarComponent::~PhysXCarComponent()
	{

	}

	void PhysXCarComponent::OnMassMessage(PhysicsMassMessagePtr message)
	{
		SetMass(message->GetMass());
	}

	void PhysXCarComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("PhysXCarComponent",new Creator<PhysXCarComponent, IComponent>);
		RegisterProperty<float>("Mass", &PhysXCarComponent::GetMass, &PhysXCarComponent::SetMass);
		RegisterProperty<SceneObjectLink>("FrontLeftWheel", &PhysXCarComponent::GetFrontLeftWheel, &PhysXCarComponent::SetFrontLeftWheel);
		RegisterProperty<SceneObjectLink>("FrontRightWheel", &PhysXCarComponent::GetFrontRightWheel, &PhysXCarComponent::SetFrontRightWheel);
		RegisterProperty<SceneObjectLink>("RearLeftWheel", &PhysXCarComponent::GetRearLeftWheel, &PhysXCarComponent::SetRearLeftWheel);
		RegisterProperty<SceneObjectLink>("RearRightWheel", &PhysXCarComponent::GetRearRightWheel, &PhysXCarComponent::SetRearRightWheel);
	}

	PxVec3 PhysXCarComponent::ComputeDim(const PxConvexMesh* cm)
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

	void PhysXCarComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXCarComponent::OnLocationLoaded,LocationLoadedMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXCarComponent::OnPositionChanged,PositionMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXCarComponent::OnWorldPositionChanged,WorldPositionMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXCarComponent::OnRotationChanged,RotationMessage,0 ));
		GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXCarComponent::OnMassMessage,PhysicsMassMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXCarComponent::OnInput,InputControllerMessage,0));


		GetSceneObject()->GetScene()->RegisterForMessage(REG_TMESS(PhysXCarComponent::OnPostSceneObjectInitialized,PostSceneObjectInitialized,0));
		PhysXPhysicsSceneManagerPtr scene_manager = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<PhysXPhysicsSceneManager>();
		scene_manager->Register(shared_from_this());

	}

	void PhysXCarComponent::Reset()
	{
		if(!m_Vehicle)
			return;
		//Set the car back to its rest state.
		m_Vehicle->setToRestState();
		//Set the car to first gear.
		m_Vehicle->mDriveDynData.forceGearChange(PxVehicleGearsData::eFIRST);
	}

	void PhysXCarComponent::OnPostSceneObjectInitialized(PostSceneObjectInitializedPtr message)
	{
		if(message->GetSceneObject() != GetSceneObject())
			return;

		//Get chassis mesh
		MeshComponentPtr geom = GetSceneObject()->GetFirstComponentByClass<IMeshComponent>();
		GASSAssert(geom,"PhysXCarComponent::OnInitialize");
		PhysXPhysicsSceneManagerPtr scene_manager = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<PhysXPhysicsSceneManager>();
		GASSAssert(scene_manager,"PhysXCarComponent::OnInitialize");
		PhysXConvexMesh chassisMesh = scene_manager->CreateConvexMesh(geom);

		std::vector<PxConvexMesh*> wheelConvexMeshes4;
		std::vector<PxVec3 > wheelCentreOffsets4;
		//Add wheels 
		std::vector<SceneObjectPtr> wheel_objects;
		wheel_objects.push_back(m_FrontLeftWheel.GetObjectPtr());
		wheel_objects.push_back(m_FrontRightWheel.GetObjectPtr());
		wheel_objects.push_back(m_RearLeftWheel.GetObjectPtr());
		wheel_objects.push_back(m_RearRightWheel.GetObjectPtr());


		//Get data from wheels
		PxVehicleWheelData wheels[4];
		PxVehicleSuspensionData susps[4];
		PxVehicleTireData tires[4];
		for(size_t i = 0 ; i < wheel_objects.size(); i++)
		{
			SceneObjectPtr child = wheel_objects[i];
			//we need to force geometry changed message to be fired before we can get data from wheel
			child->SyncMessages(0);
			MeshComponentPtr geom = child->GetFirstComponentByClass<IMeshComponent>();
			LocationComponentPtr location = child->GetFirstComponentByClass<ILocationComponent>();
			PhysXConvexMesh wheelMesh = scene_manager->CreateConvexMesh(geom);
			wheelConvexMeshes4.push_back(wheelMesh.m_ConvexMesh);
			Vec3 pos = location->GetPosition();
			wheelCentreOffsets4.push_back(PxVec3(pos.x,pos.y,pos.z));

			PhysXWheelComponentPtr wheel_comp = child->GetFirstComponentByClass<PhysXWheelComponent>();
			wheels[i] = wheel_comp->GetWheelData();
			susps[i] = wheel_comp->GetSuspensionData();
			tires[i] =  wheel_comp->GetTireData();
		}

		PxVehicleWheelsSimData* wheelsSimData = PxVehicleWheelsSimData::allocate(4);
		PxVehicleDriveSimData4W driveSimData;



		//Extract the chassis AABB dimensions from the chassis convex mesh.
		const PxVec3 chassisDims=ComputeDim(chassisMesh.m_ConvexMesh);

		//The origin is at the center of the chassis mesh.
		//Set the center of mass to be below this point and a little towards the front.
		const PxVec3 chassisCMOffset=PxVec3(0.0f,-chassisDims.y*0.5f+0.65f,0.25f);

		//Now compute the chassis mass and moment of inertia.
		//Use the moment of inertia of a cuboid as an approximate value for the chassis moi.
		PxVec3 chassisMOI
			((chassisDims.y*chassisDims.y + chassisDims.z*chassisDims.z)*m_ChassisData.mMass/12.0f,
			(chassisDims.x*chassisDims.x + chassisDims.z*chassisDims.z)*m_ChassisData.mMass/12.0f,
			(chassisDims.x*chassisDims.x + chassisDims.y*chassisDims.y)*m_ChassisData.mMass/12.0f);
		//A bit of tweaking here.  The car will have more responsive turning if we reduce the
		//y-component of the chassis moment of inertia.
		chassisMOI.y*=0.8f;

		//Let's set up the chassis data structure now.
		m_ChassisData.mMOI=chassisMOI;
		m_ChassisData.mCMOffset=chassisCMOffset;

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
		const PxF32 massRear=0.5f * m_ChassisData.mMass * (chassisDims.z-3*chassisCMOffset.z)/chassisDims.z;
		const PxF32 massFront=m_ChassisData.mMass - massRear;



		//Disable the handbrake from the front wheels and enable for the rear wheels
		wheels[PxVehicleDrive4W::eFRONT_LEFT_WHEEL].mMaxHandBrakeTorque=0.0f;
		wheels[PxVehicleDrive4W::eFRONT_RIGHT_WHEEL].mMaxHandBrakeTorque=0.0f;
		wheels[PxVehicleDrive4W::eREAR_LEFT_WHEEL].mMaxHandBrakeTorque=4000.0f;
		wheels[PxVehicleDrive4W::eREAR_RIGHT_WHEEL].mMaxHandBrakeTorque=4000.0f;
		//Enable steering for the front wheels and disable for the front wheels.
		wheels[PxVehicleDrive4W::eFRONT_LEFT_WHEEL].mMaxSteer=PxPi*0.3333f;
		wheels[PxVehicleDrive4W::eFRONT_RIGHT_WHEEL].mMaxSteer=PxPi*0.3333f;
		wheels[PxVehicleDrive4W::eREAR_LEFT_WHEEL].mMaxSteer=0.0f;
		wheels[PxVehicleDrive4W::eREAR_RIGHT_WHEEL].mMaxSteer=0.0f;

		susps[PxVehicleDrive4W::eFRONT_LEFT_WHEEL].mSprungMass=massFront*0.5f;
		susps[PxVehicleDrive4W::eFRONT_RIGHT_WHEEL].mSprungMass=massFront*0.5f;
		susps[PxVehicleDrive4W::eREAR_LEFT_WHEEL].mSprungMass=massRear*0.5f;
		susps[PxVehicleDrive4W::eREAR_RIGHT_WHEEL].mSprungMass=massRear*0.5f;

		//We need to set up geometry data for the suspension, wheels, and tires.
		//We already know the wheel centers described as offsets from the rigid body centre of mass.
		//From here we can approximate application points for the tire and suspension forces.
		//Lets assume that the suspension travel directions are absolutely vertical.
		//Also assume that we apply the tire and suspension forces 30cm below the centre of mass.
		PxVec3 suspTravelDirections[4]={PxVec3(0,-1,0),PxVec3(0,-1,0),PxVec3(0,-1,0),PxVec3(0,-1,0)};
		PxVec3 wheelCentreCMOffsets[4];
		PxVec3 suspForceAppCMOffsets[4];
		PxVec3 tireForceAppCMOffsets[4];
		for(PxU32 i=0;i<4;i++)
		{
			wheelCentreCMOffsets[i]=wheelCentreOffsets4[i]-chassisCMOffset;
			suspForceAppCMOffsets[i]=PxVec3(wheelCentreCMOffsets[i].x,-0.3f,wheelCentreCMOffsets[i].z);
			tireForceAppCMOffsets[i]=PxVec3(wheelCentreCMOffsets[i].x,-0.3f,wheelCentreCMOffsets[i].z);
		}

		//Now add the wheel, tire and suspension data.
		for(PxU32 i=0;i<4;i++)
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
		engine.mPeakTorque=500.0f;
		engine.mMaxOmega=600.0f;//approx 6000 rpm
		driveSimData.setEngineData(engine);

		//Gears
		PxVehicleGearsData gears;
		gears.mSwitchTime=0.5f;
		driveSimData.setGearsData(gears);

		//Clutch
		PxVehicleClutchData clutch;
		clutch.mStrength=10.0f;
		driveSimData.setClutchData(clutch);

		//Ackermann steer accuracy
		PxVehicleAckermannGeometryData ackermann;
		ackermann.mAccuracy=1.0f;
		ackermann.mAxleSeparation=wheelCentreOffsets4[PxVehicleDrive4W::eFRONT_LEFT_WHEEL].z - wheelCentreOffsets4[PxVehicleDrive4W::eREAR_LEFT_WHEEL].z;
		ackermann.mFrontWidth=wheelCentreOffsets4[PxVehicleDrive4W::eFRONT_RIGHT_WHEEL].x-wheelCentreOffsets4[PxVehicleDrive4W::eFRONT_LEFT_WHEEL].x;
		ackermann.mRearWidth=wheelCentreOffsets4[PxVehicleDrive4W::eREAR_RIGHT_WHEEL].x-wheelCentreOffsets4[PxVehicleDrive4W::eREAR_LEFT_WHEEL].x;
		driveSimData.setAckermannGeometryData(ackermann);

		//////////////
		//PxRigidDynamic* vehActor=createVehicleActor4W(m_ChassisData,&wheelConvexMeshes4[0],chassisMesh.m_ConvexMesh,*scene_manager->GetPxScene(),*system->GetPxSDK(),*system->GetDefaultMaterial());
		//m_Actor = vehActor;

		PhysXPhysicsSystemPtr system = SimEngine::Get().GetSimSystemManager()->GetFirstSystem<PhysXPhysicsSystem>();


		//We need a rigid body actor for the vehicle.
		//Don't forget to add the actor the scene after setting up the associated vehicle.
		m_Actor = system->GetPxSDK()->createRigidDynamic(PxTransform::createIdentity());


		const PxMaterial& wheelMaterial = *system->GetDefaultMaterial();
		PxFilterData wheelCollFilterData;
		wheelCollFilterData.word0=COLLISION_FLAG_WHEEL;
		wheelCollFilterData.word1=COLLISION_FLAG_WHEEL_AGAINST;

		//Create a query filter data for the car to ensure that cars
		//do not attempt to drive on themselves.
		PxFilterData vehQryFilterData;
		VehicleSetupVehicleShapeQueryFilterData(&vehQryFilterData);

		//Add all the wheel shapes to the actor.
		for(PxU32 i=0;i < 4;i++)
		{
			PxConvexMeshGeometry wheelGeom(wheelConvexMeshes4[i]);
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
		chassisCollFilterData.word0=COLLISION_FLAG_CHASSIS;
		chassisCollFilterData.word1=COLLISION_FLAG_CHASSIS_AGAINST;
		chassisShape->setSimulationFilterData(chassisCollFilterData);

		chassisShape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE,true);
		chassisShape->setLocalPose(PxTransform::createIdentity());

		m_Actor->setMass(m_ChassisData.mMass);
		m_Actor->setMassSpaceInertiaTensor(m_ChassisData.mMOI);
		m_Actor->setCMassLocalPose(PxTransform(m_ChassisData.mCMOffset,PxQuat::createIdentity()));

		m_Vehicle = PxVehicleDrive4W::allocate(4);
		m_Vehicle->setup(system->GetPxSDK(),m_Actor,*wheelsSimData,driveSimData,0);

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
		m_Initialized = true;
	}

	void PhysXCarComponent::OnPositionChanged(PositionMessagePtr message)
	{
		int this_id = (int)this; //we used address as id
		if(message->GetSenderID() != this_id) //Check if this message was from this class
		{
			Vec3 pos = message->GetPosition();
			SetPosition(pos);
		}
	}

	void PhysXCarComponent::OnWorldPositionChanged(WorldPositionMessagePtr message)
	{
		int this_id = (int)this; //we used address as id
		if(message->GetSenderID() != this_id) //Check if this message was from this class
		{
			Vec3 pos = message->GetPosition();
			SetPosition(pos);
		}
	}

	void PhysXCarComponent::OnRotationChanged(RotationMessagePtr message)
	{
		int this_id = (int)this; //we used address as id
		if(message->GetSenderID() != this_id) //Check if this message was from this class
		{
			Quaternion rot = message->GetRotation();
			SetRotation(rot);
		}
	}

	void PhysXCarComponent::OnLocationLoaded(LocationLoadedMessagePtr message)
	{
		PhysXPhysicsSceneManagerPtr sm = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<PhysXPhysicsSceneManager>();
		m_SceneManager = sm;
	}


	PxVehicleKeySmoothingData gKeySmoothingData=
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

	PxVehiclePadSmoothingData gCarPadSmoothingData=
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

	PxF32 gSteerVsForwardSpeedData[2*8]=
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
	PxFixedSizeLookupTable<8> gSteerVsForwardSpeedTable(gSteerVsForwardSpeedData,4);

	void PhysXCarComponent::SceneManagerTick(double delta)
	{
		int from_id = (int)this; //use address as id

		MessagePtr pos_msg(new WorldPositionMessage(GetPosition(),from_id));
		GetSceneObject()->PostMessage(pos_msg);

		MessagePtr rot_msg(new WorldRotationMessage(GetRotation(),from_id));
		GetSceneObject()->PostMessage(rot_msg);

		int mNumVehicles = 1;

		std::vector<SceneObjectPtr> wheels;
		IComponentContainer::ComponentContainerIterator cc_iter1 = GetSceneObject()->GetChildren();
		while(cc_iter1.hasMoreElements())
		{
			SceneObjectPtr child = boost::shared_dynamic_cast<GASS::SceneObject>(cc_iter1.getNext());
			wheels.push_back(child);
		}

		PxShape* carShapes[PX_MAX_NUM_WHEELS+1];
		const PxU32 numShapes=m_Vehicle->getRigidDynamicActor()->getNbShapes();
		m_Vehicle->getRigidDynamicActor()->getShapes(carShapes,numShapes);
		for(int i = 0; i < numShapes-1; i++)
		{
			Vec3 pos = PxConvert::ToGASS(carShapes[i]->getLocalPose().p);

			MessagePtr pos_msg(new PositionMessage(pos,from_id));
			wheels[i]->PostMessage(pos_msg);

			Quaternion rot = PxConvert::ToGASS(carShapes[i]->getLocalPose().q);
			MessagePtr rot_msg(new RotationMessage(rot,from_id));
			wheels[i]->PostMessage(rot_msg);
		}

		PxVehicleDrive4WRawInputData rawInputData;
		rawInputData.setDigitalAccel(m_ThrottleInput);
		rawInputData.setDigitalBrake(0);
		rawInputData.setDigitalHandbrake(0);
		if(m_SteerInput < 0)
		{
			rawInputData.setDigitalSteerLeft(fabs(m_SteerInput));
			rawInputData.setDigitalSteerRight(0);
		}
		else
		{
			rawInputData.setDigitalSteerLeft(0);
			rawInputData.setDigitalSteerRight(m_SteerInput);
		}
		rawInputData.setGearDown(false);
		rawInputData.setGearUp(false);

		PxVehicleDrive4WSmoothDigitalRawInputsAndSetAnalogInputs(gKeySmoothingData,gSteerVsForwardSpeedTable,rawInputData,delta,*m_Vehicle);


		PxVehicleDriveDynData& driveDynData=m_Vehicle->mDriveDynData;
		const PxF32 forwardSpeed = m_Vehicle->computeForwardSpeed();
		const PxF32 forwardSpeedAbs = PxAbs(forwardSpeed);
		const PxF32 sidewaysSpeedAbs = PxAbs(m_Vehicle->computeSidewaysSpeed());

		const PxU32 currentGear = driveDynData.getCurrentGear();
		const PxU32 targetGear = driveDynData.getTargetGear();

		//std::cout << "current Gear:" << currentGear << " Target:" << targetGear << "\n";
		//std::cout << "Speed:" << forwardSpeed << " Sideways:" << sidewaysSpeedAbs << "\n";

		//	MessagePtr physics_msg(new VelocityNotifyMessage(GetVelocity(true),GetAngularVelocity(true),from_id));
		//	GetSceneObject()->PostMessage(physics_msg);
	}

	void PhysXCarComponent::SetMass(float mass)
	{
		m_ChassisData.mMass = mass;
	}

	void PhysXCarComponent::SetPosition(const Vec3 &value)
	{
		if(m_Actor)
		{
			m_Actor->setGlobalPose(physx::PxTransform(PxConvert::ToPx(value), m_Actor->getGlobalPose().q));
		}
	}

	Vec3  PhysXCarComponent::GetPosition() const
	{
		Vec3 pos(0,0,0);
		if(m_Actor)
		{
			pos = PxConvert::ToGASS(m_Actor->getGlobalPose().p);
		}
		return pos;
	}

	void PhysXCarComponent::SetRotation(const Quaternion &rot)
	{
		if(m_Actor)
		{
			m_Actor->setGlobalPose(physx::PxTransform(m_Actor->getGlobalPose().p,PxConvert::ToPx(rot)));
		}
	}

	Quaternion PhysXCarComponent::GetRotation()
	{
		Quaternion q;

		if(m_Actor)
		{
			q = PxConvert::ToGASS(m_Actor->getGlobalPose().q);
		}
		return q;
	}

	void PhysXCarComponent::OnInput(InputControllerMessagePtr message)
	{
		std::string name = message->GetController();
		float value = message->GetValue();

		if (name == "Throttle")
		{
			m_ThrottleInput = value;
		}
		else if (name == "Steer")
		{
			m_SteerInput = value;
		}
	}
}
