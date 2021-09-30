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


	template<bool isThreadSafe> class MemoryPool final
	{
	public:

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

		static void* Allocate(size_t size)
		{
			//todo
			return nullptr;
		}

		template<typename T> void* Allocate()
		{
			//todo sizeof(T)
			return nullptr;
		}

		static void Deallocate(void* pPointer, size_t size)
		{
			//todo
		}


	private:

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

		//static data
		static std::mutex m_Mutex;
		static MemoryPool* s_pInstance = nullptr;

		//instance data
		int m_BucketsCount;
		MemoryPoolBucketNew m_Buckets[MemoryPoolSettings::MaxBucketsCount];

		//todo current and max requests per item size (map containers) for statistics logging

	};


}
