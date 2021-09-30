//
// Created by Alexander on 30.09.2021.
//

#include <cstring>
#include "spdlog/spdlog.h"
#include "memory_pool_settings.h"


namespace st::memory
{
	MemoryPoolSettings::MemoryPoolSettings() : m_BucketsCount(0)
	{
		std::memset(m_BucketDefinitions, 0, sizeof(BucketDefinition) * MaxBucketsCount);
	}


	int MemoryPoolSettings::GetBucketsCount() const
	{
		return m_BucketsCount;
	}


	const MemoryPoolSettings::BucketDefinition& MemoryPoolSettings::GetBucketDefinition(int index) const
	{
		assert(index >= 0);
		assert(index < m_BucketsCount);

		return m_BucketDefinitions[index];
	}


	void MemoryPoolSettings::AddBucketDefinition(int itemSize, int firstPageItemsCount, int extraPageItemsCount, bool preWarmFirstPage)
	{
		if (m_BucketsCount == MaxBucketsCount)
		{
			spdlog::error("MemoryPoolSettings: can't add bucket definition. buckets count is maximum!");
			return;
		}

		m_BucketsCount++;

		m_BucketDefinitions[m_BucketsCount - 1] = BucketDefinition(itemSize, firstPageItemsCount, extraPageItemsCount, preWarmFirstPage);
	}


	MemoryPoolSettings GetDefaultMemoryPoolSettings(bool isThreadSafe)
	{
		MemoryPoolSettings settings;

		bool conditionalPrewarm = !isThreadSafe;

		//small sizes
		settings.AddBucketDefinition(4, 1024 * 8, 1024, true);
		settings.AddBucketDefinition(8, 1024 * 8, 1024, true);
		settings.AddBucketDefinition(16, 1024 * 8, 1024, true);
		settings.AddBucketDefinition(32, 1024 * 8, 1024, true);
		settings.AddBucketDefinition(64, 1024 * 8, 1024, true);
		settings.AddBucketDefinition(128, 1024 * 8, 1024, true);

		//medium sizes
		settings.AddBucketDefinition(128 + 64, 1024 * 4, 1024, true);
		settings.AddBucketDefinition(256, 1024 * 4, 1024, true);
		settings.AddBucketDefinition(256 + 128, 1024 * 4, 1024, true);
		settings.AddBucketDefinition(512, 1024, 512, conditionalPrewarm);

		//large sizes
		settings.AddBucketDefinition(512 + 256, 1024, 256, conditionalPrewarm);
		settings.AddBucketDefinition(1024, 128, 64, conditionalPrewarm);
		settings.AddBucketDefinition(2048, 64, 32, conditionalPrewarm);
		settings.AddBucketDefinition(1024 * 4, 32, 16, conditionalPrewarm);
		settings.AddBucketDefinition(1024 * 6, 16, 8, conditionalPrewarm);
		settings.AddBucketDefinition(1024 * 8, 16, 8, false);
		settings.AddBucketDefinition(1024 * 12, 16, 8, false);
		settings.AddBucketDefinition(1024 * 16, 8, 4, false);

		return settings;
	}

}

