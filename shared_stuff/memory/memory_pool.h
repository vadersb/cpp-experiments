//
// Created by Alexander Samarin on 06.08.2021.
//

#pragma once

#include <cstddef>
#include <type_traits>
#include <mutex>
#include <cassert>


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








	class MemoryPoolSettings final
	{
	public:

		static constexpr int MaxPagesCount = 256;

		struct BucketDefinition
		{
		public:
			int m_FirstPageItemsCount;
			int m_PageItemsCount;
			bool m_PreWarmFirstPage;
		};

		MemoryPoolSettings();

	private:



		int m_BucketsCount;

		BucketDefinition m_BucketDefinitions[MaxPagesCount];


	};


	//todo memory pool uses a fixed array of buckets, so there is no need to store settings (goal: improving memory locality)

//	template<bool isThreadSafe> class MemoryPool final
//	{
//	public:
//
//		static void Init(MemoryPoolSettings* pSettings = nullptr)
//		{
//			if constexpr(isThreadSafe)
//			{
//				std::lock_guard lock(m_Mutex);
//				DoInit(pSettings);
//			}
//			else
//			{
//				DoInit(pSettings);
//			}
//		}
//
//		static void Release()
//		{
//			if constexpr(isThreadSafe)
//			{
//				std::lock_guard lock(m_Mutex);
//				DoRelease();
//			}
//			else
//			{
//				DoRelease();
//			}
//		}
//
//
//
//	private:
//
//		static inline void DoInit(MemoryPoolSettings* pSettings)
//		{
//			assert(m_IsInitialized == false);
//
//			m_pSettings = pSettings;
//
//			if (m_pSettings == nullptr)
//			{
//				m_pSettings = CreateDefaultSettings();
//			}
//
//			assert(m_pSettings != nullptr);
//
//			m_IsInitialized = true;
//		}
//
//		static inline void DoRelease()
//		{
//			assert(m_IsInitialized == true);
//			assert(m_pSettings != nullptr);
//
//			delete m_pSettings;
//			m_pSettings = nullptr;
//		}
//
//		//default settings
//		static MemoryPoolSettings* CreateDefaultSettings()
//		{
//			MemoryPoolSettings* pSettings = new MemoryPoolSettings();
//
//			return pSettings;
//		}
//
//		//instantiation is disabled
//		MemoryPool() = default;
//
//		static MemoryPoolSettings* m_pSettings;
//
//		static bool m_IsInitialized;
//
//		static std::mutex m_Mutex;
//
//
//	};
//
//	using MemoryPoolSingleThreaded = MemoryPool<false>;
//	using MemoryPoolThreadSafe = MemoryPool<true>;
//
//	template<bool isThreadSafe> bool MemoryPool<isThreadSafe>::m_IsInitialized = false;
//	template<bool isThreadSafe> MemoryPoolSettings* MemoryPool<isThreadSafe>::m_pSettings = nullptr;
//	template<bool isThreadSafe> std::mutex MemoryPool<isThreadSafe>::m_Mutex;


}
