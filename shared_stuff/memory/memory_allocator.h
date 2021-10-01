//
// Created by Alexander Samarin on 08.08.2021.
//

#pragma once

#include "memory_pool.h"

namespace st::memory
{
	template<typename T> class AllocatorSingleThreaded
	{
	public:

		typedef T value_type;

		AllocatorSingleThreaded() = default;
		template <class U> constexpr explicit AllocatorSingleThreaded (const AllocatorSingleThreaded <U>&) noexcept {}

		[[nodiscard]] T* allocate(std::size_t n)
		{
			std::size_t size = n * sizeof(T);

			void* pResult = MemoryPoolSingleThreaded::Allocate(size);

			return reinterpret_cast<T*>(pResult);
		}

		void deallocate(T* p, std::size_t n) noexcept
		{
			std::size_t size = n * sizeof(T);

			MemoryPoolSingleThreaded::Deallocate(p, size);
		}
	};

	template <class T, class U>
	bool operator==(const AllocatorSingleThreaded <T>&, const AllocatorSingleThreaded <U>&) { return true; }
	template <class T, class U>
	bool operator!=(const AllocatorSingleThreaded <T>&, const AllocatorSingleThreaded <U>&) { return false; }

	template<typename T> class AllocatorMultiThreaded
	{
	public:

		typedef T value_type;

		AllocatorMultiThreaded() = default;
		template <class U> constexpr explicit AllocatorMultiThreaded (const AllocatorMultiThreaded <U>&) noexcept {}

		[[nodiscard]] T* allocate(std::size_t n)
		{
			std::size_t size = n * sizeof(T);

			void* pResult = MemoryPoolMultiThreaded::Allocate(size);

			return reinterpret_cast<T*>(pResult);
		}

		void deallocate(T* p, std::size_t n) noexcept
		{
			std::size_t size = n * sizeof(T);

			MemoryPoolMultiThreaded::Deallocate(p, size);
		}
	};

	template <class T, class U>
	bool operator==(const AllocatorMultiThreaded <T>&, const AllocatorMultiThreaded <U>&) { return true; }
	template <class T, class U>
	bool operator!=(const AllocatorMultiThreaded <T>&, const AllocatorMultiThreaded <U>&) { return false; }
}
