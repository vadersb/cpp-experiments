//
// Created by Alexander Samarin on 06.08.2021.
//

#pragma once

#include <cstddef>
#include "memory_pool.h"

namespace st::memory
{
	template<bool isThreadSafe> class Poolable
	{
	public:

		static void* operator new(std::size_t size)
		{
			return MemoryPool<isThreadSafe>::Allocate(size);
		}

		static void operator delete(void* p, std::size_t size)
		{
			MemoryPool<isThreadSafe>::Deallocate(p, size);
		}

	protected:

		//forces any derived classes to have virtual destructor
		//it is required to receive correct *size* value in operator delete
		virtual ~Poolable() = default;

	private:

	};
}