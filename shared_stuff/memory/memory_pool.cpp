//
// Created by Alexander Samarin on 06.08.2021.
//

#include <cassert>
#include <cstdlib>
#include "memory_pool.h"
#include "internal/memory_pool_bucket.h"


namespace st::memory
{
	bool g_MemoryPoolReady = false;

	constexpr int g_bucketsCount = 10;

	constexpr int g_bucketSizes[g_bucketsCount] =
			{
					4,
					8,
					16,
					32,
					64,
					128,
					256,
					512,
					1024,
					2048,
			};

	constexpr int g_bucketItemsPerPage[g_bucketsCount] =
			{
				1024,
				1024,
				1024,
				1024,
				1024,
				512,
				256,
				128,
				64,
				32,
			};

	MemoryPoolBucket* g_Buckets[g_bucketsCount] =
			{
					nullptr,
					nullptr,
					nullptr,
					nullptr,
					nullptr,
					nullptr,
					nullptr,
					nullptr,
					nullptr,
					nullptr
			};


	MemoryPoolBucket* GetMemoryPoolBucket(size_t size);


	void MemoryPoolInit(bool preWarm)
	{
		assert(g_MemoryPoolReady == false);

		//creating buckets
		for (int i = 0; i < g_bucketsCount; i++) {
			g_Buckets[i] = new MemoryPoolBucket(g_bucketSizes[i], g_bucketItemsPerPage[i], preWarm);
		}

		g_MemoryPoolReady = true;
	}


	void MemoryPoolRelease()
	{
		assert(g_MemoryPoolReady == true);

		//destroying buckets
		for (int i = 0; i < g_bucketsCount; i++) {
			delete g_Buckets[i];
			g_Buckets[i] = nullptr;
		}

		g_MemoryPoolReady = false;
	}


	void* MemoryPoolAllocate(size_t size)
	{
		assert(g_MemoryPoolReady == true);
		assert(size > 0);

		auto pBucket = GetMemoryPoolBucket(size);

		if (pBucket == nullptr)
		{
			return std::malloc(size);
		}
		else
		{
			return pBucket->Allocate();
		}
	}


	void MemoryPoolDeallocate(void* p, size_t size)
	{
		assert(g_MemoryPoolReady == true);
		assert(p != nullptr);
		assert(size > 0);

		auto pBucket = GetMemoryPoolBucket(size);

		if (pBucket == nullptr)
		{
			std::free(p);
		}
		else
		{
			pBucket->Deallocate(p);
		}
	}


	MemoryPoolBucket* GetMemoryPoolBucket(size_t size)
	{
		for (int i = 0; i < g_bucketsCount; i++)
		{
			if (size <= g_bucketSizes[i])
			{
				return g_Buckets[i];
			}
		}

		return nullptr;
	}
}