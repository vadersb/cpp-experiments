//
// Created by Alexander Samarin on 06.08.2021.
//

#pragma once

#include <cstddef>


namespace st::memory
{
	//init and release
	void MemoryPoolInit();
	void MemoryPoolRelease();

	//allocate and deallocate
	void* MemoryPoolAllocate(size_t size);

	template<typename T> void* MemoryPoolAllocate()
	{
		return MemoryPoolAllocate(sizeof(T));
	}

	void MemoryPoolDeallocate(void* p, size_t size);

	template<typename T> void* MemoryPoolDeallocate(void* p)
	{
		MemoryPoolDeallocate(p, sizeof(T));
	}

}
