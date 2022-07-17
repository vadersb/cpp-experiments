//
// Created by Alexander Samarin on 06.08.2021.
//

#pragma once

#include <cstddef>
#include <type_traits>
#include <mutex>
#include <cassert>
#include <map>
#include <thread>
#include "internal/memory_pool_bucket.h"
#include "spdlog/spdlog.h"

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
				assert(s_InitThreadID == std::this_thread::get_id());
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
				assert(s_InitThreadID == std::this_thread::get_id());
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
				assert(s_InitThreadID == std::this_thread::get_id());
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
				assert(s_InitThreadID == std::this_thread::get_id());
				s_pInstance->DoDeallocate(pointer, sizeof(T));
			}
		}


	private:

		static constexpr int InvalidIndex = -1;

		MemoryPool() = default;

		MemoryPool(const MemoryPoolSettings& settings) : m_Requests_Total(), m_Requests_Current()
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
			s_InitThreadID = std::this_thread::get_id();
			s_pInstance = new MemoryPool(GetDefaultMemoryPoolSettings(isThreadSafe));
		}

		static inline void DoInit(const MemoryPoolSettings& settings)
		{
			assert(s_pInstance == nullptr);
			s_InitThreadID = std::this_thread::get_id();
			s_pInstance = new MemoryPool(settings);
		}

		static inline void DoRelease()
		{
			assert(s_pInstance != nullptr);
			assert(s_InitThreadID == std::this_thread::get_id());

			s_pInstance->LogStatistics();

			delete s_pInstance;
			s_pInstance = nullptr;
		}

		inline void* DoAllocate(size_t size)
		{
			RegisterRequestAllocate(size);

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
			RegisterRequestDeallocate(size);

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

		//statistics
		inline void RegisterRequestAllocate(size_t size)
		{
			//total
			{
				auto search = m_Requests_Total.find(size);
				if (search != m_Requests_Total.end())
				{
					search->second++;
				}
				else
				{
					m_Requests_Total[size] = 1;
				}
			}

			//max
			{
				auto search = m_Requests_Current.find(size);
				if (search != m_Requests_Current.end())
				{
					search->second++;

					auto inMax = m_Requests_Max.find(size);

					if (inMax->second < search->second)
					{
						inMax->second = search->second;
					}
				}
				else
				{
					m_Requests_Current[size] = 1;
					m_Requests_Max[size] = 1;
				}
			}
		}

		inline void RegisterRequestDeallocate(size_t size)
		{
			//total - no need

			//max
			{
				auto search = m_Requests_Current.find(size);
				if (search != m_Requests_Current.end())
				{
					search->second--;
				}
				else
				{
					spdlog::error("Memory pool: deallocation request for size [{}] with no previous allocation request registered!", size);
				}
			}
		}

		void LogStatistics()
		{
			if (isThreadSafe)
			{
				spdlog::info("Memory Pool (Multithreaded) requests stats:");
			}
			else
			{
				spdlog::info("Memory Pool (Single threaded) requests stats:");
			}

			for(auto& [key, value] : m_Requests_Total)
			{
				auto requestsMax = m_Requests_Max[key];

				spdlog::info("   [{}] requests. Total: {}   Max: {}", key, value, requestsMax);
			}
		}


		//static data
		static std::thread::id s_InitThreadID;
		static std::mutex m_Mutex;
		static inline MemoryPool* s_pInstance = nullptr;

		//instance data
		int m_BucketsCount;
		MemoryPoolBucket m_Buckets[MemoryPoolSettings::MaxBucketsCount];

		std::map<int32_t, int64_t> m_Requests_Total;
		std::map<int32_t, int64_t> m_Requests_Current;
		std::map<int32_t, int64_t> m_Requests_Max;

	};

	using MemoryPoolSingleThreaded [[maybe_unused]] = MemoryPool<false>;
	using MemoryPoolMultiThreaded  [[maybe_unused]] = MemoryPool<true>;

	template<bool isThreadSafe> std::thread::id MemoryPool<isThreadSafe>::s_InitThreadID;
	template<bool isThreadSafe> std::mutex MemoryPool<isThreadSafe>::m_Mutex;
}
