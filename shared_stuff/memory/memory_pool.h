//
// Created by Alexander Samarin on 06.08.2021.
//

#pragma once

#include <cstddef>


namespace st::memory
{
	//init and release
	void MemoryPoolInit(bool preWarm = false);
	void MemoryPoolRelease();

	//allocate and deallocate
	void* MemoryPoolAllocate(size_t size);

	template<typename T> T* MemoryPoolAllocate()
	{
		return static_cast<T*>(MemoryPoolAllocate(sizeof(T)));
	}

	void MemoryPoolDeallocate(void* p, size_t size);

	template<typename T> void MemoryPoolDeallocate(T* p)
	{
		MemoryPoolDeallocate(p, sizeof(T));
	}

}
