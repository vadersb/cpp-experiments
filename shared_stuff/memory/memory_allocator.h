//
// Created by Alexander Samarin on 08.08.2021.
//

#pragma once

#include "memory_pool.h"

namespace st::memory
{
	template<typename T> class Allocator
	{
	public:

		typedef T value_type;

		Allocator() = default;
		template <class U> constexpr explicit Allocator (const Allocator <U>&) noexcept {}

		[[nodiscard]] T* allocate(std::size_t n)
		{
			std::size_t size = n * sizeof(T);

			void* pResult = MemoryPoolAllocate(size);

			return reinterpret_cast<T*>(pResult);
		}

		void deallocate(T* p, std::size_t n) noexcept
		{
			std::size_t size = n * sizeof(T);

			MemoryPoolDeallocate(p, size);
		}
	};

	template <class T, class U>
	bool operator==(const Allocator <T>&, const Allocator <U>&) { return true; }
	template <class T, class U>
	bool operator!=(const Allocator <T>&, const Allocator <U>&) { return false; }
}
