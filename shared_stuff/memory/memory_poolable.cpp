//
// Created by Alexander Samarin on 06.08.2021.
//

#include "memory_poolable.h"
#include "memory_pool.h"

namespace st::memory
{

	void* Poolable::operator new(std::size_t size)
	{
		return MemoryPoolAllocate(size);
	}


	void Poolable::operator delete(void* p, std::size_t size)
	{
		MemoryPoolDeallocate(p, size);
	}

}