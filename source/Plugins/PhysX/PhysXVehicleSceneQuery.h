#ifndef VEHICLE_SCENE_QUERY_H
#define VEHICLE_SCENE_QUERY_H

#include "PhysXCommon.h"

using namespace physx;
namespace GASS
{
	enum
	{
		VEHICLE_DRIVABLE_SURFACE = 0xffff0000,
		VEHICLE_UNDRIVABLE_SURFACE = 0x0000ffff
	};

	static PxQueryHitType::Enum VehicleWheelRaycastPreFilter(
		PxFilterData filterData0, 
		PxFilterData filterData1,
		const void* constantBlock, PxU32 constantBlockSize,
		PxSceneQueryFlags& filterFlags)
	{
		//filterData0 is the vehicle suspension raycast.
		//filterData1 is the shape potentially hit by the raycast.
		PX_UNUSED(filterFlags);
		PX_UNUSED(constantBlockSize);
		PX_UNUSED(constantBlock);
		PX_UNUSED(filterData0);
		return ((0 == (filterData1.word3 & VEHICLE_DRIVABLE_SURFACE)) ? PxQueryHitType::eNONE : PxQueryHitType::eBLOCK);
	}

	//Set up query filter data so that vehicles can drive on shapes with this filter data.
	//Note that we have reserved word3 of the PxFilterData for vehicle raycast query filtering.
	void VehicleSetupDrivableShapeQueryFilterData(PxFilterData* qryFilterData);

	//Set up query filter data so that vehicles cannot drive on shapes with this filter data.
	//Note that we have reserved word3 of the PxFilterData for vehicle raycast query filtering.
	void VehicleSetupNonDrivableShapeQueryFilterData(PxFilterData* qryFilterData);

	//Set up query filter data for the shapes of a vehicle to ensure that vehicles cannot drive on themselves 
	//but can drive on the shapes of other vehicles.
	//Note that we have reserved word3 of the PxFilterData for vehicle raycast query filtering.
	void VehicleSetupVehicleShapeQueryFilterData(PxFilterData* qryFilterData);


	//Data structure for quick setup of scene queries for suspension raycasts.
	class VehicleSceneQueryData
	{
	public:

		//Allocate scene query data for up to maxNumWheels suspension raycasts.
		static VehicleSceneQueryData* Allocate(const PxU32 maxNumWheels);

		//Free allocated buffer for scene queries of suspension raycasts.
		void Free();

		//Create a PxBatchQuery instance that will be used as a single batched raycast of multiple suspension lines of multiple vehicles
		PxBatchQuery* SetUpBatchedSceneQuery(PxScene* scene);

		//Get the buffer of scene query results that will be used by PxVehicleNWSuspensionRaycasts
		PxRaycastQueryResult* GetRaycastQueryResultBuffer() {return m_MSqResults;}

		//Get the number of scene query results that have been allocated for use by PxVehicleNWSuspensionRaycasts
		PxU32 GetRaycastQueryResultBufferSize() const {return m_MNumQueries;}

		//Set the pre-filter shader 
		void SetPreFilterShader(PxBatchQueryPreFilterShader preFilterShader) {m_MPreFilterShader=preFilterShader;}

		//Set the spu pre-filter shader (essential to run filtering on spu)
		void SetSpuPreFilterShader(void* spuPreFilterShader, const PxU32 spuPreFilterShaderSize) {m_MSpuPreFilterShader=spuPreFilterShader; m_MSpuPreFilterShaderSize=spuPreFilterShaderSize;}

	private:

		//One result for each wheel.
		PxRaycastQueryResult* m_MSqResults;
		PxU32 m_MNbSqResults;

		//One hit for each wheel.
		PxRaycastHit* m_MSqHitBuffer;

		//Filter shader used to filter drivable and non-drivable surfaces
		PxBatchQueryPreFilterShader m_MPreFilterShader;

		//Ptr to compiled spu filter shader 
		//Set this on ps3 for spu raycasts
		void* m_MSpuPreFilterShader;

		//Size of compiled spu filter shader 
		//Set this on ps3 for spu raycasts.
		PxU32 m_MSpuPreFilterShaderSize;

		//Maximum number of suspension raycasts that can be supported by the allocated buffers 
		//assuming a single query and hit per suspension line.
		PxU32 m_MNumQueries;

		void Init()
		{
			m_MPreFilterShader=VehicleWheelRaycastPreFilter;
			m_MSpuPreFilterShader=nullptr;
			m_MSpuPreFilterShaderSize=0;
		}

		VehicleSceneQueryData()
		{
			Init();
		}

		~VehicleSceneQueryData()
		{
		}
	};


	//Data structure for quick setup of scene queries for suspension raycasts.
	/*class VehicleSceneQueryData
	{
	public:
		//Allocate scene query data for up to maxNumWheels suspension raycasts.
		static VehicleSceneQueryData* Allocate(const PxU32 maxNumWheels);

		//Free allocated buffer for scene queries of suspension raycasts.
		void Free();

		//Create a PxBatchQuery instance that will be used as a single batched raycast of multiple suspension lines of multiple vehicles
		PxBatchQuery* SetUpBatchedSceneQuery(PxScene* scene);

		//Get the buffer of scene query results that will be used by PxVehicleNWSuspensionRaycasts
		PxRaycastQueryResult* GetRaycastQueryResultBuffer() {return m_SqResults;}

		//Get the number of scene query results that have been allocated for use by PxVehicleNWSuspensionRaycasts
		PxU32 GetRaycastQueryResultBufferSize() const {return m_NumQueries;}

		//Set the pre-filter shader 
		void SetPreFilterShader(PxBatchQueryPreFilterShader preFilterShader) {m_PreFilterShader=preFilterShader;}


	private:
		//One result for each wheel.
		PxRaycastQueryResult* m_SqResults;

		//One hit for each wheel.
		PxRaycastHit* m_SqHitBuffer;

		//Filter shader used to filter drivable and non-drivable surfaces
		
		PxBatchQueryPreFilterShader m_PreFilterShader;

		//Maximum number of suspension raycasts that can be supported by the allocated buffers 
		//assuming a single query and hit per suspension line.
		PxU32 m_NumQueries;
		PxU32 m_Pad[2];

		void Init()
		{
			m_PreFilterShader=VehicleWheelRaycastPreFilter;
		}

		VehicleSceneQueryData()
		{
			Init();
		}

		~VehicleSceneQueryData()
		{

		}
	};*/
}


#endif 
