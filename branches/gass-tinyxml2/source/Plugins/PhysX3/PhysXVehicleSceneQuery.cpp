#include "PhysXVehicleSceneQuery.h"
#include "vehicle/PxVehicleSDK.h"
#include "PxFiltering.h"

namespace GASS
{

	void VehicleSetupDrivableShapeQueryFilterData(PxFilterData* qryFilterData)
	{
		qryFilterData->word3 = (PxU32)VEHICLE_DRIVABLE_SURFACE;
	}

	void VehicleSetupNonDrivableShapeQueryFilterData(PxFilterData* qryFilterData)
	{
		qryFilterData->word3 = (PxU32)VEHICLE_UNDRIVABLE_SURFACE;
	}

	void VehicleSetupVehicleShapeQueryFilterData(PxFilterData* qryFilterData)
	{
		qryFilterData->word3 = (PxU32)VEHICLE_UNDRIVABLE_SURFACE;
	}

	VehicleSceneQueryData* VehicleSceneQueryData::Allocate(const PxU32 max_num_wheels)
	{
		const PxU32 size = sizeof(VehicleSceneQueryData) + sizeof(PxRaycastQueryResult)*max_num_wheels + sizeof(PxRaycastHit)*max_num_wheels;
		VehicleSceneQueryData* data = (VehicleSceneQueryData*)malloc(size);
		data->Init();
		PxU8* ptr = (PxU8*) data;
		ptr += sizeof(VehicleSceneQueryData);
		data->m_SqResults = (PxRaycastQueryResult*)ptr;
		ptr +=  sizeof(PxRaycastQueryResult)*max_num_wheels;
		data->m_SqHitBuffer = (PxRaycastHit*)ptr;
		ptr += sizeof(PxRaycastHit)*max_num_wheels;
		data->m_NumQueries = max_num_wheels;
		return data;
	}

	void VehicleSceneQueryData::Free()
	{
		delete this;
	}

	PxBatchQuery* VehicleSceneQueryData::SetUpBatchedSceneQuery(PxScene* scene)
	{
		PxBatchQueryDesc sqDesc;
		sqDesc.userRaycastResultBuffer = m_SqResults;
		sqDesc.userRaycastHitBuffer = m_SqHitBuffer;
		sqDesc.raycastHitBufferSize = m_NumQueries;
		sqDesc.preFilterShader = m_PreFilterShader;
		return scene->createBatchQuery(sqDesc);
	}
}




