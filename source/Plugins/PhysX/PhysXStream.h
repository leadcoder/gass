
#pragma once

#include "PhysXCommon.h"

namespace physx
{
	class PxTriangleMesh;
	class PxConvexMesh;
	class PxPhysics;
	class PxCooking;
	class PxTriangleMeshDesc;
}

namespace GASS
{
	class MemoryOutputStream : public physx::PxOutputStream
	{
	public:
		MemoryOutputStream();
		~MemoryOutputStream() override;
		physx::PxU32 write(const void* src, physx::PxU32 count) override;
		physx::PxU32 GetSize() const {return m_Size;}
		physx::PxU8* GetData() const {return m_Data;}
	private:
		physx::PxU8* m_Data{nullptr};
		physx::PxU32 m_Size{0};
		physx::PxU32 m_Capacity{0};
	};

	class MemoryInputData: public physx::PxInputData
	{
	public:
		MemoryInputData(physx::PxU8* data, physx::PxU32 length);
		physx::PxU32 read(void* dest, physx::PxU32 count) override;
		physx::PxU32 getLength() const override;
		void seek(physx::PxU32 pos) override;
		physx::PxU32 tell() const override;
	private:
		physx::PxU32 m_Size;
		const physx::PxU8* m_Data;
		physx::PxU32 m_Pos;
	};
}

