#include "PhysXStream.h"

using namespace physx;
namespace GASS
{
	MemoryOutputStream::MemoryOutputStream() 
		
	{
	}

	MemoryOutputStream::~MemoryOutputStream()
	{
		if(m_Data)
			delete[] m_Data;
	}

	PxU32 MemoryOutputStream::write(const void* src, PxU32 size)
	{
		PxU32 expected_size = m_Size + size;
		if(expected_size > m_Capacity)
		{
			m_Capacity = expected_size + 4096;

			PxU8* new_data = new PxU8[m_Capacity];
			PX_ASSERT(newData!=NULL);

			if(new_data)
			{
				memcpy(new_data, m_Data, m_Size);
				delete[] m_Data;
			}
			m_Data = new_data;
		}
		memcpy(m_Data+m_Size, src, size);
		m_Size += size;
		return size;
	}



	MemoryInputData::MemoryInputData(PxU8* data, PxU32 length) : m_Size(length),
		m_Data(data),
		m_Pos(0)
	{
	}

	PxU32 MemoryInputData::read(void* dest, PxU32 count)
	{
		auto length = PxMin<PxU32>(count, m_Size-m_Pos);
		memcpy(dest, m_Data+m_Pos, length);
		m_Pos += length;
		return length;
	}

	PxU32 MemoryInputData::getLength() const
	{
		return m_Size;
	}

	void MemoryInputData::seek(PxU32 offset)
	{
		m_Pos = PxMin<PxU32>(m_Size, offset);
	}

	PxU32 MemoryInputData::tell() const
	{
		return m_Pos;
	}
}
