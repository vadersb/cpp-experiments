//
// Created by Alexander Samarin on 06.08.2021.
//

#include "memory_pool_bucket.h"

namespace st::memory
{
	MemoryPoolBucket::MemoryPoolBucket(int itemSize) :
	m_ItemSize(itemSize)
	{

	}


	MemoryPoolBucket::~MemoryPoolBucket()
	{

	}


	void* MemoryPoolBucket::Allocate()
	{
		return nullptr;
	}


	void MemoryPoolBucket::Deallocate(void* p)
	{

	}

}
