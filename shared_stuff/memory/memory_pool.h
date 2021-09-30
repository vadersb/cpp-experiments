//
// Created by Alexander Samarin on 06.08.2021.
//

#pragma once

#include <cstddef>
#include <type_traits>
#include <mutex>
#include <cassert>
#include "internal/memory_pool_bucket.h"

namespace st::memory
{
	template<bool isThreadSafe> class MemoryPool final
	{
	public:

		static void Init()
		{
			if constexpr(isThreadSafe)
			{
				std::lock_guard lock(m_Mutex);
				DoInit();
			}
			else
			{
				DoInit();
			}
		}

		static void Init(const MemoryPoolSettings& settings)
		{
			if constexpr(isThreadSafe)
			{
				std::lock_guard lock(m_Mutex);
				DoInit(settings);
			}
			else
			{
				DoInit(settings);
			}
		}

		//later: maybe release is just a flag, then once all threads return items via Deallocate - then do the cleanup
		static void Release()
		{
			if constexpr(isThreadSafe)
			{
				std::lock_guard lock(m_Mutex);
				DoRelease();
			}
			else
			{
				DoRelease();
			}
		}

		[[nodiscard]] static void* Allocate(size_t size)
		{
			if constexpr(isThreadSafe)
			{
				std::lock_guard lock(m_Mutex);
				assert(s_pInstance != nullptr);
				return s_pInstance->DoAllocate(size);
			}
			else
			{
				assert(s_pInstance != nullptr);
				return s_pInstance->DoAllocate(size);
			}
		}

		template<typename T> [[nodiscard]] static T* Allocate()
		{
			if constexpr(isThreadSafe)
			{
				std::lock_guard lock(m_Mutex);
				assert(s_pInstance != nullptr);
				return reinterpret_cast<T*>(s_pInstance->DoAllocate(sizeof(T)));
			}
			else
			{
				assert(s_pInstance != nullptr);
				return reinterpret_cast<T*>(s_pInstance->DoAllocate(sizeof(T)));
			}
		}

		static void Deallocate(void* pointer, size_t size)
		{
			if constexpr(isThreadSafe)
			{
				std::lock_guard lock(m_Mutex);
				assert(s_pInstance != nullptr);
				s_pInstance->DoDeallocate(pointer, size);
			}
			else
			{
				assert(s_pInstance != nullptr);
				s_pInstance->DoDeallocate(pointer, size);
			}
		}

		template<typename T>static void  Deallocate(T* pointer)
		{
			if constexpr(isThreadSafe)
			{
				std::lock_guard lock(m_Mutex);
				assert(s_pInstance != nullptr);
				s_pInstance->DoDeallocate(pointer, sizeof(T));
			}
			else
			{
				assert(s_pInstance != nullptr);
				s_pInstance->DoDeallocate(pointer, sizeof(T));
			}
		}


	private:

		static constexpr int InvalidIndex = -1;

		MemoryPool() = default;

		MemoryPool(const MemoryPoolSettings& settings)
		{
			m_BucketsCount = settings.GetBucketsCount();
			assert(m_BucketsCount > 0);

			for (int i = 0; i < m_BucketsCount; i++)
			{
				m_Buckets[i].Setup(settings.GetBucketDefinition(i));
			}
		}

		static inline void DoInit()
		{
			assert(s_pInstance == nullptr);
			s_pInstance = new MemoryPool(GetDefaultMemoryPoolSettings(isThreadSafe));
		}

		static inline void DoInit(const MemoryPoolSettings& settings)
		{
			assert(s_pInstance == nullptr);
			s_pInstance = new MemoryPool(settings);
		}

		static inline void DoRelease()
		{
			assert(s_pInstance != nullptr);
			delete s_pInstance;
			s_pInstance = nullptr;
		}

		inline void* DoAllocate(size_t size)
		{
			int bucketIndex = GetBucketIndex(size);

			if (bucketIndex == InvalidIndex)
			{
				return std::malloc(size);
			}
			else
			{
				return m_Buckets[bucketIndex].Allocate();
			}
		}

		inline void DoDeallocate(void* pointer, size_t size)
		{
			int bucketIndex = GetBucketIndex(size);

			if (bucketIndex == InvalidIndex)
			{
				std::free(pointer);
			}
			else
			{
				m_Buckets[bucketIndex].Deallocate(pointer);
			}
		}

		inline int GetBucketIndex(size_t size)
		{
			assert(m_BucketsCount > 0);

			int maxIndex = m_BucketsCount - 1;

			if (m_Buckets[maxIndex].GetItemSize() < size)
			{
				return InvalidIndex;
			}

			for (int i = 0; i < m_BucketsCount; i++)
			{
				if (m_Buckets[i].GetItemSize() >= size)
				{
					return i;
				}
			}

			assert(false);
			return InvalidIndex;
		}

		//static data
		static std::mutex m_Mutex;
		static inline MemoryPool* s_pInstance = nullptr;

		//instance data
		int m_BucketsCount;
		MemoryPoolBucket m_Buckets[MemoryPoolSettings::MaxBucketsCount];

		//todo current and max requests per item size (map containers) for statistics logging

	};

	using MemoryPoolSingleThreaded [[maybe_unused]] = MemoryPool<false>;
	using MemoryPoolMultiThreaded  [[maybe_unused]] = MemoryPool<true>;

	template<bool isThreadSafe> std::mutex MemoryPool<isThreadSafe>::m_Mutex;
}
