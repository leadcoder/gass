
#pragma once;

#include <cooking/PxCooking.h>
#include <foundation/PxIO.h>

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
		virtual ~MemoryOutputStream();
		physx::PxU32 write(const void* src, physx::PxU32 count);
		physx::PxU32 getSize() const {return m_Size;}
		physx::PxU8* getData() const {return m_Data;}
	private:
		physx::PxU8* m_Data;
		physx::PxU32 m_Size;
		physx::PxU32 m_Capacity;
	};

	class MemoryInputData: public physx::PxInputData
	{
	public:
		MemoryInputData(physx::PxU8* data, physx::PxU32 length);
		physx::PxU32 read(void* dest, physx::PxU32 count);
		physx::PxU32 getLength() const;
		void seek(physx::PxU32 pos);
		physx::PxU32 tell() const;
	private:
		physx::PxU32 m_Size;
		const physx::PxU8* m_Data;
		physx::PxU32 m_Pos;
	};
}

