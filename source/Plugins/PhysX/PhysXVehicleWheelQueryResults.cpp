// This code contains NVIDIA Confidential Information and is disclosed to you
// under a form of NVIDIA software license agreement provided separately to you.
//
// Notice
// NVIDIA Corporation and its licensors retain all intellectual property and
// proprietary rights in and to this software and related documentation and
// any modifications thereto. Any use, reproduction, disclosure, or
// distribution of this software and related documentation without an express
// license agreement from NVIDIA Corporation is strictly prohibited.
//
// ALL NVIDIA DESIGN SPECIFICATIONS, CODE ARE PROVIDED "AS IS.". NVIDIA MAKES
// NO WARRANTIES, EXPRESSED, IMPLIED, STATUTORY, OR OTHERWISE WITH RESPECT TO
// THE MATERIALS, AND EXPRESSLY DISCLAIMS ALL IMPLIED WARRANTIES OF NONINFRINGEMENT,
// MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE.
//
// Information and code furnished is believed to be accurate and reliable.
// However, NVIDIA Corporation assumes no responsibility for the consequences of use of such
// information or for any infringement of patents or other rights of third parties that may
// result from its use. No license is granted by implication or otherwise under any patent
// or patent rights of NVIDIA Corporation. Details are subject to change without notice.
// This code supersedes and replaces all information previously supplied.
// NVIDIA Corporation products are not authorized for use as critical
// components in life support devices or systems without express written approval of
// NVIDIA Corporation.
//
// Copyright (c) 2008-2014 NVIDIA Corporation. All rights reserved.
// Copyright (c) 2004-2008 AGEIA Technologies, Inc. All rights reserved.
// Copyright (c) 2001-2004 NovodeX AG. All rights reserved.  

#include "PhysXVehicleWheelQueryResults.h"
//#include "vehicle/PxVehicleSDK.h"
#include <new>
//#include "PsFoundation.h"
//#include "PsUtilities.h"
//#define CHECK_MSG(exp, msg) (!!(exp) || (physx::shdfnd::getFoundation().error(physx::PxErrorCode::eINVALID_PARAMETER, __FILE__, __LINE__, msg), 0) )

namespace GASS
{
VehicleWheelQueryResults* VehicleWheelQueryResults::Allocate(const PxU32 maxNumWheels)
{
	const PxU32 size = sizeof(VehicleWheelQueryResults) + sizeof(PxWheelQueryResult)*maxNumWheels;
	auto* res_data = (VehicleWheelQueryResults*)malloc(size);
	res_data->Init();
	PxU8* ptr = (PxU8*) res_data;
	ptr += sizeof(VehicleWheelQueryResults);
	res_data->m_MWheelQueryResults = (PxWheelQueryResult*)ptr;
	//ptr +=  sizeof(PxWheelQueryResult)*maxNumWheels;
	res_data->m_MMaxNumWheels=maxNumWheels;
	for(PxU32 i=0;i<maxNumWheels;i++)
	{
		new(&res_data->m_MWheelQueryResults[i]) PxWheelQueryResult();
	}
	return res_data;
}

void VehicleWheelQueryResults::Free()
{
	delete (this);
}

PxWheelQueryResult* VehicleWheelQueryResults::AddVehicle(const PxU32 numWheels)
{
	PX_ASSERT((mNumWheels + numWheels) <= mMaxNumWheels);
	PxWheelQueryResult* r = &m_MWheelQueryResults[m_MNumWheels];
	m_MNumWheels += numWheels;
	return r;
}
}






